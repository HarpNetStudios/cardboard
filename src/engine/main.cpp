// main.cpp: initialisation & main loop

#include "engine.h"
#include "icon.h"

#ifdef SDL_VIDEO_DRIVER_X11
#include "SDL_syswm.h"
#endif

old_string gametoken = "OFFLINE";

ICOMMAND(help, "", (), conoutf(CON_INFO, "you have been helped."));

#ifndef STANDALONE
SVARF(__hnapi, HNAPI, if (strcmp(__hnapi, HNAPI)) { offline = 1; conoutf("\f3You are using the UNOFFICIAL API server \fo\"%s\"\f3. Proceed at your own risk.", __hnapi); });
ICOMMAND(resethnapi, "", (), __hnapi = (char *)HNAPI);
#endif

extern void cleargamma();

void cleanup()
{
	#ifdef STEAM
		steam::cleanup();
	#endif
	#ifdef DISCORD
		discord::cleanup();
	#endif
	rawinput::release();
	gamepad::release();
	cleanupserver();
	SDL_ShowCursor(SDL_TRUE);
	SDL_SetRelativeMouseMode(SDL_FALSE);
	if(screen) SDL_SetWindowGrab(screen, SDL_FALSE);
	cleargamma();
	freeocta(worldroot);
	extern void clear_command(); clear_command();
	extern void clear_console(); clear_console();
	extern void clear_mdls();    clear_mdls();
	extern void clear_sound();   clear_sound();
	closelogfile();
	#ifdef __APPLE__
		if(screen) SDL_SetWindowFullscreen(screen, 0);
	#endif
	SDL_Quit();
}

extern void writeinitcfg();

void quit()                     // normal exit
{
	writeinitcfg();
	writeservercfg();
	abortconnect();
	disconnect();
	localdisconnect();
	writecfg();
	cleanup();
	logoutf("\"enjoy your run\" -fruloo");
	exit(EXIT_SUCCESS);
}

void fatal(const char *s, ...)    // failure exit
{
	static int errors = 0;
	errors++;

	if(errors <= 2) // print up to one extra recursive error
	{
		defvformatstring(msg,s,s);
		logoutf("%s", msg);

		if(errors <= 1) // avoid recursion
		{
			if(SDL_WasInit(SDL_INIT_VIDEO))
			{
				SDL_ShowCursor(SDL_TRUE);
				SDL_SetRelativeMouseMode(SDL_FALSE);
				if(screen) SDL_SetWindowGrab(screen, SDL_FALSE);
				cleargamma();
				#ifdef __APPLE__
					if(screen) SDL_SetWindowFullscreen(screen, 0);
				#endif
			}
			SDL_Quit();
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Cardboard Engine fatal error", msg, NULL);
		}
	}

	exit(EXIT_FAILURE);
}

int curtime = 0, lastmillis = 1, elapsedtime = 0, totalmillis = 1, starttime = time(0), curframetime = 0;

dynent *player = NULL;

int initing = NOT_INITING;

bool initwarning(const char *desc, int level, int type)
{
	if(initing < level) 
	{
		addchange(desc, type);
		return true;
	}
	return false;
}

VAR(desktopw, 1, 0, 0);
VAR(desktoph, 1, 0, 0);
int screenw = 0, screenh = 0;
SDL_Window *screen = NULL;
SDL_GLContext glcontext = NULL;

#define SCR_MINW 320
#define SCR_MINH 200
#define SCR_MAXW 10000
#define SCR_MAXH 10000
#define SCR_DEFAULTW 1024
#define SCR_DEFAULTH 768
VARF(scr_w, SCR_MINW, -1, SCR_MAXW, initwarning("screen resolution"));
VARF(scr_h, SCR_MINH, -1, SCR_MAXH, initwarning("screen resolution"));
VARF(depthbits, 0, 0, 32, initwarning("depth-buffer precision"));
VARF(fsaa, -1, -1, 16, initwarning("anti-aliasing"));

void writeinitcfg()
{
	stream *f = openutf8file("init.cfg", "w");
	if(!f) return;
	f->printf("// automatically written on exit, DO NOT MODIFY\n// modify settings in game\n");
	extern int fullscreen;
	f->printf("fullscreen %d\n", fullscreen);
	f->printf("scr_w %d\n", scr_w);
	f->printf("scr_h %d\n", scr_h);
	f->printf("depthbits %d\n", depthbits);
	f->printf("fsaa %d\n", fsaa);
	extern int usesound, soundchans, soundfreq, soundbufferlen;
	extern char *audiodriver;
	f->printf("usesound %d\n", usesound);
	f->printf("soundchans %d\n", soundchans);
	f->printf("soundfreq %d\n", soundfreq);
	f->printf("soundbufferlen %d\n", soundbufferlen);
	if(audiodriver[0]) f->printf("audiodriver %s\n", escapestring(audiodriver));
	delete f;
}

COMMAND(quit, "");

static void getbackgroundres(int &w, int &h)
{
	float wk = 1, hk = 1;
	if(w < 1024) wk = 1024.0f/w;
	if(h < 768) hk = 768.0f/h;
	wk = hk = max(wk, hk);
	w = int(ceil(w*wk));
	h = int(ceil(h*hk));
}

old_string backgroundcaption = "";
Texture *backgroundmapshot = NULL;
old_string backgroundmapname = "";
char *backgroundmapinfo = NULL;

void setbackgroundinfo(const char *caption = NULL, Texture *mapshot = NULL, const char *mapname = NULL, const char *mapinfo = NULL)
{
	renderedframe = false;
	copystring(backgroundcaption, caption ? caption : "");
	backgroundmapshot = mapshot;
	copystring(backgroundmapname, mapname ? mapname : "");
	if(mapinfo != backgroundmapinfo)
	{
		DELETEA(backgroundmapinfo);
		if(mapinfo) backgroundmapinfo = newstring(mapinfo);
	}
}

void restorebackground(bool force = false)
{
	if(renderedframe)
	{
		if(!force) return;
		setbackgroundinfo();
	}
	renderbackground(backgroundcaption[0] ? backgroundcaption : NULL, backgroundmapshot, backgroundmapname[0] ? backgroundmapname : NULL, backgroundmapinfo, true);
}

void bgquad(float x, float y, float w, float h, float tx = 0, float ty = 0, float tw = 1, float th = 1)
{
	gle::begin(GL_TRIANGLE_STRIP);
	gle::attribf(x,   y);   gle::attribf(tx,      ty);
	gle::attribf(x+w, y);   gle::attribf(tx + tw, ty);
	gle::attribf(x,   y+h); gle::attribf(tx,      ty + th);
	gle::attribf(x+w, y+h); gle::attribf(tx + tw, ty + th);
	gle::end();
}

