// get server information using extinfo

#define EXT_ACK                         -1
#define EXT_VERSION                     105
#define EXT_NO_ERROR                    0
#define EXT_ERROR                       1
#define EXT_PLAYERSTATS_RESP_IDS        -10
#define EXT_PLAYERSTATS_RESP_STATS      -11
#define EXT_UPTIME                      0
#define EXT_PLAYERSTATS                 1
#define EXT_TEAMSCORE                   2
struct tscore {
	cbstring team;
	int score;
	tscore() : score(0) {}
};
struct extserver {
	uint ip;
	int port;
	cbstring name;
	int clients, maxclients, gamemode;
	cbstring map;
	int gametime, mastermode, gamespeed;
	bool gamepaused;
	int protocol, uptime;
	vector<extclient*> extclients;
	vector<tscore*> teams;
	extserver() : ip(0), port(0), clients(0), maxclients(0), gamemode(0), gametime(0), mastermode(0), gamespeed(0), gamepaused(false), protocol(0), uptime(0) {
		formatstring(name, "%s", "");
		formatstring(map, "%s", "");
	}
};
vector<extserver*> extservers;

int _s = 0;
int _f = 0;

ENetSocket extservsock = ENET_SOCKET_NULL;
ENetSocket getservsock() {
	if (extservsock != ENET_SOCKET_NULL) return extservsock;
	extservsock = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
	enet_socket_set_option(extservsock, ENET_SOCKOPT_NONBLOCK, 1);
	enet_socket_set_option(extservsock, ENET_SOCKOPT_BROADCAST, 1);
	return extservsock;
}
void requestservinfo(uint ip, int port)
{
	ENetAddress address;
	address.host = ip;
	address.port = port + 1;
	ENetSocket servsock = getservsock();
	if (servsock == ENET_SOCKET_NULL) return;
	ENetBuffer buf;
	uchar send[MAXTRANS];
	ucharbuf p(send, MAXTRANS);
	putint(p, 1);
	buf.data = send;
	buf.dataLength = p.length();
	_s++;
	if (enet_socket_send(servsock, &address, &buf, 1) <= 0) _f++;
}
void requestservuptime(uint ip, int port)
{
	ENetAddress address;
	address.host = ip;
	address.port = port + 1;
	ENetSocket servsock = getservsock();
	if (servsock == ENET_SOCKET_NULL) return;
	ENetBuffer buf;
	uchar send[MAXTRANS];
	ucharbuf p(send, MAXTRANS);
	putint(p, 0);
	putint(p, EXT_UPTIME);
	buf.data = send;
	buf.dataLength = p.length();
	_s++;
	if (enet_socket_send(servsock, &address, &buf, 1) <= 0) _f++;
}
void requestservplayerstats(uint ip, int port)
{
	ENetAddress address;
	address.host = ip;
	address.port = port + 1;
	ENetSocket servsock = getservsock();
	if (servsock == ENET_SOCKET_NULL) return;
	extserver* es = NULL;
	loopv(extservers) {
		if (extservers[i]->ip == address.host && extservers[i]->port == (address.port - 1)) {
			es = extservers[i];
			break;
		}
	}
	if (es) es->extclients.shrink(0);
	ENetBuffer buf;
	uchar send[MAXTRANS];
	ucharbuf p(send, MAXTRANS);
	putint(p, 0);
	putint(p, EXT_PLAYERSTATS);
	putint(p, -1);
	buf.data = send;
	buf.dataLength = p.length();
	_s++;
	if (enet_socket_send(servsock, &address, &buf, 1) <= 0) _f++;
}
void requestservteamscore(uint ip, int port)
{
	ENetAddress address;
	address.host = ip;
	address.port = port + 1;
	ENetSocket servsock = getservsock();
	if (servsock == ENET_SOCKET_NULL) return;
	extserver* es = NULL;
	loopv(extservers) {
		if (extservers[i]->ip == address.host && extservers[i]->port == (address.port - 1)) {
			es = extservers[i];
			break;
		}
	}
	if (es) es->teams.shrink(0);
	ENetBuffer buf;
	uchar send[MAXTRANS];
	ucharbuf p(send, MAXTRANS);
	putint(p, 0);
	putint(p, EXT_TEAMSCORE);
	buf.data = send;
	buf.dataLength = p.length();
	_s++;
	if (enet_socket_send(servsock, &address, &buf, 1) <= 0) _f++;
}
void requestservinfo(char* name, int port) {
	ENetAddress address;
	address.port = port ? port : CARDBOARD_SERVER_PORT;
	if (enet_address_set_host(&address, name) && !resolverwait(name, &address)) return;
	requestservinfo(address.host, address.port);
}
void requestservuptime(char* name, int port) {
	ENetAddress address;
	address.port = port ? port : CARDBOARD_SERVER_PORT;
	if (enet_address_set_host(&address, name) && !resolverwait(name, &address)) return;
	requestservuptime(address.host, address.port);
}
void requestservplayerstats(char* name, int port) {
	ENetAddress address;
	address.port = port ? port : CARDBOARD_SERVER_PORT;
	if (enet_address_set_host(&address, name) && !resolverwait(name, &address)) return;
	requestservplayerstats(address.host, address.port);
}
void requestservteamscore(char* name, int port) {
	ENetAddress address;
	address.port = port ? port : CARDBOARD_SERVER_PORT;
	if (enet_address_set_host(&address, name) && !resolverwait(name, &address)) return;
	requestservteamscore(address.host, address.port);
}
int _i = 0;
vector<ENetAddress*> infoqueue;
vector<ENetAddress*> uptimequeue;
vector<ENetAddress*> playerstatqueue;
vector<ENetAddress*> teamstatqueue;
int inforeq = -1;
void requestallserv() {
	_s = 0;
	_i = 0;
	_f = 0;
	inforeq = -1;
	extservers.shrink(0);
	for (int i = 0; i < numservers(); i++) {
		ENetAddress* address = new ENetAddress;
		address->port = getport(i) - 1;
		if (enet_address_set_host(address, gethostname(i)) && !resolverwait(gethostname(i), address)) continue;
		infoqueue.add(address);
		uptimequeue.add(address);
		playerstatqueue.add(address);
		teamstatqueue.add(address);
	}
}
int lastextinfo = totalmillis;
VARP(extservdelay, 1, 1, 100);
void checkinfoqueue() {
	if ((totalmillis - lastextinfo) < extservdelay) return;
	inforeq++;
	inforeq %= 4;
	switch (inforeq) {
	case 0: {
		if (infoqueue.inrange(0)) {
			ENetAddress* address = infoqueue[0];
			lastextinfo = totalmillis;
			requestservinfo(address->host, address->port);
			infoqueue.remove(0);
		}
		break;
	}
	case 1: {
		if (uptimequeue.inrange(0)) {
			ENetAddress* address = uptimequeue[0];
			lastextinfo = totalmillis;
			requestservuptime(address->host, address->port);
			uptimequeue.remove(0);
		}
		break;
	}
	case 2: {
		if (playerstatqueue.inrange(0)) {
			ENetAddress* address = playerstatqueue[0];
			lastextinfo = totalmillis;
			requestservplayerstats(address->host, address->port);
			playerstatqueue.remove(0);
		}
		break;
	}
	case 3: {
		if (teamstatqueue.inrange(0)) {
			ENetAddress* address = teamstatqueue[0];
			lastextinfo = totalmillis;
			requestservteamscore(address->host, address->port);
			teamstatqueue.remove(0);
		}
		break;
	}
	}
}
void requestall(char* hostname, int port) {
	ENetAddress address;
	address.port = port ? port : CARDBOARD_SERVER_PORT;
	if (enet_address_set_host(&address, hostname) && !resolverwait(hostname, &address)) return;
	requestservinfo(address.host, address.port);
	requestservuptime(address.host, address.port);
	requestservplayerstats(address.host, address.port);
	requestservteamscore(address.host, address.port);
}
ICOMMAND(requestservinfo, "si", (char* _ip, int* port), requestservinfo(_ip, *port));
ICOMMAND(requestservuptime, "si", (char* _ip, int* port), requestservuptime(_ip, *port));
ICOMMAND(requestservplayerstats, "si", (char* _ip, int* port), requestservplayerstats(_ip, *port));
ICOMMAND(requestservteamscore, "si", (char* _ip, int* port), requestservteamscore(_ip, *port));
ICOMMAND(requestallserv, "", (), requestallserv());
ICOMMAND(requestall, "si", (char* hostname, int* port), requestall(hostname, *port));
void processservinfo()
{
	ENetSocket servsock = getservsock();
	if (servsock == ENET_SOCKET_NULL) return;
	enet_uint32 events = ENET_SOCKET_WAIT_RECEIVE;
	int s = 0;
	ENetBuffer buf;
	ENetAddress address;
	uchar data[MAXTRANS];
	buf.data = data;
	buf.dataLength = sizeof(data);
	while ((s = enet_socket_wait(servsock, &events, 0)) >= 0 && events)
	{
		int len = enet_socket_receive(servsock, &address, &buf, 1);
		_i++;
		if (len <= 0) continue;
		ucharbuf p(data, len);
		extserver* es = NULL;
		loopv(extservers) {
			if (extservers[i]->ip == address.host && extservers[i]->port == (address.port - 1)) {
				es = extservers[i];
				break;
			}
		}
		if (!es) {
			es = new extserver;
			es->ip = address.host;
			es->port = address.port - 1;
		}
		int n = getint(p);
		if (n == 0) {
			int cmd = getint(p);
			if (cmd == EXT_UPTIME) {
				int ack = getint(p);
				if (ack != EXT_ACK) {
					return;
				}
				int v = getint(p);
				if (v != EXT_VERSION) {
					return;
				}
				es->uptime = getint(p);
			}
			else if (cmd == EXT_PLAYERSTATS) {
				getint(p); // (cn = -1)
				int ack = getint(p);
				if (ack != EXT_ACK) {
					return;
				}
				int v = getint(p);
				if (v != EXT_VERSION) {
					return;
				}
				if (getint(p) == EXT_ERROR) {
					return;
				}
				int ps = getint(p);
				if (ps == EXT_PLAYERSTATS_RESP_IDS) {
					vector<int> ids;
					while (p.remaining()) {
						int id = getint(p);
						ids.add(id);
					}
					loopvrev(es->extclients) {
						bool found = false;
						loopvj(ids) {
							if (es->extclients[i]->clientnum == ids[j]) {
								found = true;
								break;
							}
						}
						if (!found) {
							es->extclients.remove(i);
						}
					}
				}
				else if (ps == EXT_PLAYERSTATS_RESP_STATS) {
					extclient* ec = NULL;
					int cn = getint(p);
					loopv(es->extclients) {
						if (es->extclients[i]->clientnum == cn) {
							ec = es->extclients[i];
							break;
						}
					}
					if (!ec) {
						ec = new extclient;
						ec->clientnum = cn;
						es->extclients.add(ec);
					}
					ec->ping = getint(p);
					cbstring strdata;
					getstring(strdata, p);
					strncpy(ec->name, strdata, MAXNAMELEN + 1);
					getstring(strdata, p);
					strncpy(ec->team, strdata, MAXTEAMLEN + 1);
					ec->frags = getint(p);
					ec->flags = getint(p);
					ec->deaths = getint(p);
					ec->accuracy = getint(p);
					ec->health = getint(p);
					ec->gunselect = getint(p);
					ec->privilege = getint(p);
					ec->state = getint(p);
					p.get((uchar*)&ec->ip, 3);
				}
			}
			else if (cmd == EXT_TEAMSCORE) {
				int ack = getint(p);
				if (ack != EXT_ACK) {
					return;
				}
				int v = getint(p);
				if (v != EXT_VERSION) {
					return;
				}
				if (getint(p) == EXT_NO_ERROR) {
					getint(p); // remaining time
					getint(p); // gamemode
					while (p.remaining()) {
						tscore* score = new tscore;
						cbstring t;
						getstring(t, p);
						filtertext(score->team, t, false);
						score->score = getint(p);
						int bases = getint(p); // bases
						if (bases > 0) for(int i = 0; i < bases; i++) getint(p);
						bool exists = false;
						loopv(es->teams) {
							if (!strcmp(es->teams[i]->team, score->team)) {
								exists = true;
								break;
							}
						}
						if (!exists) es->teams.add(score);
					}
				}
			}
		}
		else {
			es->clients = getint(p);
			int attrs = getint(p);
			if (attrs == 5) {
				es->gamepaused = false;
				es->gamespeed = 100;
			}
			es->protocol = getint(p);
			es->gamemode = getint(p);
			es->gametime = getint(p);
			es->maxclients = getint(p);
			es->mastermode = getint(p);
			if (attrs == 7) {
				es->gamepaused = getint(p);
				es->gamespeed = getint(p);
			}
			cbstring map, name;
			getstring(map, p);
			getstring(name, p);
			filtertext(es->map, map, false);
			filtertext(es->name, name);
			bool exists = false;
			loopv(extservers) {
				if (extservers[i]->ip == es->ip && extservers[i]->port == es->port) {
					exists = true;
				}
			}
			if (!exists) extservers.add(es);
		}
	}
}

