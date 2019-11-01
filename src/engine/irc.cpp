#include "engine.h"

vector<ircnet *> ircnets;

ircnet *ircfind(const char *name)
{
    if(name && *name)
    {
        loopv(ircnets) if(!strcmp(ircnets[i]->name, name)) return ircnets[i];
    }
    return NULL;
}

void ircestablish(ircnet *n)
{
    if(!n) return;
    n->lastattempt = totalmillis;
    if(n->address.host == ENET_HOST_ANY)
    {
        conoutf("looking up %s:[%d]...", n->serv, n->port);
        if(!resolverwait(n->serv, &n->address))
        {
            conoutf("unable to resolve %s:[%d]...", n->serv, n->port);
            n->state = IRC_DISC;
            return;
        }
    }

    ENetAddress address = { ENET_HOST_ANY,  (enet_uint16)n->port };
    if(*n->ip && enet_address_set_host(&address, n->ip) < 0) conoutf("failed to bind address: %s", n->ip);
    n->sock = enet_socket_create(ENET_SOCKET_TYPE_STREAM);
    if(n->sock != ENET_SOCKET_NULL && *n->ip && enet_socket_bind(n->sock, &address) < 0)
    {
        conoutf("failed to bind connection socket: %s", n->ip);
        address.host = ENET_HOST_ANY;
    }
    if(n->sock == ENET_SOCKET_NULL || connectwithtimeout(n->sock, n->serv, n->address) < 0)
    {
        conoutf(n->sock == ENET_SOCKET_NULL ? "could not open socket to %s:[%d]" : "could not connect to %s:[%d]", n->serv, n->port);
        if(n->sock != ENET_SOCKET_NULL)
        {
            enet_socket_destroy(n->sock);
            n->sock = ENET_SOCKET_NULL;
        }
        n->state = IRC_DISC;
        return;
    }
    n->state = IRC_ATTEMPT;
    conoutf("connecting to %s:[%d]...", n->serv, n->port);
}

void ircsend(ircnet *n, const char *msg, ...)
{
    if(!n) return;
    defvformatstring(str, msg, msg);
    if(n->sock == ENET_SOCKET_NULL) return;
    //there if(verbose >= 2) console(0, "[%s] >>> %s", n->name, str);
    conoutf(0, "[%s] >>> %s", n->name, str);
    concatstring(str, "\n");
    ENetBuffer buf;
    uchar ubuf[512];
    int len = strlen(str), carry = 0;
    while(carry < len)
    {
        int numu = encodeutf8(ubuf, sizeof(ubuf)-1, &((uchar *)str)[carry], len - carry, (size_t*)&carry);
        if(carry >= len) ubuf[numu++] = '\n';
        loopi(numu) switch(ubuf[i])
        {
            case '\v': ubuf[i] = '\x01'; break;
            case '\f': ubuf[i] = '\x03'; break;
        }
        buf.data = ubuf;
        buf.dataLength = numu;
        enet_socket_send(n->sock, NULL, &buf, 1);
    }
}

VAR(ircfilter, 0, 1, 2);