void renderbackground(const char *caption, Texture *mapshot, const char *mapname, const char *mapinfo, bool restore, bool force, bool splash)
{
	if(!inbetweenframes && !force) return;

	if(!restore || force || !splash) stopsounds(); // stop sounds while loading
 
	int w = screenw, h = screenh;
	if(forceaspect) w = int(ceil(h*forceaspect));
	getbackgroundres(w, h);
	gettextres(w, h);

	static int lastupdate = -1, lastw = -1, lasth = -1;
	static float backgroundu = 0, backgroundv = 0;
	if((renderedframe && !mainmenu && lastupdate != lastmillis) || lastw != w || lasth != h)
	{
		lastupdate = lastmillis;
		lastw = w;
		lasth = h;

		backgroundu = rndscale(1);
		backgroundv = rndscale(1);
	}
	else if(lastupdate != lastmillis) lastupdate = lastmillis;

	loopi(restore ? 1 : 3)
	{
		hudmatrix.ortho(0, w, h, 0, -1, 1);
		resethudmatrix();

		hudshader->set();
		gle::colorf(1, 1, 1);

		gle::defvertex(2);
		gle::deftexcoord0();

		if (!(mapshot || mapname)) {
			// background and logo
			float bu = w * 0.67f / 256.0f + backgroundu, bv = h * 0.67f / 256.0f + backgroundv;
			if (splash) settexture("data/splash.png", 0);
			else settexture("data/background.png", 0);

			if (splash) bgquad(0, 0, w, h);
			else bgquad(0, 0, w, h, 0, 0, bu, bv);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			float lh = 0.5f * min(w, h), lw = lh * 2,
				lx = 0.5f * (w - lw), ly = 0.5f * (h * 0.5f - lh);
			if (!splash)
			{
				settexture((maxtexsize ? min(maxtexsize, hwtexsize) : hwtexsize) >= 1024 && (screenw > 1280 || screenh > 800) ? "data/logo_1024.png" : "data/logo.png", 3);
				bgquad(lx, ly, lw, lh);
			}

			// cardboard badge
			float badgeh = 0.12f * min(w, h), badgew = badgeh, badgex = 20, badgey = (h - badgeh - 20);
			if (!splash)
			{
				settexture("data/cardboard.png", 3);
				bgquad(badgex, badgey, badgew, badgeh);
			}

			// cube 2 badge
			badgex = (w - badgew - 20);
			if (!splash && mainmenu)
			{
				settexture("data/cube.png", 3);
				bgquad(badgex, badgey, badgew, badgeh);
			}
		}
		else {
			// blank black box, used for map load
			gle::colorf(0, 0, 0);
			bgquad(0, 0, w, h);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
		}

		if(caption)
		{
			int tw = text_width(caption);
			float tsz = 0.04f*min(w, h)/FONTH,
				  tx = 0.5f*(w - tw*tsz), ty = h - 0.075f*1.5f*min(w, h) - 1.25f*FONTH*tsz;
			pushhudmatrix();
			hudmatrix.translate(tx, ty, 0);
			hudmatrix.scale(tsz, tsz, 1);
			flushhudmatrix();
			draw_text(caption, 0, 0);
			pophudmatrix();
		}

		if(mapshot || mapname)
		{
			int infowidth = 12*FONTH;
			float sz = 0.35f * min(w, h);
			if(mapinfo)
			{
				int mw, mh;
				text_bounds(mapinfo, mw, mh, infowidth);
				// TODO: this seems wrong, look into it
				float tsz = sz / (8 * FONTH),
					tx = (w / 2) - (mw / 3),
					ty = 0.125f * sz - FONTH * tsz;
				pushhudmatrix();
				hudmatrix.translate(tx, ty, 0);
				hudmatrix.scale(tsz, tsz, 1);
				flushhudmatrix();
				draw_text(mapinfo, 0, 0, 0xFF, 0xFF, 0xFF);
				pophudmatrix();
			}

			if(mapshot && mapshot!=notexture)
			{
				glBindTexture(GL_TEXTURE_2D, mapshot->id);
				bgquad(0, h / 10, w, h - (h / 10));
			}

			if(mapname)
			{
				int tw = text_width(mapname);
				// TODO: this seems wrong, look into it
				float tsz = sz / (7 * FONTH),
					tx = (w / 2) - (tw / 2),
					ty = 0.275f * sz - FONTH * tsz;
				pushhudmatrix();
				hudmatrix.translate(tx, ty, 0);
				hudmatrix.scale(tsz, tsz, 1);
				flushhudmatrix();
				draw_text(mapname, 0, 0, 0xFF, 0x24, 0x00);
				pophudmatrix();
			}
			
			// TODO: implement map info?
			/*
			if(mapinfo)
			{
				pushhudmatrix();
				hudmatrix.translate(x+sz+FONTH*msz, y, 0);
				hudmatrix.scale(msz, msz, 1);
				flushhudmatrix();
				draw_text(mapinfo, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, -1, infowidth);
				pophudmatrix();
			}
			*/
		}
		glDisable(GL_BLEND);
		if(!restore) swapbuffers(false);
	}

	if(!restore) setbackgroundinfo(caption, mapshot, mapname, mapinfo);

	if (splash) playsound(S_LOGO);  // sound for splash screen
}

VAR(progressbackground, 0, 0, 1);

float loadprogress = 0;

