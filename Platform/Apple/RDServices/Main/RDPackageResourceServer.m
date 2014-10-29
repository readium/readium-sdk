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
#import "RDPackage.h"
#import "RDPackageResource.h"
#import "RDPackageResourceConnection.h"


@implementation RDJavascriptExecutor
-(void)executeJavascript:(NSString *)js {
    // noop
    NSLog(@"RDJavascriptExecutor executeJavascript NOOP %@", js);
}
@end


static id m_resourceLock = nil;


@implementation RDPackageResourceServer


- (void)dealloc {
	[m_httpServer stop];
	[RDPackageResourceConnection setPackage:nil javascriptExecutor:nil];
}


+ (void)initialize {
	m_resourceLock = [[NSObject alloc] init];
}


- (id)initWithPackage:(RDPackage *)package javascriptExecutor:(RDJavascriptExecutor*)javascriptExecutor {
	if (package == nil) {
		return nil;
	}

	if (self = [super init]) {
		m_package = package;
		m_httpServer = [[HTTPServer alloc] init];

		if (m_httpServer == nil) {
			NSLog(@"The HTTP server is nil!");
			return nil;
		}

		m_httpServer.documentRoot = @"";
		[m_httpServer setConnectionClass:[RDPackageResourceConnection class]];

		NSError *error = nil;
		BOOL success = [m_httpServer start:&error];

		if (!success || error != nil) {
			if (error != nil) {
				NSLog(@"Could not start the HTTP server! %@", error);
			}

			return nil;
		}

		[RDPackageResourceConnection setPackage:package javascriptExecutor:javascriptExecutor];
	}

	return self;
}


- (int)port {
	return m_httpServer.listeningPort;
}


+ (id)resourceLock {
	return m_resourceLock;
}


@end
