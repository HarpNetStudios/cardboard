#ifndef __GAME_H__
#define __GAME_H__

#include "cube.h"

// console message types

enum
{
	CON_CHAT       = 1<<8,
	CON_TEAMCHAT   = 1<<9,
	CON_GAMEINFO   = 1<<10,
	CON_FRAG_SELF  = 1<<11,
	CON_FRAG_OTHER = 1<<12
};

// network quantization scale
#define DMF 16.0f                // for world locations
#define DNF 100.0f              // for normalized vectors
#define DVELF 1.0f              // for playerspeed based velocity vectors

enum                            // static entity types
{
	NOTUSED = ET_EMPTY,         // entity slot not in use in map
	LIGHT = ET_LIGHT,           // lightsource, attr1 = radius, attr2 = intensity
	MAPMODEL = ET_MAPMODEL,     // attr1 = angle, attr2 = idx
	PLAYERSTART,                // attr1 = angle, attr2 = team
	ENVMAP = ET_ENVMAP,         // attr1 = radius
	PARTICLES = ET_PARTICLES,
	MAPSOUND = ET_SOUND,
	SPOTLIGHT = ET_SPOTLIGHT,

	I_AMMO, I_HEALTH,

	RACE_START,
	RACE_FINISH,
	RACE_CHECKPOINT,            // attr1 = angle, attr2 = idx

	PH1, PH2, PH3,              // placeholder entities
	PH4, PH5, PH6,              //

	TELEPORT,                   // attr1 = idx, attr2 = model, attr3 = tag
	TELEDEST,                   // attr1 = angle, attr2 = idx

	PH7, PH8,                   // more placeholder entities

	JUMPPAD,                    // attr1 = zpush, attr2 = ypush, attr3 = xpush
	BASE,

	PH9, PH10, PH11,            // even more placeholder entities
	PH12, PH13,                 //

	FLAG,                       // attr1 = angle, attr2 = team
	MAXENTTYPES
};

enum
{
	TRIGGER_RESET = 0,
	TRIGGERING,
	TRIGGERED,
	TRIGGER_RESETTING,
	TRIGGER_DISAPPEARED
};

struct fpsentity : extentity
{
	int triggerstate, lasttrigger;
	
	fpsentity() : triggerstate(TRIGGER_RESET), lasttrigger(0) {} 
};


enum // gamemode flags
{
	M_TEAM       = 1<<0,
	M_NOITEMS    = 1<<1,
	M_NOAMMO     = 1<<2,
	M_INSTA      = 1<<3,
	M_TACTICS    = 1<<4,
	M_CAPTURE    = 1<<5,
	M_REGEN      = 1<<6,
	M_CTF        = 1<<7,
	M_PROTECT    = 1<<8,
	M_HOLD       = 1<<9,
	M_EDIT       = 1<<10,
	M_DEMO       = 1<<11,
	M_LOCAL      = 1<<12,
	M_LOBBY      = 1<<13,
	M_COLLECT    = 1<<14,
	M_PARKOUR    = 1<<15,
	M_GRENADE    = 1<<16,
	M_LMS        = 1<<17,
	M_ECTF       = 1<<18,
	M_R1CTF      = 1<<19,
	M_TEST       = 1<<20,
	M_BOTTOMLESS = 1<<21,
	M_RACE       = 1<<22,
};

