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

#import "RDPackageResource.h"

@class RDContainer;
@class RDMediaOverlaysSmilModel;
@class RDNavigationElement;

@interface RDPackage : NSObject <RDPackageResourceDelegate>

- (instancetype)initWithPackage:(void *)package;

@property (nonatomic, copy, readonly)   NSString *authors;
@property (nonatomic, copy, readonly)   NSString *basePath;
@property (nonatomic, copy, readonly)   NSString *copyrightOwner;
@property (nonatomic, copy, readonly)   NSString *fullTitle;
@property (nonatomic, copy, readonly)   NSString *isbn;
@property (nonatomic, copy, readonly)   NSString *language;
@property (nonatomic, copy, readonly)   NSString *modificationDateString;
@property (nonatomic, copy, readonly)   NSString *packageID;
@property (nonatomic, copy, readonly)   NSString *packageUUID;
@property (nonatomic, copy, readonly)   NSString *renditionLayout;
@property (nonatomic, copy)             NSString *rootURL;
@property (nonatomic, copy, readonly)   NSString *source;
@property (nonatomic, copy, readonly)   NSString *subtitle;
@property (nonatomic, copy, readonly)   NSString *title;

@property (nonatomic, strong, readonly) NSDictionary *dictionary;
@property (nonatomic, strong, readonly) RDNavigationElement *listOfFigures;
@property (nonatomic, strong, readonly) RDNavigationElement *listOfIllustrations;
@property (nonatomic, strong, readonly) RDNavigationElement *listOfTables;
@property (nonatomic, strong, readonly) RDMediaOverlaysSmilModel *mediaOverlaysSmilModel;
@property (nonatomic, strong, readonly) RDNavigationElement *pageList;
@property (nonatomic, strong, readonly) NSArray *spineItems;
@property (nonatomic, strong, readonly) NSArray *subjects;
@property (nonatomic, strong, readonly) RDNavigationElement *tableOfContents;

// Returns the resource at the given relative path or nil if it doesn't exist.
- (RDPackageResource *)resourceAtRelativePath:(NSString *)relativePath;

@end
