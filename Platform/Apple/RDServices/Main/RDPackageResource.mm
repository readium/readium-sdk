//
//  RDPackageResource.mm
//  RDServices
//
//  Created by Shane Meyer on 2/28/13.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#import "RDPackageResource.h"
#import <ePub3/archive.h>
#import <ePub3/filter.h>
#import <ePub3/filter_chain.h>
#import <ePub3/filter_chain_byte_stream_range.h>
#import <ePub3/filter_chain_byte_stream.h>
#import <ePub3/package.h>
#import <ePub3/utilities/byte_stream.h>
#import "RDPackage.h"


@interface RDPackageResource() {
	@private UInt8 m_buffer[4096];
	@private std::unique_ptr<ePub3::ByteStream> m_byteStream;
	@private NSUInteger m_contentLength;
	@private UInt64 m_offset;
	@private NSData *m_data;
	@private RDPackage *m_package;
	@private NSString *m_relativePath;
	@private BOOL m_isRangeRequest;
	@private BOOL m_hasProperStream;
}

@end


@implementation RDPackageResource


@synthesize contentLength = m_contentLength;
@synthesize package = m_package;
@synthesize relativePath = m_relativePath;
@synthesize isRangeRequest = m_isRangeRequest;

- (void *)byteStream {
	return m_byteStream.get();
}


- (NSData *)data {
	if (m_data == nil) {
		NSMutableData *md = [[NSMutableData alloc] initWithCapacity:m_contentLength == 0 ? 1 : m_contentLength];
		
		[self ensureProperByteStream];
		
		while (YES)
		{
			std::size_t count = m_byteStream->ReadBytes(m_buffer, sizeof(m_buffer));
			if (count == 0)
			{
				break;
			}
			
			[md appendBytes:m_buffer length:count];
		}
		
		m_data = md;
	}
	
	return m_data;
}


- (instancetype)initWithByteStream:(void *)byteStream
	package:(RDPackage *)package
	relativePath:(NSString *)relativePath
{
	if (byteStream == nil || package == nil || relativePath == nil || relativePath.length == 0) {
		return nil;
	}
	
	if (self = [super init]) {
		m_byteStream.reset((ePub3::ByteStream *)byteStream);
		m_contentLength = m_byteStream->BytesAvailable();
		m_package = package;
		m_relativePath = relativePath;
		m_isRangeRequest = NO;
		m_hasProperStream = NO;
		
		if (m_contentLength == 0) {
			NSLog(@"The resource content length is zero! %@", m_relativePath);
		}
	}
	
	return self;
}


- (NSData *)readDataOfLength:(NSUInteger)length {
	NSMutableData *md = [[NSMutableData alloc] initWithCapacity:length];
	
	[self ensureProperByteStream];
	
	if (m_isRangeRequest)
	{
		// First, check to see if m_byteStream is a FilterChainByteStreamRange, in which case we can just
		// use it in order to get the requested bytes from the given byte range.
		ePub3::FilterChainByteStreamRange *filterStream = dynamic_cast<ePub3::FilterChainByteStreamRange *>(m_byteStream.get());
		if (filterStream != nullptr)
		{
			ePub3::ByteRange range;
			range.Location(m_offset);
			NSUInteger totalRead = 0;

			while (totalRead < length)
			{
				range.Length(MIN(sizeof(m_buffer), length - totalRead));
				std::size_t count = filterStream->ReadBytes(m_buffer, sizeof(m_buffer), range);
				[md appendBytes:m_buffer length:count];
				totalRead += count;
				m_offset += count;
				range.Location(range.Location() + count);
				
				if (count != range.Length())
				{
					//TODO: this seems to happen quite often? Is this expected?
					NSLog(@"Did not read the expected number of bytes! (%lu %lu)", count, (unsigned long)range.Length());
					break;
				}
			}
			
			return md;
		}
		
		// Second, if m_byteStream is not a FilterChainByteStreamRange, then check to see if it is at least a
		// SeekableByteStream. In that case, we can still use the seek methods in order to get the bytes for the
		// requested byte range.
		ePub3::SeekableByteStream *seekableByteStream = dynamic_cast<ePub3::SeekableByteStream *>(m_byteStream.get());
		if (seekableByteStream != nullptr)
		{
			seekableByteStream->Seek(m_offset, std::ios::seekdir::beg);
			NSUInteger totalRead = 0;

			while (totalRead < length)
			{
				std::size_t toRead = MIN(sizeof(m_buffer), length - totalRead);
				std::size_t count = seekableByteStream->ReadBytes(m_buffer, toRead);
				if (count <= 0)
				{
					break;
				}

				[md appendBytes:m_buffer length:count];

				totalRead += count;
				m_offset += count;
			}

			return md;
		}
		
		NSLog(@"There was a byte range request for this resource, but RDPackageResource did not contain a byte stream that allowed to extract byte ranges.");
		NSLog(@"Returning just a buffer of bytes starting from the beginning instead.");
	}
	 
	// So, either this is not a range request, or, it was a range request but the current m_byteStream
	// is not a FilterChainByteStreamRange neither a SeekableByteStream (in which case we simply cannot
	// extract a given byte range from the byte stream). In that case, then just return a buffer that
	// fits in the length passed as an argument.
	NSData *prefetchedData = [self data];
	NSUInteger prefetchedDataLength = [prefetchedData length];
	NSUInteger adjustedLength = prefetchedDataLength < length ? prefetchedDataLength : length;
	[md appendBytes:prefetchedData.bytes length:adjustedLength];
	return md;
}


- (void)setOffset:(UInt64)offset {
	m_offset = offset;
}


- (void)ensureProperByteStream {
	if (!m_hasProperStream)
	{
		ePub3::ByteStream *byteStream = m_byteStream.release();
		m_byteStream.reset((ePub3::ByteStream *)[m_package getProperByteStream:m_relativePath currentByteStream:byteStream isRangeRequest:m_isRangeRequest]);
		m_contentLength = m_byteStream->BytesAvailable();
		m_hasProperStream = YES;
	}
}

@end