static struct gamemodeinfo
{
	const char *name;
	int flags;
	const char *info;
} gamemodes[] =
{
	// classic modes

	{ "Demo", M_DEMO | M_LOCAL, NULL},
	{ "Free For All", M_LOBBY, "Collect items for ammo. Frag everyone to score points." },
	{ "Cooperative Editing", M_EDIT, "Edit maps with multiple players simultaneously." },
	{ "Team Deathmatch", M_TEAM, "Collect items for ammo. Frag the enemy team to score points for your team." },
	{ "Instagib", M_NOITEMS | M_INSTA, "You spawn with full rifle ammo and die instantly from one shot. There are no items. Frag everyone to score points." },
	{ "Team Instagib", M_NOITEMS | M_INSTA | M_TEAM, "You spawn with full rifle ammo and die instantly from one shot. There are no items. Frag the enemy team to score points for your team." },
	{ "Tactics", M_NOITEMS | M_TACTICS, "You spawn with two random weapons and armour. There are no items. Frag everyone to score points." },
	{ "Team Tactics", M_NOITEMS | M_TACTICS | M_TEAM, "You spawn with two random weapons and armour. There are no items. Frag the enemy team to score points for your team." },
	{ "Base Capture", M_NOAMMO | M_TACTICS | M_CAPTURE | M_TEAM, "Capture neutral bases or steal enemy bases by standing next to them. Your team scores points for every 10 seconds it holds a base. You spawn with two random weapons and armour. Collect extra ammo that spawns at your bases. There are no ammo items." },
	{ "Regen Base Capture", M_NOITEMS | M_CAPTURE | M_REGEN | M_TEAM, "Capture neutral bases or steal enemy bases by standing next to them. Your team scores points for every 10 seconds it holds a base. Regenerate health and ammo by standing next to your bases. There are no items." },
	{ "Capture The Flag", M_CTF | M_TEAM, "Capture the enemy flag and bring it back to your flag to score points for your team. Collect items for ammo." },
	{ "Instagib Capture The Flag", M_NOITEMS | M_INSTA | M_CTF | M_TEAM, "Capture the enemy flag and bring it back to your flag to score points for your team. You spawn with full rifle ammo and die instantly from one shot. There are no items." },
	{ "Protect The Flag", M_CTF | M_PROTECT | M_TEAM, "Touch the enemy flag to score points for your team. Pick up your flag to protect it. Your team loses points if a dropped flag resets. Collect items for ammo." },
	{ "Instagib Protect The Flag", M_NOITEMS | M_INSTA | M_CTF | M_PROTECT | M_TEAM, "Touch the enemy flag to score points for your team. Pick up your flag to protect it. Your team loses points if a dropped flag resets. You spawn with full rifle ammo and die instantly from one shot. There are no items." },
	{ "Hold The Flag", M_CTF | M_HOLD | M_TEAM, "Hold the flag for 20 seconds to score points for your team. Collect items for ammo." },
	{ "Instagib Hold The Flag", M_NOITEMS | M_INSTA | M_CTF | M_HOLD | M_TEAM, "Hold the flag for 20 seconds to score points for your team. You spawn with full rifle ammo and die instantly from one shot. There are no items." },
	{ "Skull Collector", M_COLLECT | M_TEAM, "Frag the enemy team to drop skulls. Collect them and bring them to the enemy base to score points for your team or steal back your skulls. Collect items for ammo." },
	{ "Instagib Skull Collector", M_NOITEMS | M_INSTA | M_COLLECT | M_TEAM, "Frag the enemy team to drop skulls. Collect them and bring them to the enemy base to score points for your team or steal back your skulls. You spawn with full rifle ammo and die instantly from one shot. There are no items." },
	
	// new modes

	{ "Parkour", M_NOITEMS | M_BOTTOMLESS | M_PARKOUR, "Jump up, jump up and get down!" },
	{ "Instagib Tactics", M_NOITEMS | M_TACTICS | M_INSTA, "You spawn with two random weapons and armour and die instantly from one shot. There are no items. Frag everyone to score points."},
	{ "Team Instagib Tactics", M_NOITEMS | M_TACTICS | M_INSTA | M_TEAM, "You spawn with two random weapons and armour and die instantly from one shot. There are no items. Frag the enemy team to score points for your team."},
	{ "Grenade Battle", M_NOITEMS | M_GRENADE, "You spawn with full grenade launcher ammo. There are no items. Frag everyone to score points."},
	{ "Last Man Standing", M_LMS, "You spawn with ten lives. Frag everyone to score points. Be the last one alive to win."},
	{ "Explosive CTF", M_INSTA | M_BOTTOMLESS | M_TEAM | M_CTF | M_NOITEMS | M_ECTF, "Rockets! Grenades! Instagib! CTF! Exciting!"},
	{ "Reverse One Flag CTF", M_CTF | M_TEAM | M_R1CTF, "Capture the center flag and return it to the enemy base to score points!"},
	{ "Test Mode", M_TEST, "It might be something stupid, or it might be cool. It also might crash your game."},
	{ "Race Mode", M_RACE | M_BOTTOMLESS | M_PARKOUR, "Get to the finish as fast as you can!" },
};

#define STARTGAMEMODE (-1)
#define NUMGAMEMODES ((int)(sizeof(gamemodes)/sizeof(gamemodes[0])))

#define m_valid(mode)          ((mode) >= STARTGAMEMODE && (mode) < STARTGAMEMODE + NUMGAMEMODES)
#define m_check(mode, flag)    (m_valid(mode) && gamemodes[(mode) - STARTGAMEMODE].flags&(flag))
#define m_checknot(mode, flag) (m_valid(mode) && !(gamemodes[(mode) - STARTGAMEMODE].flags&(flag)))
#define m_checkall(mode, flag) (m_valid(mode) && (gamemodes[(mode) - STARTGAMEMODE].flags&(flag)) == (flag))
#define m_checkonly(mode, flag, exclude) (m_valid(mode) && (gamemodes[(mode) - STARTGAMEMODE].flags&((flag)|(exclude))) == (flag))

#define m_noitems      (m_check(gamemode, M_NOITEMS))
#define m_noammo       (m_check(gamemode, M_NOAMMO|M_NOITEMS))
#define m_bottomless   (m_check(gamemode, M_BOTTOMLESS))
#define m_insta        (m_check(gamemode, M_INSTA))
#define m_tactics      (m_check(gamemode, M_TACTICS))
#define m_capture      (m_check(gamemode, M_CAPTURE))
#define m_capture_only (m_checkonly(gamemode, M_CAPTURE, M_REGEN))
#define m_regencapture (m_checkall(gamemode, M_CAPTURE | M_REGEN))
#define m_ctf          (m_check(gamemode, M_CTF))
#define m_ctf_only     (m_checkonly(gamemode, M_CTF, M_PROTECT | M_HOLD))
#define m_protect      (m_checkall(gamemode, M_CTF | M_PROTECT))
#define m_hold         (m_checkall(gamemode, M_CTF | M_HOLD))
#define m_collect      (m_check(gamemode, M_COLLECT))
#define m_teammode     (m_check(gamemode, M_TEAM))
#define isteam(a,b)    (m_teammode && strcmp(a, b)==0)

#define m_parkour      (m_check(gamemode, M_PARKOUR))
#define m_grenade      (m_check(gamemode, M_GRENADE))
#define m_lms          (m_check(gamemode, M_LMS))
#define m_ectf         (m_checkall(gamemode, M_CTF | M_ECTF))
#define m_r1ctf        (m_checkall(gamemode, M_CTF | M_R1CTF))
#define m_test         (m_check(gamemode, M_TEST))
#define m_race         (m_check(gamemode, M_RACE))

