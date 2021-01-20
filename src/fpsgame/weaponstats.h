#ifndef __WEAPONSTATS_H__
#define __WEAPONSTATS_H__

namespace game {
    extern void recordpotentialdamage(fpsent *shooter);
    extern bool shouldcountdamage(fpsent *attacker, fpsent *target);
    extern void recorddamage(fpsent *attacker, fpsent *target, int damage);
    extern int playerdamage(fpsent *p, int typ, int gun = -1);
    extern int playerdamagewasted(fpsent *p, int gun = -1);
    extern int playernetdamage(fpsent *p, int gun = -1);
    extern float playeraccuracy(fpsent *p, int gun = -1);
}

#endif