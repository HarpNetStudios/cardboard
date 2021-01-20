#include "SDL.h"

extern int wayland;

#ifdef SDL_VIDEO_DRIVER_WAYLAND
 extern void setupwayland();
extern SDL_atomic_t framerequested;
#endif