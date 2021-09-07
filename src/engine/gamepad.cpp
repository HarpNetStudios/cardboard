#include "engine.h"

namespace gamepad {

	struct GamePad {
		bool buttons[SDL_CONTROLLER_BUTTON_MAX];
		int axis[SDL_CONTROLLER_AXIS_MAX];
	};

	SDL_GameController* controller;
	int numGamepads;

	enum Controllers { PLAYER1, PLAYER2, PLAYER3, PLAYER4 }; // for future use.

	const float axismax = 32767.5f;
	const int buttonsym = -100;

	void init() {
		if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) != 1)
		{
			SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
			if (SDL_GameControllerAddMappingsFromFile("data/gamecontrollerdb.txt") < 0) {
				conoutf(CON_ERROR, "SDL_GameControllerDB error: %d", SDL_GetError());
			}
		}

		int nJoysticks = SDL_NumJoysticks();
		numGamepads = 0;

		// Count how many controllers there are
		for (int i = 0; i < nJoysticks; i++)
			if (SDL_IsGameController(i))
				numGamepads++;

		// If we have some controllers attached
		if (numGamepads > 0)
		{
			// Open the controller
			SDL_GameController* pad = SDL_GameControllerOpen(0);
			if (SDL_GameControllerGetAttached(pad) == 1) {
				controller = pad;
				conoutf(CON_INFO, "Enabled \"%s\" gamepad. \fpThis feature is still experimental.", SDL_GameControllerName(controller));
			}
			else conoutf(CON_ERROR, "SDL_GetError() = ", SDL_GetError());
			SDL_GameControllerEventState(SDL_ENABLE);
		}
		else conoutf(CON_WARN, "\foNo controllers detected, please plug one in.");
	}

	void release() {
		if (controller) {
			conoutf(CON_INFO, "Disabled gamepad.");
			SDL_GameControllerClose(controller);
			controller = NULL;
		}

		if (SDL_WasInit(SDL_INIT_GAMECONTROLLER))
			SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
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

	void handletrigger(const SDL_ControllerAxisEvent &e)
	{
		bool active = (joytriggermode && e.value > joytriggermax) || (!joytriggermode && e.value > 0);
		if (dbgjoy) conoutf("trigger %s: %d", SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)e.axis), e.value);
		switch (e.axis)
		{
			case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
				processkey(-120, active);
				break;
			case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
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

	void handleaxis(const SDL_ControllerAxisEvent& e)
	{
		switch (e.axis)
		{
			case SDL_CONTROLLER_AXIS_LEFTX:
				if (joyinvsticks)
					player->camx = -axis(e.value) * (joyinvlook_x ? -1 : 1);
				else
					player->fstrafe = axis(e.value) * (joyinvmove_x ? -1 : 1);
				if (dbgjoy) conoutf("fstrafe: %f", player->fstrafe);
				break;
			case SDL_CONTROLLER_AXIS_LEFTY:
				if (joyinvsticks)
					player->camy = -axis(e.value) * (joyinvlook_y ? -1 : 1);
				else
					player->fmove = axis(e.value) * (joyinvmove_y ? -1 : 1);
				break;
			case SDL_CONTROLLER_AXIS_RIGHTX:
				if (joyinvsticks)
					player->fstrafe = axis(e.value) * (joyinvmove_x ? -1 : 1);
				else
					player->camx = -axis(e.value) * (joyinvlook_x ? -1 : 1);
				break;
			case SDL_CONTROLLER_AXIS_RIGHTY:
				if (joyinvsticks)
					player->fmove = axis(e.value) * (joyinvmove_y ? -1 : 1);
				else
					player->camy = -axis(e.value) * (joyinvlook_y ? -1 : 1);
				break;
			case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
			case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
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

	void handlebutton(const SDL_ControllerButtonEvent& e)
	{
		bool pressed = e.state == SDL_PRESSED;
		int symbol = buttonsymbol(e.button);
		if (symbol)
		{
			if (dbgjoy) conoutf(CON_DEBUG, "button %d: %s %s", symbol, SDL_GameControllerGetStringForButton((SDL_GameControllerButton)e.button), pressed ? "PRESSED" : "RELEASE");
			processkey(symbol, pressed);
		}
	}

	void handleevent(const SDL_Event& e, int focused) {
		switch (e.type)
		{
			case SDL_CONTROLLERDEVICEADDED:
				if (dbgjoy) conoutf(CON_DEBUG, "DEVICEADDED cdevice.which = %d", e.cdevice.which);
				init();
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				if (dbgjoy) conoutf(CON_DEBUG, "DEVICEREMOVED cdevice.which = %d", e.cdevice.which);
				release();
				break;

				// If a controller button is pressed
			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERBUTTONDOWN:
				handlebutton(e.cbutton);
				break;

				// And something similar for axis motion
			case SDL_CONTROLLERAXISMOTION:
				handleaxis(e.caxis);
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
