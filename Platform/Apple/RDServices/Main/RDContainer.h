//
//  RDContainer.h
//  RDServices
//
//  Created by Shane Meyer on 2/4/13.
//  Copyright (c) 2012-2013 The Readium Foundation.
//

#import <Foundation/Foundation.h>

@class RDPackage;

@interface RDContainer : NSObject {
	@private NSMutableArray *m_packages;
	@private NSString *m_path;
}

@property (nonatomic, readonly) RDPackage *firstPackage;
@property (nonatomic, readonly) NSArray *packages;
@property (nonatomic, readonly) NSString *path;

- (id)initWithPath:(NSString *)path;

@end
