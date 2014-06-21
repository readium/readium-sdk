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
	@private ePub3::MediaOverlaysSmilModel *_smilModel;
}

@property (nonatomic, copy, readwrite) NSString *activeClass;
@property (nonatomic, copy, readwrite) NSString *narrator;
@property (nonatomic, copy, readwrite) NSString *playbackActiveClass;

@property (nonatomic, strong, readwrite) NSDictionary *dictionary;

@property (nonatomic, strong, readwrite) NSMutableArray *allEscapables;
@property (nonatomic, strong, readwrite) NSArray *escapables;

@property (nonatomic, strong, readwrite) NSArray *skippables;
@property (nonatomic, strong, readwrite) NSMutableArray *allSkippables;

@property (nonatomic, strong, readwrite) NSArray *smilDatas;
@property (nonatomic, strong, readwrite) NSMutableArray *allSmilDatas;

@property (nonatomic, assign, readwrite) NSTimeInterval duration;

@end


@implementation RDMediaOverlaysSmilModel

#pragma mark - Init methods

- (instancetype)initWithMediaOverlaysSmilModel:(void *)smilModel {
    NSParameterAssert(smilModel);
    self = [super init];
	if (self) {
		_smilModel = (ePub3::MediaOverlaysSmilModel *)smilModel;
	}

	return self;
}

#pragma mark - Property

- (NSMutableArray *)allEscapables {
	if (!_allEscapables) {
		std::vector<ePub3::string>::size_type count =
        ePub3::MediaOverlaysSmilModel::GetEscapablesCount();
        
		_allEscapables = [[NSMutableArray alloc] initWithCapacity:count];
        
		for (int i = 0; i < count; i++) {
			ePub3::string s = ePub3::MediaOverlaysSmilModel::GetEscapable(i);
			[_allEscapables addObject:[NSString stringWithUTF8String:s.c_str()]];
		}
	}

	return _allEscapables;
}

- (NSArray *)escapables {
    return self.allEscapables;
}

- (NSMutableArray *)allSkippables {
	if (!_allSkippables) {
		std::vector<ePub3::string>::size_type count =
        ePub3::MediaOverlaysSmilModel::GetSkippablesCount();

		_allSkippables = [[NSMutableArray alloc] initWithCapacity:count];
		for (int i = 0; i < count; i++) {
			ePub3::string s = ePub3::MediaOverlaysSmilModel::GetSkippable(i);
			[_allSkippables addObject:[NSString stringWithUTF8String:s.c_str()]];
		}
	}
    
	return _allSkippables;
}

- (NSArray *)skippables {
    return self.allSkippables;
}

- (NSMutableArray *)allSmilDatas {
	if (!_allSmilDatas) {
		std::vector<std::shared_ptr<ePub3::SMILData>>::size_type count = _smilModel->GetSmilCount();
		_allSmilDatas = [[NSMutableArray alloc] initWithCapacity:count];

		for (int i = 0; i < count; i++) {
			ePub3::SMILData *p = _smilModel->GetSmil(i).get();
			RDSmilData *smilData = [[RDSmilData alloc] initWithSmilData:p];
			[_allSmilDatas addObject:smilData];
		}
	}
    
	return _allSmilDatas;
}

- (NSArray *)smilDatas {
    return self.allSmilDatas;
}

- (NSString *)narrator {
    if (!_narrator) {
        const ePub3::string s = _smilModel->Narrator();
        _narrator = [NSString stringWithUTF8String:s.c_str()];
    }
    return _narrator;
}

- (NSString *)playbackActiveClass {
    if (!_playbackActiveClass) {
        const ePub3::string s = _smilModel->PlaybackActiveClass();
        _playbackActiveClass = [NSString stringWithUTF8String:s.c_str()];
    }
    return _playbackActiveClass;
}

- (NSString *)activeClass {
    if (!_activeClass) {
        const ePub3::string s = _smilModel->ActiveClass();
        _activeClass = [NSString stringWithUTF8String:s.c_str()];
    }
    return _activeClass;
}

- (NSDictionary *)dictionary {
    if (!_dictionary) {
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:self.smilDatas.count];
        
        for (RDSmilData *smilData in self.smilDatas) {
            [array addObject:smilData.dictionary];
        }
        
        _dictionary =  @{
                         @"activeClass" : self.activeClass,
                         @"duration" : @(self.duration),
                         @"escapables" : self.escapables,
                         @"narrator" : self.narrator,
                         @"playbackActiveClass" : self.playbackActiveClass,
                         @"skippables" : self.skippables,
                         @"smil_models" : array,
                        };
    }

    return _dictionary;
}

- (NSTimeInterval)duration {
	NSTimeInterval ms = _smilModel->DurationMilliseconds_Calculated();
	_duration = ms / 1000.0;
    return _duration;
}

@end
