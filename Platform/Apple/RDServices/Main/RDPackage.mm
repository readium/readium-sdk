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


#import <ePub3/media-overlays_smil_model.h>
#import <ePub3/nav_table.h>
#import <ePub3/package.h>
#import <ePub3/utilities/byte_stream.h>

#import "RDPackage.h"
#import "RDMediaOverlaysSmilModel.h"
#import "RDNavigationElement.h"
#import "RDSpineItem.h"


@interface RDPackage() {
	@private std::vector<std::unique_ptr<ePub3::ByteStream>> _byteStreamVector;
	@private ePub3::Package *_package;
	@private std::vector<std::shared_ptr<ePub3::SpineItem>> _spineItemVector;
}

@property (nonatomic, copy, readwrite)   NSString *authors;
@property (nonatomic, copy, readwrite)   NSString *basePath;
@property (nonatomic, copy, readwrite)   NSString *copyrightOwner;
@property (nonatomic, copy, readwrite)   NSString *fullTitle;
@property (nonatomic, copy, readwrite)   NSString *isbn;
@property (nonatomic, copy, readwrite)   NSString *language;
@property (nonatomic, copy, readwrite)   NSString *modificationDateString;
@property (nonatomic, copy, readwrite)   NSString *packageID;
@property (nonatomic, copy, readwrite)   NSString *packageUUID;
@property (nonatomic, copy, readwrite)   NSString *renditionLayout;
@property (nonatomic, copy, readwrite)   NSString *source;
@property (nonatomic, copy, readwrite)   NSString *subtitle;
@property (nonatomic, copy, readwrite)   NSString *title;

@property (nonatomic, strong, readwrite) NSDictionary *dictionary;
@property (nonatomic, strong, readwrite) RDNavigationElement *listOfFigures;
@property (nonatomic, strong, readwrite) RDNavigationElement *listOfIllustrations;
@property (nonatomic, strong, readwrite) RDNavigationElement *listOfTables;
@property (nonatomic, strong, readwrite) RDMediaOverlaysSmilModel *mediaOverlaysSmilModel;
@property (nonatomic, strong, readwrite) RDNavigationElement *pageList;
@property (nonatomic, strong, readwrite) NSMutableArray *allSpineItems;
@property (nonatomic, strong, readwrite) NSMutableArray *allSubjects;
@property (nonatomic, strong, readwrite) RDNavigationElement *tableOfContents;

- (NSString *)sourceHrefForNavigationTable:(ePub3::NavigationTable *)navTable;

@end


@implementation RDPackage

- (instancetype)initWithPackage:(void *)package {
    NSParameterAssert(package);

    self = [super init];
	if (self) {
		_package = (ePub3::Package *)package;

		// Package ID.
		CFUUIDRef uuid = CFUUIDCreate(NULL);
		self.packageUUID = CFBridgingRelease(CFUUIDCreateString(NULL, uuid));
		CFRelease(uuid);
	}

	return self;
}


#pragma mark - RDPackageResourceDelegate methods

- (void)rdpackageResourceWillDeallocate:(RDPackageResource *)packageResource {
	for (auto i = _byteStreamVector.begin(); i != _byteStreamVector.end(); i++) {
		if (i->get() == packageResource.byteStream) {
			_byteStreamVector.erase(i);
			return;
		}
	}

	NSLog(@"The byte stream was not found!");
}

#pragma mark - Public methods

- (RDPackageResource *)resourceAtRelativePath:(NSString *)relativePath {
	if (relativePath == nil || relativePath.length == 0) {
		return nil;
	}

	NSRange range = [relativePath rangeOfString:@"#"];

	if (range.location != NSNotFound) {
		relativePath = [relativePath substringToIndex:range.location];
	}

	ePub3::string s = ePub3::string(relativePath.UTF8String);
	std::unique_ptr<ePub3::ByteStream> byteStream = _package->ReadStreamForRelativePath(s);

	if (byteStream == nullptr) {
		NSLog(@"Relative path '%@' does not have a byte stream!", relativePath);
		return nil;
	}

	RDPackageResource *resource = [[RDPackageResource alloc] initWithDelegate:self
                                                                   byteStream:byteStream.get()
                                                                      package:self
                                                                 relativePath:relativePath];

	if (resource) {
		_byteStreamVector.push_back(std::move(byteStream));
		ePub3::ConstManifestItemPtr item = _package->ManifestItemAtRelativePath(s);
		if (item) {
			const ePub3::ManifestItem::MimeType &mediaType = item->MediaType();
			resource.mimeType = [NSString stringWithUTF8String:mediaType.c_str()];
		}
	}

	return resource;
}

