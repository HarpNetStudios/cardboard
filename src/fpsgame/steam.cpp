// Steamworks SDK

#include "game.h"
#include "engine.h"
#include "cube.h"

#ifdef STEAM

#include "steam/steam_api.h"
#include "steam.h"

namespace gamepad {
	extern int dbgjoy;
}

namespace steam {

	struct CSteamScreenshots
	{
	public:
		CSteamScreenshots();

		STEAM_CALLBACK(CSteamScreenshots, OnScreenshotRequested, ScreenshotRequested_t,
			m_CallbackScreenshotRequested);
	};

	CSteamScreenshots::CSteamScreenshots() :
		m_CallbackScreenshotRequested(this, &CSteamScreenshots::OnScreenshotRequested)
	{
		SteamScreenshots()->HookScreenshots(true);
	}
	
	void CSteamScreenshots::OnScreenshotRequested(ScreenshotRequested_t* pCallback)
	{
		screenshot(NULL); // this is dumb, but apparently it works, so who cares -Y
	}

	struct CSteamAchievements
	{
	private:
		int64 m_iAppID; // Our current AppID
		Achievement_t* m_pAchievements; // Achievements data
		int m_iNumAchievements; // The number of Achievements

	public:
		CSteamAchievements(Achievement_t* Achievements, int NumAchievements);

		bool SetAchievement(const char* ID);

		STEAM_CALLBACK(CSteamAchievements, OnUserStatsStored, UserStatsStored_t,
			m_CallbackUserStatsStored);
		STEAM_CALLBACK(CSteamAchievements, OnAchievementStored, UserAchievementStored_t,
			m_CallbackAchievementStored);
	};

