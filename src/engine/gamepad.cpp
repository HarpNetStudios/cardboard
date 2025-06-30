#include "engine.h"

namespace gamepad {

	struct GamePad {
		bool buttons[SDL_GAMEPAD_BUTTON_COUNT];
		int axis[SDL_GAMEPAD_AXIS_COUNT];
	};

	SDL_Gamepad* controller;
	int numGamepads;

	enum Controllers { PLAYER1, PLAYER2, PLAYER3, PLAYER4 }; // for future use.

	const float axismax = 32767.5f;
	const int buttonsym = -100;

	void init() {
		if (SDL_WasInit(SDL_INIT_GAMEPAD) != 1)
		{
			SDL_InitSubSystem(SDL_INIT_GAMEPAD);
			if (SDL_AddGamepadMappingsFromFile("data/gamecontrollerdb.txt") < 0) {
				conoutf(CON_ERROR, "SDL_GameControllerDB error: %s", SDL_GetError());
			}
		}

		// Enumerate connected controllers
		int i, num_joysticks;
		SDL_JoystickID* joysticks = SDL_GetJoysticks(&num_joysticks);

		// If we have some controllers connected
		if (joysticks)
		{
			// Open the controller
			SDL_Gamepad* pad = SDL_OpenGamepad(joysticks[0]);
			if (SDL_GamepadConnected(pad)) {
				controller = pad;
				conoutf(CON_INFO, "Enabled \"%s\" gamepad. \fpThis feature is still experimental.", SDL_GetGamepadName(controller));
			}
			else conoutf(CON_ERROR, "SDL_GetError() = %s", SDL_GetError());
			SDL_SetGamepadEventsEnabled(true);

			SDL_free(joysticks);
		}
		else conoutf(CON_WARN, "\foNo controllers detected, please plug one in.");
	}

	void release() {
		if (controller) {
			conoutf(CON_INFO, "Disabled gamepad.");
			SDL_CloseGamepad(controller);
			controller = NULL;
		}

		if (SDL_WasInit(SDL_INIT_GAMEPAD))
			SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
	}

	VARP(joydeadzone, 0, 8000, axismax);

	float map_range(int value, float old_min, float old_max, int new_min, int new_max) {
		return ((float)new_min + ((float)new_max - (float)new_min) * ((float)value - (float)old_min) / ((float)old_max - (float)old_min));
	}

	float axis(const int value)
	{
		int val = value;
		if ((val > -joydeadzone) && (val < joydeadzone)) return 0;
		else if (val < -joydeadzone) val += joydeadzone; else val -= joydeadzone;
		float mapped = -map_range(val, -axismax + joydeadzone, axismax - joydeadzone, -1, 1);
		
		//const float scaled = -val / axismax;
		//conoutf(CON_DEBUG, "axis: %f (%d)", range, value);
		//return scaled;
		return mapped;
	}

	VAR(dbgjoy, 0, 0, 1);
	VARP(joytriggermode, 0, 0, 1);
	VARP(joytriggermax, 0, axismax / 2, axismax-1);

	void handletrigger(const SDL_GamepadAxisEvent &e)
	{
		bool active = (joytriggermode && e.value > joytriggermax) || (!joytriggermode && e.value > 0);
		if (dbgjoy) conoutf("trigger %s: %d", SDL_GetGamepadStringForAxis((SDL_GamepadAxis)e.axis), e.value);
		switch (e.axis)
		{
			case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
				processkey(-120, active);
				break;
			case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
				processkey(-121, active);
				break;
		}
	}

	// don't say I never did anything for you. -Y

	VARP(joyinvsticks, 0, 0, 1);
	VARP(joyinvlook_x, 0, 0, 1);
	VARP(joyinvlook_y, 0, 0, 1);
	VARP(joyinvmove_x, 0, 0, 1);
	VARP(joyinvmove_y, 0, 0, 1);

	void handleaxis(const SDL_GamepadAxisEvent& e)
	{
		switch (e.axis)
		{
			case SDL_GAMEPAD_AXIS_LEFTX:
				if (joyinvsticks)
					player->camx = -axis(e.value) * (joyinvlook_x ? -1 : 1);
				else
					player->fstrafe = axis(e.value) * (joyinvmove_x ? -1 : 1);
				if (dbgjoy) conoutf("fstrafe: %f", player->fstrafe);
				break;
			case SDL_GAMEPAD_AXIS_LEFTY:
				if (joyinvsticks)
					player->camy = -axis(e.value) * (joyinvlook_y ? -1 : 1);
				else
					player->fmove = axis(e.value) * (joyinvmove_y ? -1 : 1);
				break;
			case SDL_GAMEPAD_AXIS_RIGHTX:
				if (joyinvsticks)
					player->fstrafe = axis(e.value) * (joyinvmove_x ? -1 : 1);
				else
					player->camx = -axis(e.value) * (joyinvlook_x ? -1 : 1);
				break;
			case SDL_GAMEPAD_AXIS_RIGHTY:
				if (joyinvsticks)
					player->fmove = axis(e.value) * (joyinvmove_y ? -1 : 1);
				else
					player->camy = -axis(e.value) * (joyinvlook_y ? -1 : 1);
				break;
			case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
			case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
				handletrigger(e);
				break;
		}
	}

	int buttonsymbol(Uint8 sdlJoyButton)
	{
		if (sdlJoyButton > 19)
		{
			if (dbgjoy) conoutf(CON_ERROR, "\foUnsupported gamepad button %d", sdlJoyButton);
			return 0;
		}
		return buttonsym - sdlJoyButton;
	}

	void handlefocus(int focused) {
		if(focused<1) {
			player->camx = player->camy = 0.0f;
		}
	}

	void handlebutton(const SDL_GamepadButtonEvent& e)
	{
		bool pressed = e.down;
		int symbol = buttonsymbol(e.button);
		if (symbol)
		{
			if (dbgjoy) conoutf(CON_DEBUG, "button %d: %s %s", symbol, SDL_GetGamepadStringForButton((SDL_GamepadButton)e.button), pressed ? "PRESSED" : "RELEASE");
			processkey(symbol, pressed);
		}
	}

	void handleevent(const SDL_Event& e, int focused) {
		switch (e.type)
		{
			case SDL_EVENT_GAMEPAD_ADDED:
				if (dbgjoy) conoutf(CON_DEBUG, "DEVICEADDED cdevice.which = %d", e.cdevice.which);
				init();
				break;
			case SDL_EVENT_GAMEPAD_REMOVED:
				if (dbgjoy) conoutf(CON_DEBUG, "DEVICEREMOVED cdevice.which = %d", e.cdevice.which);
				release();
				break;

				// If a controller button is pressed
			case SDL_EVENT_GAMEPAD_BUTTON_UP:
			case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
				handlebutton(e.gbutton);
				break;

				// And something similar for axis motion
			case SDL_EVENT_GAMEPAD_AXIS_MOTION:
				handleaxis(e.gaxis);
				break;
		}
		//handlefocus(focused);
	}

	void setenabled(const int enabled)
	{
		#ifdef STEAM
		if (steam::input_getConnectedControllers()>0 && enabled) {
			setvar("gamepad", 0);
			conoutf(CON_WARN, "\foSteam Input enabled controller detected, use that instead.");
			release();
			return;
		}
		#endif

		if(enabled) init();
		else release();
	}

	VARFP(gamepad, 0, 0, 1, setenabled(gamepad));
}
