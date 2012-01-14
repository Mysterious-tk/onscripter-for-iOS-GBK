//
//  onsIphoneSupport.h
//  SDLiPhoneOS
//
//  Created by w s on 11-8-6.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface onsIphoneSupport : NSObject{
    NSString* documetPath;
    NSDictionary* dict;
}

-(void) oisGuessSystem;
-(float) getInfo:(const char*) info orgV: (float) v;//the info to query and v is the default value;
-(void) lg:(const char*) message;
-(const char*)getOnsPath;
@end
