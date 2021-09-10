enum { GUN_FIST = 0, GUN_SMG, GUN_SG, GUN_RIFLE, GUN_CG, GUN_RL, GUN_GL, NUMGUNS };

#define MAXRAYS 20
//#define EXP_SELFDAMDIV 2

#define EXP_SELFPUSH .12f
#define EXP_DISTSCALE 1.5f

static struct guninfo { int sound, attackdelay, damage, specialdamage, spread, projspeed, kickamount, range, rays, hitpush, ammoadd, ammomax, exprad, ttl; const char* name, * file; short part; bool hassecond; int secondconsume; }

guns[NUMGUNS] =
{
	{ S_PUNCH1,     10,  400,  400,   0,   0,  0,   14,  1,  80,  0,   1,  0,    0, "fist",            "fist",   0, false, 10 },
	{ S_ARIFLE,    150,  200,  300,  50,   0,  5, 1024,  1,  80, 30, 120,  0,    0, "smg",             "smg",    0, false, 10 },
	{ S_SG,       1400,   80,  160, 400,   0, 15,  512, 20,  80, 10,  30,  0,    0, "shotgun",         "shotg",  0,  true, 10 },
	{ S_RIFLE,    1500,  900, 1800,   0,   0, 30, 2048,  1,  80,  5,  15,  0,    0, "rifle",           "rifle",  0, false, 10 },
	{ S_CG,        100,  150,  350, 100,   0, 10,  512,  1,  80, 50, 200,  0,    0, "chaingun",        "chaing", 0,  true, 10 },
	{ S_RLFIRE,    800, 1200,    0,   0, 320, 10, 1024,  1, 160,  5,  15, 40,    0, "rocketlauncher",  "rocket", 0,  true, 10 },
	{ S_FLAUNCH,  1000,  900,    0,   0, 200, 10, 1024,  1, 250, 10,  30, 45, 2000, "grenadelauncher", "gl",     0,  true, 10 }
};

static const int GRAPPLETHROWSPEED = 320;	// Same as rocket launcher
static const int GRAPPLEPULLSPEED = 3;
static const int GRAPPLEGUNDELAY = 5;
static const int GRAPPLESEPERATION = 16;
static const int GRAPPLEMAXLENGTH = 512;
