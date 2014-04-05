//
//  RDSpineItem.h
//  RDServices
//
//  Created by Shane Meyer on 2/4/13.
//  Copyright (c) 2012-2013 The Readium Foundation.
//

#import <Foundation/Foundation.h>

@interface RDSpineItem : NSObject {
	@private NSString *m_renditionLayout;
	@private NSString *m_mediaOverlayId;
}

@property (nonatomic, readonly) NSString *baseHref;
@property (nonatomic, readonly) NSDictionary *dictionary;
@property (nonatomic, readonly) NSString *idref;
@property (nonatomic, readonly) NSString *pageSpread;
@property (nonatomic, readonly) NSString *renditionLayout;
@property (nonatomic, readonly) NSString *mediaOverlayId;

- (id)initWithSpineItem:(void *)spineItem;

@end
