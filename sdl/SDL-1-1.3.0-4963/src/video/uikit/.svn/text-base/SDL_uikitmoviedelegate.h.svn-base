//
//  SDL_uikitmoviedelegate.h
//  SDLiPhoneOS
//
//  Created by  yc on 11-1-30.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <MediaPlayer/MediaPlayer.h>
#import "SDL_uikitwindow.h"

@interface SDL_uikitmoviedelegate : UIViewController{

	UIWindow* window;
	MPMoviePlayerController *moviePlayer;
}


@property (readwrite, retain) MPMoviePlayerController *moviePlayer;
@property (readwrite, retain) UIWindow* window;
-(void)initAndPlayMovie:(NSURL *)movieURL:(SDL_Window *)sdl_window;
@end
