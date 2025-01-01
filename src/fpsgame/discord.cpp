// Discord GameSDK

#include "game.h"
#include "engine.h"
#include "cube.h"

#ifdef DISCORD

discord::Core* discordCore{};

namespace discord
{
	void discordLogging(LogLevel level, const char* message) {
		conoutf(CON_DEBUG, "Discord: [debug] %s", message);
	}

	void initDiscord() {
		if (discord::connected()) return;
		discord::Result initStatus = discord::Core::Create(623616609952464936, DiscordCreateFlags_NoRequireDiscord, &discordCore);
		if(initStatus == discord::Result::Ok && discord::connected()) {
			#ifdef STEAM
				discordCore->ActivityManager().RegisterSteam(1359640);
			#endif
			discordCore->SetLogHook(LogLevel::Debug, discordLogging);
			conoutf(CON_DEBUG, "\f0Discord: Successfully connected.");
			discordCore->ActivityManager().OnActivityJoin.Connect([](const char* secret) {
				conoutf(CON_DEBUG, "Discord: Joining from Discord invite...");
				char* decoded = (char*)b64_decode(secret, strlen(secret));
				defformatstring(connectCommand, "connect %s", decoded);
				execute(connectCommand);
			});
			discordCore->ActivityManager().OnActivitySpectate.Connect([](const char* secret) {
				conoutf(CON_DEBUG, "Discord: Spectate: %s", secret); 
			});
			discordCore->ActivityManager().OnActivityJoinRequest.Connect([](discord::User const& user) {
				conoutf(CON_DEBUG, "Discord: Join Request from %s", user.GetUsername());
				// always accept, for testing reasons
				discordCore->ActivityManager().SendRequestReply(user.GetId(), discord::ActivityJoinRequestReply::Yes, [](discord::Result res) {
					if (res == discord::Result::Ok) conoutf(CON_DEBUG, "Discord: Accepted join request automatically");
				});
			});
		} else conoutf(CON_ERROR, "\f2Discord: Failed to initialize! Status code: %d", (int)initStatus);
	}

	bool connected() {
		if (!discordCore) return false;
		return discordCore->RunCallbacks() != discord::Result::NotRunning;
	}

	void updatePresence(int gamestate, const char* modename, physent* d, bool force)
	{
		if((globalgamestate != gamestate || force) && discord::connected()) {
			old_string serverip;
			discord::Activity activity{};

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
			if(gamestate != D_MENU) {
				const ENetAddress* address = connectedpeer();

				defformatstring(buffer, "%s", modename);
				activity.SetDetails(buffer);
				if(game::maplimit >= 0) {
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
					if(!strcmp(officialmaps[i], mapName)) largeKey = game::getclientmap();
				}

				activity.GetAssets().SetLargeImage(largeKey);

				defformatstring(largeText, "Map: %s", game::getclientmap());
				activity.GetAssets().SetLargeText(largeText);

				if(address) {
					if(enet_address_get_host_ip(address, serverip, sizeof(serverip)) >= 0)
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

			fpsent* pl = (fpsent*) d;

			if(pl) // if there is actually a player involved
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

	void discordCallbacks() {
		if(discord::connected()) discordCore->RunCallbacks();
	}

	void cleanup() {
		discordCore->ActivityManager().ClearActivity([](discord::Result result) {});
	}
}
#endif