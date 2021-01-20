#include "game.h"

namespace game {
    vector<fragmessage> fragmessages; // oldest first, newest at the end

    VARP(hudfragmessages, 0, 1, 2);
    //VARFP(maxhudfragmessages, 1, 3, 10, { fragmessages.growbuf(maxhudfragmessages); fragmessages.shrink(maxhudfragmessages); });
    int maxhudfragmessages = 3; // for some reason, the above causes a crash.
    FVARP(hudfragmessageduration, 0, 3.0f, 10.0f); // in seconds
    VARP(hudfragmessagefade, 0, 1, 1);
    FVARP(hudfragmessagex, 0, 0.75f, 1.0f);
    FVARP(hudfragmessagey, 0, 0.25f, 1.0f);
    FVARP(hudfragmessagescale, 0.1f, 0.5f, 1.0f);
    VARP(hudfragmessagestackdir, -1, 0, 1); // -1 = upwards, 1 = downwards
    HVARP(hudfragmessagefilter, 0, 0x3800, 0x7FFFFFF); // default: all kills

    void addfragmessage(int contype, const char *aname, const char *vname, int gun)
    {
        if(!(contype&hudfragmessagefilter)) return;
        if(fragmessages.length()>=maxhudfragmessages) fragmessages.remove(0, fragmessages.length()-maxhudfragmessages+1);
        fragmessages.add(fragmessage(aname, vname, gun));
    }

    void clearfragmessages()
    {
        fragmessages.shrink(0);
    }

    void drawfragmessages(int w, int h)
    {
        if(fragmessages.empty()) return;

        float stepsize = (3*HICON_SIZE)/2;
        float stackdir = hudfragmessagestackdir ? hudfragmessagestackdir : (hudfragmessagey>0.5 ? 1 : -1); // default: towards closest edge
        vec2 origin = vec2(hudfragmessagex, hudfragmessagey).mul(vec2(w, h).div(hudfragmessagescale));

        pushhudmatrix();
        hudmatrix.scale(hudfragmessagescale, hudfragmessagescale, 1);
        flushhudmatrix();

        int showmillis = (int)(hudfragmessageduration*1000.0f);

        for(int i = fragmessages.length()-1; i>=0; i--)
        {
            fragmessage &m = fragmessages[i];

            if(lastmillis-m.fragtime > showmillis + (hudfragmessagefade ? 255 : 0))
            {
                // all messages before i are older, so remove all of them
                fragmessages.remove(0, i+1);
                break;
            }

            int alpha = 255 - max(0, lastmillis-m.fragtime-showmillis);

            vec2 drawposcenter = vec2(0, (fragmessages.length()-1-i)*stackdir*stepsize).add(origin);

            int tw, th; vec2 drawpos;
            if(m.attackername[0])
            {
                text_bounds(m.attackername, tw, th);
                drawpos = vec2(-2*(tw+HICON_SIZE), -th).div(2).add(drawposcenter);
                draw_text(m.attackername, drawpos.x, drawpos.y, 0xFF, 0xFF, 0xFF, alpha);
            }

            drawpos = vec2(drawposcenter).sub(HICON_SIZE / 2);
            gle::color(bvec(0xFF, 0xFF, 0xFF), alpha);
            drawicon(HICON_FIST + m.weapon, drawpos.x, drawpos.y);

            text_bounds(m.victimname, tw, th);
            drawpos = vec2(2*HICON_SIZE, -th).div(2).add(drawposcenter);
            draw_text(m.victimname, drawpos.x, drawpos.y, 0xFF, 0xFF, 0xFF, alpha);
        }

        pophudmatrix();
    }
}