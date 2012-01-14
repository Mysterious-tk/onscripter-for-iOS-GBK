//
//  onsInfo.h
//  SDLiPhoneOS
//
//  Created by w s on 11-8-6.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#ifndef SDLiPhoneOS_onsInfo_h
#define SDLiPhoneOS_onsInfo_h
struct onsInfo{
    int screen_device_width;
    int screen_device_height;
    int screen_width;
    float ScaleFactor;
    int isIpad;
    int useESC;
    int enableScala;
    const char* path;
};

extern struct onsInfo soi;
int  oisSupportInit();

 

#endif
