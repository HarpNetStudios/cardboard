#include "game.h"

namespace game {
    void recordpotentialdamage(fpsent *shooter)
    {
        if(!shooter) return;
        int gun = shooter->gunselect;
        int potentialdamage = guns[gun].damage * guns[gun].rays;
        shooter->stats.add(DMG_POTENTIAL, gun, potentialdamage);
    }

    bool shouldcountdamage(fpsent *attacker, fpsent *target)
    {
        return (attacker != target) && !isteam(attacker->team, target->team);
    }

    void recorddamage(fpsent *attacker, fpsent *target, int damage)
    {
        if(!shouldcountdamage(attacker, target)) return;
        int gun = attacker->gunselect;
        // try to fix gun used to deal the damage
        if((gun != GUN_SG && damage != guns[gun].damage) || (gun == GUN_SG && damage%10 != 0))
        {
            gun = attacker->stats.lastprojectile;
        }
        attacker->stats.add(DMG_DEALT, gun, damage);
        target->stats.add(DMG_RECEIVED, gun, damage);
    }

    #define DMG_ICOMMAND(name,ret) ICOMMAND(name, "V", (tagval *args, int numargs), { \
        int gun = numargs > 0 ? args[0].getint() : -1; \
        fpsent *p = numargs > 1 ? getclient(args[1].getint()) : hudplayer(); \
        return ret;\
    })

    int playerdamage(fpsent *p, int typ, int gun)
    {
        if(!p) p = hudplayer();
        return p->stats.get(typ, gun);
    }
    DMG_ICOMMAND(getdamagepotential, intret(playerdamage(p, DMG_POTENTIAL, gun)))
    DMG_ICOMMAND(getdamagedealt, intret(playerdamage(p, DMG_DEALT, gun)))
    DMG_ICOMMAND(getdamagereceived, intret(playerdamage(p, DMG_RECEIVED, gun)))

    int playerdamagewasted(fpsent *p, int gun)
    {
        if(!p) p = hudplayer();
        return p->stats.wasted(gun);
    }
    DMG_ICOMMAND(getdamagewasted, intret(playerdamagewasted(p, gun)))

    int playernetdamage(fpsent *p, int gun)
    {
        if(!p) p = hudplayer();
        return p->stats.net(gun);
    }
    DMG_ICOMMAND(getnetdamage, intret(playernetdamage(p, gun)))

    float playeraccuracy(fpsent *p, int gun)
    {
        if(!p) p = hudplayer();
        return p->stats.accuracy(gun);
    }
    DMG_ICOMMAND(getaccuracy, floatret(playeraccuracy(p, gun)))
}