#pragma mark - Property

- (NSMutableArray *)allSpineItems {
    if (!_allSpineItems) {
		std::shared_ptr<ePub3::SpineItem> firstSpineItem = _package->FirstSpineItem();
		size_t count = (firstSpineItem == NULL) ? 0 : firstSpineItem->Count();
		_allSpineItems = [[NSMutableArray alloc] initWithCapacity:(count == 0) ? 1 : count];

		for (size_t i = 0; i < count; i++) {
			std::shared_ptr<ePub3::SpineItem> spineItem = _package->SpineItemAt(i);
			_spineItemVector.push_back(spineItem);
			RDSpineItem *item = [[RDSpineItem alloc] initWithSpineItem:spineItem.get()];
			[_allSpineItems addObject:item];
		}
    }
    return _allSpineItems;
}

- (NSMutableArray *)allSubjects {
    if (!_allSubjects) {
		ePub3::Package::StringList vec = _package->Subjects();
		_allSubjects = [[NSMutableArray alloc] initWithCapacity:4];

		for (auto i = vec.begin(); i != vec.end(); i++) {
			ePub3::string s = *i;
			[_allSubjects addObject:[NSString stringWithUTF8String:s.c_str()]];
		}
    }
    return _allSubjects;
}

- (NSArray *)spineItems {
    return self.allSpineItems;
}

- (NSArray *)subjects {
    return self.allSubjects;
}

- (NSString *)authors {
    if (!_authors) {
        const ePub3::string s = _package->Authors();
        _authors = [NSString stringWithUTF8String:s.c_str()];
    }
    return _authors;
}

- (NSString *)basePath {
    if (!_basePath) {
        const ePub3::string s = _package->BasePath();
        _basePath = [NSString stringWithUTF8String:s.c_str()];
    }
    return _basePath;
}

- (NSString *)copyrightOwner {
    if (!_copyrightOwner) {
        const ePub3::string s = _package->CopyrightOwner();
        _copyrightOwner = [NSString stringWithUTF8String:s.c_str()];
    }
    return _copyrightOwner;
}

- (NSDictionary *)dictionary {
    if (!_dictionary) {
        NSMutableDictionary *dictRoot = [NSMutableDictionary new];
        
        NSString *rootURL = (self.rootURL == nil ? @"" : self.rootURL);
        dictRoot[@"rootUrl"] = rootURL;
        dictRoot[@"media_overlay"] = self.mediaOverlaysSmilModel.dictionary;
        
        NSString *s = self.renditionLayout;

        if (s) {
            [dictRoot setObject:s forKey:@"rendition_layout"];
            dictRoot[@"rendition_layout"] = s;
        }

        NSMutableDictionary *dictSpine = [NSMutableDictionary dictionary];
        dictRoot[@"spine"] = dictSpine;
        
        NSString *direction = @"default";
        ePub3::PageProgression pageProgression = _package->PageProgressionDirection();

        if (pageProgression == ePub3::PageProgression::LeftToRight) {
            direction = @"ltr";
        }
        else if (pageProgression == ePub3::PageProgression::RightToLeft) {
            direction = @"rtl";
        }

        dictSpine[@"direction"] = direction;
        
        NSMutableArray *items = [NSMutableArray arrayWithCapacity:[self.spineItems count]];
        dictSpine[@"items"] = items;

        for (RDSpineItem *spineItem in self.spineItems) {
            [items addObject:spineItem.dictionary];
        }
        
        _dictionary = [NSDictionary dictionaryWithDictionary:dictRoot];
    }
    
	return _dictionary;
}

- (NSString *)fullTitle {
    if (!_fullTitle) {
        const ePub3::string s = _package->FullTitle();
        _fullTitle = [NSString stringWithUTF8String:s.c_str()];
    }
    return _fullTitle;
}

- (NSString *)isbn {
    if (!_isbn) {
        const ePub3::string s = _package->ISBN();
       _isbn = [NSString stringWithUTF8String:s.c_str()];
    }
    return _isbn;
}

