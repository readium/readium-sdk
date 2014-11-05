//
//  RDPackageResourceResponse.m
//  RDServices
//
//  Created by Shane Meyer on 3/15/14.
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

#import "RDPackageResourceResponse.h"
#import "RDPackageResource.h"
#import "RDPackageResourceServer.h"


@interface RDPackageResourceResponse () {
	@private UInt64 m_offset;
	@private RDPackageResource *m_resource;
}

@end


@implementation RDPackageResourceResponse


- (UInt64)contentLength {
	return m_resource.contentLength;
}


- (NSDictionary *)httpHeaders {
	NSString *contentType = self->m_resource.mimeType;

	if (contentType) {
		return @{@"Content-Type": contentType};
	}
	else {
		return @{};
	}
}


- (instancetype)initWithResource:(RDPackageResource *)resource {
	if (resource == nil) {
		return nil;
	}

	if (self = [super init]) {
		m_resource = resource;
	}

	return self;
}


- (BOOL)isDone {
	return m_offset == m_resource.contentLength;
}


- (UInt64)offset {
	return m_offset;
}


- (NSData *)readDataOfLength:(NSUInteger)length {
	NSData *data = nil;

	@synchronized ([RDPackageResourceServer resourceLock]) {
        [m_resource setOffset:m_offset]; // ensure resource has up-to-date offset before reading
		data = [m_resource readDataOfLength:length];
	}

	if (data != nil) {
		m_offset += data.length;
	}

	return data;
}


- (void)setOffset:(UInt64)offset {
	m_offset = offset;

	@synchronized ([RDPackageResourceServer resourceLock]) {
		[m_resource setOffset:offset];
	}
}


@end
