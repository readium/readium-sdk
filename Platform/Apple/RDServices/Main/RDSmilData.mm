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
	@private NSDictionary *m_bodyDictionary;
	@private ePub3::SMILData *m_smilData;
}

- (NSDictionary *)parseTreeAudio:(const ePub3::SMILData::Audio *)node;
- (NSDictionary *)parseTreeParallel:(ePub3::SMILData::Parallel *)node;
- (NSDictionary *)parseTreeSequence:(const ePub3::SMILData::Sequence *)node;
- (NSDictionary *)parseTreeText:(const ePub3::SMILData::Text *)node;

@end


@implementation RDSmilData


- (NSDictionary *)bodyDictionary {
	if (m_bodyDictionary == nil) {
		const ePub3::SMILData::Sequence *sequence = m_smilData->Body().get();

		if (sequence == nullptr) {
			m_bodyDictionary = [[NSDictionary alloc] init];
		}
		else {
			m_bodyDictionary = [self parseTreeSequence:sequence];
		}
	}

	return m_bodyDictionary;
}


- (NSDictionary *)dictionary {
	return @{
		@"children" : @[ self.bodyDictionary ],
		@"duration" : [NSNumber numberWithDouble:self.duration],
		@"href" : self.href,
		@"id" : self.identifier,
		@"smilVersion" : self.smilVersion,
		@"spineItemId" : self.spineItemID,
	};
}


- (NSTimeInterval)duration {
	NSTimeInterval ms = m_smilData->DurationMilliseconds_Metadata();
	return ms / 1000.0;
}


- (NSString *)href {
	ePub3::ManifestItemPtr manifestItem = m_smilData->SmilManifestItem();
	const ePub3::string s = manifestItem == nullptr ? "fake.smil" : manifestItem->Href();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSString *)identifier {
	ePub3::ManifestItemPtr manifestItem = m_smilData->SmilManifestItem();
	const ePub3::string s = manifestItem == nullptr ? "" : manifestItem->Identifier();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (instancetype)initWithSmilData:(void *)smilData {
	if (smilData == nil) {
		return nil;
	}

	if (self = [super init]) {
		m_smilData = (ePub3::SMILData *)smilData;
	}

	return self;
}


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


- (NSString *)smilVersion {
	return @"3.0";
}


- (NSString *)spineItemID {
	const ePub3::string s = m_smilData->XhtmlSpineItem()->Idref();
	return [NSString stringWithUTF8String:s.c_str()];
}


@end