#define m_demo         (m_check(gamemode, M_DEMO))
#define m_edit         (m_check(gamemode, M_EDIT))
#define m_lobby        (m_check(gamemode, M_LOBBY))
#define m_timed        (m_checknot(gamemode, M_DEMO | M_EDIT | M_LOCAL | M_PARKOUR))
#define m_botmode      (m_checknot(gamemode, M_DEMO | M_LOCAL))
#define m_mp(mode)     (m_checknot(mode, M_LOCAL))

enum { MM_AUTH = -1, MM_OPEN = 0, MM_VETO, MM_LOCKED, MM_PRIVATE, MM_PASSWORD, MM_START = MM_AUTH };

static const char * const mastermodenames[] =  { "auth",   "open",   "veto",       "locked",     "private",    "password" };
static const char * const mastermodecolors[] = { "",       "\f0",    "\f2",        "\f2",        "\f3",        "\f3" };
static const char * const mastermodeicons[] =  { "server", "server", "serverlock", "serverlock", "serverpriv", "serverpriv" };

// hardcoded sounds, defined in sounds.cfg
// TODO: hardcoded sounds suck, fix this
enum
{
	S_LOGO = 0,
	S_JUMP, S_LAND, S_RIFLE, S_PUNCH1, S_SG, S_CG, S_ARIFLE,
	S_RLFIRE, S_RLHIT, S_WEAPLOAD, S_ITEMAMMO, S_ITEMHEALTH,
	S_ITEMPUP, S_ITEMSPAWN, S_TELEPORT, S_NOAMMO, S_PUPOUT,
	
	S_PAIN_SARAH_1, S_PAIN_SARAH_2, S_PAIN_SARAH_3, S_PAIN_SARAH_4, S_PAIN_SARAH_5, S_PAIN_SARAH_6,
	S_PAIN_AMBER_1, S_PAIN_AMBER_2, S_PAIN_AMBER_3, S_PAIN_AMBER_4, S_PAIN_AMBER_5, S_PAIN_AMBER_6,
	S_PAIN_ASHLEY_1, S_PAIN_ASHLEY_2, S_PAIN_ASHLEY_3, S_PAIN_ASHLEY_4, S_PAIN_ASHLEY_5, S_PAIN_ASHLEY_6,
	S_PAIN_BEA_1, S_PAIN_BEA_2, S_PAIN_BEA_3, S_PAIN_BEA_4, S_PAIN_BEA_5, S_PAIN_BEA_6,
	S_PAIN_SAVANNAH_1, S_PAIN_SAVANNAH_2, S_PAIN_SAVANNAH_3, S_PAIN_SAVANNAH_4, S_PAIN_SAVANNAH_5, S_PAIN_SAVANNAH_6,
	S_PAIN_EMILY_1, S_PAIN_EMILY_2, S_PAIN_EMILY_3, S_PAIN_EMILY_4, S_PAIN_EMILY_5, S_PAIN_EMILY_6,

	S_DIE_SARAH_1, S_DIE_SARAH_2,
	S_DIE_AMBER_1, S_DIE_AMBER_2,
	S_DIE_ASHLEY_1, S_DIE_ASHLEY_2,
	S_DIE_BEA_1, S_DIE_BEA_2,
	S_DIE_SAVANNAH_1, S_DIE_SAVANNAH_2,
	S_DIE_EMILY_1, S_DIE_EMILY_2,

	S_FLAUNCH, S_FEXPLODE,
	S_SPLASH1, S_SPLASH2,
	S_RUMBLE, S_JUMPPAD,

	S_V_BASECAP, S_V_BASELOST,
	S_V_FIGHT,
	S_V_BOOST, S_V_BOOST10,
	S_V_QUAD, S_V_QUAD10,
	S_V_RESPAWNPOINT,

	S_FLAGPICKUP,
	S_FLAGDROP,
	S_FLAGRETURN,
	S_FLAGSCORE,
	S_FLAGRESET,
	S_FLAGFAIL,

	S_BURN,
	S_CHAINSAW_ATTACK,
	S_CHAINSAW_IDLE,

	S_HIT,
	S_KILL,

	S_ALLCHAT,
	S_TEAMCHAT,

	S_SRV_CONNECT,
	S_SRV_DISCONNECT,
};

// network messages codes, c2s, c2c, s2c

enum { PRIV_NONE = 0, PRIV_MASTER, PRIV_AUTH, PRIV_ADMIN };

