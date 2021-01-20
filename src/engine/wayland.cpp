#include "engine.h"

VAR(wayland, 1, 0, 0);

#ifdef SDL_VIDEO_DRIVER_WAYLAND

#include "SDL_syswm.h"
#include <wayland-client-protocol.h>

static struct wl_surface* surface;
static struct  wl_callback_listener framecallbacklistener;

void setupcallbacklistener()
{
    wl_callback* framecallback = wl_surface_frame(surface);
    wl_callback_add_listener(framecallback, &framecallbacklistener, NULL);
}

SDL_atomic_t framerequested;

void markframerequested(void* data, struct wl_callback* framecallback, uint32_t time)
{
    wl_callback_destroy(framecallback);
    SDL_AtomicSet(&framerequested, 1);
    setupcallbacklistener();
    framecallbacklistener.done = markframerequested;
}

void setupwayland()
{
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (!SDL_GetWindowWMInfo(screen, &info)) return;
    if (info.subsystem != SDL_SYSWM_WAYLAND) return;
    surface = info.info.wl.surface;
    setupcallbacklistener();
    framecallbacklistener.done = markframerequested;
    wayland = true;
    SDL_AtomicSet(&framerequested, 1);
}

#endif