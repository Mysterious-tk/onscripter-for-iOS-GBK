/*
 * Copyright (c) 2004-2006  Kazunori "jagarl" Ueno
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// #define ROOTPATH "/mnt/KEY/CLANNAD"
// #define ROOTPATH "/home/uenok/pb"
// #define FONT "msgothic.ttc"
	/* kochi-mincho-subst.ttf あるいは -*-*-*-r-*--24-*-*-*-*-*-jisx0208.1983-* など */
	/* TrueType Font は /usr/X11R6/lib/X11/fonts/TrueType/ などに存在する必要がある */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#ifdef ENABLE_PATCH
#if !defined(WINCE)
#include<locale.h>
#endif
#endif

#include<SDL.h>
#include<vector>
#include<getopt.h>

#include"system/file.h"
#include"system/system_config.h"
#include"window/widget.h"
#include"window/system.h"

#include"music2/music.h"

#include"scn2k/scn2k.h"
#include"scn2k/scn2k_impl.h"

#ifdef ENABLE_PATCH
SDL_Joystick *g_joystick = NULL;
#endif

const char key_lb_orig[] = {0x4b, 0x45, 0x59, 0x5c, 0x83, 0x8a, 0x83, 0x67, 0x83, 0x8b, 0x83, 0x6f, 0x83, 0x58, 0x83, 0x5e, 0x81, 0x5b, 0x83, 0x59, 0x81, 0x49, 0};

const char key_lb_new[] = "KEY\\LittleBusters";


extern "C" int main(int argc, char* argv[]); /* SDL.h で定義されるので必要ないはずなんだけど…… */

int main(int argc, char *argv[]) {
	AyuSysConfig config;
	int opt = 0, end = 0, screenmode = 0;
	char rootPath[1024]  = ".";
	char font[1024]      = "default.ttf";
#ifdef ENABLE_PATCH
#if defined(PSP)
	Uint32 videoOptions  = SDL_SWSURFACE;
#else
	Uint32 videoOptions  = SDL_HWSURFACE;
#endif

#if defined(WINCE)
	char currentDir[MAX_PATH];
	strcpy(currentDir, argv[0]);
	char* cptr = currentDir;
	int i, len = strlen(currentDir);
	for(i=len-1; i>0; i--){
		if(cptr[i] == '\\' || cptr[i] == '/')
			break;
	}
	cptr[i] = '\0';
	strcpy(rootPath, currentDir);
#else
	while(1) {
		opt = getopt(argc, argv, "fdt:c:r:vh?");
		if(opt == -1) {break;}

		switch(opt) {
			case 'f':
				videoOptions |= SDL_FULLSCREEN;
				break;
			case 'd':
				videoOptions |= SDL_DOUBLEBUF;
				break;
			case 't':
				strncpy(font, optarg, 1023);
				break;
			case 'r':
				strncpy(rootPath, optarg, 1023);
				break;
			case 'v':
				// FIXME: "VERSION" undeclared?
				// printf("xclannad %s\n", VERSION);
				printf("xclannad 0.05e\n");
				end = 1;
				break;
			case 'h':
			case '?':
				printf("\nUsage: %s [OPTIONS]\n\n", argv[0]);
				printf("  -f    : full screen mode\n");
				printf("  -d    : double buffer mode\n");
				printf("  -t    : set font (typeface)\n");
				printf("  -r    : set root path (default /mnt/KEY/CLANNAD)\n");
				printf("  -v    : show version and exit\n");
				printf("  -h -? : show help and exit\n\n");
				end = 1;
				break;
		}
	}
	if(end == 1) return 0;
#endif

	printf("Settings:\n");
#if !defined(WINCE)
	printf("  Locale    : %s\n", setlocale(LC_ALL, ""));
#endif
	printf("  Root Path : %s\n", rootPath);
	printf("  Font      : %s\n", font);
	printf("\n");
#endif
	
	file_searcher.InitRoot(rootPath);
	config.LoadInitFile();
	const char* regname = config.GetParaStr("#REGNAME");
	if (strcmp(regname, key_lb_orig) == 0) { // "リトルバスターズ！ -> LittleBustersに#REGNAMEを変更
		config.SetParaStr("#REGNAME", key_lb_new);
	}
	SetFont(font);

	MuSys mu(config);
	mu.InitMusic();

	if(SDL_Init(SDL_INIT_VIDEO)) {
		printf("Unable to init SDL: %s\n", SDL_GetError());
	        return 1;
	}
	atexit(SDL_Quit);

#ifdef ENABLE_PATCH
	if(SDL_InitSubSystem( SDL_INIT_JOYSTICK ) == 0){
		g_joystick = SDL_JoystickOpen(0);
	}
#endif

	config.GetParam("#SCREENSIZE_MOD", 1, &screenmode);
	if (screenmode == 1) {
		SDL_SetVideoMode(800, 600, 0, videoOptions);
	} else {
		SDL_SetVideoMode(640, 480, 0, videoOptions);
	}
	// SDL_SetVideoMode(640, 480, 0, videoOptions);
	// SDL_SetVideoMode(800, 600, 0, SDL_HWSURFACE /*| SDL_FULLSCREEN */);
	{
		System::Main main_sys;
		PicContainer* main_panel = main_sys.root.create_node(Rect(0, 0, main_sys.root.width, main_sys.root.height), 0);
		main_panel->show();
		try {
			Scn2k scn(main_sys.event, *main_panel, mu, config);
			main_sys.Mainloop();
		} catch(...) {
			fprintf(stderr,"System faulted; exit now.\n");
		}
		delete main_panel;
	}

	mu.FinalizeMusic();

#ifdef ENABLE_PATCH
	if(g_joystick)
		SDL_JoystickClose(g_joystick);
#endif

	SDL_Quit();
}

