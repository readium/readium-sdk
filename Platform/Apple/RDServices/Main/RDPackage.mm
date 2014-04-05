//
//  RDPackage.mm
//  RDServices
//
//  Created by Shane Meyer on 2/4/13.
//  Copyright (c) 2012-2013 The Readium Foundation.
//

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
	@private ePub3::Package *m_package;
	@private std::vector<std::shared_ptr<ePub3::SpineItem>> m_spineItemVector;
}

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


- (id)initWithPackage:(void *)package {
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


- (RDNavigationElement *)pageList {
	if (m_navElemPageList == nil) {
		ePub3::NavigationTable *navTable = m_package->PageList().get();
		m_navElemPageList = [[RDNavigationElement alloc]
			initWithNavigationElement:navTable
			sourceHref:[self sourceHrefForNavigationTable:navTable]];
	}

	return m_navElemPageList;
}


- (void)rdpackageResourceWillDeallocate:(RDPackageResource *)packageResource {
	for (auto i = m_byteStreamVector.begin(); i != m_byteStreamVector.end(); i++) {
		if (i->get() == packageResource.byteStream) {
			m_byteStreamVector.erase(i);
			return;
		}
	}

	NSLog(@"The byte stream was not found!");
}


- (NSString *)renditionLayout {
	ePub3::PropertyPtr prop = m_package->PropertyMatching("layout", "rendition");
	return (prop == nullptr) ? @"" : [NSString stringWithUTF8String:prop->Value().c_str()];
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
