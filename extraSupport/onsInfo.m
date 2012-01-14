//
//  OnsInfo.cpp
//  SDLiPhoneOS
//
//  Created by w s on 11-8-6.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#import "onsIphoneSupport.h"
#include "onsInfo.h"



void oisSubInit(){
 
    onsIphoneSupport* ois = [[onsIphoneSupport alloc]init];
    soi.screen_device_height = [ois getInfo:"SCREEN_DEVICE_HEIGHT" orgV:960];
    soi.screen_device_width = [ois getInfo:"SCREEN_DEVICE_WIDTH" orgV: 640];
    soi.screen_width = [ois getInfo:"WIDTH" orgV: 800];
    soi.ScaleFactor = [ois getInfo:"SCALE_FACTOR" orgV:2];
    soi.isIpad = [ois getInfo:"IS_IPAD" orgV:0];
    soi.enableScala = [ois getInfo:"ENABLESCALA" orgV:1];
    soi.path = [ois getOnsPath];
    [ois release];
}

int oisSupportInit(){
    oisSubInit();
    return 0;
}