enum
{
	N_CONNECT = 0, N_SERVINFO, N_WELCOME, N_INITCLIENT, N_POS, N_TEXT, N_SOUND, N_CDIS,
	N_SHOOT, N_EXPLODE, N_SUICIDE,
	N_DIED, N_DAMAGE, N_HITPUSH, N_SHOTFX, N_EXPLODEFX,
	N_TRYSPAWN, N_SPAWNSTATE, N_SPAWN, N_FORCEDEATH,
	N_GUNSELECT, N_TAUNT,
	N_MAPCHANGE, N_MAPVOTE, N_TEAMINFO, N_ITEMSPAWN, N_ITEMPICKUP, N_ITEMACC, N_TELEPORT, N_JUMPPAD,
	N_PING, N_PONG, N_CLIENTPING,
	N_TIMEUP, N_FORCEINTERMISSION,
	N_SERVMSG, N_ITEMLIST, N_RESUME,
	N_EDITMODE, N_EDITENT, N_EDITF, N_EDITT, N_EDITM, N_FLIP, N_COPY, N_PASTE, N_ROTATE, N_REPLACE, N_DELCUBE, N_REMIP, N_EDITVSLOT, N_UNDO, N_REDO, N_NEWMAP, N_GETMAP, N_SENDMAP, N_CLIPBOARD, N_EDITVAR,
	N_MASTERMODE, N_KICK, N_CLEARBANS, N_CURRENTMASTER, N_SPECTATOR, N_SETMASTER, N_SETTEAM,
	N_BASES, N_BASEINFO, N_BASESCORE, N_REPAMMO, N_BASEREGEN, N_ANNOUNCE,
	N_LISTDEMOS, N_SENDDEMOLIST, N_GETDEMO, N_SENDDEMO,
	N_DEMOPLAYBACK, N_RECORDDEMO, N_STOPDEMO, N_CLEARDEMOS,
	N_TAKEFLAG, N_RETURNFLAG, N_RESETFLAG, N_INVISFLAG, N_TRYDROPFLAG, N_DROPFLAG, N_SCOREFLAG, N_INITFLAGS,
	N_SAYTEAM,
	N_CLIENT,
	N_AUTHTRY, N_AUTHKICK, N_AUTHCHAL, N_AUTHANS, N_REQAUTH,
	N_PAUSEGAME, N_GAMESPEED,
	N_ADDBOT, N_DELBOT, N_INITAI, N_FROMAI, N_BOTLIMIT, N_BOTBALANCE,
	N_MAPCRC, N_CHECKMAPS,
	N_SWITCHNAME, N_SWITCHMODEL, N_SWITCHTEAM,
	N_INITTOKENS, N_TAKETOKEN, N_EXPIRETOKENS, N_DROPTOKENS, N_DEPOSITTOKENS, N_STEALTOKENS,
	N_SERVCMD,
	N_DEMOPACKET,
	N_SPAWNLOC,
	N_RACESTART, N_RACEFINISH, N_RACECHECKPOINT, N_RACELAP, N_RACEINFO,
	NUMMSG
};

static const int msgsizes[] =               // size inclusive message token, 0 for variable or not-checked sizes
{
	N_CONNECT, 0, N_SERVINFO, 0, N_WELCOME, 1, N_INITCLIENT, 0, N_POS, 0, N_TEXT, 0, N_SOUND, 2, N_CDIS, 2,
	N_SHOOT, 0, N_EXPLODE, 0, N_SUICIDE, 1,
	N_DIED, 7, N_DAMAGE, 7, N_HITPUSH, 7, N_SHOTFX, 10, N_EXPLODEFX, 4,
	N_TRYSPAWN, 1, N_SPAWNSTATE, 14, N_SPAWN, 3, N_FORCEDEATH, 2,
	N_GUNSELECT, 2, N_TAUNT, 1,
	N_MAPCHANGE, 0, N_MAPVOTE, 0, N_TEAMINFO, 0, N_ITEMSPAWN, 2, N_ITEMPICKUP, 2, N_ITEMACC, 3,
	N_PING, 2, N_PONG, 2, N_CLIENTPING, 2,
	N_TIMEUP, 2, N_FORCEINTERMISSION, 1,
	N_SERVMSG, 0, N_ITEMLIST, 0, N_RESUME, 0,
	N_EDITMODE, 2, N_EDITENT, 11, N_EDITF, 16, N_EDITT, 16, N_EDITM, 16, N_FLIP, 14, N_COPY, 14, N_PASTE, 14, N_ROTATE, 15, N_REPLACE, 17, N_DELCUBE, 14, N_REMIP, 1, N_EDITVSLOT, 16, N_UNDO, 0, N_REDO, 0, N_NEWMAP, 2, N_GETMAP, 1, N_SENDMAP, 0, N_EDITVAR, 0,
	N_MASTERMODE, 2, N_KICK, 0, N_CLEARBANS, 1, N_CURRENTMASTER, 0, N_SPECTATOR, 3, N_SETMASTER, 0, N_SETTEAM, 0,
	N_BASES, 0, N_BASEINFO, 0, N_BASESCORE, 0, N_REPAMMO, 1, N_BASEREGEN, 5, N_ANNOUNCE, 2,
	N_LISTDEMOS, 1, N_SENDDEMOLIST, 0, N_GETDEMO, 3, N_SENDDEMO, 0,
	N_DEMOPLAYBACK, 3, N_RECORDDEMO, 2, N_STOPDEMO, 1, N_CLEARDEMOS, 2,
	N_TAKEFLAG, 3, N_RETURNFLAG, 4, N_RESETFLAG, 6, N_INVISFLAG, 3, N_TRYDROPFLAG, 1, N_DROPFLAG, 7, N_SCOREFLAG, 10, N_INITFLAGS, 0,
	N_SAYTEAM, 0,
	N_CLIENT, 0,
	N_AUTHTRY, 0, N_AUTHKICK, 0, N_AUTHCHAL, 0, N_AUTHANS, 0, N_REQAUTH, 0,
	N_PAUSEGAME, 0, N_GAMESPEED, 0,
	N_ADDBOT, 2, N_DELBOT, 1, N_INITAI, 0, N_FROMAI, 2, N_BOTLIMIT, 2, N_BOTBALANCE, 2,
	N_MAPCRC, 0, N_CHECKMAPS, 1,
	N_SWITCHNAME, 0, N_SWITCHMODEL, 2, N_SWITCHTEAM, 0,
	N_INITTOKENS, 0, N_TAKETOKEN, 2, N_EXPIRETOKENS, 0, N_DROPTOKENS, 0, N_DEPOSITTOKENS, 2, N_STEALTOKENS, 0,
	N_SERVCMD, 0,
	N_DEMOPACKET, 0,
	N_SPAWNLOC, 0,
	N_RACESTART, 0, N_RACEFINISH, 0, N_RACECHECKPOINT, 2, N_RACELAP, 2, N_RACEINFO, 7,
	-1
};

