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
