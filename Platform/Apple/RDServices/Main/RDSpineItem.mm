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
	@private NSString *m_baseHref;
	@private NSDictionary *m_dict;
	@private NSString *m_idref;
	@private NSString *m_linear;
	@private NSString *m_mediaOverlayId;
	@private NSString *m_mediaType;
	@private NSString *m_pageSpread;
	
	@private NSString *m_renditionFlow;
	@private NSString *m_renditionOrientation;
	@private NSString *m_renditionLayout;
	@private NSString *m_renditionSpread;

	@private ePub3::SpineItem *m_spineItem;
}

- (NSString *)findProperty:(NSString *)propName withOptionalPrefix:(NSString *)prefix;
- (NSString *)findProperty:(NSString *)propName withPrefix:(NSString *)prefix;

@end


@implementation RDSpineItem


@synthesize baseHref = m_baseHref;
@synthesize idref = m_idref;
@synthesize linear = m_linear;
@synthesize mediaOverlayId = m_mediaOverlayId;
@synthesize mediaType = m_mediaType;
@synthesize pageSpread = m_pageSpread;
@synthesize renditionFlow = m_renditionFlow;
@synthesize renditionOrientation = m_renditionOrientation;
@synthesize renditionLayout = m_renditionLayout;
@synthesize renditionSpread = m_renditionSpread;


- (NSDictionary *)dictionary {
	if (m_dict == nil) {
		NSMutableDictionary *dict = [[NSMutableDictionary alloc] init];

		if (self.baseHref != nil) {
			[dict setObject:self.baseHref forKey:@"href"];
		}

		if (self.idref != nil) {
			[dict setObject:self.idref forKey:@"idref"];
		}

		if (self.linear!= nil) {
			[dict setObject:self.linear forKey:@"linear"];
		}

		if (self.mediaOverlayId != nil) {
			[dict setObject:self.mediaOverlayId forKey:@"media_overlay_id"];
		}

		if (self.mediaType != nil) {
			[dict setObject:self.mediaType forKey:@"media_type"];
		}

		if (self.pageSpread != nil) {
			[dict setObject:self.pageSpread forKey:@"page_spread"];
		}

		if (self.renditionFlow != nil) {
			[dict setObject:self.renditionFlow forKey:@"rendition_flow"];
		}

		if (self.renditionOrientation != nil) {
			[dict setObject:self.renditionOrientation forKey:@"rendition_orientation"];
		}

		if (self.renditionLayout != nil) {
			[dict setObject:self.renditionLayout forKey:@"rendition_layout"];
		}

		if (self.renditionSpread != nil) {
			[dict setObject:self.renditionSpread forKey:@"rendition_spread"];
		}

		m_dict = dict;
	}

	return m_dict;
}


- (NSString *)findProperty:(NSString *)propName withOptionalPrefix:(NSString *)prefix {
	NSString *value = [self findProperty:propName withPrefix:prefix];

	if (value.length == 0) {
		value = [self findProperty:propName withPrefix:@""];
	}

	return value;
}


- (NSString *)findProperty:(NSString *)propName withPrefix:(NSString *)prefix {
	auto prop = m_spineItem->PropertyMatching([propName UTF8String], [prefix UTF8String], false);

	if (prop != nullptr) {
		return [NSString stringWithUTF8String:prop->Value().c_str()];
	}

	return @"";
}


- (instancetype)initWithSpineItem:(void *)spineItem {
	if (spineItem == nil) {
		return nil;
	}

	if (self = [super init]) {
		m_spineItem = (ePub3::SpineItem *)spineItem;
		std::shared_ptr<ePub3::ManifestItem> manifestItem = m_spineItem->ManifestItem();

		if (manifestItem == nullptr) {
			return nil;
		}

		bool l = m_spineItem->Linear();
		m_linear = l ? @"yes" : @"no";

		m_baseHref = [[NSString alloc] initWithUTF8String:manifestItem->BaseHref().c_str()];
		m_idref = [[NSString alloc] initWithUTF8String:m_spineItem->Idref().c_str()];
		m_mediaOverlayId = [[NSString alloc] initWithUTF8String:manifestItem->MediaOverlayID().c_str()];
		m_mediaType = [[NSString alloc] initWithUTF8String:manifestItem->MediaType().c_str()];
		m_pageSpread = [self findProperty:@"page-spread" withOptionalPrefix:@"rendition"];

		m_renditionFlow = [self findProperty:@"flow" withPrefix:@"rendition"];
		m_renditionOrientation = [self findProperty:@"orientation" withPrefix:@"rendition"];
		m_renditionLayout = [self findProperty:@"layout" withPrefix:@"rendition"];
		m_renditionSpread = [self findProperty:@"spread" withPrefix:@"rendition"];
	}

	return self;
}


@end
