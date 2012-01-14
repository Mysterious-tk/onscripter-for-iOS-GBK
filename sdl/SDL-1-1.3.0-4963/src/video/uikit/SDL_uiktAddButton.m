//
//  SDL_uiktAddButton.cpp
//  SDLiPhoneOS
//
//  Created by w s on 11-8-4.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

//#import "SDL_uikitkeyboard.h"
#include "SDL.h"
#import <CoreFoundation/CoreFoundation.h>
#import "SDL_uikitmoviedelegate.h"
extern SDL_Window* SDL_VideoWindow;
void SDL_addButtons(){
    // ButtonView
    int i;
    SDL_Window* window = SDL_VideoWindow;
    if (!window) {
        NSLog(@"window alloc error!");
        return ;
    }
}
/*
- (void)buttonAction:(UIButton*)button
{
	int i;
	
	for(i=0; i<_buttonNum; i++)
	{
		if(button == _buttons[i])
		{
			SDLKey key = _buttonMap[i];
			
			if([button isPressed])
			{
				[_screenView addEvent:
                 [NSDictionary dictionaryWithObjectsAndKeys:
                  @"keyDown", @"type",
                  [NSNumber numberWithInt:(int)key], @"key",
                  nil]
                 ];
			}
			else
			{
				[_screenView addEvent:
                 [NSDictionary dictionaryWithObjectsAndKeys:
                  @"keyUp", @"type",
                  [NSNumber numberWithInt:(int)key], @"key",
                  nil]
                 ];
			}
		}
	}
}*/
void SDL_releaseButtons(){
    int i;
    SDL_Window* window = SDL_VideoWindow;
    if (!window) {
        NSLog(@"window alloc error!");
        return ;
    }
    SDL_WindowData* sdlw=nil;
    UIWindow* tUiwindow = nil;
    sdlw=(SDL_WindowData*)(window->driverdata);
    tUiwindow=sdlw->uiwindow;
    
	if(sdlw->_buttonRects)
        free(sdlw->_buttonRects);
    if(sdlw->_buttonMap)
        free(sdlw->_buttonMap);
    if(sdlw->_buttons){
		for(i=0; i<sdlw->_buttonNum; i++)
		{
			[sdlw->_buttons[i] release];
		}
		free(sdlw->_buttons);
	}
    [sdlw->_buttonView release];
    
}