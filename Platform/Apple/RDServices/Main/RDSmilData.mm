//
//  RDSmilData.mm
//  RDServices
//
//  Created by Shane Meyer on 10/17/13.
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

#import "RDSmilData.h"
#import <ePub3/media-overlays_smil_data.h>


@interface RDSmilData () {
    @private ePub3::SMILData *_smilData;
}

@property (nonatomic, strong, readwrite) NSDictionary *bodyDictionary;
@property (nonatomic, strong, readwrite) NSDictionary *dictionary;
@property (nonatomic, assign, readwrite) NSTimeInterval duration;
@property (nonatomic, copy, readwrite) NSString *href;
@property (nonatomic, copy, readwrite) NSString *identifier;
@property (nonatomic, copy, readwrite) NSString *smilVersion;
@property (nonatomic, copy, readwrite) NSString *spineItemID;

@end


@implementation RDSmilData

#pragma mark - Init methods

- (instancetype)initWithSmilData:(void *)smilData {
    NSParameterAssert(smilData);
    self = [super init];
    if (self) {
        _smilData = (ePub3::SMILData *)smilData;
    }
    return self;
}

#pragma mark - Property

- (NSDictionary *)bodyDictionary {
    if (!_bodyDictionary) {
        const ePub3::SMILData::Sequence *sequence = _smilData->Body().get();
        _bodyDictionary = (sequence == nullptr)? @{} : [self parseTreeSequence:sequence];
    }

    return _bodyDictionary;
}


- (NSDictionary *)dictionary {
    if (!_dictionary) {
        _dictionary = @{
                        @"children" : @[self.bodyDictionary],
                        @"duration" : @(self.duration),
                        @"href" : self.href,
                        @"id" : self.identifier,
                        @"smilVersion" : self.smilVersion,
                        @"spineItemId" : self.spineItemID,
                    };
    }
    return _dictionary;
}

- (NSTimeInterval)duration {
    NSTimeInterval ms = _smilData->DurationMilliseconds_Metadata();
    _duration = ms / 1000.0;
    return _duration;
}


- (NSString *)href {
    if (!_href) {
        ePub3::ManifestItemPtr manifestItem = _smilData->SmilManifestItem();
        const ePub3::string s = manifestItem == nullptr ? "fake.smil" : manifestItem->Href();
        _href = [NSString stringWithUTF8String:s.c_str()];
    }
    return _href;
}

- (NSString *)identifier {
    if (!_identifier) {
        ePub3::ManifestItemPtr manifestItem = _smilData->SmilManifestItem();
        const ePub3::string s = manifestItem == nullptr ? "" : manifestItem->Identifier();
        _identifier = [NSString stringWithUTF8String:s.c_str()];
    }
    return _identifier;
}

- (NSString *)smilVersion {
    if (!_smilVersion) {
        _smilVersion = @"3.0";
    }
    return _smilVersion;
}

- (NSString *)spineItemID {
    if (!_spineItemID) {
        const ePub3::string s = _smilData->XhtmlSpineItem()->Idref();
        _spineItemID = [NSString stringWithUTF8String:s.c_str()];
    }
    return _spineItemID;
}

#pragma mark - Private methods

- (NSDictionary *)parseTreeAudio:(const ePub3::SMILData::Audio *)node {
    std::string src("");
    src.append(node->SrcFile().c_str());

    if (!node->SrcFragmentId().empty()) {
        src.append("#");
        src.append(node->SrcFragmentId().c_str());
    }

    return @{
        @"clipBegin" : [NSNumber numberWithDouble:node->ClipBeginMilliseconds() / 1000.0],
        @"clipEnd" : [NSNumber numberWithDouble:node->ClipEndMilliseconds() / 1000.0],
        @"nodeType" : [NSString stringWithUTF8String:node->Name().c_str()],
        @"src" : [NSString stringWithUTF8String:src.c_str()],
    };
}


- (NSDictionary *)parseTreeParallel:(ePub3::SMILData::Parallel *)node {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    NSMutableArray *children = [NSMutableArray array];

    dict[@"nodeType"] = [NSString stringWithUTF8String:node->Name().c_str()];
    dict[@"epubtype"] = [NSString stringWithUTF8String:node->Type().c_str()];

    auto textMedia = node->Text();

    if (textMedia != nullptr && textMedia->IsText()) {
        [children addObject:[self parseTreeText:textMedia.get()]];
    }

    auto audioMedia = node->Audio();

    if (audioMedia != nullptr && audioMedia->IsAudio()) {
        [children addObject:[self parseTreeAudio:audioMedia.get()]];
    }

    dict[@"children"] = children;
    return dict;
}


- (NSDictionary *)parseTreeSequence:(const ePub3::SMILData::Sequence *)node {
    std::string textref("");
    textref.append(node->TextRefFile().c_str());

    if (!node->TextRefFragmentId().empty()) {
        textref.append("#");
        textref.append(node->TextRefFragmentId().c_str());
    }

    NSMutableDictionary *dict = [NSMutableDictionary dictionary];

    dict[@"epubtype"] = [NSString stringWithUTF8String:node->Type().c_str()];
    dict[@"nodeType"] = [NSString stringWithUTF8String:node->Name().c_str()];
    dict[@"textref"] = [NSString stringWithUTF8String:textref.c_str()];

    NSMutableArray *children = [NSMutableArray array];
    auto count = node->GetChildrenCount();

    for (int i = 0; i < count; i++) {
        const ePub3::SMILData::TimeContainer *container = node->GetChild(i).get();

        if (container->IsSequence()) {
            [children addObject:[self parseTreeSequence:(ePub3::SMILData::Sequence *)container]];
        }
        else if (container->IsParallel()) {
            [children addObject:[self parseTreeParallel:(ePub3::SMILData::Parallel *)container]];
        }
        else {
            NSLog(@"The child is not a sequence or a parallel!");
        }
    }

    if (children.count > 0) {
        dict[@"children"] = children;
    }

    return dict;
}

- (NSDictionary *)parseTreeText:(const ePub3::SMILData::Text *)node {
    std::string src("");
    src.append(node->SrcFile().c_str());
    NSString *srcFragmentID = nil;

    if (!node->SrcFragmentId().empty()) {
        srcFragmentID = [NSString stringWithUTF8String:node->SrcFragmentId().c_str()];
        src.append("#");
        src.append(node->SrcFragmentId().c_str());
    }

    NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithDictionary:@{
        @"nodeType" : [NSString stringWithUTF8String:node->Name().c_str()],
        @"src" : [NSString stringWithUTF8String:src.c_str()],
        @"srcFile" : [NSString stringWithUTF8String:node->SrcFile().c_str()],
    }];

    if (srcFragmentID != nil) {
        dict[@"srcFragmentId"] = srcFragmentID;
    }

    return dict;
}

@end
