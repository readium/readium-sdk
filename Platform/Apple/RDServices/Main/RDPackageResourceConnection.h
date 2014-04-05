//
//  RDPackageResourceConnection.h
//  RDServices
//
//  Created by Shane Meyer on 11/23/13.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#import "HTTPConnection.h"

@class RDPackage;

@interface RDPackageResourceConnection : HTTPConnection

+ (void)setPackage:(RDPackage *)package;

@end
