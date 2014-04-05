//
//  RDMediaOverlaysSmilModel.mm
//  RDServices
//
//  Created by Shane Meyer on 10/17/13.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#import "RDMediaOverlaysSmilModel.h"
#import "RDSmilData.h"
#import <ePub3/media-overlays_smil_model.h>


@interface RDMediaOverlaysSmilModel () {
	@private ePub3::MediaOverlaysSmilModel *m_smilModel;
}

@end


@implementation RDMediaOverlaysSmilModel


- (NSString *)activeClass {
	const ePub3::string s = m_smilModel->ActiveClass();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSDictionary *)dictionary {
	NSMutableArray *array = [NSMutableArray arrayWithCapacity:self.smilDatas.count];

	for (RDSmilData *smilData in self.smilDatas) {
		[array addObject:smilData.dictionary];
	}

	return @{
		@"activeClass" : self.activeClass,
		@"duration" : [NSNumber numberWithDouble:self.duration],
		@"escapables" : self.escapables,
		@"narrator" : self.narrator,
		@"playbackActiveClass" : self.playbackActiveClass,
		@"skippables" : self.skippables,
		@"smil_models" : array,
	};
}


- (NSTimeInterval)duration {
	NSTimeInterval ms = m_smilModel->DurationMilliseconds_Calculated();
	return ms / 1000.0;
}


- (NSArray *)escapables {
	if (m_escapables == nil) {
		std::vector<ePub3::string>::size_type count =
			ePub3::MediaOverlaysSmilModel::GetEscapablesCount();

		NSMutableArray *array = [[NSMutableArray alloc] initWithCapacity:count];
		m_escapables = array;

		for (int i = 0; i < count; i++) {
			ePub3::string s = ePub3::MediaOverlaysSmilModel::GetEscapable(i);
			[array addObject:[NSString stringWithUTF8String:s.c_str()]];
		}
	}

	return m_escapables;
}


- (id)initWithMediaOverlaysSmilModel:(void *)smilModel {
	if (smilModel == nil) {
		return nil;
	}

	if (self = [super init]) {
		m_smilModel = (ePub3::MediaOverlaysSmilModel *)smilModel;
	}

	return self;
}


- (NSString *)narrator {
	const ePub3::string s = m_smilModel->Narrator();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSString *)playbackActiveClass {
	const ePub3::string s = m_smilModel->PlaybackActiveClass();
	return [NSString stringWithUTF8String:s.c_str()];
}


- (NSArray *)skippables {
	if (m_skippables == nil) {
		std::vector<ePub3::string>::size_type count =
			ePub3::MediaOverlaysSmilModel::GetSkippablesCount();

		NSMutableArray *array = [[NSMutableArray alloc] initWithCapacity:count];
		m_skippables = array;

		for (int i = 0; i < count; i++) {
			ePub3::string s = ePub3::MediaOverlaysSmilModel::GetSkippable(i);
			[array addObject:[NSString stringWithUTF8String:s.c_str()]];
		}
	}

	return m_skippables;
}


- (NSArray *)smilDatas {
	if (m_smilDatas == nil) {
		std::vector<std::shared_ptr<ePub3::SMILData>>::size_type count = m_smilModel->GetSmilCount();
		NSMutableArray *array = [[NSMutableArray alloc] initWithCapacity:count];
		m_smilDatas = array;

		for (int i = 0; i < count; i++) {
			ePub3::SMILData *p = m_smilModel->GetSmil(i).get();
			RDSmilData *smilData = [[RDSmilData alloc] initWithSmilData:p];
			[array addObject:smilData];
		}
	}

	return m_smilDatas;
}


@end