void converttext(char *dst, const char *src)
{
    int colorpos = 0; char colorstack[10];
    loopi(10) colorstack[i] = 'u'; //indicate user color
    for(int c = *src; c; c = *++src)
    {
        if(c == '\f')
        {
            c = *++src;
            if(c == 'z')
            {
                c = *++src;
                if(c) ++src;
            }
            else if(c == '[' || c == '(')
            {
                const char *end = strchr(src, c == '[' ? ']' : ')');
                src += end ? end-src : strlen(src);
            }
            else if(c == 's') { colorpos++; continue; }
            else if(c == 'S') { c = colorstack[--colorpos]; }
            int oldcolor = colorstack[colorpos]; colorstack[colorpos] = c;
            switch(c)
            {
                case 'g': case '0': case 'G': *dst++ = '\f'; *dst++ = '0'; *dst++ = '3'; break; // green
                case 'b': case '1': case 'B': *dst++ = '\f'; *dst++ = '1'; *dst++ = '2'; break; // blue
                case 'y': case '2': case 'Y': *dst++ = '\f'; *dst++ = '0'; *dst++ = '3'; break; // yellow
                case 'r': case '3': case 'R': *dst++ = '\f'; *dst++ = '0'; *dst++ = '4'; break; // red
                case 'a': case '4': *dst++ = '\f'; *dst++ = '1'; *dst++ = '4'; break; // grey
                case 'm': case '5': case 'M': *dst++ = '\f'; *dst++ = '1'; *dst++ = '3'; break; // magenta
                case 'o': case '6': case 'O': *dst++ = '\f'; *dst++ = '0'; *dst++ = '7'; break; // orange
                case 'c': case '9': case 'C': *dst++ = '\f'; *dst++ = '1'; *dst++ = '0'; break; // cyan
                case 'v': *dst++ = '\f'; *dst++ = '0'; *dst++ = '6'; break; // violet
                case 'p': *dst++ = '\f'; *dst++ = '0'; *dst++ = '6'; break; // purple
                case 'n': *dst++ = '\f'; *dst++ = '0'; *dst++ = '5'; break; // brown
                case 'd': case 'A': *dst++ = '\f'; *dst++ = '0'; *dst++ = '1'; break; // dark grey
                case 'u': case 'w': case '7': case 'k': case '8': *dst++ = '\f'; *dst++ = '0'; *dst++ = '0'; break;
                default: colorstack[colorpos] = oldcolor; break;
            }
            continue;
        }
        if(iscubeprint(c) || iscubespace(c)) *dst++ = c;
    }
    *dst = '\0';
}

void ircoutf(int relay, const char *msg, ...)
{
    defvformatstring(src, msg, msg);
	oldstring str;
	str[0] = 0;
    switch(ircfilter)
    {
        case 2: filtertext(str, src); break;
        case 1: converttext(str, src); break;
        case 0: default: copystring(str, src); break;
    }
    loopv(ircnets) if(ircnets[i]->sock != ENET_SOCKET_NULL && ircnets[i]->type == IRCT_RELAY && ircnets[i]->state == IRC_ONLINE)
    {
        ircnet *n = ircnets[i];
#if 0 // workaround for freenode's crappy dropping all but the first target of multi-target messages even though they don't state MAXTARGETS=1 in 005 string..
        mkstring(s);
        loopvj(n->channels) if(n->channels[j].state == IRCC_JOINED && n->channels[j].relay >= relay)
        {
            ircchan *c = &n->channels[j];
            if(s[0]) concatstring(s, ",");
            concatstring(s, c->name);
        }
        if(s[0]) ircsend(n, "PRIVMSG %s :%s", s, str);
#else
        loopvj(n->channels) if(n->channels[j].state == IRCC_JOINED && n->channels[j].relay >= relay)
            ircsend(n, "PRIVMSG %s :%s", n->channels[j].name, str);
#endif
    }
}

int ircrecv(ircnet *n)
{
    if(!n) return -1;
    if(n->sock == ENET_SOCKET_NULL) return -1;
    int total = 0;
    enet_uint32 events = ENET_SOCKET_WAIT_RECEIVE;
    while(enet_socket_wait(n->sock, &events, 0) >= 0 && events)
    {
        ENetBuffer buf;
        buf.data = n->input + n->inputlen;
        buf.dataLength = sizeof(n->input) - n->inputlen;
        int len = enet_socket_receive(n->sock, NULL, &buf, 1);
        if(len <= 0) return -1;
        loopi(len) switch(n->input[n->inputlen+i])
        {
            case '\x01': n->input[n->inputlen+i] = '\v'; break;
            case '\x03': n->input[n->inputlen+i] = '\f'; break;
            case '\v': case '\f': n->input[n->inputlen+i] = ' '; break;
        }
        n->inputlen += len;
        int carry = 0, decoded = decodeutf8(&n->input[n->inputcarry], n->inputlen - n->inputcarry, &n->input[n->inputcarry], n->inputlen - n->inputcarry, (size_t*)&carry);
        if(carry > decoded)
        {
            memmove(&n->input[n->inputcarry + decoded], &n->input[n->inputcarry + carry], n->inputlen - (n->inputcarry + carry));
            n->inputlen -= carry - decoded;
        }
        n->inputcarry += decoded;
        total += decoded;
    }
    return total;
}

