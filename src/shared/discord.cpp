// discord RPC

#include "engine.h"
#include "cube.h" // why is this required -Y
#include "game.h"

using namespace game;

void dis_updatepresence(int gamestate, const char* modename, fpsent* ent)
{
	char buffer[256];
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = "In a Group";
	sprintf(buffer, "Mode: %s | gamer: %s", modename);
	discordPresence.details = buffer;
	discordPresence.endTimestamp = time(0) + 5 * 60;
	discordPresence.largeImageKey = "logo-large";
	discordPresence.largeImageText = game::getclientmap();
	discordPresence.smallImageKey = "turkey-test";
	discordPresence.smallImageText = "It's a turkey!";
	discordPresence.partySize = 1;
	discordPresence.partyMax = 1337;
	discordPresence.instance = 1;
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

void dis_initdiscord()
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.errored = handleDiscordError;
	handlers.disconnected = handleDiscordDisconnected;

	// Discord_Initialize(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId)
	Discord_Initialize("436989367941070848", &handlers, 1, "0");
}