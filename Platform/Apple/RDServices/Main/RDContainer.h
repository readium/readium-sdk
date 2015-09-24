//
//  RDContainer.h
//  RDServices
//
//  Created by Shane Meyer on 2/4/13.
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

@class RDContainer;

@protocol RDContainerDelegate <NSObject>

- (BOOL)container:(RDContainer *)container handleSdkError:(NSString *)message isSevereEpubError:(BOOL)isSevereEpubError;

@optional

/**
 * Called just after the container populated the default filters into
 * the filter manager.
 * You can implement this to register custom filters.
 */
- (void)containerRegisterFilters:(RDContainer *)container;

@end

@class RDPackage;

@interface RDContainer : NSObject

@property (nonatomic, readonly) RDPackage *firstPackage;
@property (nonatomic, readonly) NSArray *packages;
@property (nonatomic, readonly) NSString *path;

- (instancetype)initWithDelegate:(id <RDContainerDelegate>)delegate path:(NSString *)path;

@end
