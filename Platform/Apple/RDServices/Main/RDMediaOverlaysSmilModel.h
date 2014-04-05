//
//  RDMediaOverlaysSmilModel.h
//  RDServices
//
//  Created by Shane Meyer on 10/17/13.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface RDMediaOverlaysSmilModel : NSObject {
	@private NSArray *m_escapables;
	@private NSArray *m_skippables;
	@private NSArray *m_smilDatas;
}

@property (nonatomic, readonly) NSString *activeClass;
@property (nonatomic, readonly) NSDictionary *dictionary;
@property (nonatomic, readonly) NSTimeInterval duration;
@property (nonatomic, readonly) NSArray *escapables;
@property (nonatomic, readonly) NSString *narrator;
@property (nonatomic, readonly) NSString *playbackActiveClass;
@property (nonatomic, readonly) NSArray *skippables;
@property (nonatomic, readonly) NSArray *smilDatas;

- (id)initWithMediaOverlaysSmilModel:(void *)smilModel;

@end
