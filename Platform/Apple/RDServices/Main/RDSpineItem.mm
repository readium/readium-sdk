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


@interface RDSpineItem() {
	@private ePub3::SpineItem *m_spineItem;
}

@end


@implementation RDSpineItem


@synthesize renditionLayout = m_renditionLayout;
@synthesize mediaOverlayId = m_mediaOverlayId;
@synthesize mediaType = m_mediaType;


- (NSString *)baseHref {
	std::shared_ptr<ePub3::ManifestItem> manifestItem = m_spineItem->ManifestItem();

	if (manifestItem != NULL) {
		const ePub3::string s = manifestItem->BaseHref();
		return [NSString stringWithUTF8String:s.c_str()];
	}

	return nil;
}


- (NSDictionary *)dictionary {
	NSMutableDictionary *dict = [NSMutableDictionary dictionary];

	NSString *s = self.baseHref;

	if (s != nil) {
		[dict setObject:s forKey:@"href"];
	}

	s = self.idref;

	if (s != nil) {
		[dict setObject:s forKey:@"idref"];
	}

	s = self.pageSpread;

	if (s != nil) {
		[dict setObject:s forKey:@"page_spread"];
	}

	if (self.renditionLayout != nil) {
		[dict setObject:self.renditionLayout forKey:@"rendition_layout"];
	}

	if (self.mediaOverlayId != nil) {
		[dict setObject:self.mediaOverlayId forKey:@"media_overlay_id"];
	}

	if (self.mediaType != nil) {
		[dict setObject:self.mediaType forKey:@"media_type"];
	}

	return dict;
}


- (NSString *)idref {
	const ePub3::string s = m_spineItem->Idref();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (id)initWithSpineItem:(void *)spineItem {
	if (spineItem == nil) {
		return nil;
	}

	if (self = [super init]) {
		m_spineItem = (ePub3::SpineItem *)spineItem;
		ePub3::PropertyPtr prop = m_spineItem->PropertyMatching("layout", "rendition");

		if (prop == nullptr) {
			m_renditionLayout = @"";
		}
		else {
			m_renditionLayout = [[NSString alloc] initWithUTF8String:prop->Value().c_str()];
		}

		auto mediaOverlayID = m_spineItem->ManifestItem()->MediaOverlayID();
		m_mediaOverlayId = [[NSString alloc] initWithUTF8String: mediaOverlayID.c_str()];

		auto mediaType = m_spineItem->ManifestItem()->MediaType();
		m_mediaType = [[NSString alloc] initWithUTF8String:mediaType.c_str()];
	}

	return self;
}


- (NSString *)pageSpread  {
	if (m_spineItem->Spread() == ePub3::PageSpread::Left) {
		return @"page-spread-left";
	}

	if (m_spineItem->Spread() == ePub3::PageSpread::Right) {
		return @"page-spread-right";
	}

	return @"";
}


@end
