//
//  RDPackageResourceServer.h
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

#import <Foundation/Foundation.h>

@class RDPackage;
@class RDPackageResourceServer;

@protocol RDPackageResourceServerDelegate <NSObject>

/**
 This function is called by a RDPackageResourceServer instance
 (which corresponding reference is passed as a parameter)
 to request the execution of the given Javascript
 (which is passed as a string parameter).
 
 The RDPackageResourceServer is agnostic to the actual mechanism used to run such code,
 thus why this functionality is delegated to an object that implements the RDPackageResourceServerDelegate protocol
 (which is passed as a parameter for the "initWithDelegate" function of the RDPackageResourceServer interface).
 For example, this delegate object may be an application-level class
 that utilizes a particular "Web View" browser component.
 
 There is no guarantee that this function gets signaled on the main thread,
 for example it may be invoked from background threads such as those allocated by HTTP server sockets.
 Do not block code execution for long periods of time, as this may result in HTTP timeouts.
 Make sure to execute the Javascript as soon as possible, as timing may be critical
 (e.g. JS instructions meant to be performed immediately after an incoming HTTP request)
 
 @param packageResourceServer a reference to the object instance that calls this function
 @param javaScript the code to execute
 @result N/A - the caller assumes that the Javascript was executed successfully (or is about to be)
 */
- (void)
	packageResourceServer:(RDPackageResourceServer *)packageResourceServer
	executeJavaScript:(NSString *)javaScript;

@end

@interface RDPackageResourceServer : NSObject

@property (nonatomic, readonly) RDPackage *package;
@property (nonatomic, readonly) int port;
@property (nonatomic, readonly) NSData *specialPayloadAnnotationsCSS;
@property (nonatomic, readonly) NSData *specialPayloadMathJaxJS;

- (BOOL)startHTTPServer;
- (void)stopHTTPServer;

- (void)executeJavaScript:(NSString *)javaScript;

- (instancetype)
	initWithDelegate:(id <RDPackageResourceServerDelegate>)delegate
	package:(RDPackage *)package
	specialPayloadAnnotationsCSS:(NSData *)specialPayloadAnnotationsCSS
	specialPayloadMathJaxJS:(NSData *)specialPayloadMathJaxJS;

+ (id)resourceLock;

@end
