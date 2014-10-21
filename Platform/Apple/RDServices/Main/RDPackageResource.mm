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
#import <ePub3/package.h>
#import <ePub3/utilities/byte_stream.h>
#import "RDPackage.h"


@interface RDPackageResource() {
	@private ePub3::ByteStream *m_byteStream;
	@private NSUInteger m_contentLength;
	@private UInt64 m_offset;
}

@end


@implementation RDPackageResource


@synthesize byteStream = m_byteStream;
@synthesize contentLength = m_contentLength;
@synthesize package = m_package;
@synthesize relativePath = m_relativePath;


- (NSData *)data {
	if (m_data == nil) {
		NSMutableData *md = [[NSMutableData alloc] initWithCapacity:
			m_contentLength == 0 ? 1 : m_contentLength];

		while (YES) {
			std::size_t count = m_byteStream->ReadBytes(m_buffer, sizeof(m_buffer));

			if (count == 0) {
				break;
			}

			[md appendBytes:m_buffer length:count];
		}

		m_data = md;
	}

	return m_data;
}


- (void)dealloc {
	[m_delegate rdpackageResourceWillDeallocate:self];
}


- (id)
	initWithDelegate:(id <RDPackageResourceDelegate>)delegate
	byteStream:(void *)byteStream
	package:(RDPackage *)package
	relativePath:(NSString *)relativePath
{
	if (byteStream == nil || package == nil || relativePath == nil || relativePath.length == 0) {
		return nil;
	}

	if (self = [super init]) {
		m_byteStream = (ePub3::ByteStream *)byteStream;
		m_contentLength = m_byteStream->BytesAvailable();
		m_delegate = delegate;
		m_package = package;
		m_relativePath = relativePath;

		if (m_contentLength == 0) {
			NSLog(@"The resource content length is zero! %@", m_relativePath);
		}
	}

	return self;
}


- (NSData *)readDataOfLength:(NSUInteger)length {
	NSMutableData *md = [[NSMutableData alloc] initWithCapacity:length];

	ePub3::ByteRangeFilterSyncStream *filterStream =
		dynamic_cast<ePub3::ByteRangeFilterSyncStream *>(m_byteStream);

	if (filterStream == nullptr) {
		NSLog(@"The byte stream is not a FilterChainSyncStream!");
	}
	else {
		ePub3::ByteRange range;
		range.Location(m_offset);
		NSUInteger totalRead = 0;

		while (totalRead < length) {
			range.Length(MIN(sizeof(m_buffer), length - totalRead));
			std::size_t count = filterStream->ReadBytes(m_buffer, sizeof(m_buffer), range);
			[md appendBytes:m_buffer length:count];
			totalRead += count;
            range.Location(range.Location() + count);

			if (count != range.Length()) {
				NSLog(@"Did not read the expected number of bytes! (%lu %lu)",
					count, (unsigned long)range.Length());
				break;
			}
		}
	}

	return md;
}


- (void)setOffset:(UInt64)offset {
	m_offset = offset;
}

- (BOOL)isByteRangeResource
{
    ePub3::ByteRangeFilterSyncStream *filterStream = dynamic_cast<ePub3::ByteRangeFilterSyncStream *>(m_byteStream);
    return (filterStream != nullptr);
}


@end