void renderprogress(float bar, const char *text, GLuint tex, bool background)   // also used during loading
{
	if(!inbetweenframes || drawtex) return;

	extern int menufps, maxfps;
	int fps = menufps ? (maxfps ? min(maxfps, menufps) : menufps) : maxfps;
	if(fps)
	{
		static int lastprogress = 0;
		int ticks = SDL_GetTicks(), diff = ticks - lastprogress;
		if(bar > 0 && diff >= 0 && diff < (1000 + fps-1)/fps) return;
		lastprogress = ticks;
	}

	clientkeepalive();      // make sure our connection doesn't time out while loading maps etc.
	
	SDL_PumpEvents(); // keep the event queue awake to avoid 'beachball' cursor

	extern int mesa_swap_bug, curvsync;
	bool forcebackground = progressbackground || (mesa_swap_bug && (curvsync || totalmillis==1));
	if(background || forcebackground) restorebackground(forcebackground);

	int w = screenw, h = screenh;
	if(forceaspect) w = int(ceil(h*forceaspect));
	getbackgroundres(w, h);
	gettextres(w, h);

	hudmatrix.ortho(0, w, h, 0, -1, 1);
	resethudmatrix();

	hudshader->set();
	gle::colorf(1, 1, 1);

	gle::defvertex(2);
	gle::deftexcoord0();

	float fh = h / 30, fw = w,
		fx = 0,
		fy = h - (h / 30),
		fu1 = 0 / 512.0f, fu2 = 511 / 512.0f,
		fv1 = 0 / 64.0f, fv2 = 52 / 64.0f;
	settexture("data/loading_frame.png", 3);
	bgquad(fx, fy, fw, fh, fu1, fv1, fu2-fu1, fv2-fv1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float bw = fw, bh = fh,
		  bx = fx, by = fy,
		  bv1 = 0/32.0f, bv2 = 20/32.0f,
		  su1 = 0/32.0f, su2 = 7/32.0f, sw = fw*7/511.0f,
		  eu1 = 23/32.0f, eu2 = 30/32.0f, ew = fw*7/511.0f,
		  mw = bw - sw - ew,
		  ex = bx+sw + max(mw*bar, fw*7/511.0f);
	if(bar > 0)
	{
		settexture("data/loading_bar.png", 3);
		gle::begin(GL_QUADS);
		gle::attribf(bx,    by);    gle::attribf(su1, bv1);
		gle::attribf(bx+sw, by);    gle::attribf(su2, bv1);
		gle::attribf(bx+sw, by+bh); gle::attribf(su2, bv2);
		gle::attribf(bx,    by+bh); gle::attribf(su1, bv2);

		gle::attribf(bx+sw, by);    gle::attribf(su2, bv1);
		gle::attribf(ex,    by);    gle::attribf(eu1, bv1);
		gle::attribf(ex,    by+bh); gle::attribf(eu1, bv2);
		gle::attribf(bx+sw, by+bh); gle::attribf(su2, bv2);

		gle::attribf(ex,    by);    gle::attribf(eu1, bv1);
		gle::attribf(ex+ew, by);    gle::attribf(eu2, bv1);
		gle::attribf(ex+ew, by+bh); gle::attribf(eu2, bv2);
		gle::attribf(ex,    by+bh); gle::attribf(eu1, bv2);
		gle::end();
	}

	if(text)
	{
		int tw = text_width(text);
		float tsz = bh*0.8f/FONTH;
		if(tw*tsz > mw) tsz = mw/tw;
		pushhudmatrix();
		hudmatrix.translate(bx+sw, by + (bh - FONTH*tsz)/2, 0);
		hudmatrix.scale(tsz, tsz, 1);
		flushhudmatrix();
		draw_text(text, 0, 0);
		pophudmatrix();
	}

	glDisable(GL_BLEND);

	if(tex)
	{
		glBindTexture(GL_TEXTURE_2D, tex);
		float sz = 0.35f*min(w, h), x = 0.5f*(w-sz), y = 0.5f*min(w, h) - sz/15;
		bgquad(x, y, sz, sz);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		settexture("data/mapshot_frame.png", 3);
		bgquad(x, y, sz, sz);
		glDisable(GL_BLEND);
	}

	swapbuffers(false);
}

int keyrepeatmask = 0, textinputmask = 0;
Uint32 textinputtime = 0;
VAR(textinputfilter, 0, 5, 1000);

void keyrepeat(bool on, int mask)
{
	if(on) keyrepeatmask |= mask;
	else keyrepeatmask &= ~mask;
}

void textinput(bool on, int mask)
{
	if(on)
	{
		if(!textinputmask)
		{
			SDL_StartTextInput();
			textinputtime = SDL_GetTicks();
		}
		textinputmask |= mask;
	}
	else if(textinputmask)
	{
		textinputmask &= ~mask;
		if(!textinputmask) SDL_StopTextInput();
	}
}

bool minimized = false, initwindowpos = false;

void setfullscreen(int mode)
{
	if(!screen) return;

	int fullscreen_mode = (mode == 2 ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN);

	SDL_SetWindowFullscreen(screen, mode ? fullscreen_mode : 0);
	if(!mode) // if windowed
	{
		SDL_SetWindowSize(screen, scr_w, scr_h);
		if(initwindowpos)
		{
			int winx = SDL_WINDOWPOS_CENTERED, winy = SDL_WINDOWPOS_CENTERED;
			SDL_SetWindowPosition(screen, winx, winy);
			initwindowpos = false;
		}
	}
}

VARF(fullscreen, 0, 0, 2, setfullscreen(fullscreen));

void resetfullscreen()
{
	setfullscreen(0);
	setfullscreen(1);
}

void screenres(int w, int h)
{               
	scr_w = clamp(w, SCR_MINW, SCR_MAXW);
	scr_h = clamp(h, SCR_MINH, SCR_MAXH);
	if(screen)
	{           
		if(fullscreen == 2) // fullscreen desktop
		{
			scr_w = min(scr_w, desktopw);
			scr_h = min(scr_h, desktoph);
		}
		if(SDL_GetWindowFlags(screen) & SDL_WINDOW_FULLSCREEN)
		{
			if(fullscreen == 2) gl_resize();
			else resetfullscreen();
			initwindowpos = true;
		} 
		else
		{
			SDL_SetWindowSize(screen, scr_w, scr_h);
			SDL_SetWindowPosition(screen, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			initwindowpos = false;
		}
	}
	else
	{
		initwarning("screen resolution");
	}
}       

ICOMMAND(screenres, "ii", (int *w, int *h), screenres(*w, *h));

static void setgamma(int val)
{   
	if(screen && SDL_SetWindowBrightness(screen, val/100.0f) < 0) conoutf(CON_ERROR, "Could not set gamma: %s", SDL_GetError());
}   

static int curgamma = 100;
VARFNP(gamma, reqgamma, 30, 100, 300,
{
	if(initing || reqgamma == curgamma) return;
	curgamma = reqgamma;
	setgamma(curgamma);
});

void restoregamma()
{       
	if(initing || reqgamma == 100) return;
	curgamma = reqgamma;
	setgamma(curgamma);
}

void cleargamma()
{
	if(curgamma != 100 && screen) SDL_SetWindowBrightness(screen, 1.0f);
}

int curvsync = -1;
void restorevsync()
{
	if(initing || !glcontext) return;
	extern int vsync, vsynctear;
	if(!SDL_GL_SetSwapInterval(vsync ? (vsynctear ? -1 : 1) : 0))
		curvsync = vsync;
}

VARFP(vsync, 0, 0, 1, restorevsync());
VARFP(vsynctear, 0, 0, 1, { if(vsync) restorevsync(); });

static void seticon(SDL_Window* window)
{
	// these masks are needed to tell SDL_CreateRGBSurface(From)
	// to assume the data it gets is byte-wise RGB(A) data
	Uint32 rmask, gmask, bmask, amask;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		int shift = (my_icon.bytes_per_pixel == 3) ? 8 : 0;
		rmask = 0xff000000 >> shift;
		gmask = 0x00ff0000 >> shift;
		bmask = 0x0000ff00 >> shift;
		amask = 0x000000ff >> shift;
	#else // little endian, like x86
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = (cardboard_icon.bytes_per_pixel == 3) ? 0 : 0xff000000;
	#endif
	SDL_Surface* icon = SDL_CreateRGBSurfaceFrom((void*)cardboard_icon.pixel_data,
		cardboard_icon.width, cardboard_icon.height, cardboard_icon.bytes_per_pixel * 8,
		cardboard_icon.bytes_per_pixel * cardboard_icon.width, rmask, gmask, bmask, amask);

	SDL_SetWindowIcon(window, icon);

	SDL_FreeSurface(icon);
}

void setupscreen()
{
	if(glcontext)
	{
		SDL_GL_DeleteContext(glcontext);
		glcontext = NULL;
	}
	if(screen)
	{
		SDL_DestroyWindow(screen);
		screen = NULL;
	}
	curvsync = -1;

	SDL_Rect desktop;
	if(SDL_GetDisplayBounds(0, &desktop) < 0) fatal("failed querying desktop bounds: %s", SDL_GetError());
	desktopw = desktop.w;
	desktoph = desktop.h;

	if(scr_h < 0) scr_h = fullscreen ? desktoph : SCR_DEFAULTH;
	if(scr_w < 0) scr_w = (scr_h*desktopw)/desktoph;
	scr_w = clamp(scr_w, SCR_MINW, SCR_MAXW);
	scr_h = clamp(scr_h, SCR_MINH, SCR_MAXH);
	if(fullscreen == 2)
	{
		scr_w = min(scr_w, desktopw);
		scr_h = min(scr_h, desktoph);
	}

	int winx = SDL_WINDOWPOS_UNDEFINED, winy = SDL_WINDOWPOS_UNDEFINED, winw = scr_w, winh = scr_h, flags = SDL_WINDOW_RESIZABLE;
	if(fullscreen)
	{
		if(fullscreen == 2)
		{
			winw = desktopw;
			winh = desktoph;
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		else flags |= SDL_WINDOW_FULLSCREEN;
		initwindowpos = true;
	}

	SDL_GL_ResetAttributes();
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	#if !defined(WIN32) && !defined(__APPLE__)
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	#endif
	static const int configs[] =
	{
		0x3, /* try everything */
		0x2, 0x1, /* try disabling one at a time */
		0 /* try disabling everything */
	};
	int config = 0;
	if(!depthbits) SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	if(!fsaa)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	}
	loopi(sizeof(configs)/sizeof(configs[0]))
	{
		config = configs[i];
		if(!depthbits && config&1) continue;
		if(fsaa<=0 && config&2) continue;
		if(depthbits) SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, config&1 ? depthbits : 24);
		if(fsaa>0)
		{
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, config&2 ? 1 : 0);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config&2 ? fsaa : 0);
		}
		defformatstring(window_title, "%s %s %s (Cardboard Engine)", game::gametitle, game::gamestage, game::gameversion);
		screen = SDL_CreateWindow(window_title, winx, winy, winw, winh, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | flags);
		
		// we couldn't create the window, something went horribly wrong
		if(!screen) continue;

		seticon(screen);

	#ifdef __APPLE__
		static const int glversions[] = { 32, 20 };
	#else
		static const int glversions[] = { 33, 32, 31, 30, 20 };
	#endif
		loopj(sizeof(glversions)/sizeof(glversions[0]))
		{
			glcompat = glversions[j] <= 30 ? 1 : 0;
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glversions[j] / 10);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glversions[j] % 10);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, glversions[j] >= 32 ? SDL_GL_CONTEXT_PROFILE_CORE : 0);
			glcontext = SDL_GL_CreateContext(screen);
			if(glcontext) break;
		}
		if(glcontext) break;
	}
	if(!screen) fatal("failed to create OpenGL window: %s", SDL_GetError());
	else if(!glcontext) fatal("failed to create OpenGL context: %s", SDL_GetError());
	else
	{
		if(depthbits && (config&1)==0) conoutf(CON_WARN, "%d bit z-buffer not supported - disabling", depthbits);
		if(fsaa>0 && (config&2)==0) conoutf(CON_WARN, "%dx anti-aliasing not supported - disabling", fsaa);
	}

	SDL_SetWindowMinimumSize(screen, SCR_MINW, SCR_MINH);
	SDL_SetWindowMaximumSize(screen, SCR_MAXW, SCR_MAXH);

	SDL_GetWindowSize(screen, &screenw, &screenh);
}