void ircnewnet(int type, const char *name, const char *serv, int port, const char *nick, const char *ip, const char *passkey)
{
    if(!name || !*name || !serv || !*serv || !port || !nick || !*nick) return;
    ircnet *m = ircfind(name);
    if(m)
    {
        if(m->state != IRC_DISC) conoutf("ircnet %s already exists", m->name);
        else ircestablish(m);
        return;
    }
    ircnet &n = *ircnets.add(new ircnet);
    n.type = type;
    n.state = IRC_DISC;
    n.sock = ENET_SOCKET_NULL;
    n.port = port;
    n.lastattempt = 0;
    copystring(n.name, name);
    copystring(n.serv, serv);
    copystring(n.nick, nick);
    copystring(n.ip, ip);
    copystring(n.passkey, passkey);
    n.address.host = ENET_HOST_ANY;
    n.address.port = n.port;
    n.input[0] = n.authname[0] = n.authpass[0] = 0;
    conoutf("added irc %s %s (%s:%d) [%s]", type == IRCT_RELAY ? "relay" : "client", name, serv, port, nick);
}

ICOMMAND(ircaddclient, "ssisss", (const char *n, const char *s, int *p, const char *c, const char *h, const char *z), {
    ircnewnet(IRCT_CLIENT, n, s, *p, c, h, z);
});
ICOMMAND(ircaddrelay, "ssisss", (const char *n, const char *s, int *p, const char *c, const char *h, const char *z), {
    ircnewnet(IRCT_RELAY, n, s, *p, c, h, z);
});
ICOMMAND(ircserv, "ss", (const char *name, const char *s), {
    ircnet *n = ircfind(name);
    if(!n) { conoutf("no such ircnet: %s", name); return; }
    if(!s || !*s) { conoutf("%s current server is: %s", n->name, n->serv); return; }
    copystring(n->serv, s);
});
ICOMMAND(ircport, "ss", (const char *name, const char *s), {
    ircnet *n = ircfind(name);
    if(!n) { conoutf("no such ircnet: %s", name); return; }
    if(!s || !*s || !parseint(s)) { conoutf("%s current port is: %d", n->name, n->port); return; }
    n->port = parseint(s);
});
ICOMMAND(ircnick, "ss", (const char *name, const char *s), {
    ircnet *n = ircfind(name);
    if(!n) { conoutf("no such ircnet: %s", name); return; }
    if(!s || !*s) { conoutf("%s current nickname is: %s", n->name, n->nick); return; }
    copystring(n->nick, s);
});
ICOMMAND(ircbind, "ss", (const char *name, const char *s), {
    ircnet *n = ircfind(name);
    if(!n) { conoutf("no such ircnet: %s", name); return; }
    if(!s || !*s) { conoutf("%s currently bound to: %s", n->name, n->ip); return; }
    copystring(n->ip, s);
});
ICOMMAND(ircpass, "ss", (const char *name, const char *s), {
    ircnet *n = ircfind(name);
    if(!n) { conoutf("no such ircnet: %s", name); return; }
    if(!s || !*s) { conoutf("%s current password is: %s", n->name, n->passkey && *n->passkey ? "<set>" : "<not set>"); return; }
    copystring(n->passkey, s);
});
ICOMMAND(ircauth, "sss", (const char *name, const char *s, const char *t), {
    ircnet *n = ircfind(name);
    if(!n) { conoutf("no such ircnet: %s", name); return; }
    if(!s || !*s || !t || !*t) { conoutf("%s current authority is: %s (%s)", n->name, n->authname, n->authpass && *n->authpass ? "<set>" : "<not set>"); return; }
    copystring(n->authname, s);
    copystring(n->authpass, t);
});
ICOMMAND(ircconnect, "s", (const char *name), {
    ircnet *n = ircfind(name);
    if(!n) { conoutf("no such ircnet: %s", name); return; }
    if(n->state != IRC_DISC) { conoutf("ircnet %s is already connected", n->name); return; }
    ircestablish(n);
});

ircchan *ircfindchan(ircnet *n, const char *name)
{
    if(n && name && *name)
    {
        loopv(n->channels) if(!strcasecmp(n->channels[i].name, name))
            return &n->channels[i];
    }
    return NULL;
}

