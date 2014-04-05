//
//  RDSmilData.h
//  RDServices
//
//  Created by Shane Meyer on 10/17/13.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface RDSmilData : NSObject {
	@private NSDictionary *m_bodyDictionary;
}

@property (nonatomic, readonly) NSDictionary *bodyDictionary;
@property (nonatomic, readonly) NSDictionary *dictionary;
@property (nonatomic, readonly) NSTimeInterval duration;
@property (nonatomic, readonly) NSString *href;
@property (nonatomic, readonly) NSString *identifier;
@property (nonatomic, readonly) NSString *smilVersion;
@property (nonatomic, readonly) NSString *spineItemID;

- (id)initWithSmilData:(void *)smilData;

@end