void resetgl()
{
	clearchanges(CHANGE_GFX);

	renderbackground("resetting OpenGL");

	extern void cleanupva();
	extern void cleanupparticles();
	extern void cleanupdecals();
	extern void cleanupblobs();
	extern void cleanupsky();
	extern void cleanupmodels();
	extern void cleanupprefabs();
	extern void cleanuplightmaps();
	extern void cleanupblendmap();
	extern void cleanshadowmap();
	extern void cleanreflections();
	extern void cleanupglare();
	extern void cleanupdepthfx();
	cleanupva();
	cleanupparticles();
	cleanupdecals();
	cleanupblobs();
	cleanupsky();
	cleanupmodels();
	cleanupprefabs();
	cleanuptextures();
	cleanuplightmaps();
	cleanupblendmap();
	cleanshadowmap();
	cleanreflections();
	cleanupglare();
	cleanupdepthfx();
	cleanupshaders();
	cleanupgl();
	
	setupscreen();
	gl_init();

	inbetweenframes = false;
	if (!reloadtexture(*notexture) ||
		!reloadtexture("data/logo.png") ||
		!reloadtexture("data/logo_1024.png") ||
		!reloadtexture("data/background.png") ||
		!reloadtexture("data/mapshot_frame.png") ||
		!reloadtexture("data/loading_frame.png") ||
		!reloadtexture("data/loading_bar.png") ||
		!reloadtexture("data/cardboard.png") ||
		!reloadtexture("data/cube.png"))
	{
		fatal("failed to reload core texture");
	}

	reloadfonts();
	inbetweenframes = true;
	renderbackground("initializing...");
	restoregamma();
	restorevsync();
	reloadshaders();
	reloadtextures();
	initlights();
	#ifdef DISCORD
		discord::updatePresence(discord::D_MENU);
	#endif
	allchanged(true);
}

COMMAND(resetgl, "");

static queue<SDL_Event, 32> events;

static inline bool filterevent(const SDL_Event &event)
{
	switch(event.type)
	{
		case SDL_MOUSEMOTION:
			if(!(SDL_GetWindowFlags(screen) & SDL_WINDOW_FULLSCREEN))
			{
				if(event.motion.x == screenw / 2 && event.motion.y == screenh / 2)
					return false;  // ignore any motion events generated by SDL_WarpMouse
				#ifdef __APPLE__
				if(event.motion.y == 0)
					return false;  // let mac users drag windows via the title bar
				#endif
			}
			break;
	}
	return true;
}