	CSteamAchievements::CSteamAchievements(Achievement_t* Achievements, int NumAchievements) :
		m_iAppID(0),
		m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored),
		m_CallbackAchievementStored(this, &CSteamAchievements::OnAchievementStored)
	{
		m_iAppID = SteamUtils()->GetAppID();
		m_pAchievements = Achievements;
		m_iNumAchievements = NumAchievements;
	}

	bool CSteamAchievements::SetAchievement(const char* ID)
	{
		SteamUserStats()->SetAchievement(ID);
		return SteamUserStats()->StoreStats();
	}

	void CSteamAchievements::OnUserStatsStored(UserStatsStored_t* pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (pCallback->m_nGameID == m_iAppID)
		{
			if (pCallback->m_eResult == k_EResultOK)
			{
				//conoutf("Stored stats for Steam");
			}
			else
			{
				//conoutf("StatsStored - failed, %d", pCallback->m_eResult);
			}
		}
	}

	void CSteamAchievements::OnAchievementStored(UserAchievementStored_t* pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (pCallback->m_nGameID == m_iAppID)
		{
			logoutf("[STEAM] Achievement \"%s\" stored.", pCallback->m_rgchAchievementName);
		}
	}

	// Achievement array which will hold data about the achievements and their state
	Achievement_t g_Achievements[] =
	{
			_ACH_ID(ACH_FIRST_LAUNCH, "Welcome to Carmine Impact!"),
			_ACH_ID(ACH_PLAY_ONLINE, "Branching Out"),
			_ACH_ID(ACH_CHAR_SARAH, "Sarah"),
			_ACH_ID(ACH_CHAR_AMBER, "Amber"),
			_ACH_ID(ACH_CHAR_ASHLEY, "Ashley"),
			_ACH_ID(ACH_CHAR_BEA, "Bea"),
			_ACH_ID(ACH_CHAR_SAVANNAH, "Savannah"),
			_ACH_ID(ACH_CHAR_EMILY, "Emily"),
			_ACH_ID(ACH_PLAY_BOTS, "Who needs friends?"),
	};

	// Global access to Achievements object
	CSteamAchievements* g_SteamAchievements = NULL;

	InputHandle_t *inputHandles = new InputHandle_t[STEAM_INPUT_MAX_COUNT];

	InputActionSetHandle_t playSetHandle;
	InputActionSetHandle_t menuSetHandle;

	InputDigitalActionHandle_t menuButtonHandle;

	InputDigitalActionHandle_t playActionHandles[numPlayActions] = {};
	InputDigitalActionHandle_t menuActionHandles[numMenuActions] = {};
	InputDigitalActionHandle_t customActionHandles[numCustomActions] = {};

	InputAnalogActionHandle_t moveHandle;
	InputAnalogActionHandle_t cameraMouseHandle;
	InputAnalogActionHandle_t cameraGamepadHandle;

	int initSteam()
	{
		if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid)) // Replace with your App ID
		{
			return 1;
		}

		if (!SteamAPI_Init())
		{
			fatal("Steam must be running to play this version of %s.\n\nPlease try again after opening Steam.", game::gametitle);
			return 1;
		}

		SteamInput()->Init(true);

		playSetHandle = SteamInput()->GetActionSetHandle("InGameControls");
		menuSetHandle = SteamInput()->GetActionSetHandle("MenuControls");

		input_getConnectedControllers();

		menuButtonHandle = SteamInput()->GetDigitalActionHandle("menu");

		for (int i = 0; i < numPlayActions; i++) playActionHandles[i] = SteamInput()->GetDigitalActionHandle(playActions[i]);
		for (int i = 0; i < numMenuActions; i++) menuActionHandles[i] = SteamInput()->GetDigitalActionHandle(menuActions[i]);
		for (int i = 0; i < numCustomActions; i++) customActionHandles[i] = SteamInput()->GetDigitalActionHandle(customActions[i]);

		moveHandle = SteamInput()->GetAnalogActionHandle("move");
		cameraMouseHandle = SteamInput()->GetAnalogActionHandle("camera_mouse");
		cameraGamepadHandle = SteamInput()->GetAnalogActionHandle("camera_gamepad");

		// temporary
		SteamInput()->ActivateActionSet(inputHandles[0], playSetHandle);

		g_SteamAchievements = new CSteamAchievements(g_Achievements, 8);

		return 0;
	}

	void cleanup() {
		// Shutdown Steam Input
		SteamInput()->Shutdown();
		// Shutdown Steam
		SteamAPI_Shutdown();
		// Delete the SteamAchievements object
		if (g_SteamAchievements) delete g_SteamAchievements;
	}

	void steamCallbacks() {
		SteamAPI_RunCallbacks();
	}

	void setAchievement(const char* achievement) {
		if(g_SteamAchievements) g_SteamAchievements->SetAchievement(achievement);
	}

	void setPlayerAchievement(int player) {
		if(player >= 0 && player <= 5) setAchievement(g_Achievements[ACH_CHAR_SARAH+player].m_pchAchievementID);
	}

	int getSteamID() {
		return SteamUser()->GetSteamID().ConvertToUint64();
	}

	void addScreenshot(const char* path, int width, int height)
	{
		SteamScreenshots()->AddScreenshotToLibrary(path, NULL, width, height);
	}

	/*
	void updatePresence(int gamestate, const char* modename, physent* d, bool force)
	{
		if ((globalgamestate != gamestate || force) && SteamUser()->BLoggedOn()) {
			old_string serverip;

			switch (gamestate)
			{
			case D_MENU:
				activity.SetState("In the menus");
				break;
			case D_PLAYING:
				activity.SetState("Offline");
				break;
			case D_SPECTATE:
				activity.SetState("Spectating");
				break;
			default:
				activity.SetState("SOMETHING BROKE");
				activity.SetDetails("Tell the #bugs channel about what you did!");
				activity.GetAssets().SetLargeImage("logo-large");
				activity.GetAssets().SetSmallImage("turkey-test");
				break;
			}
			if (gamestate != D_MENU) {
				const ENetAddress* address = connectedpeer();

				defformatstring(buffer, "%s", modename);
				activity.SetDetails(buffer);
				if (game::maplimit >= 0) {
					time_t curtime;
					uint32_t endtimedelta = (game::maplimit - lastmillis) / 1000;
					time(&curtime);
					activity.GetTimestamps().SetStart(0);
					activity.GetTimestamps().SetEnd((uint32_t)(curtime + endtimedelta));
				}
				else {
					activity.GetTimestamps().SetStart(0);
					activity.GetTimestamps().SetEnd(0);
				}
				activity.GetAssets().SetLargeImage(game::getclientmap());

				const char* mapName = game::getclientmap();
				const char* largeKey = "unknown-map";
				int len = sizeof(officialmaps) / sizeof(officialmaps[0]);

				for (int i = 0; i < len; ++i)
				{
					if (!strcmp(officialmaps[i], mapName)) largeKey = game::getclientmap();
				}

				activity.GetAssets().SetLargeImage(largeKey);

				defformatstring(largeText, "Map: %s", game::getclientmap());
				activity.GetAssets().SetLargeText(largeText);

				if (address) {
					if (enet_address_get_host_ip(address, serverip, sizeof(serverip)) >= 0)
					{
						conoutf(CON_DEBUG, "ip: %s", serverip);
						activity.SetState("Online");
						defformatstring(partykey, "%s %u", serverip, address->port);
						defformatstring(partyid, "S_%s", partykey);
						const char* b64key = b64_encode((unsigned char*)partykey, strlen(partykey));
						activity.GetParty().SetId(partyid);
						activity.GetParty().GetSize().SetCurrentSize(game::players.length());
						activity.GetParty().GetSize().SetMaxSize(game::players.length() + 1);
						activity.GetSecrets().SetJoin(b64key);
						conoutf(CON_DEBUG, "discord join secret: %s", b64key);
					}
					else {
						conoutf("address: %u, port: %u", address->host, address->port);
					}
				}
			}
			else
			{
				activity.GetTimestamps().SetStart(starttime);
				activity.GetTimestamps().SetEnd(0);
				activity.GetAssets().SetLargeImage("logo-large");
			}

			fpsent* pl = (fpsent*)d;

			if (pl) // if there is actually a player involved
			{
				defformatstring(icon, "player-%d", pl->playermodel);
				//conoutf("imagekey: %s, playermodel: %d", icon, playermodel);
				activity.GetAssets().SetSmallImage(icon);
				defformatstring(plname, "%s (%d)", pl->name, pl->frags);
				activity.GetAssets().SetSmallText(plname);
			}
			discordCore->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
				if (result != discord::Result::Ok) conoutf(CON_ERROR, "\f2Discord: Failed to update activity.");
				});
			globalgamestate = gamestate;
		}
	}
	*/

	int input_getConnectedControllers() {
		return SteamInput()->GetConnectedControllers(inputHandles);
	}

	void input_setColor(int controller, bvec color) {
		if (controller + 1 > sizeof(inputHandles) || controller < 0) return;
		ESteamInputType controllerType = SteamInput()->GetInputTypeForHandle(inputHandles[controller]);
		if (controllerType == k_ESteamInputType_SteamController) return; // Prevent Steam Controller LED from flickering -Y
		SteamInput()->SetLEDColor(inputHandles[controller], color.r, color.g, color.b, k_ESteamInputLEDFlag_SetColor);
	}

	void input_updateActions(int gamestate) {
		InputActionSetHandle_t handle = playSetHandle;

		switch (gamestate)
		{
			case ST_MENU:
				handle = menuSetHandle;
				break;
			case ST_PLAYING:
				handle = playSetHandle;
				break;
		}

		SteamInput()->ActivateActionSet(inputHandles[0], handle);
	}

	void input_registerBinds() {
		execute("bind SI_MENU [togglemainmenu]");
		execute("bind SI_ATTACK [attack]");
		execute("bind SI_JUMP [jump]");
		execute("bind SI_LAST_WEAP [weapon]");
		execute("bind SI_WEAP_C_UP [cycleweapon 0 1 2 3 4 5 6]");
		execute("bind SI_WEAP_C_DN [cycleweapon 6 5 4 3 2 1 0]");
		execute("bind SI_WEAP_FI [setweapon FI]");
		execute("bind SI_WEAP_AR [setweapon AR]");
		execute("bind SI_WEAP_SG [setweapon SG]");
		execute("bind SI_WEAP_RI [setweapon RI]");
		execute("bind SI_WEAP_CG [setweapon CG]");
		execute("bind SI_WEAP_RL [setweapon RL]");
		execute("bind SI_WEAP_GL [setweapon GL]");
		execute("bind SI_TAUNT [taunt]");
		execute("bind SI_ZOOM_TOGGLE [togglezoom]");
		execute("bind SI_ZOOM_HOLD [holdzoom]");
		execute("bind SI_SCOREBOARD [showscores]");
		execute("bind SI_DROP_FLAG [dropflag]");

		/*
		execute("bind SI_MENU_SELECT []");
		execute("bind SI_MENU_CANCEL []");
		execute("bind SI_MENU_UP []");
		execute("bind SI_MENU_DOWN []");
		execute("bind SI_MENU_LEFT []");
		execute("bind SI_MENU_RIGHT []");
		execute("bind SI_MENU_TAB_NEXT []");
		execute("bind SI_MENU_TAB_PREV []");
		*/
	}

	bool input_getDigitalAction(InputDigitalActionHandle_t action, bool ignoreActive) {
		InputDigitalActionData_t data = SteamInput()->GetDigitalActionData(inputHandles[0], action);
		return ((data.bActive || ignoreActive) && data.bState);
	}

	vec2 input_getAnalogAction(InputAnalogActionHandle_t action) {
		InputAnalogActionData_t data = SteamInput()->GetAnalogActionData(inputHandles[0], action);
		return vec2(data.x, data.y);
	}

	bool playActionsActive[numPlayActions] = {};
	bool menuActionsActive[numMenuActions] = {};
	bool customActionsActive[numCustomActions] = {};

	bool menuButtonActive = false;

	vec2 lastMoveStick = vec2(0,0);

	void input_checkController() {

		bool status = steam::input_getDigitalAction(menuButtonHandle, false);
		if (status && !menuButtonActive) {
			processkey(-2000, true);
			menuButtonActive = status;
		}
		else if (!status && menuButtonActive) {
			processkey(-2000, false);
			menuButtonActive = status;
		}

		for (int i = 0; i < numPlayActions; i++)
		{
			bool status = steam::input_getDigitalAction(playActionHandles[i]);
			if (status && !playActionsActive[i]) {
				processkey(-2001-i, true);
				playActionsActive[i] = status;
			}
			else if (!status && playActionsActive[i]) {
				processkey(-2001-i, false);
				playActionsActive[i] = status;
			}
		}

		vec2 moveStick = input_getAnalogAction(moveHandle);
		if (moveStick.y != lastMoveStick.y)
		{
			player->fmove = moveStick.y;
			lastMoveStick.y = moveStick.y;
		}
		if (moveStick.x != lastMoveStick.x)
		{
			player->fstrafe = -moveStick.x;
			lastMoveStick.x = moveStick.x;
		}

		// there are subtle differences between these, let the user decide -Y
		vec2 cameraStick = input_getAnalogAction(cameraGamepadHandle);
		player->camx = cameraStick.x;
		player->camy = -cameraStick.y;

		vec2 cameraMouse = input_getAnalogAction(cameraMouseHandle);
		if (cameraMouse.magnitude() > 0.01f) // fixes strange mouse behavior when controller plugged in -Y
			mousemove(cameraMouse.x, cameraMouse.y);

		for (int i = 0; i < numMenuActions; i++)
		{
			bool status = steam::input_getDigitalAction(menuActionHandles[i]);
			if (status && !menuActionsActive[i]) {
				processkey(-2100-i, true);
				menuActionsActive[i] = status;
			}
			else if (!status && menuActionsActive[i]) {
				processkey(-2100-i, false);
				menuActionsActive[i] = status;
			}
		}

		for (int i = 0; i < numCustomActions; i++)
		{
			bool status = steam::input_getDigitalAction(customActionHandles[i]);
			if (status && !customActionsActive[i]) {
				processkey(-3000-i, true);
				customActionsActive[i] = status;
			}
			else if (!status && customActionsActive[i]) {
				processkey(-3000-i, false);
				customActionsActive[i] = status;
			}
		}
		SteamInput()->RunFrame();
	}

	ICOMMAND(steaminput_binding, "", (), { SteamInput()->ShowBindingPanel(inputHandles[0]); });

	// this is fucking stupid, but i'm sleep deprived and this seemed like the easiest way -Y
	ICOMMAND(_steam_botmatch, "", (), { setAchievement("ACH_PLAY_BOTS"); });
}

#endif
#ifndef STEAM
ICOMMAND(_steam_botmatch, "", (), { return; });
#endif
