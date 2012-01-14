/*
 SDL - Simple DirectMedia Layer
 Copyright (C) 1997-2009 Sam Lantinga
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
 Sam Lantinga
 slouken@libsdl.org
 */

#import "SDL_uikitview.h"
#import "SDL_uikitmoviedelegate.h"
//#include "../../events/SDL_mouse.c"

#include "SDL_config.h"
#include "onsInfo.h"
#include "../../events/SDL_keyboard_c.h"
#include "../../events/SDL_mouse_c.h"
#include "../../events/SDL_touch_c.h"
#include "SDL_uikitButton.h"

#if SDL_IPHONE_KEYBOARD
#import "keyinfotable.h"
#import "SDL_uikitappdelegate.h"
#import "SDL_uikitwindow.h"
#endif



@implementation SDL_uikitview
@synthesize hasSendSwp;

#ifdef SDL_IPHONE_KEYBOARD

int SDL_iPhoneKeyboardShow(SDL_Window * window) ;
int SDL_iPhoneKeyboardHide(SDL_Window * window) ;
SDL_bool SDL_iPhoneKeyboardIsShown(SDL_Window * window);
#endif
extern  SDL_Window* SDL_VideoWindow;

- (void)dealloc {
#if SDL_IPHONE_KEYBOARD
	[textField release];
#endif
	[super dealloc];
}

