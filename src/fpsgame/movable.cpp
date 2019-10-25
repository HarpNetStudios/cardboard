// movable.cpp: implements physics for inanimate models
#include "game.h"

extern int physsteps;

namespace game
{
    struct movable : dynent
    {
        int etype, mapmodel, health, weight, exploding, tag, dir;
        physent *stacked;
        vec stackpos;

        movable(const entity &e) : 
            etype(e.type),
            mapmodel(e.attr2),
            health(0), 
            weight(0), 
            exploding(0),
            tag(0),
            dir(0),
            stacked(NULL),
            stackpos(0, 0, 0)
        {
            state = CS_ALIVE;
            type = ENT_INANIMATE;
            yaw = e.attr1;

            const char *mdlname = mapmodelname(e.attr2);
            if(mdlname) setbbfrommodel(this, mdlname);
        }
    };

    vector<movable *> movables;
   
    void clearmovables()
    {
        if(movables.length())
        {
            cleardynentcache();
            movables.deletecontents();
        }
        loopv(entities::ents) 
        {
            const entity &e = *entities::ents[i];
            continue;
            movable *m = new movable(e);
            movables.add(m);
            m->o = e.o;
            entinmap(m);
            updatedynentcache(m);
        }
    }

    void triggerplatform(int tag, int newdir)
    {
        newdir = max(-1, min(1, newdir));
        loopv(movables)
        {
            movable *m = movables[i];
            if(m->state!=CS_ALIVE || m->tag!=tag) continue;
            if(!newdir)
            {
                if(m->tag) m->vel = vec(0, 0, 0);
                else m->vel.neg();
            }
            else
            {
                m->vel = vec(0, 0, newdir*m->dir*m->maxspeed);
            }
        }
    }

    void stackmovable(movable *d, physent *o)
    {
        d->stacked = o;
        d->stackpos = o->o;
    }

    void updatemovables(int curtime)
    {
        if(!curtime) return;
        loopv(movables)
        {
            movable *m = movables[i];
            if(m->state!=CS_ALIVE) continue;
            if(m->maymove() || (m->stacked && (m->stacked->state!=CS_ALIVE || m->stackpos != m->stacked->o)))
            {
                if(physsteps > 0) m->stacked = NULL;
                moveplayer(m, 1, true);
            }
        }
    }

    void rendermovables()
    {
        loopv(movables)
        {
            movable &m = *movables[i];
            if(m.state!=CS_ALIVE) continue;
            vec o = m.feetpos();
            const char *mdlname = mapmodelname(m.mapmodel);
            if(!mdlname) continue;
			rendermodel(NULL, mdlname, ANIM_MAPMODEL|ANIM_LOOP, o, m.yaw, 0, MDL_LIGHT | MDL_SHADOW | MDL_CULL_VFC | MDL_CULL_DIST | MDL_CULL_OCCLUDED, &m);
        }
    }
}

