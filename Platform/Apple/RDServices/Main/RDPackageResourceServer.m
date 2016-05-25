//
//  RDPackageResourceServer.m
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

#import "RDPackageResourceServer.h"
#import "HTTPServer.h"
#import "RDPackageResourceConnection.h"


static id m_resourceLock = nil;


@interface RDPackageResourceServer () {
	@private __weak id <RDPackageResourceServerDelegate> m_delegate;
	@private HTTPServer *m_httpServer;
	@private RDPackage *m_package;
	@private NSData *m_specialPayloadAnnotationsCSS;
	@private NSData *m_specialPayloadMathJaxJS;
    @private UInt16 m_serverPort;
}

@end


@implementation RDPackageResourceServer


@synthesize package = m_package;
@synthesize specialPayloadAnnotationsCSS = m_specialPayloadAnnotationsCSS;
@synthesize specialPayloadMathJaxJS = m_specialPayloadMathJaxJS;


- (void)dealloc {
	[m_httpServer stop];
}


- (void)executeJavaScript:(NSString *)javaScript {
	[m_delegate packageResourceServer:self executeJavaScript:javaScript];
}


+ (void)initialize {
	m_resourceLock = [[NSObject alloc] init];
}


- (instancetype)
	initWithDelegate:(id <RDPackageResourceServerDelegate>)delegate
	package:(RDPackage *)package
	specialPayloadAnnotationsCSS:(NSData *)specialPayloadAnnotationsCSS
	specialPayloadMathJaxJS:(NSData *)specialPayloadMathJaxJS
{
	if (delegate == nil || package == nil) {
		return nil;
	}

	if (self = [super init]) {
		m_delegate = delegate;
		m_httpServer = [[HTTPServer alloc] init];
		m_package = package;
		m_specialPayloadAnnotationsCSS = specialPayloadAnnotationsCSS;
		m_specialPayloadMathJaxJS = specialPayloadMathJaxJS;

		if (m_httpServer == nil) {
			NSLog(@"The HTTP server is nil!");
			return nil;
		}

		m_httpServer.documentRoot = @"";
		[m_httpServer setConnectionClass:[RDPackageResourceConnection class]];

        BOOL success = [self startHTTPServer];
        if (!success) {
            return nil;
        }

		[RDPackageResourceConnection setPackageResourceServer:self];
	}

	return self;
}


- (int)port {
	return m_httpServer.listeningPort;
}


+ (id)resourceLock {
	return m_resourceLock;
}

- (BOOL)startHTTPServer {
    if (m_serverPort > 0) {
        [m_httpServer setPort:m_serverPort];
    }
    
    NSError *error = nil;
    BOOL success = [m_httpServer start:&error];
    if (!success || error != nil) {
        if (error != nil) {
            NSLog(@"Could not start the HTTP server! %@", error);
        }
        return NO;
    }
    
    if (m_serverPort == 0) {
        m_serverPort = [m_httpServer listeningPort];
    }
    
    return success;
}

- (void)stopHTTPServer {
    [m_httpServer stop];
}

@end
