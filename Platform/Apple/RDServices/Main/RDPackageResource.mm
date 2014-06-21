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
#import <ePub3/package.h>
#import <ePub3/utilities/byte_stream.h>
#import "RDPackage.h"


@interface RDPackageResource() {
	@private ePub3::ByteStream *_byteStream;
    @private UInt8 _buffer[4096];

}


@property (nonatomic, assign, readwrite) NSUInteger contentLength;
@property (nonatomic, strong, readwrite) RDPackage *package;
@property (nonatomic, strong, readwrite) NSData *data;
@property (nonatomic, copy, readwrite) NSString *relativePath;
@property (nonatomic, assign, readwrite) id<RDPackageResourceDelegate> delegate;

@end


@implementation RDPackageResource


#pragma mark - Init methods

- (instancetype)initWithDelegate:(id <RDPackageResourceDelegate>)delegate
                      byteStream:(void *)byteStream
                         package:(RDPackage *)package
                    relativePath:(NSString *)relativePath {
    NSParameterAssert(byteStream);
    NSParameterAssert(package);
    NSParameterAssert(relativePath);
    NSParameterAssert(relativePath.length);

	if (self = [super init]) {
		_byteStream = (ePub3::ByteStream *)byteStream;
		self.contentLength = _byteStream->BytesAvailable();
		self.delegate = delegate;
		self.package = package;
		self.relativePath = relativePath;

		if (!self.contentLength) {
			NSLog(@"The resource content length is zero! %@", self.relativePath);
		}
	}

	return self;
}

#pragma mark - Dealloc method

- (void)dealloc {
	[_delegate rdpackageResourceWillDeallocate:self];
}

#pragma mark - Public methods

- (NSData *)readDataOfLength:(NSUInteger)length {
	NSMutableData *md = [[NSMutableData alloc] initWithCapacity:length == 0 ? 1 : length];
	NSUInteger totalRead = 0;

	while (totalRead < length) {
		NSUInteger thisLength = MIN(sizeof(_buffer), length - totalRead);
		std::size_t count = _byteStream->ReadBytes(_buffer, thisLength);
		totalRead += count;
		[md appendBytes:_buffer length:count];
        
		if (count != thisLength) {
			NSLog(@"Did not read the expected number of bytes! (%lu %lu)", count, (unsigned long)thisLength);
			break;
		}
	}

	return md;
}

- (void)setOffset:(UInt64)offset {
	ePub3::SeekableByteStream* seekStream = dynamic_cast<ePub3::SeekableByteStream*>(_byteStream);
	ePub3::ByteStream::size_type pos = seekStream->Seek(offset, std::ios::beg);
    
	if (pos != offset) {
		NSLog(@"Setting the byte stream offset failed! pos = %lu, offset = %llu", pos, offset);
	}
}

- (void *)byteStream {
    return _byteStream;
}

#pragma mark - Property

- (NSData *)data {
	if (!_data) {
		NSMutableData *md = [[NSMutableData alloc] initWithCapacity:MAX(self.contentLength, 1)];

		while (YES) {
			std::size_t count = _byteStream->ReadBytes(_buffer, sizeof(_buffer));

			if (count == 0) {
				break;
			}

			[md appendBytes:_buffer length:count];
		}

		_data = [NSData dataWithData:md];
	}

	return _data;
}

@end
