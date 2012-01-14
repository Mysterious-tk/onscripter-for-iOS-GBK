//
//  SDL_uikitButton.h
//  SDLiPhoneOS
//
//  Created by w s on 11-8-4.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "SDL.h"

@interface SDL_uikitButton : NSObject
-(id)addButton:(SDL_Window*) window;
-(void)ycbuttonAction:(id)sender;
@end
