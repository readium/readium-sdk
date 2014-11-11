//
//  RDPackage.mm
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

#import "RDPackage.h"
#import <ePub3/media-overlays_smil_model.h>
#import <ePub3/nav_table.h>
#import <ePub3/package.h>
#import <ePub3/utilities/byte_stream.h>
#import "RDMediaOverlaysSmilModel.h"
#import "RDNavigationElement.h"
#import "RDPackageResource.h"
#import "RDSpineItem.h"


@interface RDPackage() <RDPackageResourceDelegate> {
	@private std::vector<std::unique_ptr<ePub3::ByteStream>> m_byteStreamVector;
	@private RDMediaOverlaysSmilModel *m_mediaOverlaysSmilModel;
	@private RDNavigationElement *m_navElemListOfFigures;
	@private RDNavigationElement *m_navElemListOfIllustrations;
	@private RDNavigationElement *m_navElemListOfTables;
	@private RDNavigationElement *m_navElemPageList;
	@private RDNavigationElement *m_navElemTableOfContents;
	@private ePub3::Package *m_package;
	@private NSString *m_packageUUID;
	@private NSMutableArray *m_spineItems;
	@private std::vector<std::shared_ptr<ePub3::SpineItem>> m_spineItemVector;
	@private NSMutableArray *m_subjects;
}

- (NSString *)findProperty:(NSString *)propName withOptionalPrefix:(NSString *)prefix;
- (NSString *)findProperty:(NSString *)propName withPrefix:(NSString *)prefix;

- (NSString *)sourceHrefForNavigationTable:(ePub3::NavigationTable *)navTable;

@end


@implementation RDPackage


@synthesize packageUUID = m_packageUUID;
@synthesize spineItems = m_spineItems;
@synthesize subjects = m_subjects;


- (NSString *)authors {
	const ePub3::string s = m_package->Authors();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSString *)basePath {
	const ePub3::string s = m_package->BasePath();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSString *)copyrightOwner {
	const ePub3::string s = m_package->CopyrightOwner();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSDictionary *)dictionary {
	NSMutableDictionary *dictRoot = [NSMutableDictionary dictionary];

	NSString *rootURL = (self.rootURL == nil ? @"" : self.rootURL);
	[dictRoot setObject:rootURL forKey:@"rootUrl"];

	[dictRoot setObject:self.mediaOverlaysSmilModel.dictionary forKey:@"media_overlay"];

	NSString *s = self.renditionLayout;
	if (s != nil) {
		[dictRoot setObject:s forKey:@"rendition_layout"];
	}

	NSString *ss = self.renditionFlow;
	if (ss != nil) {
		[dictRoot setObject:ss forKey:@"rendition_flow"];
	}

	NSString *sss = self.renditionSpread;
	if (sss != nil) {
		[dictRoot setObject:sss forKey:@"rendition_spread"];
	}
    
	NSString *ssss = self.renditionOrientation;
	if (ssss != nil) {
		[dictRoot setObject:ssss forKey:@"rendition_orientation"];
	}

	NSMutableDictionary *dictSpine = [NSMutableDictionary dictionary];
	[dictRoot setObject:dictSpine forKey:@"spine"];

	NSString *direction = @"default";
	ePub3::PageProgression pageProgression = m_package->PageProgressionDirection();

	if (pageProgression == ePub3::PageProgression::LeftToRight) {
		direction = @"ltr";
	}
	else if (pageProgression == ePub3::PageProgression::RightToLeft) {
		direction = @"rtl";
	}

	[dictSpine setObject:direction forKey:@"direction"];

	NSMutableArray *items = [NSMutableArray arrayWithCapacity:m_spineItems.count];
	[dictSpine setObject:items forKey:@"items"];

	for (RDSpineItem *spineItem in self.spineItems) {
		[items addObject:spineItem.dictionary];
	}

	return dictRoot;
}


