#ifndef __FRAGMESSAGES_H__
#define __FRAGMESSAGES_H__

#include "game.h"

namespace game {
	struct fragmessage {
		cbstring attackername, victimname;
		int weapon;
		int fragtime;
		bool headshot;

		fragmessage(const char *aname, const char *vname, int fragweapon, bool fragheadshot)
		{
			copystring(attackername, aname ? aname : "");
			copystring(victimname, vname);
			weapon = fragweapon;
			fragtime = lastmillis;
			headshot = fragheadshot;
		}
	};

	extern vector<fragmessage> fragmessages;
	extern int hudfragmessages;
	extern void addfragmessage(int contype, const char *aname, const char *vname, int gun, bool fragheadshot = false);
	extern void clearfragmessages();
	extern void drawfragmessages(int w, int h);
}

#endif