enum { IRCC_NONE = 0, IRCC_JOINING, IRCC_JOINED, IRCC_KICKED, IRCC_BANNED };
enum { IRCCT_NONE = 0, IRCCT_AUTO };
struct irclines
{
    int newlines;
    vector<char *> lines;
   
    void reset()
    {
        lines.deletearrays();
        newlines = 0;
    }

    void choplines(int limit)
    {
        while(lines.length() >= limit)
        {
            delete[] lines.remove(0);
            newlines = min(newlines, lines.length());
        }
    }

    void addline(const char *str, int limit = -1)
    {
#ifndef STANDALONE
        if(limit >= 0) choplines(limit);
        lines.add(newstring(str));
#endif
    }
};
struct ircchan : irclines
{
    int state, type, relay, lastjoin, lastsync;
    oldstring name, friendly, passkey;

    ircchan() { reset(); }
    ~ircchan() { reset(); }

    void reset()
    {
        state = IRCC_NONE;
        type = IRCCT_NONE;
        relay = lastjoin = lastsync = 0;
        name[0] = friendly[0] = passkey[0] = 0;
        irclines::reset();
    }
};
enum { IRCT_NONE = 0, IRCT_CLIENT, IRCT_RELAY, IRCT_MAX };
enum { IRC_DISC = 0, IRC_ATTEMPT, IRC_CONN, IRC_ONLINE, IRC_MAX };
struct ircnet : irclines
{
    int type, state, port, lastattempt, inputcarry, inputlen;
    oldstring name, serv, nick, ip, passkey, authname, authpass;
    ENetAddress address;
    ENetSocket sock;
    vector<ircchan> channels;
    uchar input[4096];

    ircnet() { reset(); }
    ~ircnet() { reset(); }

    void reset()
    {
        type = IRCT_NONE;
        state = IRC_DISC;
        inputcarry = inputlen = 0;
        port = lastattempt = 0;
        name[0] = serv[0] = nick[0] = ip[0] = passkey[0] = authname[0] = authpass[0] = 0;
        channels.shrink(0);
        irclines::reset();
    }
};

extern vector<ircnet *> ircnets;

extern ircnet *ircfind(const char *name);
extern void ircestablish(ircnet *n);
extern void ircsend(ircnet *n, const char *msg, ...);
extern void ircoutf(int relay, const char *msg, ...);
extern int ircrecv(ircnet *n);
extern void ircnewnet(int type, const char *name, const char *serv, int port, const char *nick, const char *ip = "", const char *passkey = "");
extern ircchan *ircfindchan(ircnet *n, const char *name);
extern bool ircjoin(ircnet *n, ircchan *c);
extern bool ircenterchan(ircnet *n, const char *name);
extern bool ircnewchan(int type, const char *name, const char *channel, const char *friendly = "", const char *passkey = "", int relay = 0);
extern void ircparse(ircnet *n);
extern void ircdiscon(ircnet *n);
extern void irccleanup();
extern void ircslice();
