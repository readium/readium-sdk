//
//  RDSpineItem.mm
//  RDServices
//
//  Created by Shane Meyer on 2/4/13.
//  Copyright (c) 2012-2013 The Readium Foundation.
//

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