- (NSString *)fullTitle {
	const ePub3::string s = m_package->FullTitle();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (instancetype)initWithPackage:(void *)package {
	if (package == nil) {
		return nil;
	}

	if (self = [super init]) {
		m_package = (ePub3::Package *)package;

		// Package ID.

		CFUUIDRef uuid = CFUUIDCreate(NULL);
		m_packageUUID = CFBridgingRelease(CFUUIDCreateString(NULL, uuid));
		CFRelease(uuid);

		// Spine items.

		std::shared_ptr<ePub3::SpineItem> firstSpineItem = m_package->FirstSpineItem();
		size_t count = (firstSpineItem == NULL) ? 0 : firstSpineItem->Count();
		m_spineItems = [[NSMutableArray alloc] initWithCapacity:(count == 0) ? 1 : count];

		for (size_t i = 0; i < count; i++) {
			std::shared_ptr<ePub3::SpineItem> spineItem = m_package->SpineItemAt(i);
			m_spineItemVector.push_back(spineItem);
			RDSpineItem *item = [[RDSpineItem alloc] initWithSpineItem:spineItem.get()];
			[m_spineItems addObject:item];
		}

		// Subjects.

		ePub3::Package::StringList vec = m_package->Subjects();
		m_subjects = [[NSMutableArray alloc] initWithCapacity:4];

		for (auto i = vec.begin(); i != vec.end(); i++) {
			ePub3::string s = *i;
			[m_subjects addObject:[NSString stringWithUTF8String:s.c_str()]];
		}
	}

	return self;
}


- (NSString *)isbn {
	const ePub3::string s = m_package->ISBN();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSString *)language {
	const ePub3::string s = m_package->Language();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (RDNavigationElement *)listOfFigures {
	if (m_navElemListOfFigures == nil) {
		ePub3::NavigationTable *navTable = m_package->ListOfFigures().get();
		m_navElemListOfFigures = [[RDNavigationElement alloc]
			initWithNavigationElement:navTable
			sourceHref:[self sourceHrefForNavigationTable:navTable]];
	}

	return m_navElemListOfFigures;
}


- (RDNavigationElement *)listOfIllustrations {
	if (m_navElemListOfIllustrations == nil) {
		ePub3::NavigationTable *navTable = m_package->ListOfIllustrations().get();
		m_navElemListOfIllustrations = [[RDNavigationElement alloc]
			initWithNavigationElement:navTable
			sourceHref:[self sourceHrefForNavigationTable:navTable]];
	}

	return m_navElemListOfIllustrations;
}


- (RDNavigationElement *)listOfTables {
	if (m_navElemListOfTables == nil) {
		ePub3::NavigationTable *navTable = m_package->ListOfTables().get();
		m_navElemListOfTables = [[RDNavigationElement alloc]
			initWithNavigationElement:navTable
			sourceHref:[self sourceHrefForNavigationTable:navTable]];
	}

	return m_navElemListOfTables;
}


- (RDMediaOverlaysSmilModel *)mediaOverlaysSmilModel {
	if (m_mediaOverlaysSmilModel == nil) {
		ePub3::MediaOverlaysSmilModel *smilModel = m_package->MediaOverlaysSmilModel().get();
		m_mediaOverlaysSmilModel = [[RDMediaOverlaysSmilModel alloc]
			initWithMediaOverlaysSmilModel:smilModel];
	}

	return m_mediaOverlaysSmilModel;
}


- (NSString *)modificationDateString {
	const ePub3::string s = m_package->ModificationDate();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSString *)packageID {
	const ePub3::string s = m_package->PackageID();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (void)packageResourceWillDeallocate:(RDPackageResource *)packageResource {
	for (auto i = m_byteStreamVector.begin(); i != m_byteStreamVector.end(); i++) {
		if (i->get() == packageResource.byteStream) {
			m_byteStreamVector.erase(i);
			return;
		}
	}

	NSLog(@"The byte stream was not found!");
}


- (RDNavigationElement *)pageList {
	if (m_navElemPageList == nil) {
		ePub3::NavigationTable *navTable = m_package->PageList().get();
		m_navElemPageList = [[RDNavigationElement alloc]
			initWithNavigationElement:navTable
			sourceHref:[self sourceHrefForNavigationTable:navTable]];
	}

	return m_navElemPageList;
}

- (NSString *)findProperty:(NSString *)propName withOptionalPrefix:(NSString *)prefix {
    NSString *value = [self findProperty:propName withPrefix:prefix];

    if (value.length == 0) {
        value = [self findProperty:propName withPrefix:@""];
    }

    return value;
}


- (NSString *)findProperty:(NSString *)propName withPrefix:(NSString *)prefix {
    auto prop = m_package->PropertyMatching([propName UTF8String], [prefix UTF8String]);

    if (prop != nullptr) {
        return [NSString stringWithUTF8String:prop->Value().c_str()];
    }

    return @"";
}

- (NSString *)renditionLayout {
    return [self findProperty:@"layout" withPrefix:@"rendition"];
}
- (NSString *)renditionFlow {
    return [self findProperty:@"flow" withPrefix:@"rendition"];
}
- (NSString *)renditionSpread {
    return [self findProperty:@"spread" withPrefix:@"rendition"];
}
- (NSString *)renditionOrientation {
    return [self findProperty:@"orientation" withPrefix:@"rendition"];
}


- (RDPackageResource *)resourceAtRelativePath:(NSString *)relativePath {
	if (relativePath == nil || relativePath.length == 0) {
		return nil;
	}

	NSRange range = [relativePath rangeOfString:@"#"];

	if (range.location != NSNotFound) {
		relativePath = [relativePath substringToIndex:range.location];
	}

	ePub3::string s = ePub3::string(relativePath.UTF8String);
	std::unique_ptr<ePub3::ByteStream> byteStream = m_package->ReadStreamForRelativePath(s);

	if (byteStream == nullptr) {
		NSLog(@"Relative path '%@' does not have a byte stream!", relativePath);
		return nil;
	}

	RDPackageResource *resource = [[RDPackageResource alloc]
		initWithDelegate:self
		byteStream:byteStream.get()
		package:self
		relativePath:relativePath];

	if (resource != nil) {
		m_byteStreamVector.push_back(std::move(byteStream));
		ePub3::ConstManifestItemPtr item = m_package->ManifestItemAtRelativePath(s);

		if (item) {
			const ePub3::ManifestItem::MimeType &mediaType = item->MediaType();
			resource.mimeType = [NSString stringWithUTF8String:mediaType.c_str()];
		}
	}

	return resource;
}


- (NSString *)source {
	const ePub3::string s = m_package->Source();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSString *)sourceHrefForNavigationTable:(ePub3::NavigationTable *)navTable {
	if (navTable == nil) {
		return nil;
	}

	const ePub3::string s = navTable->SourceHref();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSString *)subtitle {
	const ePub3::string s = m_package->Subtitle();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (RDNavigationElement *)tableOfContents {
	if (m_navElemTableOfContents == nil) {
		ePub3::NavigationTable *navTable = m_package->TableOfContents().get();
		m_navElemTableOfContents = [[RDNavigationElement alloc]
			initWithNavigationElement:navTable
			sourceHref:[self sourceHrefForNavigationTable:navTable]];
	}

	return m_navElemTableOfContents;
}


- (NSString *)title {
	const ePub3::string s = m_package->Title();
	return [NSString stringWithUTF8String:s.c_str()];
}


@end
