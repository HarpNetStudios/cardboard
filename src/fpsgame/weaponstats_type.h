#ifndef __WEAPONSTATS_TYPE_H__
#define __WEAPONSTATS_TYPE_H__

#define NUM_MP_GUNS (GUN_SMG+1 - GUN_FIST)
#define guninrange(i) (i >= 0 && i < NUM_MP_GUNS)

enum { DMG_POTENTIAL = 0, DMG_DEALT, DMG_RECEIVED, NUM_DMG_TYPES };
#define dmgtypeinrange(i) (i >= 0 && i < NUM_DMG_TYPES)

struct weaponstats {
    int lastprojectile;

    int damage[NUM_DMG_TYPES][NUM_MP_GUNS];

    weaponstats()
    {
        reset();
    }

    void reset()
    {
        lastprojectile = -1;
        for (int i = 0; i < NUM_DMG_TYPES; i++)
            for (int j = 0; j < NUM_MP_GUNS; j++) 
                damage[i][j] = 0;
    }

    void add(int typ, int gun, int dmg)
    {
        if(!dmgtypeinrange(typ) || !guninrange(gun)) return;
        damage[typ][gun] += dmg;
        if(typ == DMG_POTENTIAL && (gun == GUN_RL || gun == GUN_GL)) lastprojectile = gun;
    }

    int get(int typ, int gun)
    {
        if(!dmgtypeinrange(typ)) return 0;
        if(guninrange(gun)) return damage[typ][gun];
        int total = 0;
        for (int i = 0; i < NUM_MP_GUNS; i++) total += damage[typ][i];
        return total;
    }

    int wasted(int gun)
    {
        return get(DMG_POTENTIAL, gun) - get(DMG_DEALT, gun);
    }

    int net(int gun)
    {
        return get(DMG_DEALT, gun) - get(DMG_RECEIVED, gun);
    }

    float accuracy(int gun)
    {
        float nonzerototal = max(1.0f, (float)get(DMG_POTENTIAL, gun));
        return min(100.0f, (float)get(DMG_DEALT, gun) / nonzerototal * 100.0f);
    }
};

#endif