extserver* getextserv(uint ip, int port) {
	loopv(extservers) {
		extserver* es = extservers[i];
		if (es->ip == ip && es->port == port) return es;
	}
	return NULL;
}
int getextservidx(uint ip, int port) {
	loopv(extservers) {
		extserver* es = extservers[i];
		if (es->ip == ip && es->port == port) return i;
	}
	return -1;
}
int getextservidx(char* hostname, int port) {
	ENetAddress address;
	address.port = port ? port : CARDBOARD_SERVER_PORT;
	if (enet_address_set_host(&address, hostname) && !resolverwait(hostname, &address)) return -1;
	return getextservidx(address.host, address.port);
}
ICOMMAND(getextservidx, "si", (char* hostname, int* port), intret(getextservidx(hostname, *port)));
ICOMMAND(numservers, "", (), intret(numservers()));
ICOMMAND(numextservers, "", (), intret(extservers.length()));
char* extservname(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return extservers[i]->name;
	}
	return (char*)"";
}
ICOMMAND(extservname, "i", (int* i), result(extservname(*i)));
char* extservip(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		defformatstring(ip, "%d.%d.%d.%d", extservers[i]->ip % 256, (extservers[i]->ip / 256) % 256, (extservers[i]->ip / 256 / 256) % 256, extservers[i]->ip / 256 / 256 / 256);
		return newstring(ip);
	}
	return (char*)"0.0.0.0";
}
ICOMMAND(extservip, "i", (int* i), result(extservip(*i)));
int extservport(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return extservers[i]->port;
	}
	return 0;
}
ICOMMAND(extservport, "i", (int* i), intret(extservport(*i)));
int extservmastermode(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return extservers[i]->mastermode;
	}
	return -2;
}
ICOMMAND(extservmastermode, "i", (int* i), intret(extservmastermode(*i)));
char* extservmastermodename(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return (char*)server::mastermodename(extservers[i]->mastermode, "unknown");
	}
	return (char*)"unknown";
}
ICOMMAND(extservmastermodename, "i", (int* i), result(extservmastermodename(*i)));
int extservgamemode(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return extservers[i]->gamemode;
	}
	return -1;
}
ICOMMAND(extservgamemode, "i", (int* i), intret(extservgamemode(*i)));
char* extservgamemodename(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return (char*)server::modename(extservers[i]->gamemode, "unknown");
	}
	return (char*)"unknown";
}
ICOMMAND(extservgamemodename, "i", (int* i), result(extservgamemodename(*i)));
char* extservmap(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return extservers[i]->map;
	}
	return (char*)"";
}
ICOMMAND(extservmap, "i", (int* i), result(extservmap(*i)));
int extservnumclients(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return extservers[i]->clients;
	}
	return 0;
}
ICOMMAND(extservnumclients, "i", (int* i), intret(extservnumclients(*i)));
int extservmaxclients(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return extservers[i]->maxclients;
	}
	return 0;
}
ICOMMAND(extservmaxclients, "i", (int* i), intret(extservmaxclients(*i)));
int extservgametime(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return extservers[i]->gametime;
	}
	return 0;
}
ICOMMAND(extservgametime, "i", (int* i), intret(extservgametime(*i)));
int extservuptime(int i) {
	if (extservers.inrange(i) && extservers[i]) {
		return extservers[i]->uptime;
	}
	return 0;
}
ICOMMAND(extservuptime, "i", (int* i), intret(extservuptime(*i)));
bool sortextclients(extclient* a, extclient* b) {
	if (a->state != CS_SPECTATOR && b->state == CS_SPECTATOR) return true;
	if (a->state == CS_SPECTATOR && b->state != CS_SPECTATOR) return false;
	if (a->state == CS_SPECTATOR && b->state == CS_SPECTATOR) {
		if (a->clientnum < b->clientnum) return true;
		if (a->clientnum > b->clientnum) return false;
		return false;
	}
	if (a->flags > b->flags) return true;
	if (a->flags < b->flags) return false;
	if (a->frags > b->frags) return true;
	if (a->frags < b->frags) return false;
	if (a->clientnum < b->clientnum) return true;
	if (a->clientnum > b->clientnum) return false;
	return false;
}
bool sortteams(tscore* a, tscore* b) {
	if (a->score > b->score) return true;
	if (a->score < b->score) return false;
	if (a->team < b->team) return true;
	return false;
}
void extservclients(int i)
{
	vector<char> buf;
	cbstring cn;
	int numclients = 0;
	if (!extservers.inrange(i) || !extservers[i]) return;
	extserver* es = extservers[i];
	es->extclients.sort(sortextclients);
	loopvj(es->extclients) if (es->extclients[j])
	{
		formatstring(cn, "%d", es->extclients[j]->clientnum);
		if (numclients++) buf.add(' ');
		buf.put(cn, strlen(cn));
	}
	buf.add('\0');
	result(buf.getbuf());
}
ICOMMAND(extservclients, "i", (int* i), extservclients(*i));
void extservteams(int i)
{
	vector<char> buf;
	cbstring cn;
	int numclients = 0;
	if (!extservers.inrange(i) || !extservers[i]) return;
	extserver* es = extservers[i];
	es->teams.sort(sortteams);
	loopvj(es->teams) if (es->teams[j])
	{
		formatstring(cn, "%s", escapestring(es->teams[j]->team));
		if (numclients++) buf.add(' ');
		buf.put(cn, strlen(cn));
	}
	buf.add('\0');
	result(buf.getbuf());
}
ICOMMAND(extservteams, "i", (int* i), extservteams(*i));
int extservteamscore(int i, char* team) {
	if (extservers.inrange(i) && extservers[i]) {
		extserver* es = extservers[i];
		loopvj(es->teams) {
			tscore* t = es->teams[j];
			if (t && !strcmp(t->team, team)) {
				return t->score;
			}
		}
		return 0;
	}
	return 0;
}
ICOMMAND(extservteamscore, "is", (int* i, char* team), intret(extservteamscore(*i, team)));
char* extservclientname(int i, int cn) {
	if (extservers.inrange(i) && extservers[i]) {
		extserver* es = extservers[i];
		loopvj(es->extclients) {
			extclient* ec = es->extclients[j];
			if (ec && ec->clientnum == cn) {
				return ec->name;
			}
		}
		return (char*)"";
	}
	return (char*)"";
}
ICOMMAND(extservclientname, "ii", (int* i, int* cn), result(extservclientname(*i, *cn)));
char* extservclientteam(int i, int cn) {
	if (extservers.inrange(i) && extservers[i]) {
		extserver* es = extservers[i];
		loopvj(es->extclients) {
			extclient* ec = es->extclients[j];
			if (ec && ec->clientnum == cn) {
				return ec->team;
			}
		}
		return (char*)"";
	}
	return (char*)"";
}
ICOMMAND(extservclientteam, "ii", (int* i, int* cn), result(extservclientteam(*i, *cn)));
int extservclientfrags(int i, int cn) {
	if (extservers.inrange(i) && extservers[i]) {
		extserver* es = extservers[i];
		loopvj(es->extclients) {
			extclient* ec = es->extclients[j];
			if (ec && ec->clientnum == cn) {
				return ec->frags;
			}
		}
		return 0;
	}
	return 0;
}
ICOMMAND(extservclientfrags, "ii", (int* i, int* cn), intret(extservclientfrags(*i, *cn)));
int extservclientflags(int i, int cn) {
	if (extservers.inrange(i) && extservers[i]) {
		extserver* es = extservers[i];
		loopvj(es->extclients) {
			extclient* ec = es->extclients[j];
			if (ec && ec->clientnum == cn) {
				return ec->flags;
			}
		}
		return 0;
	}
	return 0;
}
ICOMMAND(extservclientflags, "ii", (int* i, int* cn), intret(extservclientflags(*i, *cn)));
int extservclientspectator(int i, int cn) {
	if (extservers.inrange(i) && extservers[i]) {
		extserver* es = extservers[i];
		loopvj(es->extclients) {
			extclient* ec = es->extclients[j];
			if (ec && ec->clientnum == cn) {
				return ec->state == CS_SPECTATOR ? 1 : 0;
			}
		}
		return 0;
	}
	return 0;
}
ICOMMAND(extservclientspectator, "ii", (int* i, int* cn), intret(extservclientspectator(*i, *cn)));
int extservclientprivilege(int i, int cn) {
	if (extservers.inrange(i) && extservers[i]) {
		extserver* es = extservers[i];
		loopvj(es->extclients) {
			extclient* ec = es->extclients[j];
			if (ec && ec->clientnum == cn) {
				return ec->privilege;
			}
		}
		return 0;
	}
	return 0;
}
ICOMMAND(extservclientprivilege, "ii", (int* i, int* cn), intret(extservclientprivilege(*i, *cn)));