#define CARDBOARD_LANINFO_PORT 34999
#define CARDBOARD_SERVER_PORT 35000
#define CARDBOARD_SERVINFO_PORT 35001
#define CARDBOARD_MASTER_PORT 35002
#define PROTOCOL_VERSION 1012           // bump when protocol changes
#define DEMO_VERSION 2                  // bump when demo format changes
#define DEMO_MAGIC "CARDBOARD_DEMO"

struct demoheader
{
	char magic[14]; // must be the same length as the DEMO_MAGIC above
	int version, protocol;
};

#define MAXNAMELEN 20
#define MAXTEAMLEN 8

enum
{
	HICON_HEALTH = 0,

	HICON_FIST,
	HICON_ARIFLE,
	HICON_SG,
	HICON_RIFLE,
	HICON_CG,
	HICON_RL,
	HICON_GL,

	HICON_RED_FLAG,
	HICON_BLUE_FLAG,
	HICON_NEUTRAL_FLAG,

	HICON_TOKEN,

	HICON_FIST_OFF,

	HICON_X       = 20,
	HICON_Y       = 1650,
	HICON_TEXTY   = 1644,
	HICON_STEP    = 343,
	HICON_SIZE    = 120,
	HICON_SPACE   = 40
};

// all official maps, used for savemap

static const char* officialmaps[] =
{
	"city_block",
	"color",
	"concrete_hell"
	"cosmic",
	"duabo",
	"flux",
	"fz_burn",
	"hr",
	"indust1",
	"neo_falls",
	"neo_noir",
	"pirate_ship",
	"precipice",
	"reactor",
	"realm",
	"retrograde",
	"secondevermap",
	"siege",
	"wave",
	"zigguraut",
};

#include "weapon.h"

// TODO: get rid of this entirely
static struct itemstat { int add, max, sound; const char* name; int icon, info; } itemstats[] =
{
	{30,   120, S_ITEMAMMO,   "AM", HICON_ARIFLE, NUMGUNS},
	{250, 1000, S_ITEMHEALTH,  "H", HICON_HEALTH,      -1},
};

#include "ai.h"

// inherited by fpsent and server clients
struct fpsstate
{
	int health, maxhealth;
	int gunselect;
	int gunwait[NUMGUNS];
	int ammo[NUMGUNS];
	int aitype, skill;

	// race
	int racetime;
	int racelaps;
	int racecheckpoint;
	int racerank;
	int racestate;

	fpsstate() : maxhealth(1000), aitype(AI_NONE), skill(0) {}

	void baseammo(int gun, int k = 2, int scale = 1)
	{
		ammo[gun] = (guns[gun].ammoadd * k) / scale;
	}

	void addammo(int gun, int k = 1, int scale = 1)
	{
		ammo[gun] = min(ammo[gun] + (guns[gun].ammoadd * k) / scale, guns[gun].ammomax);
	}

	bool hasmaxammo(int gun)
	{
		return ammo[gun] >= guns[gun].ammomax;
	}

	bool canpickup(int type)
	{
		switch (type) {
			case RACE_START:
			case RACE_FINISH:
			case RACE_CHECKPOINT:
				return true;
			case I_AMMO:
			{
				int fulltypes = 0;
				loopi(NUMGUNS - 1)
				{
					if(hasmaxammo(i + 1)) fulltypes++;
				}
				return fulltypes != (NUMGUNS - 1);
			}
			case I_HEALTH:
				return health < maxhealth;
			default:
				return false;
		}
	}

	void pickup(int type)
	{
		switch (type) {
			case I_AMMO:
			{
				loopi(NUMGUNS - 1)
				{
					ammo[i + 1] = min(ammo[i + 1] + guns[i + 1].ammoadd, guns[i + 1].ammomax);
				}
				break;
			}
			case I_HEALTH:
				health = min(health + 250, maxhealth);
				break;
			default:
				break;
		}
	}

	void respawn()
	{
		maxhealth = 1000;
		health = maxhealth;

		loopi(NUMGUNS)
		{
			ammo[i] = gunwait[i] = 0;
		}
		gunselect = GUN_ARIFLE;
		ammo[GUN_FIST] = 1;
	}

