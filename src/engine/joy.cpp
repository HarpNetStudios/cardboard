#include"engine.h"
namespace joystick
{
    bool isenabled = false;
    int activeindex = -1;
    SDL_Joystick *stick = NULL;
	
	enum
	{
		AXIS_X = 0, // left/right lean
		AXIS_Y, // forward/back lean
		AXIS_ZX, // X but second stick
		AXIS_ZY // Y but second stick
	};

	const float axismax = 32767.5f;
	const int buttonsym = -100;
	const int hatsym = -200;
	const int hatlimit = 4;

	void release()
	{
		if (stick)
		{
			if (SDL_JoystickGetAttached(stick))
				SDL_JoystickClose(stick);
			stick = NULL;
			activeindex = -1;
		}
		if (isenabled)
		{
			isenabled = false;
			SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
		}
	}

	void acquire(const int joystickindex)
    {
        if (!isenabled)
        {
            SDL_InitSubSystem(SDL_INIT_JOYSTICK);
            isenabled = true;
        }
        int count = SDL_NumJoysticks();
		if(count == 0)
		{
			conoutf(CON_ERROR, "\f3Failed to find any joysticks");
			release();
		}
        else if (count > joystickindex)
        {
            stick = SDL_JoystickOpen(joystickindex);
            if (!stick)
            {
                conoutf(CON_ERROR, "Failed to open joystick %d", joystickindex);
            }
            else
            {
                activeindex = joystickindex;
                conoutf
                    ( CON_DEBUG
                    , "Opened joystick \"%s\" with %d axes, %d buttons, %d hats"
                    , SDL_JoystickName(stick)
                    , SDL_JoystickNumAxes(stick)
                    , SDL_JoystickNumButtons(stick)
					, SDL_JoystickNumHats(stick)
                    );
            }
        }
        else
        {
            conoutf
                ( CON_ERROR
                , "Joystick is enabled and index %d selected, but there are only %d joysticks"
                , joystickindex
                , count
                );
        }
    }

	void setenabled(const int joystickindex)
    {
        if (joystickindex == activeindex) return;
        if (joystickindex >= 0)
        {
            acquire(joystickindex);
        }
        else
        {
            release();
        }
    }

	float axis(const int value)
	{
		const float scaled = -value / axismax;
		//conoutf(CON_DEBUG, "axis: %f (%f)", scaled, value);
		return scaled;
	}

    void handleaxis(const SDL_JoyAxisEvent &e)
    {
		switch (e.axis)
		{
		case AXIS_X:
			player->fstrafe = clamp(axis(e.value), -1.0f, 1.0f);
			//conoutf("fstrafe: %f", player->fstrafe);
			break;
		case AXIS_Y:
			player->fmove = clamp(axis(e.value), -1.0f, 1.0f);
			//conoutf("fmove: %f", player->fmove);
			break;

		// shit's fucked, fix and figure it out later -Y
		case AXIS_ZX:
			//joymove(-clamp(axis(e.value), -10.0f, 10.0f), 0);
			break;
		case AXIS_ZY:
			//joymove(0, -clamp(axis(e.value), -10.0f, 10.0f));
			break;
		}
    }

	int buttonsymbol(Uint8 sdlJoyButton)
	{
		if (sdlJoyButton >= buttonsym - hatsym)
		{
			conoutf(CON_ERROR, "Unsupported joystick button %d", sdlJoyButton);
			return 0;
		}
		return buttonsym - sdlJoyButton;
	}

	void handlebutton(const SDL_JoyButtonEvent& e)
	{
		int symbol = buttonsymbol(e.button);
		if (symbol)
		{
			processkey(symbol, e.state == SDL_PRESSED);
		}
	}

	void handlehat(const SDL_JoyHatEvent& e)
	{
		static int hatstates[hatlimit] =
		{ SDL_HAT_CENTERED, SDL_HAT_CENTERED, SDL_HAT_CENTERED, SDL_HAT_CENTERED };
		if (e.hat >= hatlimit)
		{
			conoutf(CON_ERROR, "Unsupported joystick hat %d", e.hat);
			return;
		}
		const int current = e.value;
		const int previous = hatstates[e.hat];

		static const int dirs[4] =
		{ SDL_HAT_UP, SDL_HAT_DOWN, SDL_HAT_LEFT, SDL_HAT_RIGHT };
		for (int i = 0; i < 4; i++)
		{
			const int dir = dirs[i];
			if ((previous & dir) != (current & dir))
			{
				int sym = hatsym - e.hat * 4 - i;
				processkey(sym, current & dir);
			}
		}
		hatstates[e.hat] = current;
	}

    void handleevent(const SDL_Event &e)
    {
        switch (e.type)
        {
        case SDL_JOYAXISMOTION:
            handleaxis(e.jaxis);
            break;
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
			handlebutton(e.jbutton);
            break;
		case SDL_JOYHATMOTION:
			handlehat(e.jhat);
        }
    }

    VARFP(joystick, -1, -1, 100, setenabled(joystick));
}
