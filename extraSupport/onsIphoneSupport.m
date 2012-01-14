//
//  onsIphoneSupport.m
//  SDLiPhoneOS
//
//  Created by w s on 11-8-6.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#import "onsIphoneSupport.h"
#import "onsInfo.h"


const char* OnsPath;
@implementation onsIphoneSupport

- (id)init
{
    self = [super init];
    if (self) {
        NSArray* tpaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSAllDomainsMask, YES);
        documetPath = [tpaths objectAtIndex:0];
        NSMutableString* onsTPath = [[NSMutableString alloc] initWithString:documetPath];
        [onsTPath appendString:@"/onsPlist.plist"];
        dict = [[NSDictionary alloc] initWithContentsOfFile:[[NSBundle mainBundle]pathForResource:@"onsPlist" ofType:@"plist"]];
        NSLog([[NSBundle mainBundle]pathForResource:@"onsPlist" ofType:@"plist"]);
        
    }
    
    return self;
}
-(void) oisGuessSystem{
    

}
-(const char*)getOnsPath{
    return [documetPath UTF8String];
}

-(void) lg:(const char*) message{
    NSString* ns = [[NSString alloc]initWithUTF8String:message];
    NSLog(ns);
}


-(float) getInfo:(const char* )info orgV: (float) v{
    float oldV = v;

    if(dict == nil){
        return oldV;
    }
    NSString* infoStr = [[NSString alloc]initWithUTF8String:info];
    NSString* ret = [dict objectForKey:infoStr];
    if(ret == nil){
     
        return oldV;
    }
    v = [ret floatValue];
    
    return v;
}
-(void)dealloc{
    if(dict!=nil)
        [dict release];
    
}
@end