	void spawnstate(int gamemode)
	{
		if (m_insta)
		{
			health = 1;
		}

		if (m_race)
		{
			racetime = 0;
			racelaps = 0;
			racecheckpoint = 0;
		}

		if (m_demo)
		{
			gunselect = GUN_FIST;
		}
		else if (m_ectf)
		{
			ammo[GUN_RL] = ammo[GUN_GL] = INT_MAX;
			gunselect = GUN_RL;
		}
		else if (m_bottomless) // I have my doubts about this high ammo count becoming a problem, hopefully it doesn't. -Y 05/13/19
		{
			ammo[GUN_SG] = INT_MAX;
			ammo[GUN_RIFLE] = INT_MAX;
			ammo[GUN_CG] = INT_MAX;
			ammo[GUN_RL] = INT_MAX;
			ammo[GUN_GL] = INT_MAX;
			gunselect = GUN_RIFLE;
		}
		else if (m_tactics)
		{
			int spawngun1 = rnd(6) + 1, spawngun2;
			baseammo(spawngun1);
			do spawngun2 = rnd(6) + 1; while (spawngun1 == spawngun2);
			baseammo(spawngun2);
			gunselect = spawngun1;
		}
		else if (m_insta)
		{
			ammo[GUN_RIFLE] = 100;
			gunselect = GUN_RIFLE;
		}
		else if (m_regencapture)
		{
			ammo[GUN_ARIFLE] = 40;
			ammo[GUN_GL] = 1;
			gunselect = GUN_ARIFLE;
		}
		else if (m_grenade)
		{
			gunselect = GUN_GL;
			ammo[GUN_GL] = 100;
		}
		else
		{
			loopi(NUMGUNS - 1) baseammo(i + 1);
			gunselect = GUN_ARIFLE;
			ammo[GUN_ARIFLE] = 40;
		}

		if (m_parkour)
		{
			ammo[GUN_ARIFLE] = 0;
		}
	}

	// just subtract damage here, can set death, etc. later in code calling this
	int dodamage(int damage)
	{
		health -= damage;
		return damage;
	}

	int hasammo(int gun, int exclude = -1)
	{
		return gun >= 0 && gun <= NUMGUNS && gun != exclude && ammo[gun] > 0;
	}
};

#include "weaponstats_type.h"

struct userinfo
{
	enum statuscode
	{
		EMPTY = 0,
		OK,
		FAIL
	};

	old_string tags = "";
	int wskins[NUMGUNS];

	int status = EMPTY;
};

struct fpsent : dynent, fpsstate
{
	int weight;                         // affects the effectiveness of hitpush
	int clientnum, privilege, lastupdate, plag, ping;
	int lifesequence;                   // sequence id for each respawn, used in damage test
	int respawned, suicided;
	int lastpain;
	int lastgun;
	int lastaction[NUMGUNS];
	int lastattackgun;
	int lasthitpushgun;
	int ammotype; // TODO: what is this? poorly named
	bool attacking;
	int attacksound, attackchan, idlesound, idlechan;
	int lasttaunt;
	int lastpickup, lastpickupindex, lastpickupmillis, lastbase, lastrepammo, flagpickup, tokens;
	vec lastcollect;
	int frags, flags, deaths, totaldamage, totalshots;
	int suicides;
	weaponstats stats;

	editinfo *edit;
	float deltayaw, deltapitch, deltaroll, newyaw, newpitch, newroll;
	int smoothmillis;
	userinfo* user;

	old_string name, team, info, pubtoken;
	int playermodel;
	ai::aiinfo *ai;
	int ownernum, lastnode;

	float maxcps;

	fpsent* lastattacker;
	fpsent* lasthitpushattacker;

	vec muzzle;

	bool hasflag;

	fpsent() : weight(100), clientnum(-1), privilege(PRIV_NONE), lastupdate(0), plag(0), ping(0), lifesequence(0), respawned(-1), suicided(-1), lastpain(0), attacksound(-1), attackchan(-1), idlesound(-1), idlechan(-1), frags(0), flags(0), deaths(0), totaldamage(0), totalshots(0), suicides(0), edit(NULL), smoothmillis(-1), playermodel(-1), ai(NULL), ownernum(-1), muzzle(-1, -1, -1), hasflag(false), maxcps(0.0f)
	{
		name[0] = team[0] = info[0] = 0;
		user = new userinfo;
		respawn();
	}
	~fpsent()
	{
		freeeditinfo(edit);
		if(attackchan >= 0) stopsound(attacksound, attackchan);
		if(idlechan >= 0) stopsound(idlesound, idlechan);
		if(ai) delete ai;
	}

	void hitpush(int damage, const vec &dir, fpsent *actor, int gun)
	{
		// push players based on damage dealt
		// TODO: weight*10 to compensate for health unit change, more reasonable push values
		vec push(dir);
		push.mul((actor==this && guns[gun].exprad ? EXP_SELFPUSH : 0.06f) * guns[gun].hitpush * damage / weight);
		vel.add(push);

		falling.z = 0;
		lasthitpushgun = gun;
		lasthitpushattacker = actor;
	}

	void stopattacksound()
	{
		if(attackchan >= 0) stopsound(attacksound, attackchan, 250);
		attacksound = attackchan = -1;
	}

	void stopidlesound()
	{
		if(idlechan >= 0) stopsound(idlesound, idlechan, 100);
		idlesound = idlechan = -1;
	}

	void respawn()
	{
		dynent::reset();
		fpsstate::respawn();
		respawned = suicided = -1;
		loopi(NUMGUNS) lastaction[i] = 0;
		lastattackgun = gunselect;
		attacking = false;
		lasttaunt = 0;
		lastgun = GUN_FIST;
		lastpickup = -1;
		lastpickupmillis = 0;
		ammotype = gunselect; // TODO: what is this????
		lastbase = lastrepammo = -1;
		flagpickup = 0;
		tokens = 0;
		lastcollect = vec(-1e10f, -1e10f, -1e10f);

		lastnode = -1;
		maxcps = 0.0f;
		lastattacker = NULL;
		lasthitpushattacker = NULL;
		lasthitpushgun = -1;

		stopattacksound();
	}

