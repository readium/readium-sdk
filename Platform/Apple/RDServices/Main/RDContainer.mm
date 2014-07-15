//
//  RDContainer.mm
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

#import <ePub3/container.h>
#import <ePub3/initialization.h>

#import "RDContainer.h"
#import "RDPackage.h"


@interface RDContainer() {
    @private std::shared_ptr<ePub3::Container> _container;
    @private ePub3::Container::PackageList _packageList;
}

@property (nonatomic, strong, readwrite) RDPackage *firstPackage;
@property (nonatomic, strong, readwrite) NSMutableArray *allPackages;
@property (nonatomic, copy, readwrite) NSString *path;

@end


@implementation RDContainer


#pragma mark - Init methods


+ (void)initialize {
    ePub3::InitializeSdk();
    ePub3::PopulateFilterManager();
}

- (instancetype)initWithPath:(NSString *)path {
    
    NSParameterAssert(path);
    NSParameterAssert([path length]);
    if (![[NSFileManager defaultManager] fileExistsAtPath:path]) {
        return nil;
    }

    if (self = [super init]) {
        self.path = path;
        _container = ePub3::Container::OpenContainer(path.UTF8String);
        
    }
    [self loadPackages];
    return self;
}

+ (instancetype)containerWithPath:(NSString *)path {
    return [[[self class] alloc] initWithPath:path];
}


#pragma mark - Property methods

- (RDPackage *)firstPackage {
    if (!_firstPackage){
        _firstPackage = [self.allPackages firstObject];
    }
    return _firstPackage;
}

- (NSArray *)packages {
    return self.allPackages;
}

- (NSMutableArray *)allPackages {
    if (!_allPackages) {
        _allPackages = [[NSMutableArray alloc] initWithCapacity:4];
    }
    return _allPackages;
}

#pragma mark - Private methods

- (void)loadPackages {
    _packageList = _container->Packages();
    for (auto i = _packageList.begin(); i != _packageList.end(); i++) {
        RDPackage *package = [[RDPackage alloc] initWithPackage:i->get()];
        [self.allPackages addObject:package];
    }
}

@end