template <int SIZE> static inline bool pumpevents(queue<SDL_Event, SIZE> &events)
{
	while(events.empty())
	{
		SDL_PumpEvents();
		databuf<SDL_Event> buf = events.reserve(events.capacity());
		int n = SDL_PeepEvents(buf.getbuf(), buf.remaining(), SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
		if(n <= 0) return false;
		loopi(n) if(filterevent(buf.buf[i])) buf.put(buf.buf[i]);
		events.addbuf(buf);
	}
	return true;
}

static int interceptkeysym = 0;

static int interceptevents(void *data, SDL_Event *event)
{
	switch(event->type)
	{
		case SDL_MOUSEMOTION: return 0;
		case SDL_KEYDOWN:
			if(event->key.keysym.sym == interceptkeysym)
			{
				interceptkeysym = -interceptkeysym;
				return 0;
			}
			break;
	}
	return 1;
}

static void clearinterceptkey()
{
	SDL_DelEventWatch(interceptevents, NULL);
	interceptkeysym = 0;
}

bool interceptkey(int sym)
{
	if(!interceptkeysym)
	{
		interceptkeysym = sym;
		SDL_FilterEvents(interceptevents, NULL);
		if(interceptkeysym < 0)
		{
			interceptkeysym = 0;
			return true;
		}
		SDL_AddEventWatch(interceptevents, NULL);
	}
	else if(abs(interceptkeysym) != sym) interceptkeysym = sym;
	SDL_PumpEvents();
	if(interceptkeysym < 0)
	{
		clearinterceptkey();
		interceptkeysym = sym;
		SDL_FilterEvents(interceptevents, NULL);
		interceptkeysym = 0;
		return true;
	}
	return false;
}

static void ignoremousemotion()
{
	SDL_PumpEvents();
	SDL_FlushEvent(SDL_MOUSEMOTION);
}

static void checkmousemotion(int &dx, int &dy)
{
	while(pumpevents(events))
	{
		SDL_Event &event = events.removing();
		if(event.type != SDL_MOUSEMOTION) return;
		dx += event.motion.xrel;
		dy += event.motion.yrel;
		events.remove();
	}
}

void checkinput()
{
	if(interceptkeysym) clearinterceptkey();
	//int lasttype = 0, lastbut = 0;
	bool mousemoved = false;
	int focused = 0;

	if (rawinput::enabled) rawinput::flush();

	while(pumpevents(events))
	{
		SDL_Event &event = events.remove();

		switch(event.type)
		{
			case SDL_QUIT:
				quit();
				return;

			case SDL_TEXTINPUT:
				if(textinputmask && int(event.text.timestamp-textinputtime) >= textinputfilter)
				{
					uchar buf[SDL_TEXTINPUTEVENT_TEXT_SIZE+1];
					size_t len = decodeutf8(buf, sizeof(buf)-1, (const uchar *)event.text.text, strlen(event.text.text));
					if(len > 0) { buf[len] = '\0'; processtextinput((const char *)buf, len); }
				}
				break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if(keyrepeatmask || !event.key.repeat)
					processkey(event.key.keysym.sym, event.key.state==SDL_PRESSED, event.key.keysym.mod | SDL_GetModState());
				break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
						quit();
						break;

					case SDL_WINDOWEVENT_ENTER:
						focused = 1;
						break;

					case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_FOCUS_LOST:
						focused = -1;
						break;

					case SDL_WINDOWEVENT_MINIMIZED:
						minimized = true;
						break;

					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
						minimized = false;
						break;

					case SDL_WINDOWEVENT_RESIZED:
						break;

					case SDL_WINDOWEVENT_SIZE_CHANGED:
					{
						SDL_GetWindowSize(screen, &screenw, &screenh);
						// TODO: this seems weird, take a look
						if(fullscreen != 2 || !(SDL_GetWindowFlags(screen) & SDL_WINDOW_FULLSCREEN))
						{
							scr_w = clamp(screenw, SCR_MINW, SCR_MAXW);
							scr_h = clamp(screenh, SCR_MINH, SCR_MAXH);
						}
						gl_resize();
						break;
					}
				}
				gamepad::handlefocus(focused);
				break;

			case SDL_MOUSEMOTION:
				if (rawinput::debugrawmouse)
				{
					conoutf("%d sdl mouse motion (%d, %d) [%d, %d]",
						lastmillis, event.motion.xrel, event.motion.yrel, event.motion.x, event.motion.y);
				}
				if (!rawinput::enabled)
				{
					int dx = event.motion.xrel, dy = event.motion.yrel;
					checkmousemotion(dx, dy);
					if (!g3d_movecursor(event.motion.x, event.motion.y)) {
						mousemove(dx, dy);
						mousemoved = true;
					}
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				if (rawinput::enabled) break;

				switch(event.button.button)
				{
					case SDL_BUTTON_LEFT: processkey(-1, event.button.state==SDL_PRESSED); break;
					case SDL_BUTTON_MIDDLE: processkey(-2, event.button.state==SDL_PRESSED); break;
					case SDL_BUTTON_RIGHT: processkey(-3, event.button.state==SDL_PRESSED); break;
					case SDL_BUTTON_X1: processkey(-6, event.button.state==SDL_PRESSED); break;
					case SDL_BUTTON_X2: processkey(-7, event.button.state==SDL_PRESSED); break;
					case SDL_BUTTON_X2 + 1: processkey(-10, event.button.state==SDL_PRESSED); break;
					case SDL_BUTTON_X2 + 2: processkey(-11, event.button.state==SDL_PRESSED); break;
					case SDL_BUTTON_X2 + 3: processkey(-12, event.button.state==SDL_PRESSED); break;
				}
				//lasttype = event.type;
				//lastbut = event.button.button;
				break;

			case SDL_MOUSEWHEEL:
				if(event.wheel.y > 0) { processkey(-4, true); processkey(-4, false); }
				else if(event.wheel.y < 0) { processkey(-5, true); processkey(-5, false); }
				else if(event.wheel.x > 0) { processkey(-8, true); processkey(-8, false); }
				else if(event.wheel.x < 0) { processkey(-9, true); processkey(-9, false); }
				break;

			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
			case SDL_CONTROLLERDEVICEREMAPPED:
			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERAXISMOTION:
				gamepad::handleevent(event, focused);
				break;
		}
	}
}

void swapbuffers(bool overlay)
{
	gle::disable();
	SDL_GL_SwapWindow(screen);
}
 
VARP(menufps, 0, 60, 1000);
VARP(maxfps, 0, 200, 1000);
VARFP(maxtps, 0, 0, 1000, { if (maxtps && maxtps < 60) { conoutf("Can't set maxtps < 60"); maxtps = 60; } });

void ratelimit(int& millis, int lastdrawmillis, bool& draw)
{
	int fpslimit = (mainmenu || minimized) && menufps ? (maxfps ? min(maxfps, menufps) : menufps) : maxfps;
	if (!fpslimit) draw = true;
	int tpslimit = minimized ? 100 : (maxtps ? max(maxtps, fpslimit) : 0);
	if (!fpslimit && !tpslimit) return;
	int delay = 1;
	if (tpslimit) delay = max(1000 / tpslimit - (millis - totalmillis), 0);
	// should we draw?
	int fpsdelay = INT_MAX;
	if (!minimized && fpslimit)
	{
		static int fpserror = 0;
		fpsdelay = 1000 / fpslimit - (millis - lastdrawmillis);
		if (fpsdelay < 0)
		{
			draw = true;
		}
		else
		{
			if (fpserror >= fpslimit) fpsdelay++;
			if (fpsdelay <= delay)
			{
				draw = true;
				if (fpserror >= fpslimit) fpserror -= fpslimit;
				fpserror += 1000 % fpslimit;
			}
		}
	}
	delay = min(delay, fpsdelay);
	if (delay > 0)
	{
		SDL_Delay(delay);
		millis += delay;
	}
}

#if defined(WIN32) && !defined(_DEBUG) && !defined(__GNUC__)
void stackdumper(unsigned int type, EXCEPTION_POINTERS *ep)
{
	if(!ep) fatal("unknown type");
	EXCEPTION_RECORD *er = ep->ExceptionRecord;
	CONTEXT *context = ep->ContextRecord;
	char out[512];
	formatstring(out, "Cardboard Engine Win32 Exception: 0x%x [0x%x]\n\n", er->ExceptionCode, er->ExceptionCode==EXCEPTION_ACCESS_VIOLATION ? er->ExceptionInformation[1] : -1);
	SymInitialize(GetCurrentProcess(), NULL, TRUE);
	// TODO: this ifdef can be removed when we drop 32-bit support
#ifdef _AMD64_
	STACKFRAME64 sf = {{context->Rip, 0, AddrModeFlat}, {}, {context->Rbp, 0, AddrModeFlat}, {context->Rsp, 0, AddrModeFlat}, 0};
	while(::StackWalk64(IMAGE_FILE_MACHINE_AMD64, GetCurrentProcess(), GetCurrentThread(), &sf, context, NULL, ::SymFunctionTableAccess, ::SymGetModuleBase, NULL))
	{
		union { IMAGEHLP_SYMBOL64 sym; char symext[sizeof(IMAGEHLP_SYMBOL64) + sizeof(old_string)]; };
		sym.SizeOfStruct = sizeof(sym);
		sym.MaxNameLength = sizeof(symext) - sizeof(sym);
		IMAGEHLP_LINE64 line;
		line.SizeOfStruct = sizeof(line);
		DWORD64 symoff;
		DWORD lineoff;
		if(SymGetSymFromAddr64(GetCurrentProcess(), sf.AddrPC.Offset, &symoff, &sym) && SymGetLineFromAddr64(GetCurrentProcess(), sf.AddrPC.Offset, &lineoff, &line))
#else
	STACKFRAME sf = {{context->Eip, 0, AddrModeFlat}, {}, {context->Ebp, 0, AddrModeFlat}, {context->Esp, 0, AddrModeFlat}, 0};
	while(::StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &sf, context, NULL, ::SymFunctionTableAccess, ::SymGetModuleBase, NULL))
	{
		union { IMAGEHLP_SYMBOL sym; char symext[sizeof(IMAGEHLP_SYMBOL) + sizeof(old_string)]; };
		sym.SizeOfStruct = sizeof(sym);
		sym.MaxNameLength = sizeof(symext) - sizeof(sym);
		IMAGEHLP_LINE line;
		line.SizeOfStruct = sizeof(line);
		DWORD symoff, lineoff;
		if(SymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &symoff, &sym) && SymGetLineFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &lineoff, &line))
#endif
		{
			char *del = strrchr(line.FileName, '\\');
			concformatstring(out, "%s - %s [%d]\n", sym.Name, del ? del + 1 : line.FileName, line.LineNumber);
		}
	}
	fatal(out);
}
#endif

