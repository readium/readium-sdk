//
//  RDPackageResource.mm
//  RDServices
//
//  Created by Shane Meyer on 2/28/13.
//  Copyright (c) 2012-2013 The Readium Foundation.
//

#import "RDPackageResource.h"
#import <ePub3/archive.h>
#import <ePub3/package.h>
#import <ePub3/utilities/byte_stream.h>
#import "RDPackage.h"


@interface RDPackageResource() {
	@private ePub3::ByteStream *m_byteStream;
	@private NSUInteger m_contentLength;
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
	NSMutableData *md = [[NSMutableData alloc] initWithCapacity:length == 0 ? 1 : length];
	NSUInteger totalRead = 0;

	while (totalRead < length) {
		NSUInteger thisLength = MIN(sizeof(m_buffer), length - totalRead);
		std::size_t count = m_byteStream->ReadBytes(m_buffer, thisLength);
		totalRead += count;
		[md appendBytes:m_buffer length:count];

		if (count != thisLength) {
			NSLog(@"Did not read the expected number of bytes! (%lu %lu)",
				count, (unsigned long)thisLength);
			break;
		}
	}

	return md;
}


- (void)setOffset:(UInt64)offset {
	ePub3::SeekableByteStream* seekStream = dynamic_cast<ePub3::SeekableByteStream*>(m_byteStream);
	ePub3::ByteStream::size_type pos = seekStream->Seek(offset, std::ios::beg);

	if (pos != offset) {
		NSLog(@"Setting the byte stream offset failed! pos = %lu, offset = %llu", pos, offset);
	}
}


@end
