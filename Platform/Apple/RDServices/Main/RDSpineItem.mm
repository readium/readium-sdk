//
//  RDSpineItem.mm
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

#import "RDSpineItem.h"
#import <ePub3/manifest.h>
#import <ePub3/spine.h>


@interface RDSpineItem () {
	@private ePub3::SpineItem *_spineItem;
}

@property (nonatomic, strong, readwrite) NSDictionary *dictionary;

@property (nonatomic, copy, readwrite) NSString *baseHref;
@property (nonatomic, copy, readwrite) NSString *idref;
@property (nonatomic, copy, readwrite) NSString *mediaOverlayId;
@property (nonatomic, copy, readwrite) NSString *mediaType;
@property (nonatomic, copy, readwrite) NSString *pageSpread;
@property (nonatomic, copy, readwrite) NSString *renditionFlow;
@property (nonatomic, copy, readwrite) NSString *renditionLayout;
@property (nonatomic, copy, readwrite) NSString *renditionSpread;

@end


@implementation RDSpineItem

#pragma mark - Init methods

- (instancetype)initWithSpineItem:(void *)spineItem {
    NSParameterAssert(spineItem);

    self = [super init];
	if (self) {
		_spineItem = (ePub3::SpineItem *)spineItem;
		std::shared_ptr<ePub3::ManifestItem> manifestItem = _spineItem->ManifestItem();
        
		if (manifestItem == nullptr) {
			return nil;
		}

		self.baseHref = [[NSString alloc] initWithUTF8String:manifestItem->BaseHref().c_str()];
		self.idref = [[NSString alloc] initWithUTF8String:_spineItem->Idref().c_str()];
		self.mediaOverlayId = [[NSString alloc] initWithUTF8String:manifestItem->MediaOverlayID().c_str()];
		self.mediaType = [[NSString alloc] initWithUTF8String:manifestItem->MediaType().c_str()];
		self.pageSpread = [self findProperty:@"page-spread" withOptionalPrefix:@"rendition"];
		self.renditionFlow = [self findProperty:@"flow" withPrefix:@"rendition"];
		self.renditionLayout = [self findProperty:@"layout" withPrefix:@"rendition"];
		self.renditionSpread = [self findProperty:@"spread" withPrefix:@"rendition"];
	}
    
	return self;
}

#pragma mark - Property

- (NSDictionary *)dictionary {
	if (!_dictionary) {
		NSMutableDictionary *dict = [NSMutableDictionary new];

		if (self.baseHref) {
			dict[@"href"] = self.baseHref;
		}

		if (self.idref) {
			dict[@"idref"] = self.idref;
		}

		if (self.mediaOverlayId) {
            dict[@"media_overlay_id"] = self.mediaOverlayId;
		}

		if (self.mediaType) {
			dict[@"media_type"] = self.mediaType;
		}

		if (self.pageSpread) {
			dict[@"page_spread"] = self.pageSpread;
		}

		if (self.renditionFlow) {
			dict[@"rendition_flow"] = self.renditionFlow;
		}

		if (self.renditionLayout) {
			dict[@"rendition_layout"] = self.renditionLayout;
		}

		if (self.renditionSpread) {
			dict[@"rendition_spread"] = self.renditionSpread;
		}
		_dictionary = [NSDictionary dictionaryWithDictionary:dict];
	}

	return _dictionary;
}

#pragma mark - Private methods

- (NSString *)findProperty:(NSString *)propName withOptionalPrefix:(NSString *)prefix {
	NSString *value = [self findProperty:propName withPrefix:prefix];

	if (!value.length) {
		value = [self findProperty:propName withPrefix:@""];
	}

	return value;
}


- (NSString *)findProperty:(NSString *)propName withPrefix:(NSString *)prefix {
	auto prop = _spineItem->PropertyMatching([propName UTF8String], [prefix UTF8String]);

	if (prop != nullptr) {
		return [NSString stringWithUTF8String:prop->Value().c_str()];
	}

	return @"";
}

@end