#define MAXFPSHISTORY 60

int fpspos = 0, fpshistory[MAXFPSHISTORY];

void resetfpshistory()
{
	loopi(MAXFPSHISTORY) fpshistory[i] = 1;
	fpspos = 0;
}

void updatefpshistory(int millis)
{
	fpshistory[fpspos++] = max(1, min(1000, millis));
	if(fpspos>=MAXFPSHISTORY) fpspos = 0;
}

void getfps(int &fps, int &bestdiff, int &worstdiff)
{
	int total = fpshistory[MAXFPSHISTORY-1], best = total, worst = total;
	loopi(MAXFPSHISTORY-1)
	{
		int millis = fpshistory[i];
		total += millis;
		if(millis < best) best = millis;
		if(millis > worst) worst = millis;
	}

	fps = (1000*MAXFPSHISTORY)/total;
	bestdiff = 1000/best-fps;
	worstdiff = fps-1000/worst;
}

void getfps_(int *raw)
{
	int fps, bestdiff, worstdiff;
	if(*raw) fps = 1000/fpshistory[(fpspos+MAXFPSHISTORY-1)%MAXFPSHISTORY];
	else getfps(fps, bestdiff, worstdiff);
	intret(fps);
}

COMMANDN(getfps, getfps_, "i");

bool inbetweenframes = false, renderedframe = true;

static bool findarg(int argc, char **argv, const char *str)
{
	for(int i = 1; i<argc; i++) if(strstr(argv[i], str)==argv[i]) return true;
	return false;
}

static int clockrealbase = 0, clockvirtbase = 0;
static void clockreset() { clockrealbase = SDL_GetTicks(); clockvirtbase = totalmillis; }
VARFP(clockerror, 990000, 1000000, 1010000, clockreset());
VARFP(clockfix, 0, 0, 1, clockreset());

int getclockmillis()
{
	int millis = SDL_GetTicks() - clockrealbase;
	if(clockfix) millis = int(millis*(double(clockerror)/1000000));
	millis += clockvirtbase;
	return max(millis, totalmillis);
}

VAR(numcpus, 1, 1, 128);

VARFP(offline, 0, 0, 1, { getuserinfo_(false); });

