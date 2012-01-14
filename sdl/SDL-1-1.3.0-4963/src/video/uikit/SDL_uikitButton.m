//
//  SDL_uikitButton.m
//  SDLiPhoneOS
//
//  Created by w s on 11-8-4.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#import "SDL_uikitButton.h"
#include "SDL_compat.h"
#include "SDL_sysvideo.h"
#include "SDL.h"
#include "onsInfo.h"

#include "SDL_uikitwindow.h"
//#include <CGGeometry.h>
//CGGEOMETRY_H_
#include <CoreFoundation/CoreFoundation.h>

@implementation SDL_uikitButton

extern SDL_Window *SDL_VideoWindow;

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}
- (id)addButton:(SDL_Window*) window{
    int i;
    SDL_WindowData* sdlw;
    UIWindow* tUiwindow ;
    sdlw=(SDL_WindowData*)(window->driverdata);
   
    
    sdlw->_buttonNum = 5;
    sdlw->_buttonMap = (SDLKey*)SDL_malloc(sizeof(SDLKey) * sdlw->_buttonNum);
    sdlw->_buttonRects = (CGRect*)SDL_malloc(sizeof(CGRect) * sdlw->_buttonNum);
    
    
    for(i=0; i<sdlw->_buttonNum; i++)
    {
        sdlw->_buttonMap[i] = SDLK_UNKNOWN;
    }
    
    
	int button_layout = 1;
	
	CGRect buttonViewFrame;
    int _buttonWidth=400,_buttonHeight=100;
    
	if(button_layout){
		buttonViewFrame.size.width  = _buttonWidth;
		buttonViewFrame.size.height = _buttonHeight * sdlw->_buttonNum;
	} else {
		buttonViewFrame.size.width  = _buttonWidth * sdlw->_buttonNum;
		buttonViewFrame.size.height = _buttonHeight;
	}
    if(!soi.isIpad){
        buttonViewFrame.origin.x = 480 - buttonViewFrame.size.width;
        buttonViewFrame.origin.y = 320 - buttonViewFrame.size.height;
    }
    else{
        buttonViewFrame.origin.x = 1024 - buttonViewFrame.size.width;
        buttonViewFrame.origin.y = 768 - buttonViewFrame.size.height;
    }
    
    sdlw->_buttonView = [[UIView alloc] initWithFrame:buttonViewFrame];
    {
        char  name[32];
		char* value;
		int i;
        
        if(sdlw->_buttonNum > 2) sdlw->_buttonMap[1] = SDLK_s;
		if(sdlw->_buttonNum > 1) sdlw->_buttonMap[0] = SDLK_ESCAPE;
        
        for(i = 0; i < sdlw->_buttonNum; i++){
            sprintf(name, "IPHONE_BUTTON_%d", i);
            sdlw->_buttonRects[i] = CGRectMake(0.0f, (float)(_buttonHeight*(sdlw->_buttonNum-1-i)), (float)_buttonWidth, (float)_buttonHeight);
            sdlw->_buttons[i] = [[UIButton alloc]initWithFrame:sdlw->_buttonRects[i]];

         //   [sdlw->_buttons[i] addTarget:self action:@selector(ycbuttonAction:) forControlEvents:UIControlEventTouchUpInside];
            UIImage* icon  = [[UIImage alloc] initWithContentsOfFile:@"button.png"];
            [sdlw->_buttons[i] setBackgroundImage:icon forState:UIControlStateNormal];
            [sdlw->_buttons[i] setTitle:@"niu" forState:UIControlStateNormal];
            [sdlw->_buttons[i] setEnabled:true];
            [sdlw->_buttonView addSubview:sdlw->_buttons[i]];
        }
        
    }
  //  [sdlw->landscapeView addSubview:sdlw->_buttonView];
   
    [sdlw->view addSubview:sdlw->_buttonView];
    [sdlw->_buttonView setHidden:NO];

 
    return self;
}

-(void)ycbuttonAction:(id)sender{
    /*
    UIButton* send = (UIButton*)sender;
    SDL_WindowData* sdlw=nil;
    UIWindow* tUiwindow = nil;

    sdlw=(SDL_WindowData*)(SDL_VideoWindow->driverdata);
    
    for(int i = 0 ; i < 5; i++){
        if(sdlw->_buttons[i] == send){
            NSString* a = [[NSString alloc] initWithFormat:@"the pressed button is %d\n",i];
            NSLog(a);
            [a release];
            return;
        }
    }*/
  //  NSLog(@"pressed ,but not button related\n");
    return;
    
    
}

@end
