// discord RPC

#include "engine.h"
#include "cube.h"

namespace discord
{
	void dis_updatepresence(int gamestate, const char* modename, string playername, int playermodel)
	{
		char buffer[128];
		DiscordRichPresence discordPresence;
		memset(&discordPresence, 0, sizeof(discordPresence));
		switch (gamestate) {
			case D_MENU:
				discordPresence.state = "In the menus";
				//formatstring(buffer, "Mode: %s | gamer: %s", modename, "one");
				//discordPresence.details = buffer;
				discordPresence.startTimestamp = starttime;
				discordPresence.largeImageKey = "logo-large";
				discordPresence.smallImageKey = "turkey-test";
				discordPresence.smallImageText = "It's a turkey! Wow!"; // turkey time baby
				discordPresence.partyId = "";
				discordPresence.partySize = NULL;
				discordPresence.partyMax = NULL;
				discordPresence.joinSecret = "";
				break;
			case D_PLAYING:
				discordPresence.state = "Playing the game";
				formatstring(buffer, "Mode: %s | gamer: %s", modename, "one");
				discordPresence.details = buffer;
				discordPresence.endTimestamp = time(0) + 5 * 60;
				discordPresence.largeImageKey = "logo-large";
				discordPresence.largeImageText = game::getclientmap();
				discordPresence.smallImageKey = "turkey-test";
				discordPresence.smallImageText = "It's a turkey! Wow!"; // maximum turkey
				discordPresence.partyId = "party1234";
				discordPresence.partySize = 1;
				discordPresence.partyMax = 6;
				discordPresence.joinSecret = "join";
				break;
			case D_SPECTATE:
				discordPresence.state = "Spectating";
				formatstring(buffer, "Mode: %s | gamer: %s", modename, "two");
				discordPresence.details = buffer;
				discordPresence.endTimestamp = time(0) + 5 * 60;
				discordPresence.largeImageKey = "turkey-test";
				discordPresence.largeImageText = game::getclientmap();
				discordPresence.smallImageKey = "logo-large";
				discordPresence.smallImageText = "It's a turkey!";
				discordPresence.partyId = "party1234";
				discordPresence.partySize = 1;
				discordPresence.partyMax = 6;
				discordPresence.joinSecret = "join";
				break;
			default:
				discordPresence.state = "SOMETHING BROKE";
				discordPresence.details = "";
				discordPresence.largeImageKey = "logo-large";
				discordPresence.largeImageText = "Message Yellowberry#0483 about what you did!";
				discordPresence.smallImageKey = "";
				discordPresence.smallImageText = "";
				discordPresence.partyId = "";
				discordPresence.partySize = 1;
				discordPresence.partyMax = 1;
				discordPresence.joinSecret = "join";
				break;
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