bool getuserinfo_(bool debug, bool first) {
	if(offline) // don't waste time trying to check everything if we are offline.
	{
		if(isconnected()) disconnect();
		return false;
	}
	if(!strcmp(gametoken, "OFFLINE")) // check if playing without logging into launcher
	{
		if(!first) conoutf(CON_ERROR, "\f3[HNID] Please restart the game and log in with your HNID to play online!");
		offline = 1;
		return false;
	}
	old_string apiurl;
	formatstring(apiurl, "%s/game/login?game=1", __hnapi);
	const char* thing = web_auth(apiurl, gametoken, debug);
	if(!thing[0]) {
		conoutf(CON_ERROR, "\f3[HNID] No data recieved from server, switching to offline mode!");
		offline = 1;
		return false; 
	}
	cJSON *json = cJSON_Parse(thing);

	// error handling
	const cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "status");
	const cJSON* message = cJSON_GetObjectItemCaseSensitive(json, "message");
	if(cJSON_IsNumber(status) && cJSON_IsString(message)) {
		if(status->valueint > 0) {
			conoutf(CON_ERROR, "\f3[HNID] Could not log in! Error: \"%s\" (%d)", message->valuestring, status->valueint);
			offline = 1;
			return false;
		}
		else {
			// actual parse
			const cJSON* name = NULL;
			name = cJSON_GetObjectItemCaseSensitive(json, "username");
			const cJSON* pubt = NULL;
			pubt = cJSON_GetObjectItemCaseSensitive(json, "public");
			if(cJSON_IsString(name) && (name->valuestring != NULL) && cJSON_IsString(pubt) && (pubt->valuestring != NULL))
			{
				game::switchname(name->valuestring);
				game::setpubtoken(pubt->valuestring);
				if (!first) conoutf(CON_INFO, "\f1[HNID] Successfully logged in as %s!", name->valuestring);
				return true;
			}
			conoutf(CON_ERROR, "\f3[HNID] Malformed JSON recieved from server (server blocked?)");
			logoutf("[MALFORMED JSON]: %s", thing);
			offline = 1;
			return false;
		}
	}
	else {
		conoutf(CON_ERROR, "\f3[HNID] Malformed JSON recieved from server (server blocked?)");
		logoutf("[MALFORMED JSON]: %s", thing);
		offline = 1;
		return false;
	}
}

COMMANDN(getuserinfo, getuserinfo_, "i");

ICOMMAND(getgametoken, "", (), result(gametoken));
#define MAXTOKENLEN 64

int globalgamestate = -1;

void setgametoken(const char* token) {
	filtertext(gametoken, token, false, false, MAXTOKENLEN);
	if(player!=NULL) getuserinfo_(false);
}

#ifdef DISCORD
	VARN(ext_discord, ext_discord_enabled, 1, 1, 0);
#else
	VARN(ext_discord, ext_discord_enabled, 1, 0, 0);
#endif

#ifdef STEAM
	VARN(ext_steam, ext_steam_enabled, 1, 1, 0);
#else
	VARN(ext_steam, ext_steam_enabled, 1, 0, 0);
#endif

