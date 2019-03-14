// discord RPC

#include "engine.h"
#include "cube.h"
#include "game.h"

namespace discord
{
	void dis_updatepresence(int gamestate, const char* modename, string playername, int playermodel)
	{
		char buffer[128];
		char gay[8];
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
				discordPresence.smallImageKey = "turkey-test";
				discordPresence.smallImageText = "Message Yellowberry#0483 about what you did!";
				discordPresence.partyId = "";
				discordPresence.partySize = 0;
				discordPresence.partyMax = 0;
				discordPresence.joinSecret = "";
				discordPresence.spectateSecret = "";
				break;
		}
		if (gamestate != D_MENU) {
			defformatstring(buffer, "Mode: %s", modename);
			discordPresence.details = buffer;
			discordPresence.startTimestamp = 0;
			discordPresence.endTimestamp = time(0) + 5 * 60;
			discordPresence.largeImageKey = game::getclientmap();
			discordPresence.largeImageText = game::getclientmap();
			defformatstring(gay, "player-%d", playermodel);
			//conoutf("shit: %s, balls: %d", gay, playermodel);
			discordPresence.smallImageKey = gay;
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
		conoutf("Discord: connected to user %s#%s - %s",
			connectedUser->username,
			connectedUser->discriminator,
			connectedUser->userId);
		//SDL_Quit();
		//exit(EXIT_SUCCESS);
	}

	void handleDiscordDisconnected(int errcode, const char* message)
	{
		conoutf("Discord: disconnected (%d: %s)", errcode, message);
	}

	void handleDiscordError(int errcode, const char* message)
	{
		conoutf("Discord: error (%d: %s)", errcode, message);
	}

	void handleDiscordJoin(const char* secret)
	{
		conoutf("Discord: join (%s)\n", secret);
	}

	static void handleDiscordSpectate(const char* secret)
	{
		conoutf("Discord: spectate (%s)\n", secret);
	}

	void handleDiscordJoinRequest(const DiscordUser* request)
	{
		int response = -1;
		//char yn[4];
		conoutf("Discord: join request from %s#%s - %s",
			request->username,
			request->discriminator,
			request->userId);
		/*do {
			conoutf("Accept? (y/n)");
			if (!prompt(yn, sizeof(yn))) {
				break;
			}

			if (!yn[0]) {
				continue;
			}

			if (yn[0] == 'y') {
				response = DISCORD_REPLY_YES;
				break;
			}

			if (yn[0] == 'n') {
				response = DISCORD_REPLY_NO;
				break;
			}
		} while (1);*/
		if (response != -1) {
			Discord_Respond(request->userId, response);
		}
	}
	void dis_initdiscord()
	{
		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		handlers.ready = handleDiscordReady;
		handlers.errored = handleDiscordError;
		handlers.disconnected = handleDiscordDisconnected;
		handlers.joinGame = handleDiscordJoin;
		handlers.joinRequest = handleDiscordJoinRequest;
		handlers.spectateGame = handleDiscordSpectate;

		// Discord_Initialize(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId)
		Discord_Initialize("436989367941070848", &handlers, 1, "0");
	}
}