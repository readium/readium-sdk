//
//  RDContainer.mm
//  RDServices
//
//  Created by Shane Meyer on 2/4/13.
//  Copyright (c) 2012-2013 The Readium Foundation.
//

#import "RDContainer.h"
#import <ePub3/container.h>
#import <ePub3/initialization.h>
#import "RDPackage.h"


@interface RDContainer() {
	@private std::shared_ptr<ePub3::Container> m_container;
	@private ePub3::Container::PackageList m_packageList;
}

@end


@implementation RDContainer


@synthesize packages = m_packages;
@synthesize path = m_path;


- (RDPackage *)firstPackage {
	return m_packages.count == 0 ? nil : [m_packages objectAtIndex:0];
}


+ (void)initialize {
	ePub3::InitializeSdk();
	ePub3::PopulateFilterManager();
}


- (id)initWithPath:(NSString *)path {
	if (path == nil || ![[NSFileManager defaultManager] fileExistsAtPath:path]) {
		return nil;
	}

	if (self = [super init]) {
		m_path = path;
		m_container = ePub3::Container::OpenContainer(path.UTF8String);

		if (m_container == nullptr) {
			return nil;
		}

		m_packageList = m_container->Packages();
		m_packages = [[NSMutableArray alloc] initWithCapacity:4];

		for (auto i = m_packageList.begin(); i != m_packageList.end(); i++) {
			RDPackage *package = [[RDPackage alloc] initWithPackage:i->get()];
			[m_packages addObject:package];
		}
	}

	return self;
}


@end