- (NSString *)language {
    if (!_language) {
        const ePub3::string s = _package->Language();
        _language =  [NSString stringWithUTF8String:s.c_str()];
    }
    return _language;
}

- (RDNavigationElement *)listOfFigures {
	if (!_listOfFigures) {
		ePub3::NavigationTable *navTable = _package->ListOfFigures().get();
        if (navTable) {
            _listOfFigures = [[RDNavigationElement alloc] initWithNavigationElement:navTable
                                                                         sourceHref:[self sourceHrefForNavigationTable:navTable]];
        }
	}

	return _listOfFigures;
}

- (RDNavigationElement *)listOfIllustrations {
	if (!_listOfIllustrations) {
		ePub3::NavigationTable *navTable = _package->ListOfIllustrations().get();
        if (navTable) {
            _listOfIllustrations = [[RDNavigationElement alloc] initWithNavigationElement:navTable
                                                                               sourceHref:[self sourceHrefForNavigationTable:navTable]];
        }
	}
    
	return _listOfIllustrations;
}

- (RDNavigationElement *)listOfTables {
	if (!_listOfTables) {
		ePub3::NavigationTable *navTable = _package->ListOfTables().get();
        if (navTable) {
            _listOfTables = [[RDNavigationElement alloc] initWithNavigationElement:navTable
                                                                        sourceHref:[self sourceHrefForNavigationTable:navTable]];
        }
	}
    
	return _listOfTables;
}

- (RDMediaOverlaysSmilModel *)mediaOverlaysSmilModel {
	if (!_mediaOverlaysSmilModel) {
		ePub3::MediaOverlaysSmilModel *smilModel = _package->MediaOverlaysSmilModel().get();
		_mediaOverlaysSmilModel = [[RDMediaOverlaysSmilModel alloc] initWithMediaOverlaysSmilModel:smilModel];
	}
    
	return _mediaOverlaysSmilModel;
}

- (NSString *)modificationDateString {
    if (!_modificationDateString) {
        const ePub3::string s = _package->ModificationDate();
        _modificationDateString = [NSString stringWithUTF8String:s.c_str()];
    }
    return _modificationDateString;
}

- (NSString *)packageID {
    if (!_packageID) {
        const ePub3::string s = _package->PackageID();
        _packageID = [NSString stringWithUTF8String:s.c_str()];
    }
    return _packageID;
}


- (RDNavigationElement *)pageList {
	if (!_pageList) {
		ePub3::NavigationTable *navTable = _package->PageList().get();
		_pageList = [[RDNavigationElement alloc] initWithNavigationElement:navTable
                                                                sourceHref:[self sourceHrefForNavigationTable:navTable]];
	}
    
	return _pageList;
}

- (NSString *)renditionLayout {
    if (!_renditionLayout) {
        ePub3::PropertyPtr prop = _package->PropertyMatching("layout", "rendition");
        _renditionLayout = (prop == nullptr) ? @"" : [NSString stringWithUTF8String:prop->Value().c_str()];
    }
    return _renditionLayout;
}

- (NSString *)source {
    if (!_source) {
        const ePub3::string s = _package->Source();
        _source = [NSString stringWithUTF8String:s.c_str()];
    }
    return _source;
}

- (NSString *)subtitle {
    if (!_subtitle) {
        const ePub3::string s = _package->Subtitle();
        _subtitle = [NSString stringWithUTF8String:s.c_str()];
    }
    return _subtitle;
}


- (RDNavigationElement *)tableOfContents {
	if (!_tableOfContents) {
		ePub3::NavigationTable *navTable = _package->TableOfContents().get();
		_tableOfContents = [[RDNavigationElement alloc] initWithNavigationElement:navTable
                                                                       sourceHref:[self sourceHrefForNavigationTable:navTable]];
	}

	return _tableOfContents;
}

- (NSString *)title {
    if (!_title) {
        const ePub3::string s = _package->Title();
        _title = [NSString stringWithUTF8String:s.c_str()];
    }
    return _title;
}

#pragma mark - Private methods

- (NSString *)sourceHrefForNavigationTable:(ePub3::NavigationTable *)navTable {
	if (!navTable) {
		return nil;
	}

	const ePub3::string s = navTable->SourceHref();
	return [NSString stringWithUTF8String:s.c_str()];
}

@end