bool ircjoin(ircnet *n, ircchan *c)
{
    if(!n || !c) return false;
    if(n->state == IRC_DISC)
    {
        conoutf("ircnet %s is not connected", n->name);
        return false;
    }
    if(*c->passkey) ircsend(n, "JOIN %s :%s", c->name, c->passkey);
    else ircsend(n, "JOIN %s", c->name);
    c->state = IRCC_JOINING;
    c->lastjoin = totalmillis;
    c->lastsync = 0;
    return true;
}

bool ircenterchan(ircnet *n, const char *name)
{
    if(!n) return false;
    ircchan *c = ircfindchan(n, name);
    if(!c)
    {
        conoutf("ircnet %s has no channel called %s ready", n->name, name);
        return false;
    }
    return ircjoin(n, c);
}

bool ircnewchan(int type, const char *name, const char *channel, const char *friendly, const char *passkey, int relay)
{
    if(!name || !*name || !channel || !*channel) return false;
    ircnet *n = ircfind(name);
    if(!n)
    {
        conoutf("no such ircnet: %s", name);
        return false;
    }
    ircchan *c = ircfindchan(n, channel);
    if(c)
    {
        conoutf("%s already has channel %s", n->name, c->name);
        return false;
    }
    ircchan &d = n->channels.add();
    d.state = IRCC_NONE;
    d.type = type;
    d.relay = relay;
    d.lastjoin = d.lastsync = 0;
    copystring(d.name, channel);
    copystring(d.friendly, friendly && *friendly ? friendly : channel);
    copystring(d.passkey, passkey);
    if(n->state != IRC_DISC) ircjoin(n, &d);
    conoutf("%s added channel %s", n->name, d.name);
    return true;
}

ICOMMAND(ircaddchan, "ssssi", (const char *n, const char *c, const char *f, const char *z, int *r), {
    ircnewchan(IRCCT_AUTO, n, c, f, z, *r);
});
ICOMMAND(ircjoinchan, "ssssi", (const char *n, const char *c, const char *f, const char *z, int *r), {
    ircnewchan(IRCCT_NONE, n, c, f, z, *r);
});
ICOMMAND(ircpasschan, "sss", (const char *name, const char *chan, const char *s), {
    ircnet *n = ircfind(name);
    if(!n) { conoutf("no such ircnet: %s", name); return; }
    ircchan *c = ircfindchan(n, chan);
    if(!c) { conoutf("no such %s channel: %s", n->name, chan); return; }
    if(!s || !*s) { conoutf("%s channel %s current password is: %s", n->name, c->name, c->passkey && *c->passkey ? "<set>" : "<not set>"); return; }
    copystring(c->passkey, s);
});
ICOMMAND(ircrelaychan, "sss", (const char *name, const char *chan, const char *s), {
    ircnet *n = ircfind(name);
    if(!n) { conoutf("no such ircnet: %s", name); return; }
    ircchan *c = ircfindchan(n, chan);
    if(!c) { conoutf("no such %s channel: %s", n->name, chan); return; }
    if(!s || !*s) { conoutf("%s channel %s current relay level is: %d", n->name, c->name, c->relay); return; }
    c->relay = parseint(s);
});
ICOMMAND(ircfriendlychan, "sss", (const char *name, const char *chan, const char *s), {
    ircnet *n = ircfind(name);
    if(!n) { conoutf("no such ircnet: %s", name); return; }
    ircchan *c = ircfindchan(n, chan);
    if(!c) { conoutf("no such %s channel: %s", n->name, chan); return; }
    if(!s || !*s) { conoutf("%s channel %s current friendly name is: %s", n->name, c->name, c->friendly); return; }
    copystring(c->friendly, s);
});

#define MAXLINES 100

void ircprintf(ircnet *n, int relay, const char *target, const char *msg, ...)
{
    defvformatstring(str, msg, msg);
    oldstring s;
    if(target && *target && strcasecmp(target, n->nick))
    {
        ircchan *c = ircfindchan(n, target);
        if(c)
        {
            formatstring(s, "\fs\fa[%s]\fS", n->name);
            c->addline(str, MAXLINES);
            if(n->type == IRCT_RELAY && c->relay >= relay)
                server::sendservmsgf("\fs\fa[%s]\fS %s", c->friendly, str);
        }
        else
        {
            formatstring(s, "\fs\fa[%s]\fS", n->name);
            n->addline(str, MAXLINES);
        }
    }
    else
    {
        formatstring(s, "\fs\fa[%s]\fS", n->name);
        n->addline(newstring(str), MAXLINES);
    }
    conoutf(CON_INFO, "%s %s", s, str); // console is not used to relay
}

