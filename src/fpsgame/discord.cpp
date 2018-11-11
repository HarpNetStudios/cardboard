// discord RPC

#include "engine.h"
#include "cube.h"

namespace discord
{

	char* dis_modeltoname(int model)
	{
		char* name;
		switch (model)
		{
			case 0:
				name = "player-test";
				break;
			case 1:
				name = "player-amber";
				break;
			case 2:
				name = "";
				break;
			case 3:
				name = "";
				break;
			case 4:
				name = "";
				break;
			case 5:
				name = "";
				break;
			default:
				name = "";
				break;
		}
		return name;
	}

	void dis_ass()
	{
		conoutf(game::getclientmap());
	}

	COMMAND(dis_ass,"N");

	void dis_updatepresence(int gamestate, const char* modename, string playername, int playermodel)
	{
		char buffer[128];
		DiscordRichPresence discordPresence;
		memset(&discordPresence, 0, sizeof(discordPresence));
		switch (gamestate)
		{
			case D_MENU:
				discordPresence.state = "In the menus";
				break;
			case D_PLAYING:
				discordPresence.state = "Playing the game";
				break;
			case D_SPECTATE:
				discordPresence.state = "Spectating";
				break;
			default:
				discordPresence.state = "SOMETHING BROKE";
				discordPresence.details = "Message Yellowberry#0483 about what you did!";
				discordPresence.largeImageKey = "logo-large";
				discordPresence.largeImageText = "Message Yellowberry#0483 about what you did!";
				discordPresence.smallImageKey = "turkey-text";
				discordPresence.smallImageText = "Message Yellowberry#0483 about what you did!";
				discordPresence.partyId = "";
				discordPresence.partySize = 0;
				discordPresence.partyMax = 0;
				discordPresence.joinSecret = "";
				discordPresence.spectateSecret = "";
				break;
		}
		if (gamestate != D_MENU) {
			formatstring(buffer, "Mode: %s", modename);
			discordPresence.details = buffer;
			discordPresence.startTimestamp = 0;
			discordPresence.endTimestamp = time(0) + 5 * 60;
			discordPresence.largeImageKey = game::getclientmap();
			discordPresence.largeImageText = game::getclientmap();
			discordPresence.smallImageKey = dis_modeltoname(playermodel);
			discordPresence.smallImageText = playername;
			discordPresence.partyId = "w00t!";
			discordPresence.partySize = 1;
			discordPresence.partyMax = 6;
			discordPresence.joinSecret = "join";
			discordPresence.spectateSecret = "spookytate";
		} 
		else 
		{ 
			discordPresence.details = "";
			discordPresence.startTimestamp = starttime;
			discordPresence.endTimestamp = 0;
			discordPresence.largeImageKey = "logo-large";
			discordPresence.largeImageText = "";
			discordPresence.partyId = "";
			discordPresence.partySize = NULL;
			discordPresence.partyMax = NULL;
			discordPresence.joinSecret = "";
			discordPresence.spectateSecret = "";
		}
		Discord_UpdatePresence(&discordPresence);
	}

	void handleDiscordReady(const DiscordUser* connectedUser)
	{
		conoutf("\nDiscord: connected to user %s#%s - %s",
			connectedUser->username,
			connectedUser->discriminator,
			connectedUser->userId);
	}

	void handleDiscordDisconnected(int errcode, const char* message)
	{
		conoutf("\nDiscord: disconnected (%d: %s)", errcode, message);
	}

	void handleDiscordError(int errcode, const char* message)
	{
		conoutf("\nDiscord: error (%d: %s)", errcode, message);
	}

	static void handleDiscordJoin(const char* secret)
	{
		conoutf("\nDiscord: join (%s)\n", secret);
	}

	void dis_initdiscord()
	{
		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		handlers.ready = handleDiscordReady;
		handlers.errored = handleDiscordError;
		handlers.disconnected = handleDiscordDisconnected;
		handlers.joinGame = handleDiscordJoin;

		// Discord_Initialize(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId)
		Discord_Initialize("436989367941070848", &handlers, 1, "0");
	}
}