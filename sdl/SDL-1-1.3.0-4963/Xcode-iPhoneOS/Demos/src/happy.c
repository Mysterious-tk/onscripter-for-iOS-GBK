/*
 *	happy.c
 *	written by Holmes Futrell
 *	use however you want
 */

#include "SDL.h"
#include "sdl_opengles.h"
#include "common.h"

#define NUM_HAPPY_FACES 100     /* number of faces to draw */
#define MILLESECONDS_PER_FRAME 16       /* about 60 frames per second */
#define HAPPY_FACE_SIZE 32      /* width and height of happyface (pixels) */

static SDL_Texture *texture = 0;    /* reference to texture holding happyface */

static struct
{
    float x, y;                 /* position of happyface */
    float xvel, yvel;           /* velocity of happyface */
} faces[NUM_HAPPY_FACES];

/*
	Sets initial positions and velocities of happyfaces
	units of velocity are pixels per millesecond
*/
void
initializeHappyFaces()
{
    int i;
    for (i = 0; i < NUM_HAPPY_FACES; i++) {
        faces[i].x = randomFloat(0.0f, SCREEN_WIDTH - HAPPY_FACE_SIZE);
        faces[i].y = randomFloat(0.0f, SCREEN_HEIGHT - HAPPY_FACE_SIZE);
        faces[i].xvel = randomFloat(-0.1f, 0.1f);
        faces[i].yvel = randomFloat(-0.1f, 0.1f);
    }
}

void
render(void)
{

    int i;
    SDL_Rect srcRect;
    SDL_Rect dstRect;

    /* setup boundaries for happyface bouncing */
    Uint16 maxx = SCREEN_WIDTH - HAPPY_FACE_SIZE;
    Uint16 maxy = SCREEN_HEIGHT - HAPPY_FACE_SIZE;
    Uint16 minx = 0;
    Uint16 miny = 0;

    /* setup rects for drawing */
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = HAPPY_FACE_SIZE;
    srcRect.h = HAPPY_FACE_SIZE;
    dstRect.w = HAPPY_FACE_SIZE;
    dstRect.h = HAPPY_FACE_SIZE;

    /* fill background in with black */
    SDL_SetRenderDrawColor(0, 0, 0, 255);
    SDL_RenderFill(NULL);

    /*
       loop through all the happy faces:
       - update position
       - update velocity (if boundary is hit)
       - draw
     */
    for (i = 0; i < NUM_HAPPY_FACES; i++) {
        faces[i].x += faces[i].xvel * MILLESECONDS_PER_FRAME;
        faces[i].y += faces[i].yvel * MILLESECONDS_PER_FRAME;
        if (faces[i].x > maxx) {
            faces[i].x = maxx;
            faces[i].xvel = -faces[i].xvel;
        } else if (faces[i].y > maxy) {
            faces[i].y = maxy;
            faces[i].yvel = -faces[i].yvel;
        }
        if (faces[i].x < minx) {
            faces[i].x = minx;
            faces[i].xvel = -faces[i].xvel;
        } else if (faces[i].y < miny) {
            faces[i].y = miny;
            faces[i].yvel = -faces[i].yvel;
        }
        dstRect.x = faces[i].x;
        dstRect.y = faces[i].y;
        SDL_RenderCopy(texture, &srcRect, &dstRect);
    }
    /* update screen */
    SDL_RenderPresent();

}

/*
	loads the happyface graphic into a texture
*/
void
initializeTexture()
{
    SDL_Surface *bmp_surface;
    SDL_Surface *bmp_surface_rgba;
    int format = SDL_PIXELFORMAT_ABGR8888;      /* desired texture format */
    Uint32 Rmask, Gmask, Bmask, Amask;  /* masks for desired format */
    int bpp;                    /* bits per pixel for desired format */
    /* load the bmp */
    bmp_surface = SDL_LoadBMP("icon.bmp");
    if (bmp_surface == NULL) {
        fatalError("could not load bmp");
    }
    /* set white to transparent on the happyface */
    SDL_SetColorKey(bmp_surface, 1,
                    SDL_MapRGB(bmp_surface->format, 255, 255, 255));
    SDL_PixelFormatEnumToMasks(format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
    /*
       create a new RGBA surface and blit the bmp to it
       this is an extra step, but it seems to be necessary
       is this a bug?
     */
    bmp_surface_rgba =
        SDL_CreateRGBSurface(0, bmp_surface->w, bmp_surface->h, bpp, Rmask,
                             Gmask, Bmask, Amask);
    SDL_BlitSurface(bmp_surface, NULL, bmp_surface_rgba, NULL);

    /* convert RGBA surface to texture */
    texture = SDL_CreateTextureFromSurface(format, bmp_surface_rgba);
    if (texture == 0) {
        fatalError("could not create texture");
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    /* free up allocated memory */
    SDL_FreeSurface(bmp_surface_rgba);
    SDL_FreeSurface(bmp_surface);
}
extern SDL_Window *SDL_VideoWindow;
int
main(int argc, char *argv[])
{

    SDL_Window *window;
    Uint32 startFrame;
    Uint32 endFrame;
    Uint32 delay;
    int done;

    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fatalError("Could not initialize SDL");
    }
    window = SDL_CreateWindow(NULL, 0, 0, 960,640 ,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
                                SDL_WINDOW_BORDERLESS);

    SDL_CreateRenderer(window, -1, 0);
/*
 glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, 320,480 );
	glRotatef(-90, 0, 0, 1);
	glOrthof(0, 480, 320, 0, -400, 400);
	*/
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, 640, 960);
	glRotatef(-90, 0, 0, 1);
	glOrthof(0.0, (GLfloat) 960, (GLfloat) 640, 0.0, 0, 100.0f);
    initializeTexture();
    initializeHappyFaces();

    
     SDL_VideoWindow = window;
    /* main loop */
    done = 0;
    while (!done) {
        startFrame = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = 1;
            }
        }
        render();
        endFrame = SDL_GetTicks();

        /* figure out how much time we have left, and then sleep */
        delay = MILLESECONDS_PER_FRAME - (endFrame - startFrame);
        if (delay < 0) {
            delay = 0;
        } else if (delay > MILLESECONDS_PER_FRAME) {
            delay = MILLESECONDS_PER_FRAME;
        }
        SDL_Delay(delay);
    }

    /* cleanup */
    SDL_DestroyTexture(texture);
    /* shutdown SDL */
    SDL_Quit();

    return 0;

}
