//
//  RDPackageResource.h
//  RDServices
//
//  Created by Shane Meyer on 2/28/13.
//  Copyright (c) 2012-2013 The Readium Foundation.
//

#import <Foundation/Foundation.h>

@class RDPackage;
@class RDPackageResource;

@protocol RDPackageResourceDelegate

- (void)rdpackageResourceWillDeallocate:(RDPackageResource *)packageResource;

@end

@interface RDPackageResource : NSObject {
	@private UInt8 m_buffer[4096];
	@private NSData *m_data;
	@private __weak id <RDPackageResourceDelegate> m_delegate;
	@private RDPackage *m_package;
	@private NSString *m_relativePath;
}

@property (nonatomic, readonly) void *byteStream;
@property (nonatomic, readonly) NSUInteger contentLength;
@property (nonatomic, readonly) NSData *data;
@property (nonatomic, readonly) RDPackage *package;

// The relative path associated with this resource.
@property (nonatomic, readonly) NSString *relativePath;

- (id)
	initWithDelegate:(id <RDPackageResourceDelegate>)delegate
	byteStream:(void *)byteStream
	package:(RDPackage *)package
	relativePath:(NSString *)relativePath;

- (NSData *)readDataOfLength:(NSUInteger)length;
- (void)setOffset:(UInt64)offset;

@end