void ircchatprintf(ircnet *n, int relay, const char *target, const char *msg, ...)
{
    defvformatstring(str, msg, msg);
    oldstring s;
    if(target && *target && strcasecmp(target, n->nick))
    {
        ircchan *c = ircfindchan(n, target);
        if(c)
        {
            formatstring(s, "\fs\fa[%s:%s]\fS", n->name, c->name);
            c->addline(str, MAXLINES);
            if(n->type == IRCT_RELAY && c->relay >= relay)
                server::sendservmsgf("\fs\fa[%s]\fS %s", c->friendly, str);
        }
        else
        {
            formatstring(s, "\fs\fa[%s:%s]\fS", n->name, target);
            n->addline(str, MAXLINES);
        }
		conoutf(CON_IRC, "%s %s", s, str);
		//conoutf(CON_IRC, "%s %s", s, str);
    }
    else
    {
        formatstring(s, "\fs\fa[%s]\fS", n->name);
        n->addline(newstring(str), MAXLINES);
		conoutf(CON_INFO, "%s %s", s, str);
    }
}

void ircprocess(ircnet *n, char *user[3], int g, int numargs, char *w[])
{
    if(!strcasecmp(w[g], "NOTICE") || !strcasecmp(w[g], "PRIVMSG"))
    {
        if(numargs > g+2)
        {
            bool ismsg = (strcasecmp(w[g], "NOTICE")!=0);
            int len = strlen(w[g+2]);
            if(w[g+2][0] == '\v' && w[g+2][len-1] == '\v')
            {
                char *p = w[g+2];
                p++;
                const char *word = p;
                p += strcspn(p, " \v\0");
                if(p-word > 0)
                {
                    char *q = newstring(word, p-word);
                    p++;
                    const char *start = p;
                    p += strcspn(p, "\v\0");
                    char *r = p-start > 0 ? newstring(start, p-start) : newstring("");
                    if(ismsg)
                    {
                        if(!strcasecmp(q, "ACTION"))
                            ircprintf(n, 1, g ? w[g+1] : NULL, "\fv* %s %s", user[0], r);
                        else
                        {
                            ircprintf(n, 4, g ? w[g+1] : NULL, "\f3%s requests: %s %s", user[0], q, r);

                            if(!strcasecmp(q, "VERSION"))
                                ircsend(n, "NOTICE %s :\vVERSION %s %s %s (Cardboard)\v", user[0], game::gametitle, game::gamestage, game::gameversion);
                            else if(!strcasecmp(q, "PING")) // eh, echo back
                                ircsend(n, "NOTICE %s :\vPING %s\v", user[0], r);
                        }
                    }
                    else ircprintf(n, 4, g ? w[g+1] : NULL, "\f3%s replied: %s %s", user[0], q, r);
                    DELETEA(q); DELETEA(r);
                }
            }
            else if(ismsg)
            {
                if(n->type == IRCT_RELAY && g && strcasecmp(w[g+1], n->nick) && !strncasecmp(w[g+2], n->nick, strlen(n->nick)))
                {
                    const char *p = &w[g+2][strlen(n->nick)];
                    while(p && (*p == ':' || *p == ';' || *p == ',' || *p == '.' || *p == ' ' || *p == '\t')) p++;
                    if(p && *p) ircchatprintf(n, 0, w[g+1], "\fa<\fw%s\fa>\fw %s", user[0], p);
                }
                else ircchatprintf(n, 1, g ? w[g+1] : NULL, "\fa<\fw%s\fa>\fw %s", user[0], w[g+2]);
            }
            else ircchatprintf(n, 2, g ? w[g+1] : NULL, "\fo-%s- %s", user[0], w[g+2]);
        }
    }
    else if(!strcasecmp(w[g], "NICK"))
    {
        if(numargs > g+1)
        {
            if(!strcasecmp(user[0], n->nick)) copystring(n->nick, w[g+1]);
            ircprintf(n, 3, NULL, "\fm%s (%s@%s) is now known as %s", user[0], user[1], user[2], w[g+1]);
        }
    }
    else if(!strcasecmp(w[g], "JOIN"))
    {
        if(numargs > g+1)
        {
            ircchan *c = ircfindchan(n, w[g+1]);
            if(c && !strcasecmp(user[0], n->nick))
            {
                c->state = IRCC_JOINED;
                c->lastjoin = c->lastsync = totalmillis;
            }
            ircprintf(n, 3, w[g+1], "\fg%s (%s@%s) has joined", user[0], user[1], user[2]);
        }
    }
    else if(!strcasecmp(w[g], "PART"))
    {
        if(numargs > g+1)
        {
            ircchan *c = ircfindchan(n, w[g+1]);
            if(c && !strcasecmp(user[0], n->nick))
            {
                c->state = IRCC_NONE;
                c->lastjoin = totalmillis;
                c->lastsync = 0;
            }
            ircprintf(n, 3, w[g+1], "\fo%s (%s@%s) has left", user[0], user[1], user[2]);
        }
    }
    else if(!strcasecmp(w[g], "QUIT"))
    {
        if(numargs > g+1) ircprintf(n, 3, NULL, "\f3%s (%s@%s) has quit: %s", user[0], user[1], user[2], w[g+1]);
        else ircprintf(n, 3, NULL, "\f3%s (%s@%s) has quit", user[0], user[1], user[2]);
    }
    else if(!strcasecmp(w[g], "KICK"))
    {
        if(numargs > g+2)
        {
            ircchan *c = ircfindchan(n, w[g+1]);
            if(c && !strcasecmp(w[g+2], n->nick))
            {
                c->state = IRCC_KICKED;
                c->lastjoin = totalmillis;
                c->lastsync = 0;
            }
            ircprintf(n, 3, w[g+1], "\f3%s (%s@%s) has kicked %s from %s", user[0], user[1], user[2], w[g+2], w[g+1]);
        }
    }
    else if(!strcasecmp(w[g], "MODE"))
    {
        if(numargs > g+2)
        {
			oldstring modestr;
			modestr[0] = 0;
            loopi(numargs-g-2)
            {
                if(i) concatstring(modestr, " ");
                concatstring(modestr, w[g+2+i]);
            }
            ircprintf(n, 4, w[g+1], "\f3%s (%s@%s) sets mode: %s %s", user[0], user[1], user[2], w[g+1], modestr);
        }
        else if(numargs > g+1)
            ircprintf(n, 4, w[g+1], "\f3%s (%s@%s) sets mode: %s", user[0], user[1], user[2], w[g+1]);
    }
    else if(!strcasecmp(w[g], "PING"))
    {
        if(numargs > g+1)
        {
            ircprintf(n, 2, NULL, "%s PING %s", user[0], w[g+1]);
            ircsend(n, "PONG %s", w[g+1]);
        }
        else
        {
            int t = time(NULL);
            ircprintf(n, 4, NULL, "%s PING (empty)", user[0]);
            ircsend(n, "PONG %d", t);
        }
    }
    else
    {
        int numeric = *w[g] && *w[g] >= '0' && *w[g] <= '9' ? atoi(w[g]) : 0, off = 0;
		oldstring s;
		s[0] = 0;
        #define irctarget(a) (!strcasecmp(n->nick, a) || *a == '#' || ircfindchan(n, a))
        char *targ = numargs > g+1 && irctarget(w[g+1]) ? w[g+1] : NULL;
        if(numeric)
        {
            off = numeric == 353 ? 2 : 1;
            if(numargs > g+off+1 && irctarget(w[g+off+1]))
            {
                targ = w[g+off+1];
                off++;
            }
        }
        else concatstring(s, user[0]);
        for(int i = g+off+1; numargs > i && w[i]; i++)
        {
            if(s[0]) concatstring(s, " ");
            concatstring(s, w[i]);
        }
        if(numeric) switch(numeric)
        {
            case 1:
            {
                if(n->state == IRC_CONN)
                {
                    n->state = IRC_ONLINE;
                    ircprintf(n, 4, NULL, "\fbnow connected to %s as %s", user[0], n->nick);
                    if(*n->authname && *n->authpass) ircsend(n, "PRIVMSG %s :%s", n->authname, n->authpass);
                }
                break;
            }
            case 433:
            {
                if(n->state == IRC_CONN)
                {
                    concatstring(n->nick, "_");
                    ircsend(n, "NICK %s", n->nick);
                }
                break;
            }
            case 471:
            case 473:
            case 474:
            case 475:
            {
                ircchan *c = ircfindchan(n, w[g+2]);
                if(c)
                {
                    c->state = IRCC_BANNED;
                    c->lastjoin = totalmillis;
                    c->lastsync = 0;
                    if(c->type == IRCCT_AUTO)
                        ircprintf(n, 4, w[g+2], "\fbwaiting 5 mins to rejoin %s", c->name);
                }
                break;
            }
            default: break;
        }
        if(s[0]) ircchatprintf(n, 4, targ, "\fw%s %s", w[g], s);
        else ircprintf(n, 4, targ, "\fw%s", w[g]);
    }
}

