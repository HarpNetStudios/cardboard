static struct guninfo { int sound, attackdelay, damage, spread, projspeed, kickamount, range, rays, hitpush, exprad, ttl; const char* name, * file; short part; bool hassecond; int secondconsume; }

guns[NUMGUNS] =
{
	{ S_PUNCH1,     10,  400,   0,   0,  0,   14,  1,  80,  0,    0, "fist",            "fist",   0, false, 10 },
	{ S_ARIFLE,    150,  200,  50,   0,  5, 1024,  1,  80,  0,    0, "smg",             "smg",    0, false, 10 },
	{ S_SG,       1400,   80, 400,   0, 15,  512, 20,  80,  0,    0, "shotgun",         "shotg",  0, true, 10 },
	{ S_RIFLE,    1500,  900,   0,   0, 30, 2048,  1,  80,  0,    0, "rifle",           "rifle",  0, false, 10 },
	{ S_CG,        100,  150, 100,   0, 10,  512,  1,  80,  0,    0, "chaingun",        "chaing", 0, true, 10 },
	{ S_RLFIRE,    800, 1200,   0, 320, 10, 1024,  1, 160, 40,    0, "rocketlauncher",  "rocket", 0, true, 10 },
	{ S_FLAUNCH,  1000,  900,   0, 200, 10, 1024,  1, 250, 45, 1500, "grenadelauncher", "gl",     0, true, 10 },
	{ S_FLAUNCH,   200,   20,   0, 200,  1, 1024,  1,  80, 40,    0, "fireball",        NULL,     PART_FIREBALL1, true, 10 },
	{ S_ICEBALL,   200,   40,   0, 120,  1, 1024,  1,  80, 40,    0, "iceball",         NULL,     PART_FIREBALL2, true, 10 },
	{ S_SLIMEBALL, 200,   30,   0, 640,  1, 1024,  1,  80, 40,    0, "slimeball",       NULL,     PART_FIREBALL3, true, 10 },
	{ S_PIGR1,     250,   50,   0,   0,  1,   12,  1,  80,  0,    0, "bite",            NULL,     0, true, 10 },
	{ -1,            0,  120,   0,   0,  0,    0,  1,  80, 40,    0, "barrel",          NULL,     0, true, 10 }
};