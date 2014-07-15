//
//  RDNavigationElement.mm
//  RDServices
//
//  Created by Shane Meyer on 4/18/13.
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

#import "RDNavigationElement.h"
#import <ePub3/nav_element.h>
#import <ePub3/nav_point.h>


@interface RDNavigationElement() {
	@private ePub3::NavigationElement *_element;
}

@property (nonatomic, strong, readwrite) NSArray *children;
@property (nonatomic, copy, readwrite) NSString *content;
@property (nonatomic, copy, readwrite) NSString *sourceHref;
@property (nonatomic, copy, readwrite) NSString *title;

@end


@implementation RDNavigationElement


#pragma mark - Init methods

- (instancetype)initWithNavigationElement:(void *)element sourceHref:(NSString *)sourceHref {
    NSParameterAssert(element);

    self = [super init];
	if (self) {
		_element = (ePub3::NavigationElement *)element;
		self.sourceHref = sourceHref;
	}
    
	return self;
}

#pragma mark - Property

- (NSArray *)children {
	if (!_children) {
		NSMutableArray *array = [NSMutableArray new];
        ePub3::NavigationList navigationList;
		navigationList = _element->Children();

		for (auto i = navigationList.begin(); i != navigationList.end(); i++) {
			RDNavigationElement *element = [[RDNavigationElement alloc]
				initWithNavigationElement:i->get() sourceHref:self.sourceHref];
			[array addObject:element];
		}
        _children = [NSArray arrayWithArray:array];
	}

	return _children;
}


- (NSString *)content {
    if (!_content) {
        ePub3::NavigationPoint *point = dynamic_cast<ePub3::NavigationPoint *>(_element);
        if (!point) {
            const ePub3::string s = point->Content();
            _content = [NSString stringWithUTF8String:s.c_str()];
        }
    }
    return _content;
}

- (NSString *)title {
    if (!_title) {
        const ePub3::string s = _element->Title();
        _title = [NSString stringWithUTF8String:s.c_str()];
    }
    return _title;
}


@end