- (id)initWithFrame:(CGRect)frame {

	self = [super initWithFrame: frame];
	
#if SDL_IPHONE_KEYBOARD
	[self initializeKeyboard];
#endif	

[self initGesture];
    gs.longPress = gs.swipDown = gs.fDTap = gs.fTap  = gs.isTriTap = 0;
	return self;

}
- (void)initGesture{
    UILongPressGestureRecognizer* fingerLongPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
    [self addGestureRecognizer:fingerLongPress];
    [fingerLongPress release];
    /*
    UISwipeGestureRecognizer* fingerSwip = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(handleSwipGesture:)];
    [fingerSwip setDirection:UISwipeGestureRecognizerDirectionUp];
    [self addGestureRecognizer:fingerSwip];
    [fingerSwip release];
   */
    
    UITapGestureRecognizer* fingerDTap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(handleDTapGesture:)];
    [fingerDTap setNumberOfTapsRequired:1];
    [fingerDTap setNumberOfTouchesRequired:2];
    [self addGestureRecognizer:fingerDTap];
    [fingerDTap release];
    
    UITapGestureRecognizer* fingerTriTap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(handleTriTapGesture:)];
    [fingerTriTap setNumberOfTapsRequired:1];
    [fingerTriTap setNumberOfTouchesRequired:3];
    [self addGestureRecognizer:fingerTriTap];
    [fingerTriTap release];
    
    
    UITapGestureRecognizer* fingerTap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(handleTapGesture:)];
    [fingerTap setNumberOfTapsRequired:1];
    [fingerTap setNumberOfTouchesRequired:1];
    [self addGestureRecognizer:fingerTap];
    [fingerTap release];
    
    /*
    UISwipeGestureRecognizer* fingerSwipL = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(handleSwipGestureR:)];
    [fingerSwipL setDirection:UISwipeGestureRecognizerDirectionDown];
    [self addGestureRecognizer:fingerSwipL];
    [fingerSwipL release];   
    */
}
-(IBAction)handleLongPressGesture:(id)sender{

    NSLog(@"long press performed");
    
    SDL_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_S);
    SDL_SendKeyboardKey(SDL_RELEASED,SDL_SCANCODE_S);
   // SDL_SendKeyboardKey(SDL_PRESSED,SDL_SCANCODE_ESCAPE);
  //  SDL_SendKeyboardKey(SDL_RELEASED, SDL_SCANCODE_ESCAPE);
    gs.swipUp = gs.longPress = gs.swipDown = gs.fDTap = gs.fTap = 0;

}
-(IBAction)handleTriTapGesture:(id)sender{
    if(soi.isIpad)
        return;
    SDL_WindowData *data;
	SDL_uikitview *view;
    data = (SDL_WindowData *)SDL_VideoWindow->driverdata;
    view = data->view;
    fprintf(stderr, "tri Taped\n");
    if(gs.isTriTap){
        view.transform = CGAffineTransformScale(CGAffineTransformMakeRotation(0), 1.0, 1.0);
        view.center = CGPointMake(160, 240);
        gs.isTriTap^=1;
    }
    else{
          view.center = CGPointMake(160, 150);
#ifdef CLANNAD
        view.center = CGPointMake(160, 210);
#else
        view.center = CGPointMake(160, 270);
#endif
        view.transform = CGAffineTransformScale(CGAffineTransformMakeRotation(0), 1.0, 1.12476);
        gs.isTriTap^=1;
    }
    [[UIApplication sharedApplication] setStatusBarHidden:YES];
}
-(IBAction)handleSwipGestureR:(id)sender{
//    if(gs.swipUp)
 //       return;
    NSLog(@"swip up performed");
    SDL_SendKeyboardKey(SDL_PRESSED,SDL_SCANCODE_KP_ENTER);
    SDL_SendKeyboardKey(SDL_RELEASED, SDL_SCANCODE_KP_ENTER);
 
    

    gs.swipUp = gs.longPress = gs.swipDown = gs.fDTap = gs.fTap = 0;
    gs.swipUp = 1;
}
-(IBAction)handleSwipGesture:(id)sender{
//    if(gs.swipDown)
  //      return;
    NSLog(@"swip down performed");

 SDL_SendKeyboardKey(SDL_PRESSED,SDL_SCANCODE_SPACE);
 SDL_SendKeyboardKey(SDL_RELEASED, SDL_SCANCODE_SPACE);  

    gs.swipUp = gs.longPress = gs.swipDown = gs.fDTap = gs.fTap = 0;
    gs.swipDown = 1;
}
-(IBAction)handleDTapGesture:(id)sender{//把双键当成右键还是esc
 //   if(gs.fDTap)
  //      return;
    NSLog(@"Dtap performed");
#ifdef CLANNAD
    SDL_SendKeyboardKey(SDL_PRESSED,SDL_SCANCODE_ESCAPE);
    SDL_SendKeyboardKey(SDL_RELEASED, SDL_SCANCODE_ESCAPE);
#else
    SDL_SendMouseMotion(NULL,SDL_PRESSED,100,100);
    SDL_SendMouseButton(NULL,SDL_PRESSED,SDL_BUTTON_RIGHT);
    
    SDL_SendMouseMotion(NULL,SDL_RELEASED,101,101);
	SDL_SendMouseButton(NULL,SDL_RELEASED,SDL_BUTTON_RIGHT);
 //   SDL_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_RIGHT);
//    SDL_SendKeyboardKey(SDL_RELEASED,SDL_SCANCODE_RIGHT);
#endif
    /*
    SDL_SendMouseMotion(NULL,SDL_PRESSED,100,100);
    SDL_SendMouseButton(NULL,SDL_PRESSED,SDL_BUTTON_RIGHT);
    
    SDL_SendMouseMotion(NULL,SDL_RELEASED,101,101);
	SDL_SendMouseButton(NULL,SDL_RELEASED,SDL_BUTTON_RIGHT);
    */
    gs.swipUp =gs.longPress = gs.swipDown = gs.fDTap = gs.fTap = 0;
    gs.fDTap = 1;
}
-(IBAction)handleTapGesture:(id)sender{
    NSLog(@"tap performed");
    CGPoint locationInView = [sender locationInView:self];
    CGFloat old=locationInView.x;
    locationInView.x=locationInView.y;
    if (soi.isIpad) {
        locationInView.y=768-old;
    }
	else	locationInView.y=320-old;
    
    SDL_SendMouseMotion(NULL,SDL_PRESSED,locationInView.x,locationInView.y);
    SDL_SendMouseButton(NULL,SDL_PRESSED,SDL_BUTTON_LEFT);
    
    SDL_SendMouseMotion(NULL,SDL_RELEASED,locationInView.x+1,locationInView.y+1);
	SDL_SendMouseButton(NULL,SDL_RELEASED,SDL_BUTTON_LEFT);
    
    fprintf(stderr, "the loca x = %f, y = %f\n",locationInView.x,locationInView.y);
    gs.swipUp =gs.longPress = gs.swipDown = gs.fDTap = gs.fTap = 0;
}
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
#ifdef  CLANNAD
    UITouch *touch = [[event allTouches] anyObject];


    CGPoint locationInView = [touch locationInView:self];
    CGFloat old=locationInView.x;
    locationInView.x=locationInView.y;
   
    if (soi.isIpad) {
        locationInView.y=768-old;
    }
	else	locationInView.y=320-old;

        fprintf(stderr, "touch begin..");
    SDL_SendMouseMotion(NULL,SDL_PRESSED,locationInView.x,locationInView.y);
    SDL_SendMouseButton(NULL,SDL_PRESSED,SDL_BUTTON_LEFT);
