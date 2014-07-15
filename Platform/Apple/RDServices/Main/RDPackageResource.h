//
//  RDPackageResource.h
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
@class RDPackageResource;

@protocol RDPackageResourceDelegate

- (void)rdpackageResourceWillDeallocate:(RDPackageResource *)packageResource;

@end


@interface RDPackageResource : NSObject

- (instancetype)initWithDelegate:(id <RDPackageResourceDelegate>)delegate
                      byteStream:(void *)byteStream
                         package:(RDPackage *)package
                    relativePath:(NSString *)relativePath;

@property (nonatomic, assign, readonly) id<RDPackageResourceDelegate> delegate;
@property (nonatomic, assign, readonly) NSUInteger contentLength;
@property (nonatomic, copy) NSString *mimeType;
@property (nonatomic, strong, readonly) RDPackage *package;
@property (nonatomic, strong, readonly) NSData *data;

// The relative path associated with this resource.
@property (nonatomic, copy, readonly) NSString *relativePath;

- (NSData *)readDataOfLength:(NSUInteger)length;
- (void)setOffset:(UInt64)offset;
- (void *)byteStream;

@end