void ircparse(ircnet *n)
{
    const int MAXWORDS = 25;
    char *w[MAXWORDS], *p = (char *)n->input, *start = p, *end = &p[n->inputcarry];
    loopi(MAXWORDS) w[i] = NULL;
    while(p < end)
    {
        bool full = false;
        int numargs = 0, g = 0;
        while(iscubespace(*p)) { if(++p >= end) goto cleanup; }
        start = p;
        if(*p == ':') { g = 1; ++p; }
        for(;;)
        {
            const char *word = p;
            if(*p == ':') { word++; full = true; } // uses the rest of the input line then
            while(*p != '\r' && *p != '\n' && (full || *p != ' ')) { if(++p >= end) goto cleanup; }

            if(numargs < MAXWORDS) w[numargs++] = newstring(word, p-word);

            if(*p == '\n' || *p == '\r') { ++p; start = p; break; }
            else while(*p == ' ') { if(++p >= end) goto cleanup; }
        }
        if(numargs)
        {
            char *user[3] = { NULL, NULL, NULL };
            if(g)
            {
                char *t = w[0], *u = strrchr(t, '!');
                if(u)
                {
                    user[0] = newstring(t, u-t);
                    t = u + 1;
                    u = strrchr(t, '@');
                    if(u)
                    {
                        user[1] = newstring(t, u-t);
                        if(*u++) user[2] = newstring(u);
                        else user[2] = newstring("*");
                    }
                    else
                    {
                        user[1] = newstring("*");
                        user[2] = newstring("*");
                    }
                }
                else
                {
                    user[0] = newstring(t);
                    user[1] = newstring("*");
                    user[2] = newstring("*");
                }
            }
            else
            {
                user[0] = newstring("*");
                user[1] = newstring("*");
                user[2] = newstring(n->serv);
            }
            if(numargs > g) ircprocess(n, user, g, numargs, w);
            loopi(3) DELETEA(user[i]);
        }
    cleanup:
        loopi(MAXWORDS) DELETEA(w[i]);
    }
    int parsed = start - (char *)n->input;
    if(parsed > 0)
    {
        memmove(n->input, start, n->inputlen - parsed);
        n->inputcarry -= parsed;
        n->inputlen -= parsed;
    }
}

