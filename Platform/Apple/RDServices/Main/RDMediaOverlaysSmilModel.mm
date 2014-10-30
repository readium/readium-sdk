//
//  RDMediaOverlaysSmilModel.mm
//  RDServices
//
//  Created by Shane Meyer on 10/17/13.
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

#import "RDMediaOverlaysSmilModel.h"
#import "RDSmilData.h"
#import <ePub3/media-overlays_smil_model.h>


@interface RDMediaOverlaysSmilModel () {
	@private NSArray *m_escapables;
	@private NSArray *m_skippables;
	@private NSArray *m_smilDatas;
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


- (instancetype)initWithMediaOverlaysSmilModel:(void *)smilModel {
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
