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
#import "NSDate+RDDateAsString.h"


NSString * const kCacheControlHTTPHeader = @"no-transform,public,max-age=3000,s-maxage=9000";


@interface RDPackageResourceResponse () {
	@private UInt64 m_offset;
	@private UInt64 m_offsetInitial;
	@private  BOOL m_isRangeRequest;
	@private RDPackageResource *m_resource;
}

@end


@implementation RDPackageResourceResponse


- (UInt64)contentLength {
	return m_resource.contentLength;
}


- (NSDictionary *)httpHeaders {
	if(m_resource.relativePath) {
		NSString* ext = [[m_resource.relativePath pathExtension] lowercaseString];
		if([ext isEqualToString:@"xhtml"] || [ext isEqualToString:@"html"]) {
			return [NSDictionary dictionaryWithObject:@"application/xhtml+xml" forKey:@"Content-Type"]; // FORCE
		}
		else if([ext isEqualToString:@"xml"]) {
			return [NSDictionary dictionaryWithObject:@"application/xml" forKey:@"Content-Type"]; // FORCE
		}
	}

	NSString *contentType = self->m_resource.mimeType;
    
    // Add cache-control, expires and last-modified HTTP headers so the webview caches shared assets
    NSDate *now = [NSDate date];
    NSString *nowStr = [now dateAsString];
    NSString *expStr = [[now dateByAddingTimeInterval:60*60*24*10] dateAsString];
    if (contentType) {
        return @{@"Content-Type": contentType, @"Cache-Control": kCacheControlHTTPHeader, @"Last-Modified": nowStr, @"Expires": expStr};
    }
    else {
        return @{@"Cache-Control": kCacheControlHTTPHeader, @"Last-Modified": nowStr, @"Expires": expStr};
    }
}


- (instancetype)initWithResource:(RDPackageResource *)resource {
	if (resource == nil) {
		return nil;
	}

	if (self = [super init]) {
		m_resource = resource;
		m_isRangeRequest = NO;
	}

	return self;
}


- (BOOL)isDone {
	bool isDone = !m_isRangeRequest
		? (m_offset >= m_resource.contentLength)
		: (m_offset >= (m_offsetInitial + m_resource.contentLengthCheck));
	return isDone;
}


- (UInt64)offset {
	return m_offset;
}

//- (BOOL)isChunked
//{
//    return YES; // we do not know the content length in advance
//}

- (NSData *)readDataOfLength:(NSUInteger)length {
	NSData *data = nil;
	
	@synchronized ([RDPackageResourceServer resourceLock]) {
		data = [m_resource readDataOfLength:length offset:m_offset isRangeRequest:m_isRangeRequest];
	}
	
	if (data != nil) {
		m_offset += data.length;
	}

	if (data == nil || data.length == 0)
	{
		printf("readDataOfLength NO DATA  %s (%d)\n", [m_resource.relativePath UTF8String], length);
	}

	if (data == nil)
	{
		data = [NSData data];
	}

	return data;
}


- (void)setOffset:(UInt64)offset {
	m_offset = offset;
	m_offsetInitial = offset;
	m_isRangeRequest = YES;
}


@end
