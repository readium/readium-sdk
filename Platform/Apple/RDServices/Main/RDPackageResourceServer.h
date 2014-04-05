//
//  RDPackageResourceServer.h
//  RDServices
//
//  Created by Shane Meyer on 2/28/13.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#import <Foundation/Foundation.h>

@class HTTPServer;
@class RDPackage;

@interface RDPackageResourceServer : NSObject {
	@private HTTPServer *m_httpServer;
	@private RDPackage *m_package;
}

@property (nonatomic, readonly) int port;

- (id)initWithPackage:(RDPackage *)package;
+ (id)resourceLock;

@end
