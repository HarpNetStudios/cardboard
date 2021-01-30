#include "engine.h"

namespace gamepad {

	struct GamePad {
		bool buttons[SDL_CONTROLLER_BUTTON_MAX];
		int axis[SDL_CONTROLLER_AXIS_MAX];
	};

	SDL_GameController* controller;
	int numGamepads;

	enum Controllers { PLAYER1, PLAYER2, PLAYER3, PLAYER4 };

	const float axismax = 32767.0f; // needs to be a float to cheese some math. -Y
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
			// TODO: Add multi-controller support.
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
			conoutf(CON_INFO, "Disabled \"%s\" gamepad.", SDL_GameControllerName(controller));
			SDL_GameControllerClose(controller);
		}

		if (SDL_WasInit(SDL_INIT_GAMECONTROLLER))
			SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
	}

	VARP(joydeadzone, 0, 8000, axismax);

	float axis(const int value)
	{
		int val = value;
		if ((val > -joydeadzone) && (val < joydeadzone)) val = 0;
		const float scaled = -val / axismax;
		//conoutf(CON_DEBUG, "axis: %f (%f)", scaled, value);
		return scaled;
	}

	VAR(joydebug, 0, 0, 1);
	VARP(joytriggermode, 0, 0, 1);
	VARP(joytriggermax, 0, axismax / 2, axismax);

	void handletrigger(const SDL_ControllerAxisEvent &e)
	{
		bool active = (joytriggermode && e.value > joytriggermax) || (!joytriggermode && e.value > 0);
		if (joydebug) conoutf("trigger: %d", e.value);
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

	void handleaxis(const SDL_ControllerAxisEvent& e)
	{
		switch (e.axis)
		{
			case SDL_CONTROLLER_AXIS_LEFTX:
				player->fstrafe = axis(e.value);
				if (joydebug) conoutf("fstrafe: %f", player->fstrafe);
				break;
			case SDL_CONTROLLER_AXIS_LEFTY:
				player->fmove = axis(e.value);
				if (joydebug) conoutf("fmove: %f", player->fmove);
				break;
			case SDL_CONTROLLER_AXIS_RIGHTX:
				player->camx = -axis(e.value);
				if (joydebug) conoutf("axis ZX: %f", player->camx);
				break;
			case SDL_CONTROLLER_AXIS_RIGHTY:
				player->camy = -axis(e.value);
				if (joydebug) conoutf("axis ZY: %f", player->camy);
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
			if (joydebug) conoutf(CON_ERROR, "\foUnsupported gamepad button %d", sdlJoyButton);
			return 0;
		}
		return buttonsym - sdlJoyButton;
	}

	void handlebutton(const SDL_ControllerButtonEvent& e)
	{
		bool pressed = e.state == SDL_PRESSED;
		int symbol = buttonsymbol(e.button);
		if (symbol)
		{
			if (joydebug) conoutf(CON_DEBUG, "button %d: %s %s", symbol, SDL_GameControllerGetStringForButton((SDL_GameControllerButton)e.button), pressed ? "PRESSED" : "RELEASE");
			processkey(symbol, pressed);
		}
	}

	void handleevent(const SDL_Event& e) {
		switch (e.type)
		{
			case SDL_CONTROLLERDEVICEADDED:
				if (joydebug) conoutf(CON_DEBUG, "DEVICEADDED cdevice.which = %d", e.cdevice.which);
				init();
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				if (joydebug) conoutf(CON_DEBUG, "DEVICEREMOVED cdevice.which = %d", e.cdevice.which);
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
	}

	void setenabled(const int enabled)
	{
		if(enabled) init();
		else release();
	}

	VARFP(gamepad, 0, 0, 1, setenabled(gamepad));
}

