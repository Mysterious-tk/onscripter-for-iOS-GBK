//
//  SDL_uikitmoviedelegate.m
//  SDLiPhoneOS
//
//  Created by  yc on 11-1-30.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "SDL_uikitmoviedelegate.h"
#import "onsInfo.h"



@implementation SDL_uikitmoviedelegate


@synthesize moviePlayer;
@synthesize window;

-(void)initAndPlayMovie:(NSURL *)movieURL :(SDL_Window *)sdl_window
{
	SDL_WindowData* sdlw;
	sdlw=(SDL_WindowData*)sdl_window->driverdata;
	window=sdlw->uiwindow;
	if (!window) {
		NSLog(@"window alloc error!");
		return ;
	}
	MPMoviePlayerController *mp = [[MPMoviePlayerController alloc] initWithContentURL:movieURL];
	if (mp)
	{
        mp.scalingMode = MPMovieScalingModeAspectFill;
		self.moviePlayer = mp;
		 
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(moviePlayBackDidFinish:)
													 name:MPMoviePlayerPlaybackDidFinishNotification
												   object:moviePlayer];
                      
	//	self.moviePlayer.view.transform = CGAffineTransformMakeRotation(M_PI/2);
	//	 moviePlayer.view.frame = CGRectMake(-80, 80, 480.0, 320.0);
        if(soi.isIpad)
            moviePlayer.view.frame = CGRectMake(-130, 130, 1024, 768);
            else
		moviePlayer.view.frame = CGRectMake(-80, 80, 480, 320.0);
		 window.transform = CGAffineTransformMakeRotation(M_PI/2);
        [moviePlayer setScalingMode:MPMovieScalingModeAspectFit];
        [moviePlayer setControlStyle:MPMovieControlStyleFullscreen];
		[[UIApplication sharedApplication] setStatusBarHidden:YES];
		[mp release];
      //  [self.moviePlayer setControlStyle: MPMovieControlStyleDefault];
	//	moviePlayer.view.transform =  CGAffineTransformMakeRotation(M_PI/2);
       
		 [sdlw->uiwindow addSubview:self.moviePlayer.view];
		 [self.moviePlayer play];
  
      
	}

}
-(void) moviePlayBackDidFinish:(NSNotification*)notification
{
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:MPMoviePlayerPlaybackDidFinishNotification
												  object:moviePlayer];
	if([moviePlayer respondsToSelector:@selector(setFullscreen:animated:)]){
		
	//	[moviePlayer.view dismissMoviePlayerViewControllerAnimated];
		[moviePlayer.view removeFromSuperview];
		window.transform =CGAffineTransformMakeRotation(0);
	}
}
-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation { 
	return ((interfaceOrientation == UIInterfaceOrientationLandscapeLeft) ||
			(interfaceOrientation == UIInterfaceOrientationLandscapeRight));
}

-(void)dealloc {

    	[[UIApplication sharedApplication] setStatusBarHidden:YES];
	[[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:MPMoviePlayerPlaybackDidFinishNotification object:nil];
	[moviePlayer release];
	[super dealloc];
    	[[UIApplication sharedApplication] setStatusBarHidden:YES];
	 }

@end