void ircdiscon(ircnet *n)
{
    if(!n) return;
    ircprintf(n, 4, NULL, "disconnected from %s (%s:[%d])", n->name, n->serv, n->port);
    enet_socket_destroy(n->sock);
    n->state = IRC_DISC;
    n->sock = ENET_SOCKET_NULL;
    n->lastattempt = totalmillis;
}

void irccleanup()
{
    loopv(ircnets) if(ircnets[i]->sock != ENET_SOCKET_NULL)
    {
        ircnet *n = ircnets[i];
        ircsend(n, "QUIT");
        ircdiscon(n);
    }
}

void ircslice()
{
    loopv(ircnets)
    {
        ircnet *n = ircnets[i];
        if(n->sock != ENET_SOCKET_NULL && n->state != IRC_DISC)
        {
            switch(n->state)
            {
                case IRC_ATTEMPT:
                {
					if (*n->passkey) ircsend(n, "PASS %s", n->passkey);
					ircsend(n, "NICK %s", n->nick);
					ircsend(n, "PING 1");
					ircsend(n, "USER %s %s %s :%s", n->nick, n->nick, n->serv, n->nick);
					ircsend(n, "PING 1");
                    n->state = IRC_CONN;
                    loopvj(n->channels)
                    {
                        ircchan *c = &n->channels[j];
                        c->state = IRCC_NONE;
                        c->lastjoin = totalmillis;
                        c->lastsync = 0;
                    }
                    break;
                }
                case IRC_ONLINE: 
                {
                    loopvj(n->channels)
                    {
                        ircchan *c = &n->channels[j];
                        if(c->type == IRCCT_AUTO && c->state != IRCC_JOINED && (!c->lastjoin || totalmillis-c->lastjoin >= (c->state != IRCC_BANNED ? 5000 : 300000)))
                            ircjoin(n, c);
                    }
                    // fall through
                }
                case IRC_CONN:
                {
                    if(n->state == IRC_CONN && totalmillis-n->lastattempt >= 60000)
                    {
                        ircprintf(n, 4, NULL, "connection attempt timed out");
                        ircdiscon(n);
                    }
                    else switch(ircrecv(n))
                    {
						case -1: ircdiscon(n); break; // fail  
                        case 0: break;
                        default: ircparse(n); break;
                    }
                    break;
                }
                default:
                {
                    ircdiscon(n);
                    break;
                }
            }
        }
        else if(!n->lastattempt || totalmillis-n->lastattempt >= 60000) ircestablish(n);
    }
}
#ifndef STANDALONE
void irccmd(ircnet *n, ircchan *c, char *s)
{
    char *p = s;
    if(*p == '/')
    {
        p++;
        const char *word = p;
        p += strcspn(p, " \n\0");
        if(p-word > 0)
        {
            char *q = newstring(word, p-word), *r = newstring(*p ? ++p : "");
            if(!strcasecmp(q, "ME"))
            {
                if(c)
                {
                    ircsend(n, "PRIVMSG %s :\vACTION %s\v", c->name, r);
                    ircprintf(n, 1, c->name, "\fv* %s %s", n->nick, r);
                }
                else ircprintf(n, 4, NULL, "\f9you are not on a channel");
            }
            else if(!strcasecmp(q, "JOIN"))
            {
                ircchan *d = ircfindchan(n, r);
                if(d) ircjoin(n, d);
                else ircnewchan(IRCCT_AUTO, n->name, r);
            }
            else if(!strcasecmp(q, "PART"))
            {
                ircchan *d = ircfindchan(n, r);
                if(d) ircsend(n, "PART %s", d->name);
                else if(c) ircsend(n, "PART %s", c->name);
                else ircsend(n, "PART %s", r);
            }
            else if(*r) ircsend(n, "%s %s", q, r); // send it raw so we support any command
            else ircsend(n, "%s", q);
            DELETEA(q); DELETEA(r);
            return;
        }
        ircprintf(n, 4, c ? c->name : NULL, "\f9you are not on a channel");
    }
    else if(c)
    {
        ircsend(n, "PRIVMSG %s :%s", c->name, p);
        //ircprintf(n, 1, c->name, "\fw<%s> %s", n->nick, p);
        ircchatprintf(n, 1, c->name, "\fw<%s> %s", n->nick, p);
    }
    else
    {
        ircsend(n, "%s", p);
        ircprintf(n, 4, NULL, "\fa>%s< %s", n->nick, p);
    }
}

ICOMMAND(sayirc, "sss", (const char *net, const char *chan, const char *text), {
	if (!net[0] || !chan[0] || !text[0]) return;
	ircnet *in = ircfind(net);
	if (!in)
	{
		conoutf("\f3you are not connected to an IRC network");
		return;
	}
	ircchan *ic = ircfindchan(in, chan);
	if (!in)
	{
		conoutf("\f3you are not on a channel");
		return;
	}
	static char ntext[MAXTRANS];
	strcpy(ntext, text);
	irccmd(in, ic, ntext);
});

#endif
ICOMMAND(ircconns, "", (void), { int num = 0; loopv(ircnets) if(ircnets[i]->state >= IRC_ATTEMPT) num++; intret(num); });