#endif
    return;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
#ifdef  CLANNAD
    UITouch *touch = [[event allTouches] anyObject];
 
    
    CGPoint locationInView = [touch locationInView:self];
    CGFloat old=locationInView.x;
    locationInView.x=locationInView.y;

    if (soi.isIpad) {
        locationInView.y=768-old;
    }
	else	locationInView.y=320-old;
    

    fprintf(stderr, "touch end..");
    SDL_SendMouseMotion(NULL,SDL_RELEASED,locationInView.x,locationInView.y);
    SDL_SendMouseButton(NULL,SDL_RELEASED,SDL_BUTTON_LEFT);
   return;
#endif     
    return;
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	/*
		this can happen if the user puts more than 5 touches on the screen
		at once, or perhaps in other circumstances.  Usually (it seems)
		all active touches are canceled.
	*/
	
	SDL_SendMouseButton(NULL,SDL_RELEASED,SDL_BUTTON_LEFT);
	[self touchesEnded: touches withEvent: event];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
   
	return;
}

/*
	---- Keyboard related functionality below this line ----
*/
#if SDL_IPHONE_KEYBOARD

/* Is the iPhone virtual keyboard visible onscreen? */
- (BOOL)keyboardVisible {
	return keyboardVisible;
}

/* Set ourselves up as a UITextFieldDelegate */
- (void)initializeKeyboard {
		
	textField = [[[UITextField alloc] initWithFrame: CGRectZero] autorelease];
	textField.delegate = self;
	/* placeholder so there is something to delete! */
	textField.text = @" ";	
	
	/* set UITextInputTrait properties, mostly to defaults */
	textField.autocapitalizationType = UITextAutocapitalizationTypeNone;
	textField.autocorrectionType = UITextAutocorrectionTypeNo;
	textField.enablesReturnKeyAutomatically = NO;
	textField.keyboardAppearance = UIKeyboardAppearanceDefault;
	textField.keyboardType = UIKeyboardTypeDefault;
	textField.returnKeyType = UIReturnKeyDefault;
	textField.secureTextEntry = NO;	
	
	textField.hidden = YES;
	keyboardVisible = NO;
	/* add the UITextField (hidden) to our view */
	[self addSubview: textField];
}

/* reveal onscreen virtual keyboard */
- (void)showKeyboard {
	keyboardVisible = YES;
	[textField becomeFirstResponder];
}

/* hide onscreen virtual keyboard */
- (void)hideKeyboard {
	keyboardVisible = NO;
	[textField resignFirstResponder];
}

/* UITextFieldDelegate method.  Invoked when user types something. */
- (BOOL)textField:(UITextField *)_textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
	
	if ([string length] == 0) {
		/* it wants to replace text with nothing, ie a delete */
		SDL_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_DELETE);
		SDL_SendKeyboardKey(SDL_RELEASED, SDL_SCANCODE_DELETE);
	}
	else {
		/* go through all the characters in the string we've been sent
		   and convert them to key presses */
		int i;
		for (i=0; i<[string length]; i++) {
			
			unichar c = [string characterAtIndex: i];
			
			Uint16 mod = 0;
			SDL_scancode code;
			
			if (c < 127) {
				/* figure out the SDL_scancode and SDL_keymod for this unichar */
				code = unicharToUIKeyInfoTable[c].code;
				mod  = unicharToUIKeyInfoTable[c].mod;
			}
			else {
				/* we only deal with ASCII right now */
				code = SDL_SCANCODE_UNKNOWN;
				mod = 0;
			}
			
			if (mod & KMOD_SHIFT) {
				/* If character uses shift, press shift down */
				SDL_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_LSHIFT);
			}
			/* send a keydown and keyup even for the character */
			SDL_SendKeyboardKey(SDL_PRESSED, code);
			SDL_SendKeyboardKey(SDL_RELEASED, code);
			if (mod & KMOD_SHIFT) {
				/* If character uses shift, press shift back up */
				SDL_SendKeyboardKey(SDL_RELEASED, SDL_SCANCODE_LSHIFT);
			}			
		}
	}
	return NO; /* don't allow the edit! (keep placeholder text there) */
}

/* Terminates the editing session */
- (BOOL)textFieldShouldReturn:(UITextField*)_textField {
	[self hideKeyboard];
	return YES;
}

#endif

@end

/* iPhone keyboard addition functions */
#if SDL_IPHONE_KEYBOARD



