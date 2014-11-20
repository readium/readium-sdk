//
//  RDPackage.h
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

@class RDMediaOverlaysSmilModel;
@class RDNavigationElement;
@class RDPackageResource;

@interface RDPackage : NSObject

@property (nonatomic, readonly) NSString *authors;
@property (nonatomic, readonly) NSString *basePath;
@property (nonatomic, readonly) NSString *copyrightOwner;
@property (nonatomic, readonly) NSDictionary *dictionary;
@property (nonatomic, readonly) NSString *fullTitle;
@property (nonatomic, readonly) NSString *isbn;
@property (nonatomic, readonly) NSString *language;
@property (nonatomic, readonly) RDNavigationElement *listOfFigures;
@property (nonatomic, readonly) RDNavigationElement *listOfIllustrations;
@property (nonatomic, readonly) RDNavigationElement *listOfTables;
@property (nonatomic, readonly) RDMediaOverlaysSmilModel *mediaOverlaysSmilModel;
@property (nonatomic, readonly) NSString *modificationDateString;
@property (nonatomic, readonly) NSString *packageID;
@property (nonatomic, readonly) NSString *packageUUID;
@property (nonatomic, readonly) RDNavigationElement *pageList;

@property (nonatomic, readonly) NSString *renditionLayout;
@property (nonatomic, readonly) NSString *renditionFlow;
@property (nonatomic, readonly) NSString *renditionSpread;
@property (nonatomic, readonly) NSString *renditionOrientation;

@property (nonatomic, strong) NSString *rootURL;
@property (nonatomic, readonly) NSString *source;
@property (nonatomic, readonly) NSArray *spineItems;
@property (nonatomic, readonly) NSArray *subjects;
@property (nonatomic, readonly) NSString *subtitle;
@property (nonatomic, readonly) RDNavigationElement *tableOfContents;
@property (nonatomic, readonly) NSString *title;

- (instancetype)initWithPackage:(void *)package;

// Returns the resource at the given relative path or nil if it doesn't exist.
- (RDPackageResource *)resourceAtRelativePath:(NSString *)relativePath;

// Gets the current Byte Stream and returns the proper Byte Stream for the case.
// There can be three possible byte streams:
// - A simple ZipFileByteStream when no ContentFilter objects apply for this resource.
// - A FilterChainByteStreamRange when a Byte Range request has been made, and only one ContentFilter object applies.
// - A FilterChainByteStream when it is not a Byte Range request or more than one ContentFilter applies.
- (void *)getProperByteStream:(NSString *)relativePath currentByteStream:(void *)currentByteStream isRangeRequest:(BOOL)isRangeRequest;

@end
