// discord RPC

#include "game.h"
#include "engine.h"
#include "cube.h"

namespace discord
{
	void updatePresence(int gamestate, const char* modename, oldstring playername, int playermodel)
	{
		if (globalgamestate != gamestate) {
			oldstring partykey;
			DiscordRichPresence discordPresence;
			memset(&discordPresence, 0, sizeof(discordPresence));
			switch (gamestate)
			{
			case D_MENU:
				discordPresence.state = "In the menus";
				break;
			case D_PLAYING:
				discordPresence.state = "Playing";
				break;
			case D_SPECTATE:
				discordPresence.state = "Spectating";
				break;
			case D_QUITTING:
				discordPresence.state = "Quitting game";
				discordPresence.startTimestamp = 0;
				discordPresence.endTimestamp = 0;
				break;
			default:
				discordPresence.state = "SOMETHING BROKE";
				discordPresence.details = "Tell the #bugs channel about what you did!";
				discordPresence.largeImageKey = "logo-large";
				discordPresence.largeImageText = "Tell the #bugs channel about what you did!";
				discordPresence.smallImageKey = "turkey-test";
				discordPresence.smallImageText = "Tell the #bugs channel about what you did!";
				discordPresence.partyId = "";
				discordPresence.partySize = 0;
				discordPresence.partyMax = 0;
				discordPresence.joinSecret = "";
				discordPresence.spectateSecret = "";
				break;
			}
			if (gamestate != D_MENU) {
				const ENetAddress* address = connectedpeer();

				defformatstring(buffer, "Mode: %s", modename);
				discordPresence.details = buffer;
				if (game::maplimit >= 0) {
					time_t curtime;
					uint32_t endtimedelta = (game::maplimit - lastmillis) / 1000;
					time(&curtime);
					//conoutf("current time: %d", endtimedelta);
					//conoutf("time: %u, end match time: %u", (uint32_t)curtime, (uint32_t)curtime + (uint32_t)endtimedelta);
					discordPresence.startTimestamp = 0;
					discordPresence.endTimestamp = (uint32_t)(curtime + endtimedelta);
				}
				else {
					discordPresence.startTimestamp = 0;
					discordPresence.endTimestamp = 0;
				}
				discordPresence.largeImageKey = game::getclientmap();
				discordPresence.largeImageText = game::getclientmap();
				if (address) {
					if (enet_address_get_host_ip(address, partykey, sizeof(partykey)) >= 0)
					{
						//conoutf("%s:%d", partykey, address->port);
						defformatstring(newpartykey, "%s:%u", partykey, address->port);
						//conoutf(newpartykey);
						defformatstring(partyid, "%s%s", "srv:", newpartykey);
						discordPresence.partyId = partyid;
						discordPresence.partySize = game::players.length();
						discordPresence.partyMax = game::players.length();
						discordPresence.joinSecret = newpartykey;
					}
				}
				else {
					discordPresence.state = "Playing alone";
				}
			}
			else
			{
				discordPresence.details = "";
				discordPresence.startTimestamp = starttime;
				discordPresence.endTimestamp = 0;
				discordPresence.largeImageKey = "logo-large";
				discordPresence.largeImageText = "";
				discordPresence.partyId = "";
				discordPresence.partySize = 0;
				discordPresence.partyMax = 0;
				discordPresence.joinSecret = "";
				discordPresence.spectateSecret = "";
			}
			defformatstring(icon, "player-%d", playermodel);
			//conoutf("imagekey: %s, playermodel: %d", icon, playermodel);
			discordPresence.smallImageKey = icon;
			discordPresence.smallImageText = playername;
			Discord_UpdatePresence(&discordPresence);
			globalgamestate = gamestate;
		}
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
		// This code never runs???
		conoutf("Discord: join (%s)\n", secret);

		// split at colon
		char* glob = (char*)secret; // make a temp value i can change
		char* addr = strtok(glob, ":"); // give me everything before the colon
		char* port = strtok(NULL, " "); // use the rest as a port number

		// do le connect (i hope)
		conoutf("DEBUG: trying to connect to %s:%d", addr, atoi(port)); // leaving this in for live testing
		connectserv(addr, atoi(port), "");

		//disconnect();
	}

	static void handleDiscordSpectate(const char* secret)
	{
		conoutf("Discord: spectate (%s)\n", secret);
	}

	void handleDiscordJoinRequest(const DiscordUser* request)
	{
		// This code doesn't run either?
		int response = DISCORD_REPLY_YES;
		conoutf("Discord: join request from %s#%s - %s",
			request->username,
			request->discriminator,
			request->userId);
		Discord_Respond(request->userId, response);
	}
	void initDiscord()
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

		Discord_Initialize("623616609952464936", &handlers, 1, "0");
	}
}