	int respawnwait(int secs, int delay = 0)
	{
		return max(0, secs - (::lastmillis - lastpain - delay)/1000);
	}
};

struct teamscore
{
	const char *team;
	int score;
	teamscore() : team(""), score(0) {}
	teamscore(const char *s, int n) : team(s), score(n) {}

	static bool compare(const teamscore &x, const teamscore &y)
	{
		if(x.score > y.score) return true;
		if(x.score < y.score) return false;
		return strcmp(x.team, y.team) < 0;
	}
};

struct extclient {
	int clientnum, ping;
	old_string name, team;
	int frags, flags, deaths, accuracy, health, gunselect, privilege, state;
	uint ip;
	extclient()
	{
		clientnum = -1;
		ping = 0;
		frags = 0;
		flags = 0;
		deaths = 0;
		accuracy = 0;
		health = 0;
		gunselect = 0;
		privilege = 0;
		state = 0;
		ip = 0;
		copystring(name, "CardboardPlayer", MAXNAMELEN);
		copystring(team, "", MAXTEAMLEN);
	}
};

static inline uint hthash(const teamscore &t) { return hthash(t.team); }
static inline bool htcmp(const char *key, const teamscore &t) { return htcmp(key, t.team); }

#define MAXTEAMS 128

struct teaminfo
{
	char team[MAXTEAMLEN+1];
	int frags;
};

static inline uint hthash(const teaminfo &t) { return hthash(t.team); }
static inline bool htcmp(const char *team, const teaminfo &t) { return !strcmp(team, t.team); }

namespace entities
{
	extern vector<extentity *> ents;

	extern const char *entmdlname(int type);
	extern const char *itemname(int i);
	extern int itemicon(int i);

	extern void preloadentities();
	extern void renderentities();
	extern void resettriggers();
	extern void checktriggers();
	extern void checkitems(fpsent *d);
	extern void checkquad(int time, fpsent *d);
	extern void resetspawns();
	extern void spawnitems(bool force = false);
	extern void putitems(packetbuf &p);
	extern void setspawn(int i, bool on);
	extern void teleport(int n, fpsent *d);
	extern void pickupeffects(int n, fpsent *d);
	extern void teleporteffects(fpsent *d, int tp, int td, bool local = true);
	extern void jumppadeffects(fpsent *d, int jp, bool local = true);

	extern void repammo(fpsent* d, bool local = true);
}

namespace game
{
	struct scoregroup : teamscore
	{
		vector<fpsent*> players;
	};

	struct clientmode
	{
		virtual ~clientmode() {}

		virtual void preload() {}
		virtual int clipconsole(int w, int h) { return 0; }
		virtual void drawhud(fpsent *d, int w, int h) {}
		virtual void rendergame() {}
		virtual void respawned(fpsent *d) {}
		virtual void setup() {}
		virtual void checkitems(fpsent *d) {}
		virtual int respawnwait(fpsent *d, int delay = 0) { return 0; }
		virtual void pickspawn(fpsent* d) { findplayerspawn(d); }
		virtual int getspawngroup(fpsent *d) { return 0; }
		virtual float ratespawn(fpsent *d, const extentity &e) { return 1.0f; }
		virtual void senditems(packetbuf &p) {}
		virtual void removeplayer(fpsent *d) {}
		virtual void died(fpsent *victim, fpsent *actor) {}
		virtual void gameover() {}
		virtual bool hidefrags() { return false; }
		virtual int getteamscore(const char *team) { return 0; }
		virtual void getteamscores(vector<teamscore> &scores) {}
		virtual void aifind(fpsent *d, ai::aistate &b, vector<ai::interest> &interests) {}
		virtual bool aicheck(fpsent *d, ai::aistate &b) { return false; }
		virtual bool aidefend(fpsent *d, ai::aistate &b) { return false; }
		virtual bool aipursue(fpsent *d, ai::aistate &b) { return false; }
		virtual bool canfollow(const fpsent* spec, const fpsent* player) { return spec->state == CS_SPECTATOR; }
		virtual void renderscoreboard(g3d_gui& g, scoregroup& sg, int fgcolor, int bgcolor) {}
	};

	extern clientmode *cmode;
	extern void setclientmode();

	// fps
	extern int gamemode, nextmode;
	extern old_string clientmap;
	extern bool intermission;
	extern int maptime, maprealtime, maplimit;
	extern fpsent *player1;
	extern vector<fpsent *> players, clients;
	extern int lastspawnattempt;
	extern int lasthit;
	extern int respawnent;
	extern int following;
	extern int smoothmove, smoothdist;

