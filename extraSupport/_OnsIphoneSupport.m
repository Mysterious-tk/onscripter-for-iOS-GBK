//
//  OnsIphoneSupport.cpp
//  ___PROJECTNAME___
//
//  Created by w s on 11-7-30.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "OnsIphoneSupport.h"
#import <Foundation/Foundation.h>


void OnsIphoneSupport::getOnsPath(){
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSAllDomainsMask, YES);
    NSString* documetPath = [paths objectAtIndex:0];
    for(int i = 0; i < paths.count;i++)
        lg([[paths objectAtIndex:i] UTF8String]);
    OnsPath = [documetPath UTF8String];
     
}
void OnsIphoneSupport::lg(const char* message){
    NSString* ns = [[NSString alloc]initWithUTF8String:message];
    NSLog(ns);
    [ns release];
}
int OnsIphoneSupport::getHeight(){
    int h = 320;
    NSMutableDictionary* dict = [[NSMutableDictionary alloc]initWithContentsOfFile:@"/onsPlist.plist"];
    if(dict == nil){
        return h;
    }
    NSString* height = [dict objectForKey:@"WIDTH"];
    if(height == nil){
        [dict release];
        return 320;
    }
    [dict release];
    h =  [height intValue];
    [height release];
    return h;
}
int OnsIphoneSupport::getWidth(){
    int w = 480;
    getOnsPath();
    NSMutableString* paths = [[NSMutableString alloc]initWithUTF8String:OnsPath];
    [paths appendString:@"/onsPlist.plist"];
    NSLog(paths);
    NSMutableDictionary* dict = [[NSMutableDictionary alloc]initWithContentsOfFile:paths];
    if(dict == nil){
        return w;
    }
    NSString* width = [dict objectForKey:@"WIDTH"];
    NSLog(width);
    if(width == nil){
        [dict release];
        return 480;
    }
  
    w =  [width intValue];
    [dict release];

    return w;
}
int OnsIphoneSupport::getInfo(const char* info,int v){
    int oldV = v;
    NSMutableString* paths;
    NSMutableDictionary* dict;
    NSString* infoStr;
    getOnsPath();
    paths = [[NSMutableString alloc]initWithUTF8String:OnsPath];
    [paths appendString:@"/onsPlist.plist"];
    NSLog(paths);
    dict = [[NSMutableDictionary alloc]initWithContentsOfFile:paths];
    if(dict == nil){
        return oldV;
    }
    infoStr = [[NSString alloc]initWithUTF8String:info];
    NSString* ret = [dict objectForKey:infoStr];
    if(ret == nil){
        [dict release];
        return oldV;
    }
    v = [ret intValue];
    
    return v;
}
