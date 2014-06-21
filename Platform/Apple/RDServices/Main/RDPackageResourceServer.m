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


@interface RDPackageResourceServer ()

@property (nonatomic, strong) HTTPServer *httpServer;
@property (nonatomic, strong) RDPackage *package;

@end

@implementation RDPackageResourceServer

#pragma mark - Global var
static id gResourceLock = nil;

#pragma mark - Init methods

+ (void)initialize {
	gResourceLock = [NSObject new];
}

- (instancetype)initWithPackage:(RDPackage *)package {
    NSParameterAssert(package);
    self = [super init];
	if (self) {
		self.package = package;
		self.httpServer = [HTTPServer new];

		if (!self.httpServer) {
			NSLog(@"The HTTP server is nil!");
			return nil;
		}

		self.httpServer.documentRoot = @"";
		[self.httpServer setConnectionClass:[RDPackageResourceConnection class]];

		NSError *error = nil;
		BOOL success = [self.httpServer start:&error];

		if (!success || error != nil) {
			if (error != nil) {
				NSLog(@"Could not start the HTTP server! %@", error);
			}

			return nil;
		}

		[RDPackageResourceConnection setPackage:package];
	}

	return self;
}

#pragma mark - Dealloc method

- (void)dealloc {
	[self.httpServer stop];
	[RDPackageResourceConnection setPackage:nil];
}


#pragma mark - Public methods

+ (id)resourceLock {
	return gResourceLock;
}

#pragma mark - Property

- (int)port {
	return self.httpServer.listeningPort;
}

@end
