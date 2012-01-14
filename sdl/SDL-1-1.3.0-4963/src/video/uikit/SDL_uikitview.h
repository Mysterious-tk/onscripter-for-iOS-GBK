/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2010 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#import <UIKit/UIKit.h>
#include "SDL_stdinc.h"
#include "SDL_events.h"
#import <MediaPlayer/MediaPlayer.h>

#define kMinGestureLen 60
#define kMaxVar 20

#define IPHONE_TOUCH_EFFICIENT_DANGEROUS
#define FIXED_MULTITOUCH

#ifndef IPHONE_TOUCH_EFFICIENT_DANGEROUS
#define MAX_SIMULTANEOUS_TOUCHES 5
#endif

struct gesState {
    int  longPress;
    int swipDown;
    int swipUp;
    int fDTap;//两指触摸
    int fTap;//单指触摸
    int isTriTap;
};


/* *INDENT-OFF* */
#if SDL_IPHONE_KEYBOARD
@interface SDL_uikitview : UIView<UITextFieldDelegate,UIGestureRecognizerDelegate> {
#else
@interface SDL_uikitview : UIView {
#endif
	bool hasSendSwp;
	CGPoint gestureStartPoint;
#ifdef FIXED_MULTITOUCH
	long touchId;
#ifndef IPHONE_TOUCH_EFFICIENT_DANGEROUS
	UITouch *finger[MAX_SIMULTANEOUS_TOUCHES];
#endif
#endif
    
    struct gesState gs;
#if SDL_IPHONE_KEYBOARD
	UITextField *textField;
	BOOL keyboardVisible;
#endif	
	
}
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;

- (void)initGesture;

#if SDL_IPHONE_KEYBOARD
- (void)showKeyboard;
- (void)hideKeyboard;
- (void)initializeKeyboard;
@property (readonly) BOOL keyboardVisible;

#endif 
@property bool hasSendSwp;
@property struct gesState gs;
-(IBAction)handleTriTapGesture:(id)sender;
-(IBAction)handleLongPressGesture:(id)sender;
-(IBAction)handleTapGesture:(id)sender;
@end
/* *INDENT-ON* */