int SDL_iPhoneKeyboardShow(SDL_Window * window) {
	
	SDL_WindowData *data;
	SDL_uikitview *view;
	
	if (NULL == window) {
		SDL_SetError("Window does not exist");
		return -1;
	}
	
	data = (SDL_WindowData *)window->driverdata;
	view = data->view;
	
	if (nil == view) {
		SDL_SetError("Window has no view");
		return -1;
	}
	else {
		[view showKeyboard];
		return 0;
	}
}

int SDL_iPhoneKeyboardHide(SDL_Window * window) {
	
	SDL_WindowData *data;
	SDL_uikitview *view;
	
	if (NULL == window) {
		SDL_SetError("Window does not exist");
		return -1;
	}	
	
	data = (SDL_WindowData *)window->driverdata;
	view = data->view;
	
	if (NULL == view) {
		SDL_SetError("Window has no view");
		return -1;
	}
	else {
		[view hideKeyboard];
		return 0;
	}
}

SDL_bool SDL_iPhoneKeyboardIsShown(SDL_Window * window) {
	
	SDL_WindowData *data;
	SDL_uikitview *view;
	
	if (NULL == window) {
		SDL_SetError("Window does not exist");
		return SDL_FALSE;
	}	
	
	data = (SDL_WindowData *)window->driverdata;
	view = data->view;
	
	if (NULL == view) {
		SDL_SetError("Window has no view");
		return SDL_FALSE;
	}
	else {
		return (view.keyboardVisible == SDL_TRUE)?SDL_TRUE:SDL_FALSE;
	}
}

int SDL_iPhoneKeyboardToggle(SDL_Window * window) {
	
	SDL_WindowData *data;
	SDL_uikitview *view;
	
	if (NULL == window) {
		SDL_SetError("Window does not exist");
		return -1;
	}	
	
	data = (SDL_WindowData *)window->driverdata;
	view = data->view;
	
	if (NULL == view) {
		SDL_SetError("Window has no view");
		return -1;
	}
	else {
		if (SDL_iPhoneKeyboardIsShown(window)) {
			SDL_iPhoneKeyboardHide(window);
		}
		else {
			SDL_iPhoneKeyboardShow(window);
		}
		return 0;
	}
}

#else

/* stubs, used if compiled without keyboard support */

int SDL_iPhoneKeyboardShow(SDL_Window * window) {
	SDL_SetError("Not compiled with keyboard support");
	return -1;
}

int SDL_iPhoneKeyboardHide(SDL_Window * window) {
	SDL_SetError("Not compiled with keyboard support");
	return -1;
}

SDL_bool SDL_iPhoneKeyboardIsShown(SDL_Window * window) {
	return 0;
}

int SDL_iPhoneKeyboardToggle(SDL_Window * window) {
	SDL_SetError("Not compiled with keyboard support");
	return -1;
}


#endif /* SDL_IPHONE_KEYBOARD */

//adding support for video play by YC


int SDL_iPhoneMoviePlay(char const* str)//it mush a non-relative path,so already added path before
{
	// Register to receive a notification when the movie has finished playing. 
    /*
	NSString *charStr = [[NSString alloc]initWithCString:str];
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	
	NSString *documentsDirectory = [paths objectAtIndex:0];

	NSFileManager *fileManage = [NSFileManager defaultManager];
     */
	//NSString *myDirectory = [documentsDirectory stringByAppendingPathComponent:@"video/movie.mp4"];
    NSString* docuPath = [[NSString alloc]initWithUTF8String:soi.path];
	NSMutableString *realPath = [[NSMutableString alloc]initWithString:docuPath];
    if(strlen(str) == 0)
        return 0;
    if(str[0] != '/')
        [realPath appendString:@"/"];
    NSString* subStr = [[NSString alloc] initWithUTF8String:str];
    [realPath appendString:subStr];
	NSURL *charUrl = [[NSURL alloc]initFileURLWithPath:realPath] ;
    NSLog(@"play with");
    NSLog(realPath);
	SDL_uikitmoviedelegate* sdl_umd = [[SDL_uikitmoviedelegate alloc] init];
	[sdl_umd initAndPlayMovie:charUrl:SDL_VideoWindow];
	return 1;
}
    
//add button support
int SDL_iPhoneButton(){
    SDL_uikitButton* btn = [[SDL_uikitButton alloc] init];
    [btn addButton:SDL_VideoWindow];
    [btn release];
    return 0;
}