	extern bool clientoption(const char *arg);
	extern bool getplayerinfo(fpsent* d);
	extern fpsent *getclient(int cn);
	extern fpsent *newclient(int cn);
	extern const char* colorname(fpsent* d, const char* name = NULL, const char* prefix = "", const char* suffix = "", const char* alt = NULL, bool tags = false, bool scoreboard = false);
	extern const char* teamcolorname(fpsent* d, const char* alt = "you", bool tags = false);
	extern const char *teamcolor(const char *name, bool sameteam, const char *alt = NULL);
	extern const char *teamcolor(const char *name, const char *team, const char *alt = NULL);
	extern bool spectating(physent* d);
	extern void teamsound(bool sameteam, int n, const vec* loc = NULL);
	extern void teamsound(fpsent* d, int n, const vec* loc = NULL);
	extern fpsent *pointatplayer();
	extern fpsent *hudplayer();
	extern fpsent *followingplayer(fpsent *fallback = NULL);
	extern void stopfollowing();
	extern void clientdisconnected(int cn, bool notify = true);
	extern void clearclients(bool notify = true);
	extern void startgame();
	extern float proximityscore(float x, float lower, float upper);
	extern void pickgamespawn(fpsent *d);
	extern void spawnplayer(fpsent *d);
	extern void deathstate(fpsent *d, bool restore = false);
	extern void damaged(int damage, fpsent *d, fpsent *actor, int gun, bool headshot = false, bool local = true);
	extern void killed(fpsent *d, fpsent *actor, int gun, bool headshot = false);
	extern void timeupdate(int timeremain);
	extern void msgsound(int n, physent *d = NULL);
	extern void drawicon(int icon, float x, float y, float sz = 120);
	const char *mastermodecolor(int n, const char *unknown);
	const char *mastermodeicon(int n, const char *unknown);

	// client
	extern bool connected, remote, demoplayback;
	extern old_string servdesc;
	extern vector<uchar> messages;
	extern void processservinfo();
	extern void checkinfoqueue();

	extern int parseplayer(const char *arg);
	extern void ignore(int cn);
	extern void unignore(int cn);
	extern bool isignored(int cn);
	extern bool addmsg(int type, const char *fmt = NULL, ...);
	extern void switchname(const char *name);
	extern void switchteam(const char *name);
	extern void switchplayermodel(int playermodel);
	extern void sendmapinfo();
	extern void stopdemo();
	extern void changemap(const char *name, int mode);
	extern void forceintermission();
	extern void c2sinfo(bool force = false);
	extern void sendposition(fpsent *d, bool reliable = false);
	extern void setpubtoken(const char* pubtoken);
	extern void checkflag();

	// extinfo
	extern void updateextinfo();
	extern extclient* getextclient(int clientnum);
	extern void resetextinfo();
	extern vector<extclient*> extclients;
	extern bool _extinfo;

	// weapon
	extern int getweapon(const char *name);
	extern const char* getweaponname(int gun);
	extern void shoot(fpsent *d, const vec &targ);
	extern void shoteffects(int gun, const vec &from, const vec &to, fpsent *d, bool local, int id, int prevaction);
	extern void explode(bool local, fpsent *owner, const vec &v, dynent *safe, int dam, int gun);
	extern void explodeeffects(int gun, fpsent *d, bool local, int id = 0);
	extern void damageeffect(int damage, fpsent *d, bool thirdperson = true);
	extern void gibeffect(int damage, const vec &vel, fpsent *d);
	extern float intersectdist;
	extern bool intersect(dynent *d, const vec &from, const vec &to, float &dist = intersectdist);
	extern dynent *intersectclosest(const vec &from, const vec &to, fpsent *at, float &dist = intersectdist);
	extern void clearbouncers();
	extern void updatebouncers(int curtime);
	extern void removebouncers(fpsent *owner);
	extern void renderbouncers();
	extern void clearprojectiles();
	extern void updateprojectiles(int curtime);
	extern void removeprojectiles(fpsent *owner);
	extern void renderprojectiles();
	extern void preloadbouncers();
	extern void removeweapons(fpsent *owner);
	extern void updateweapons(int curtime);
	extern void gunselect(int gun, fpsent *d);
	extern void weaponswitch(fpsent *d);
	extern void avoidweapons(ai::avoidset &obstacles, float radius);
	extern bool isheadshot(dynent* d, vec from, vec to);

	// scoreboard
	extern void showscores(bool on);
	extern void getbestplayers(vector<fpsent *> &best);
	extern void getbestteams(vector<const char *> &best);
	extern void clearteaminfo();
	extern void setteaminfo(const char *team, int frags);
	extern int statuscolor(fpsent *d, int color);
	extern int showclientnum;

	// render
	struct playermodelinfo
	{
		const char *ffa, *blueteam, *redteam, *hudguns, *vwep,
				   *ffaicon, *blueicon, *redicon;
		bool ragdoll;
	};

	extern int playermodel, teamskins, testteam;

	extern void saveragdoll(fpsent *d);
	extern void clearragdolls();
	extern void moveragdolls();
	extern void changedplayermodel();
	extern const playermodelinfo &getplayermodelinfo(fpsent *d);
	extern int chooserandomplayermodel(int seed);
	extern void swayhudgun(int curtime);
	extern vec hudgunorigin(int gun, const vec &from, const vec &to, fpsent *d);
}

#include "fragmessages.h"
#include "weaponstats.h"

namespace server
{
	extern const char *modename(int n, const char *unknown = "unknown");
	extern const char *mastermodename(int n, const char *unknown = "unknown");
	extern void startintermission();
	extern void stopdemo();
	extern void timeupdate(int secs);
	extern const char *getdemofile(const char *file, bool init);
	extern void forcemap(const char *map, int mode);
	extern void forcepaused(bool paused);
	extern void forcegamespeed(int speed);
	extern void hashpassword(int cn, int sessionid, const char *pwd, char *result, int maxlen = MAXSTRLEN);
	extern int msgsizelookup(int msg);
	extern bool serveroption(const char *arg);
}

#endif

