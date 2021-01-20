#ifndef __FRAGMESSAGES_H__
#define __FRAGMESSAGES_H__

#include "game.h"

namespace game {
    struct fragmessage {
        oldstring attackername, victimname;
        int weapon;
        int fragtime;

        fragmessage(const char *aname, const char *vname, int fragweapon)
        {
            copystring(attackername, aname ? aname : "");
            copystring(victimname, vname);
            weapon = fragweapon;
            fragtime = lastmillis;
        }
    };

    extern vector<fragmessage> fragmessages;
    extern int hudfragmessages;
    extern void addfragmessage(int contype, const char *aname, const char *vname, int gun);
    extern void clearfragmessages();
    extern void drawfragmessages(int w, int h);
}

#endif