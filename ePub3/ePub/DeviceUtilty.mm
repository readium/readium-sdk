//
//  DeviceUtilty.m
//  ePub3
//
//  Created by Zak Hsieh on 14/08/2017.
//  Copyright © 2017 The Readium Foundation and contributors. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import "DeviceUtility.h"

bool IsiPad() {
    return (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad);
}