int main(int argc, char **argv)
{
	#ifdef WIN32
	//atexit((void (__cdecl *)(void))_CrtDumpMemoryLeaks);
	#ifndef _DEBUG
	#ifndef __GNUC__
	__try {
	#endif
	#endif
	#endif

	setlogfile(NULL);

	int dedicated = 0;
	char *load = NULL, *initscript = NULL;

	initing = INIT_RESET;
	// set home dir first
	for(int i = 1; i<argc; i++) if(argv[i][0]=='-' && argv[i][1] == 'q') { sethomedir(&argv[i][2]); break; }
	// set log after home dir, but before anything else
	for(int i = 1; i<argc; i++) if(argv[i][0]=='-' && argv[i][1] == 'g')
	{
		const char *file = argv[i][2] ? &argv[i][2] : "log.txt";
		setlogfile(file);
		logoutf("Setting log file: %s", file);
		break;
	}

	// get gametoken, we need that
	for (int i = 1; i < argc; i++) if (argv[i][0] == '-' && argv[i][1] == 'c') { setgametoken(&argv[i][2]); break; }
	if (!gametoken[0]) strcpy(gametoken, "OFFLINE");
	if (!strcmp(gametoken, "OFFLINE")) offline = 1;

	execfile("init.cfg", false);
	for(int i = 1; i<argc; i++)
	{
		if(argv[i][0]=='-') switch(argv[i][1])
		{
			// reordered alpha to make it easier to see what's being used already. -Y 03/14/19
			case 'a': fsaa = atoi(&argv[i][2]); break;
			case 'b': /* compat, ignore */ break;
			case 'c': if(gametoken[0]) logoutf("Using game token: [REDACTED]"); break;
			case 'd': dedicated = atoi(&argv[i][2]); if(dedicated <= 0) dedicated = 2; break;
			case 'f': /* compat, ignore */ break;
			case 'g': break;
			case 'h': scr_h = clamp(atoi(&argv[i][2]), SCR_MINH, SCR_MAXH); if(!findarg(argc, argv, "-w")) scr_w = -1; break;
			case 'k':
			{
				const char *dir = addpackagedir(&argv[i][2]);
				if(dir) logoutf("Adding package directory: %s", dir);
				break;
			}
			case 'l':
			{
				char pkgdir[] = "packages/";
				load = strstr(path(&argv[i][2]), path(pkgdir));
				if(load) load += sizeof(pkgdir) - 1;
				else load = &argv[i][2];
				break;
			}
			case 'q': if(homedir[0]) logoutf("Using home directory: %s", homedir); break;
			case 'r': /* compat, ignore */ break;
			case 's': /* compat, ignore */ break;
			case 't': fullscreen = atoi(&argv[i][2]); break;
			case 'v': /* compat, ignore */ break;
			case 'w': scr_w = clamp(atoi(&argv[i][2]), SCR_MINW, SCR_MAXW); if(!findarg(argc, argv, "-h")) scr_h = -1; break;
			case 'x': initscript = &argv[i][2]; break;
			case 'z': depthbits = atoi(&argv[i][2]); break;
			default: if(!serveroption(argv[i])) gameargs.add(argv[i]); break;
		}
		else gameargs.add(argv[i]);
	}
	initing = NOT_INITING;

	numcpus = clamp(SDL_GetCPUCount(), 1, 128);

	if(dedicated <= 1)
	{
		logoutf("init: sdl");

		if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_AUDIO)<0) fatal("Unable to initialize SDL: %s", SDL_GetError());

#ifdef SDL_VIDEO_DRIVER_X11
		SDL_version version;
		SDL_GetVersion(&version);
		if (SDL_VERSIONNUM(version.major, version.minor, version.patch) <= SDL_VERSIONNUM(2, 0, 12))
			sdl_xgrab_bug = 1;
#endif
	}
	
	logoutf("init: net");
	if(enet_initialize()<0) fatal("Unable to initialise network module");
	atexit(enet_deinitialize);
	enet_time_set(0);

	logoutf("init: game");
	game::parseoptions(gameargs);
	initserver(dedicated>0, dedicated>1);  // never returns if dedicated
	ASSERT(dedicated <= 1);
	game::initclient();

	// this is before video so we can cleanly catch a steam init failure.
	#ifdef STEAM
		logoutf("init: steam");
		steam::initSteam();
		steam::steamCallbacks();
	#endif

	logoutf("init: video");
	SDL_SetHint(SDL_HINT_GRAB_KEYBOARD, "0");
	#if !defined(WIN32) && !defined(__APPLE__)
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
	#endif
	setupscreen();
	SDL_ShowCursor(SDL_FALSE);
	SDL_StopTextInput(); // workaround for spurious text-input events getting sent on first text input toggle?

	logoutf("init: gl");
	gl_checkextensions();
	gl_init();
	notexture = textureload("packages/textures/notexture.png");
	if (!notexture) fatal("could not find core textures (are you using the launcher?)");

	logoutf("init: console");
	if (!execfile("data/stdlib.cfg", false)) fatal("cannot find data files (you are running from the wrong folder, try .bat file in the main folder)"); // this is the first file we load.
	if (!execfile("data/lang.cfg", false)) fatal("cannot find lang config"); // after this point in execution, translations are safe to use.
	if (!execfile("data/font.cfg", false)) fatal("cannot find font definitions");
	if (!setfont("default")) fatal("no default font specified");

	logoutf("init: sound");
	execfile("data/sounds.cfg"); // load sounds early
	initsound();

	inbetweenframes = true;
	renderbackground(NULL, NULL, NULL, NULL, false, false, true); // render splash

	logoutf("init: world");
	camera1 = player = game::iterdynents(0);
	emptymap(0, true, NULL, false);

	logoutf("init: cfg");
	initing = INIT_LOAD;
	execfile("data/keymap.cfg");
	execfile("data/stdedit.cfg");
	execfile("data/menus.cfg");
	execfile("data/heightmap.cfg");
	execfile("data/blendbrush.cfg");
	defformatstring(gamecfgname, "data/game_%s.cfg", game::gameident());
	execfile(gamecfgname);
	if(game::savedservers()) execfile(game::savedservers(), false);
	
	identflags |= IDF_PERSIST;
	
	if(!execfile(game::savedconfig(), false)) 
	{
		execfile(game::defaultconfig());
		writecfg(game::restoreconfig());
	}
	execfile(game::autoexec(), false);

	identflags &= ~IDF_PERSIST;

	initing = INIT_GAME;
	game::loadconfigs();

	initing = NOT_INITING;

	// TODO: improve the UX of this, game just appears to freeze if it can't connect quickly
	logoutf("init: auth");
	if (strcmp(__hnapi, HNAPI)) renderprogress(0.0f, "connecting to UNOFFICIAL auth server...");
	else renderprogress(0.0f, "connecting to auth server...");
	getuserinfo_(false, true);
	if (strcmp(__hnapi, HNAPI)) renderprogress(1.0f, "connected to UNOFFICIAL auth server");
	else renderprogress(1.0f, "connected to auth server");

	logoutf("init: render");
	restoregamma();
	restorevsync();
	loadshaders();
	initparticles();
	initdecals();

	#ifdef STEAM
		logoutf("init: steam input");
		steam::input_registerBinds();
	#endif

	identflags |= IDF_PERSIST; // anything executed past here save to save to config

	#ifdef DISCORD
		logoutf("init: discord");
		discord::initDiscord();
		discord::updatePresence(discord::D_MENU);
	#endif

	#ifdef STEAM
		steam::input_updateActions(steam::ST_MENU);
	#endif

	logoutf("init: mainloop");

	if (execfile("once.cfg", false)) remove(findfile("once.cfg", "rb"));
	if (execfile("update.cfg", false)) remove(findfile("update.cfg", "rb"));

	if (load)
	{
		logoutf("init: localconnect");
		//localconnect();
		game::changemap(load);
	}

	if(initscript) execute(initscript);

	initmumble();
	resetfpshistory();

	// TODO: is this still needed?
	ignoremousemotion();

	#ifdef STEAM
		steam::steamCallbacks();
		steam::setAchievement("ACH_FIRST_LAUNCH");
	#endif

	for(;;)
	{
		static int frames = 0, lastdrawmillis = 0;
		int millis = getclockmillis();
		bool draw = false;
		ratelimit(millis, lastdrawmillis, draw);
		if (draw)
		{
			static int frametimeerr = 0;
			int scaledframetime = game::scaletime(millis - lastdrawmillis) + frametimeerr;
			curframetime = scaledframetime / 100;
			frametimeerr = scaledframetime % 100;
		}
		elapsedtime = millis - totalmillis;
		static int timeerr = 0;
		int scaledtime = game::scaletime(elapsedtime) + timeerr;
		curtime = scaledtime/100;
		timeerr = scaledtime%100;
		if(!multiplayer(false) && curtime>200) curtime = 200;
		if(!game::ispaused()) lastmillis += curtime;
		totalmillis = millis;
		updatetime();
 
		checkinput();
		menuprocess();
#ifndef NO_EDITOR
		tryedit();
#endif

		if(lastmillis) game::updateworld();

		checksleep(lastmillis);

		serverslice(false, 0);

		if (draw)
		{
			if (frames) updatefpshistory(millis - lastdrawmillis);
			frames++;
		}

		// miscellaneous general game effects
		recomputecamera();
		if(draw) updateparticles();
		updatesounds();

		if(minimized) continue;

		if (draw) {
			inbetweenframes = false;
			if (mainmenu)
			{
				gl_drawmainmenu();
				#ifdef DISCORD
					discord::updatePresence(discord::D_MENU);
				#endif
				#ifdef STEAM
					steam::input_updateActions(steam::ST_MENU);
				#endif
			}
			else
			{ 
				#ifdef STEAM
					steam::input_updateActions(steam::ST_PLAYING);
				#endif
				gl_drawframe();
			} 
			swapbuffers();
			renderedframe = inbetweenframes = true;
			lastdrawmillis = millis;
		}

		#ifdef STEAM
			steam::steamCallbacks();
			if(steam::input_getConnectedControllers()) steam::input_checkController();
		#endif
		#ifdef DISCORD
			discord::discordCallbacks();
		#endif
	}
	
	ASSERT(0);   
	return EXIT_FAILURE;

	#if defined(WIN32) && !defined(_DEBUG) && !defined(__GNUC__)
	} __except(stackdumper(0, GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH) { return 0; }
	#endif
}
