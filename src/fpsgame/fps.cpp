#include "game.h"
#include "weaponstats.h"

namespace game
{
	const char* gametitle = "Carmine Impact"; // the name of the game
	const char* gamestage = "Alpha"; // development stage
	const char* gameversion = "3.0.0"; // version

	ICOMMAND(version, "", (), {
		defformatstring(vers, "%s %s %s", gametitle, gamestage, gameversion);
		int len = strlen(vers);
		char* k = newstring(len);
		filtertext(k, vers, true, false, len);
		stringret(k);
	});

	bool intermission = false;
	int maptime = 0, maprealtime = 0, maplimit = -1;
	int respawnent = -1;
	int lasthit = 0, lastspawnattempt = 0;

	int following = -1, followdir = 0;

	fpsent *player1 = NULL;         // our client
	vector<fpsent *> players;       // other clients
	int savedammo[NUMGUNS];

	bool clientoption(const char *arg) { return false; }

	void taunt()
	{
		if(player1->state!=CS_ALIVE || player1->physstate<PHYS_SLOPE) return;
		if(lastmillis-player1->lasttaunt<1000) return;
		player1->lasttaunt = lastmillis;
		loopi(NUMGUNS)
		{
			player1->lastaction[i] = lastmillis;
			player1->gunwait[i] = 1000;
		}
		addmsg(N_TAUNT, "rc", player1);

		// TODO: taunt sounds
		playsound(S_ITEMSPAWN, &game::hudplayer()->o);
	}
	COMMAND(taunt, "");

	ICOMMAND(getfollow, "", (),
	{
		fpsent *f = followingplayer();
		intret(f ? f->clientnum : -1);
	});

	bool spectating(physent* d)
	{
		return d->state == CS_SPECTATOR;
	}

	const bool canfollow(const fpsent* const spec, const fpsent* const player)
	{
		return spec && player &&
			player->state != CS_SPECTATOR &&
			((!cmode && spec->state == CS_SPECTATOR) ||
			(cmode && cmode->canfollow(spec, player)));
	}

	void follow(char *arg)
	{
		if(arg[0] ? spectating(player1) : following>=0)
		{
			int ofollowing = following;
			following = arg[0] ? parseplayer(arg) : -1;
			if (following == player1->clientnum) following = -1;
			followdir = 0;
			if (following != ofollowing) clearfragmessages();
			conoutf("following: %s", following >= 0 ? "on" : "off");
		}
	}
	COMMAND(follow, "s");

	void nextfollow(int dir)
	{
		if(!spectating(player1) || clients.empty())
		{
			stopfollowing();
			return;
		}
		int cur = following >= 0 ? following : (dir > 0 ? clients.length() - 1 : 0);
		loopv(clients)
		{
			cur = (cur + dir + clients.length()) % clients.length();
			if(clients[cur] && canfollow(player1, clients[cur]))
			{
				if (following != cur) clearfragmessages();
				if (following<0) conoutf("follow on");
				following = cur;
				followdir = dir;
				return;
			}
		}
		stopfollowing();
	}
	ICOMMAND(nextfollow, "i", (int *dir), nextfollow(*dir < 0 ? -1 : 1));


	const char *getclientmap() { return clientmap; }

	void resetgamestate()
	{
		clearprojectiles();
		clearbouncers();
	}

	fpsent *spawnstate(fpsent *d)              // reset player state not persistent accross spawns
	{
		d->respawn();
		d->spawnstate(gamemode);
		return d;
	}

	void respawnself()
	{
		if(ispaused()) return;
		if(m_mp(gamemode))
		{
			int seq = (player1->lifesequence<<16)|((lastmillis/1000)&0xFFFF);
			if(player1->respawned!=seq) { addmsg(N_TRYSPAWN, "rc", player1); player1->respawned = seq; }
		}
		else
		{
			spawnplayer(player1);
			showscores(false);
			lasthit = 0;
			if(cmode) cmode->respawned(player1);
		}
	}

	fpsent *pointatplayer()
	{
		loopv(players) if(players[i] != player1 && intersect(players[i], player1->o, worldpos)) return players[i];
		return NULL;
	}

	void stopfollowing()
	{
		if(following<0) return;
		following = -1;
		followdir = 0;
		clearfragmessages();
		conoutf("follow off");
	}

	fpsent *followingplayer(fpsent *fallback)
	{
		if(player1->state!=CS_SPECTATOR || following<0) return fallback;
		fpsent *target = getclient(following);
		if(target && target->state!=CS_SPECTATOR) return target;
		return fallback;
	}

	fpsent *hudplayer()
	{
		if(thirdperson && allowthirdperson()) return player1;
		return followingplayer(player1);
	}

	void setupcamera()
	{
		fpsent *target = followingplayer();
		if(target)
		{
			player1->yaw = target->yaw;
			player1->pitch = target->state==CS_DEAD ? 0 : target->pitch;
			player1->o = target->o;
			player1->resetinterp();
		}
	}

	bool allowthirdperson(bool msg)
	{
		return player1->state==CS_SPECTATOR || player1->state==CS_EDITING || m_edit || m_parkour || !multiplayer(msg);
	}
	ICOMMAND(allowthirdperson, "b", (int *msg), intret(allowthirdperson(*msg!=0) ? 1 : 0));

	bool detachcamera()
	{
		fpsent *d = hudplayer();
		return d->state==CS_DEAD;
	}

	bool collidecamera()
	{
		switch(player1->state)
		{
			case CS_EDITING: return false;
			case CS_SPECTATOR: return followingplayer()!=NULL;
		}
		return true;
	}

	VARP(smoothmove, 0, 75, 100);
	VARP(smoothdist, 0, 32, 64);

	void predictplayer(fpsent *d, bool move)
	{
		d->o = d->newpos;
		d->yaw = d->newyaw;
		d->pitch = d->newpitch;
		d->roll = d->newroll;
		if(move)
		{
			moveplayer(d, 1, false);
			d->newpos = d->o;
		}
		float k = 1.0f - float(lastmillis - d->smoothmillis)/smoothmove;
		if(k>0)
		{
			d->o.add(vec(d->deltapos).mul(k));
			d->yaw += d->deltayaw*k;
			if(d->yaw<0) d->yaw += 360;
			else if(d->yaw>=360) d->yaw -= 360;
			d->pitch += d->deltapitch*k;
			d->roll += d->deltaroll*k;
		}
	}

	void otherplayers(int curtime)
	{
		loopv(players)
		{
			fpsent *d = players[i];
			if(d == player1 || d->ai) continue;

			if(d->state==CS_DEAD && d->ragdoll) moveragdoll(d);
			else if(!intermission)
			{
				if (lastmillis - d->lastaction[d->gunselect] >= d->gunwait[d->gunselect]) 
				{
					d->gunwait[d->gunselect] = 0;
				}
			}

			const int lagtime = totalmillis-d->lastupdate;
			if(!lagtime || intermission) continue;
			else if(lagtime>1000 && d->state==CS_ALIVE)
			{
				d->state = CS_LAGGED;
				continue;
			}
			if(d->state==CS_ALIVE || d->state==CS_EDITING)
			{
				if(smoothmove && d->smoothmillis>0) predictplayer(d, true);
				else moveplayer(d, 1, false);
			}
			else if(d->state==CS_DEAD && !d->ragdoll && lastmillis-d->lastpain<2000) moveplayer(d, 1, true);
		}
	}

	void updateworld()        // main game update loop
	{
		if (!maptime)
		{
			maptime = lastmillis;
			maprealtime = totalmillis;
			return;
		}
		if (!curtime || ispaused()) 
		{
			gets2c();
			if (curtime && spectating(player1))
			{
				fakephysicsframe();
				moveplayer(player1, 10, true);
			}
			if(player1->clientnum>=0) c2sinfo();
			return;
		}

		physicsframe();
		ai::navigate();
		updateweapons(curtime);
		otherplayers(curtime);
		ai::update();
		moveragdolls();
		gets2c();
		if(connected)
		{
			#ifdef DISCORD
				discord::updatePresence((spectating(player1) ? discord::D_SPECTATE : discord::D_PLAYING), gamemodes[gamemode + -(STARTGAMEMODE)].name, player1);
			#endif
			if(player1->state == CS_DEAD)
			{
				if(player1->ragdoll) moveragdoll(player1);
				else if(lastmillis-player1->lastpain<2000)
				{
					player1->fmove = player1->fstrafe = 0.0f;
					moveplayer(player1, 10, true);
				}
			}
			else if(!intermission)
			{
				if(player1->ragdoll) cleanragdoll(player1);
				moveplayer(player1, 10, true);
				swayhudgun(curtime);
				entities::checkitems(player1);
				if(cmode) cmode->checkitems(player1);
			}
			updateextinfo();
		}
		checkflag(); // frame lag check
		if(player1->clientnum>=0) c2sinfo();   // do this last, to reduce the effective frame lag
		checkinfoqueue();
		processservinfo();
	}

	float proximityscore(float x, float lower, float upper)
	{
		if(x <= lower) return 1.0f;
		if(x >= upper) return 0.0f;
		float a = x - lower, b = x - upper;
		return (b * b) / (a * a + b * b);
	}

	static inline float harmonicmean(float a, float b) { return a + b > 0 ? 2 * a * b / (a + b) : 0.0f; }

	// avoid spawning near other players
	float ratespawn(dynent *d, const extentity &e)
	{
		fpsent *p = (fpsent *)d;
		vec loc = vec(e.o).addz(p->eyeheight);
		float maxrange = !m_noitems ? 400.0f : (cmode ? 300.0f : 110.0f);
		float minplayerdist = maxrange;
		loopv(players)
		{
			const fpsent *o = players[i];
			if(o == p)
			{
				if(m_noitems || (o->state != CS_ALIVE && lastmillis - o->lastpain > 3000)) continue;
			}
			else if(o->state != CS_ALIVE || isteam(o->team, p->team)) continue;

			vec dir = vec(o->o).sub(loc);
			float dist = dir.squaredlen();
			if(dist >= minplayerdist*minplayerdist) continue;
			dist = sqrtf(dist);
			dir.mul(1/dist);

			// scale actual distance if not in line of sight
			if(raycube(loc, dir, dist) < dist) dist *= 1.5f;
			minplayerdist = min(minplayerdist, dist);
		}
		float rating = 1.0f - proximityscore(minplayerdist, 80.0f, maxrange);
		return cmode ? harmonicmean(rating, cmode->ratespawn(p, e)) : rating;
	}

	void pickgamespawn(fpsent *d)
	{
		int tag = cmode ? cmode->getspawngroup(d) : 0;

		if (m_teammode && !cmode)
		{
			tag = (!strcmp(d->team, "red") ? 1 : (!strcmp(d->team, "blue") ? 2 : 0));
		}
		else if (m_race && cmode) 
		{ 
			cmode->pickspawn(d);
			return;
		}

		findplayerspawn(d, -1, tag);
	}

	void spawnplayer(fpsent *d)   // place at random spawn
	{
		pickgamespawn(d);
		spawnstate(d);
		if(d==player1)
		{
			if(editmode) d->state = CS_EDITING;
			else if(d->state != CS_SPECTATOR) d->state = CS_ALIVE;
		}
		else d->state = CS_ALIVE;
	}

	VARP(spawnwait, 0, 0, 1000);

	void respawn()
	{
		if(player1->state==CS_DEAD)
		{
			player1->attacking = false;
			int wait = cmode ? cmode->respawnwait(player1) : 0;
			if(wait>0)
			{
				lastspawnattempt = lastmillis;
				//conoutf(CON_GAMEINFO, "\f2you must wait %d second%s before respawn!", wait, wait!=1 ? "s" : "");
				return;
			}
			if(lastmillis < player1->lastpain + spawnwait) return;
			respawnself();
		}
	}
	COMMAND(respawn, "");

	// inputs

	VARP(attackspawn, 0, 1, 1);

	void doattack(bool on)
	{
		if(!connected || intermission) return;
		if((player1->attacking = on) && attackspawn) respawn();
	}

	VARP(jumpspawn, 0, 1, 1);

	bool canjump()
	{
		if(!connected || intermission) return false;
		if(jumpspawn) respawn();
		return player1->state!=CS_DEAD;
	}

	bool canhover()
	{
		return spectating(player1) || player1->state == CS_EDITING;
	}

	bool allowmove(physent *d)
	{
		// taunts do not prevent movement, they prevent shooting now
		return true;
	}

	VARP(hitsound, 0, 0, 1);
	VARP(killsound, 0, 0, 1);

	VARP(parkourhitmarker, 0, 0, 1);

	// TODO: rework for no-damage modes (parkour, etc)
	void damaged(int damage, fpsent *d, fpsent *actor, int gun, bool headshot, bool local)
	{
		// no team damage
		if (isteam(d->team, actor->team)) return;

		recorddamage(actor, d, damage, headshot);

		if((d->state != CS_ALIVE && d->state != CS_LAGGED && d->state != CS_SPAWNING) || intermission) return;

		if(local && !m_parkour) damage = d->dodamage(damage);
		else if(actor == player1) return;

		fpsent *h = hudplayer();
		if(h!=player1 && actor==h && d!=actor)
		{
			if ((hitsound && lasthit != lastmillis && !m_parkour) || (m_parkour && parkourhitmarker)) playsound(S_HIT);
			if (!m_parkour) lasthit = lastmillis;
		}
		if (!m_parkour)
		{
			if (d == h)
			{
				damageblend(damage);
				damagecompass(damage, actor->o);
			}
			// TODO: why is this commented out
			damageeffect(damage, d, d != h);

			ai::damaged(d, actor);

			if (d->health <= 0) { if (local) killed(d, actor, gun); }

			// TODO: switch sound system to text keys, see Bube
			if (d == h) playsound(S_PAIN_SARAH_6 + (6 * d->playermodel));
			else playsound(S_PAIN_SARAH_1 + rnd(5) + (6 * d->playermodel), &d->o);
		}
	}

	VARP(deathscore, 0, 1, 1);

	void deathstate(fpsent *d, bool restore)
	{
		d->state = CS_DEAD;
		d->lastpain = lastmillis;
		if(!restore)
		{
			gibeffect(max(-d->health, 0), d->vel, d);
			d->deaths++;
		}
		if(d==player1)
		{
			if(deathscore) showscores(true);
			disablezoom();
			if(!restore) loopi(NUMGUNS) savedammo[i] = player1->ammo[i];
			d->attacking = false;
			//d->pitch = 0;
			d->roll = 0;
			playsound(S_DIE_SARAH_1 + rnd(1) + (2 * d->playermodel));
		}
		else
		{
			d->fmove = d->fstrafe = 0.0f;
			d->vertical = 0;
			d->resetinterp();
			d->smoothmillis = 0;
			playsound(S_DIE_SARAH_1 + rnd(1) + (2 * d->playermodel), &d->o);
		}
	}

	VARP(teamcolorfrags, 0, 1, 1);

	VARP(verbosekill, 0, 0, 1);
	VARP(logkill, 0, 0, 1);

	void killed(fpsent* d, fpsent* actor, int gun, bool headshot)
	{
		if(d->state==CS_EDITING)
		{
			d->editstate = CS_DEAD;
			d->deaths++;
			if(d!=player1) d->resetinterp();
			return;
		}
		else if((d->state!=CS_ALIVE && d->state != CS_LAGGED && d->state != CS_SPAWNING) || intermission) return;

		if(cmode) cmode->died(d, actor);

		fpsent *h = followingplayer(player1);
		int contype = d==h || actor==h ? CON_FRAG_SELF : CON_FRAG_OTHER;

		const char* hstag = " \f3(headshot)";

		const char *dname = "", *aname = "";
		if(m_teammode && teamcolorfrags)
		{
			dname = teamcolorname(d, "you", true);
			aname = teamcolorname(actor, "you", true);
		}
		else
		{
			dname = colorname(d, NULL, "", "", "you", true);
			aname = colorname(actor, NULL, "", "", "you", true);
		}
		if (logkill)
		{
			if (verbosekill) {
				if (actor->type == ENT_AI)
				{
					conoutf(contype, "\f2%s\f2 got killed by %s with %s%s!", dname, aname, getweaponname(gun), headshot ? hstag : "");
				}
				else if (d == actor)
				{
					d->suicides++;
					conoutf(contype, "\f2%s\f2 suicided%s", dname, d == player1 ? "!" : "");
				}
				else
				{
					if (d == player1) conoutf(contype, "\f2%s\f2 got fragged by %s with %s%s!", dname, aname, getweaponname(gun), headshot ? hstag : "");
					else conoutf(contype, "\f2%s\f2 fragged %s with %s%s!", aname, dname, getweaponname(gun), headshot ? hstag : "");
				}
			} 
			else
			{
				if (actor->type == ENT_AI)
				{
					conoutf(contype, "\f2%s\f2 > %s > %s%s", dname, getweaponname(gun), aname, headshot ? hstag : "");
				}
				else if (d == actor)
				{
					d->suicides++;
					conoutf(contype, "\f2world > %s", dname);
				}
				else
				{
					conoutf(contype, "\f2%s\f2 > %s > %s%s", aname, getweaponname(gun), dname, headshot ? hstag : "");
				}
			}
		}

		// play local killsound if enabled
		if (d != actor && actor == player1 && killsound) playsound(S_KILL);

		// run `ondeath` on local death
		if (d == player1) execident("ondeath");

		// add frag message for kill
		if (d == actor) addfragmessage(contype, NULL, dname, HICON_TOKEN - HICON_FIST, false);
		else addfragmessage(contype, aname, dname, d->lasthitpushgun, headshot);

		deathstate(d);
		ai::killed(d, actor);

		#ifdef DISCORD // this updates every time anyone gets a kill, shouldn't be an issue.
			discord::updatePresence((player1->state == CS_SPECTATOR ? discord::D_SPECTATE : discord::D_PLAYING), gamemodes[gamemode + -(STARTGAMEMODE)].name, player1, true);
		#endif
	}

	void timeupdate(int secs)
	{
		server::timeupdate(secs);
		if(secs > 0)
		{
			maplimit = lastmillis + secs*1000;
		}
		else
		{
			intermission = true;
			player1->attacking = false;
			if(cmode) cmode->gameover();
			conoutf(CON_GAMEINFO, "\f2intermission:");
			conoutf(CON_GAMEINFO, "\f2game has ended!");
			if (m_ctf) conoutf(CON_GAMEINFO, "\f2player frags: %d, flags: %d, deaths: %d, kdr: %.2f", player1->frags, player1->flags, player1->deaths, (float)player1->frags / max(player1->deaths, 1));
			else if (m_collect) conoutf(CON_GAMEINFO, "\f2player frags: %d, skulls: %d, deaths: %d, kdr: %.2f", player1->frags, player1->flags, player1->deaths, (float)player1->frags / max(player1->deaths, 1));
			else if (!m_parkour) conoutf(CON_GAMEINFO, "\f2player frags: %d, deaths: %d, kdr: %.2f", player1->frags, player1->deaths, (float)player1->frags / max(player1->deaths, 1));
			int accuracy = (player1->totaldamage * 100) / max(player1->totalshots, 1);
			if (!m_parkour) conoutf(CON_GAMEINFO, "\f2player total damage dealt: %d, damage wasted: %d, accuracy: %d%%", player1->totaldamage, player1->totalshots - player1->totaldamage, accuracy);

			showscores(true);
			disablezoom();

			execident("intermission");

			#ifdef STEAM
				steam::setPlayerAchievement(player1->playermodel);
			#endif
		}
	}

	ICOMMAND(getfrags, "", (), intret(hudplayer()->frags));
	ICOMMAND(getflags, "", (), intret(hudplayer()->flags));
	ICOMMAND(getdeaths, "", (), intret(hudplayer()->deaths));
	ICOMMAND(gettotaldamage, "", (), intret(playerdamage(NULL, DMG_DEALT)));
	ICOMMAND(gettotalshots, "", (), intret(playerdamage(NULL, DMG_POTENTIAL)));
	ICOMMAND(getkdr, "", (), floatret((float)player1->frags / max(player1->deaths, 1)));

	vector<fpsent *> clients;

	fpsent *newclient(int cn)   // ensure valid entity
	{
		if(cn < 0 || cn > max(0xFF, MAXCLIENTS + MAXBOTS))
		{
			neterr("clientnum", false);
			return NULL;
		}

		if(cn == player1->clientnum) return player1;

		while(cn >= clients.length()) clients.add(NULL);
		if(!clients[cn])
		{
			fpsent *d = new fpsent;
			d->clientnum = cn;
			clients[cn] = d;
			players.add(d);
		}
		return clients[cn];
	}

	fpsent *getclient(int cn)   // ensure valid entity
	{
		if(cn == player1->clientnum) return player1;
		return clients.inrange(cn) ? clients[cn] : NULL;
	}

	void clientdisconnected(int cn, bool notify)
	{
		if(!clients.inrange(cn)) return;
		if(following==cn)
		{
			if(followdir) nextfollow(followdir);
			else stopfollowing();
		}
		unignore(cn);
		fpsent *d = clients[cn];
		if(!d) return;
		if(notify && d->name[0]) conoutf("\f4leave:\f7 %s", colorname(d));
		playsound(S_SRV_DISCONNECT);
		removeweapons(d);
		removetrackedparticles(d);
		removetrackeddynlights(d);
		if(cmode) cmode->removeplayer(d);
		players.removeobj(d);
		DELETEP(clients[cn]);
		cleardynentcache();

		#ifdef DISCORD
			discord::updatePresence((player1->state == CS_SPECTATOR ? discord::D_SPECTATE : discord::D_PLAYING), gamemodes[gamemode + -(STARTGAMEMODE)].name, player1, true);
		#endif
	}

	void clearclients(bool notify)
	{
		loopv(clients) if(clients[i]) clientdisconnected(i, notify);
	}

	void initclient()
	{
		player1 = spawnstate(new fpsent);
		filtertext(player1->name, "CardboardPlayer", false, false, MAXNAMELEN);
		players.add(player1);
	}

	static old_string cname[3];
	static int cidx = 0;

	VARP(showtags, 0, 1, 1);
	VAR(dbgplayerinfo, 0, 0, 1);

	// TODO: see if this can be rewritten better
	bool getplayerinfo(fpsent* d)
	{
		if(offline) // can't retrieve info in offline mode
		{
			d->user = new userinfo;
			return false;
		}
		if (!d->name[0] || !strcmp(d->name, "CardboardPlayer")) return false; // no name or default name
		if (d->user->status == userinfo::OK) return true; // already have player info
		if (dbgplayerinfo) conoutf(CON_INFO, "\fs\f1getting player info for %s...\fr", d->name);
		copystring(d->user->tags, "");
		loopi(NUMGUNS) d->user->wskins[i] = 0;
		old_string apiurl;
		formatstring(apiurl, "%s/game/playerinfo?game=1&name=%s", __hnapi, d->name);
		const char* resp = web_get(apiurl, false);
		cJSON* json = cJSON_Parse(resp);

		// error handling
		const cJSON* jstatus = cJSON_GetObjectItemCaseSensitive(json, "status");
		const cJSON* jmessage = cJSON_GetObjectItemCaseSensitive(json, "message");
		if (cJSON_IsNumber(jstatus) && cJSON_IsString(jmessage)) {
			if (jstatus->valueint > 0) {
				if (dbgplayerinfo) conoutf(CON_INFO, "\fs\f3getting player info for %s failed! (non-zero) [%s]\fr", d->name, jmessage->valuestring);
				d->user->status = userinfo::FAIL;
				return false;
			}
			else {
				// actual parse
				const cJSON* tag = NULL;
				const cJSON* color = NULL;

				const cJSON* tags = NULL;
				const cJSON* tagitm = NULL;

				old_string conc = "";

				tags = cJSON_GetObjectItemCaseSensitive(json, "tags");
				cJSON_ArrayForEach(tagitm, tags)
				{
					tag = cJSON_GetObjectItemCaseSensitive(tagitm, "tag");
					if (cJSON_IsString(tag) && (tag->valuestring != NULL))
					{
						color = cJSON_GetObjectItemCaseSensitive(tagitm, "color");
						if (strcmp(tag->valuestring, ""))
						{
							if (dbgplayerinfo) conoutf(CON_DEBUG, "tag is \"%s\"", tag->valuestring);
							if (cJSON_IsString(color) && (color->valuestring != NULL) && (strcmp(color->valuestring, "")))
							{
								if (dbgplayerinfo) conoutf(CON_DEBUG, "color is \"%s\"", color->valuestring);
								concformatstring(conc, "\fs\f%s[%s]\fr", color->valuestring, tag->valuestring);
							}
							else {
								concformatstring(conc, "[%s]", tag->valuestring);
							}
						}
						else if (cJSON_IsString(color) && (color->valuestring != NULL))
						{
							if (dbgplayerinfo) conoutf(CON_DEBUG, "color is \"%s\"", color->valuestring);
							concformatstring(conc, "\f%s", color->valuestring);
						}
					}
				}
				defformatstring(fulltag, "%s", conc);
				copystring(d->user->tags, fulltag);
				d->user->status = userinfo::OK;
				free(json);
				return true;
			}
		}
		else {
			//conoutf(CON_ERROR, "malformed JSON recieved from server");
			free(json);
			conoutf(CON_INFO, "\fs\f3getting tags for %s failed! (malformed JSON)\fr", d->name);
			d->user->status = userinfo::FAIL;
			return false;
		}
		free(json);
		conoutf(CON_INFO, "\fs\f3getting tags for %s failed! (no tag)\fr", d->name);
		d->user->status = userinfo::FAIL;
		return false;
	}

	VARP(showmodeinfo, 0, 1, 1);

	void startgame()
	{
		clearprojectiles();
		clearbouncers();
		clearragdolls();

		clearteaminfo();

		// reset perma-state
		loopv(players)
		{
			fpsent *d = players[i];
			d->frags = d->flags = 0;
			d->deaths = 0;
			d->totaldamage = 0;
			d->totalshots = 0;
			d->maxhealth = 1000;
			d->lifesequence = -1;
			d->respawned = d->suicided = -2;
		}

		setclientmode();

		intermission = false;
		maptime = maprealtime = 0;
		maplimit = -1;

		if(cmode)
		{
			cmode->preload();
			cmode->setup();
		}

		getplayerinfo(player1);

		const char* info = m_valid(gamemode) ? gamemodes[gamemode - STARTGAMEMODE].info : NULL;
		if (showmodeinfo && info) {
			conoutf(CON_GAMEINFO, "\f2%s: \f0%s", server::modename(gamemode), info); // gamemode info, triggered twice on first map load?????? -Y
		}

		if(player1->playermodel != playermodel) switchplayermodel(playermodel);

		showscores(false);
		disablezoom();
		lasthit = 0;

		execident("mapstart");
	}

	void loadingmap(const char *name)
	{
		execident("playsong");
	}

	void startmap(const char *name)   // called just after a map load
	{
		ai::savewaypoints();
		ai::clearwaypoints(true);

		respawnent = -1; // so we don't respawn at an old spot
		if(!m_mp(gamemode)) spawnplayer(player1);
		else findplayerspawn(player1, -1);
		entities::resetspawns();
		copystring(clientmap, name ? name : "");

		sendmapinfo();
	}

	const char *getmapinfo()
	{
		return showmodeinfo && m_valid(gamemode) ? gamemodes[gamemode - STARTGAMEMODE].name : NULL;
	}

	const char *getscreenshotinfo()
	{
		return server::modename(gamemode, NULL);
	}

	void physicstrigger(physent *d, bool local, int floorlevel, int waterlevel, int material)
	{
		if     (waterlevel>0) { if(material!=MAT_LAVA) playsound(S_SPLASH1, d==player1 ? NULL : &d->o); }
		else if(waterlevel<0) playsound(material==MAT_LAVA ? S_BURN : S_SPLASH2, d==player1 ? NULL : &d->o);
		if     (floorlevel>0) { if(d==player1 || d->type!=ENT_PLAYER || ((fpsent *)d)->ai) msgsound(S_JUMP, d); }
		else if(floorlevel<0) { if(d==player1 || d->type!=ENT_PLAYER || ((fpsent *)d)->ai) msgsound(S_LAND, d); }
	}

	void dynentcollide(physent *d, physent *o, const vec &dir)
	{
		// none needed
	}

	void msgsound(int n, physent *d)
	{
		if(!d || d==player1)
		{
			addmsg(N_SOUND, "ci", d, n);
			playsound(n);
		}
		else
		{
			if(d->type==ENT_PLAYER && ((fpsent *)d)->ai)
				addmsg(N_SOUND, "ci", d, n);
			playsound(n, &d->o);
		}
	}

	int numdynents() { return players.length(); }

	dynent *iterdynents(int i)
	{
		if(i<players.length()) return players[i];
		return NULL;
	}

	bool duplicatename(fpsent *d, const char *name = NULL, const char *alt = NULL)
	{
		if(!name) name = d->name;
		if(alt && d != player1 && !strcmp(name, alt)) return true;
		loopv(players) if(d!=players[i] && !strcmp(name, players[i]->name)) return true;
		return false;
	}

	const char* colorname(fpsent* d, const char* name, const char* prefix, const char* suffix, const char* alt, bool tags, bool scoreboard)
	{
		if (!name) name = d->name;
		bool dup = !name[0] || (showclientnum >= 2 && !scoreboard && (!alt || d != player1)) || duplicatename(d, name, alt) || d->aitype != AI_NONE;
		cidx = (cidx + 1) % 3;
		if (d->aitype == AI_NONE && showtags && tags) {
			getplayerinfo(d);
			prefix = d->user->tags;
			if (prefix == NULL) prefix = "";
		}
		if (dup || prefix[0] || suffix[0])
		{
			if (dup)
			{
				if (d->aitype == AI_NONE)
				{
					formatstring(cname[cidx], "%s%s \fs\f8(%d)\fr%s", prefix, name, d->clientnum, suffix);
				}
				else
				{
					formatstring(cname[cidx], "%s\fs\f1%s \f4(%d) \f5[%d]\fr%s", prefix, name, d->skill, d->clientnum, suffix);
				}
			}
			else formatstring(cname[cidx], "%s%s%s", prefix, name, suffix);
			return cname[cidx];
		}
		else formatstring(cname[cidx], "%s", name);
		return cname[cidx];
	}

	VARP(teamcolortext, 0, 1, 1);

	const char* teamcolorname(fpsent* d, const char* alt, bool tags)
	{
		if (!teamcolortext || !m_teammode || d->state == CS_SPECTATOR) return colorname(d, NULL, "", "", alt, tags);
		return colorname(d, NULL, strcmp(d->team, "red") ? "\fs\f1" : "\fs\f3", "\fr", alt, tags);
	}

	// unused??
	const char* teamcolor(const char* name, bool sameteam, const char* alt)
	{
		if (!teamcolortext || !m_teammode) return sameteam || !alt ? name : alt;
		cidx = (cidx + 1) % 3;
		formatstring(cname[cidx], sameteam ? "\fs\f1%s\fr" : "\fs\f3%s\fr", sameteam || !alt ? name : alt);
		return cname[cidx];
	}

	const char* teamcolor(const char* name, const char* team, const char* alt)
	{
		cidx = (cidx + 1) % 3;
		if (!team) return name; // team could be null, maybe indicative of problems elsewhere?
		formatstring(cname[cidx], strcmp(team, "red") ? "\fs\f1%s\fr" : "\fs\f3%s\fr", name);
		return cname[cidx];
	}

	VARP(teamsounds, 0, 1, 1);

	void teamsound(bool sameteam, int n, const vec *loc)
	{
		playsound(n, loc, NULL, teamsounds ? (m_teammode && sameteam ? SND_USE_ALT : SND_NO_ALT) : 0);
	}

	void teamsound(fpsent *d, int n, const vec *loc)
	{
		teamsound(isteam(d->team, player1->team), n, loc);
	}

	void suicide(physent *d)
	{
		if(d==player1 || (d->type==ENT_PLAYER && ((fpsent *)d)->ai))
		{
			if(d->state!=CS_ALIVE) return;
			fpsent *pl = (fpsent *)d;
			if(!m_mp(gamemode)) killed(pl, pl, 0);
			else
			{
				int seq = (pl->lifesequence<<16)|((lastmillis/1000)&0xFFFF);
				if(pl->suicided!=seq) { addmsg(N_SUICIDE, "rc", pl); pl->suicided = seq; }
			}
		}
	}
	ICOMMAND(suicide, "", (), suicide(player1));
	ICOMMAND(kill, "", (), suicide(player1)); // troll

	bool needminimap() { return m_ctf || m_protect || m_hold || m_capture || m_collect || m_race; }

	void drawicon(int icon, float x, float y, float sz)
	{
		defformatstring(iconmap, "packages/hud/items%d.png", (int)(icon / 16));
		settexture(iconmap);
		float tsz = 0.25f, tx = tsz*(icon%4), ty = tsz*(icon/4);
		gle::defvertex(2);
		gle::deftexcoord0();
		gle::begin(GL_TRIANGLE_STRIP);
		gle::attribf(x,    y);    gle::attribf(tx,     ty);
		gle::attribf(x+sz, y);    gle::attribf(tx+tsz, ty);
		gle::attribf(x,    y+sz); gle::attribf(tx,     ty+tsz);
		gle::attribf(x+sz, y+sz); gle::attribf(tx+tsz, ty+tsz);
		gle::end();
	}

	// TODO: I don't like this, rewrite this
	void drawhealth(fpsent* d, bool isinsta = false, float tx = 0, float ty = 0, float tw = 1, float th = 1)
	{
		float barh = HICON_SIZE, barw = ((HICON_TEXTY / 2) - barh) + (HICON_SIZE * .5);
		float h = (d->state == CS_DEAD ? 0 : ((d->health * barh) / d->maxhealth)), w = HICON_SIZE * .5;
		float x = (HICON_X / 2), y = ((HICON_TEXTY / 2) - h) + (HICON_SIZE * .5);

		//conoutf("%f/%f=%f", (d->health * barh), (m_insta ? 1 : d->maxhealth), (d->health*barh)/(m_insta ? 1 : d->maxhealth));
		//conoutf("%d (%d)", d->health, d->maxhealth);

		settexture("packages/hud/health_bar_back.png", 3);
		gle::begin(GL_TRIANGLE_STRIP);
		gle::attribf(x, barw);   gle::attribf(tx, ty);
		gle::attribf(x + w, barw);   gle::attribf(tx + tw, ty);
		gle::attribf(x, barw + barh); gle::attribf(tx, ty + th);
		gle::attribf(x + w, barw + barh); gle::attribf(tx + tw, ty + th);
		gle::end();

		settexture("packages/hud/health_bar.png", 3);
		gle::begin(GL_TRIANGLE_STRIP);
		gle::attribf(x, y);   gle::attribf(tx, ty);
		gle::attribf(x + w, y);   gle::attribf(tx + tw, ty);
		gle::attribf(x, y + h); gle::attribf(tx, ty + th);
		gle::attribf(x + w, y + h); gle::attribf(tx + tw, ty + th);
		gle::end();
	}

	float abovegameplayhud(int w, int h)
	{
		switch(hudplayer()->state)
		{
			case CS_EDITING:
			case CS_SPECTATOR:
				return 1;
			default:
				return 1500.0f/1800.0f;
		}
	}

	VARP(ammohud, 0, 1, 1);
	VARP(ammohudhidemelee, 0, 0, 1);
	VARP(ammohudspacing, 0, 0, 1);
	VARP(ammohudoffset, 0, 0, 3);
	FVARP(ammohudscale, 1, 1, 2);

	VARP(healthbar, 0, 1, 1);
	VARP(healthcolors, 0, 1, 1);

	VARP(delayammo, 0, 1, 1);

	void drawammohud(fpsent* d) // this is messy, clean up later -Y 04/03/2020
	{
		gle::color(bvec::hexcolor(0xFFFFFF), 255);
		int gunact = 0; // ammount of active guns
		for (int i = 0; i < int(7 - ammohudhidemelee); ++i) {
			gle::color(bvec::hexcolor(!(lastmillis - d->lastaction[i + ammohudhidemelee] >= d->gunwait[i + ammohudhidemelee]) ? 0x888888 : 0xFFFFFF), 255);
			if (d->ammo[i + ammohudhidemelee] > 0)
			{
				int spa;
				if (!ammohudspacing) { spa = gunact; gunact++; } // only count up when ammo is present
				else spa = i + ammohudhidemelee; // always count up



				drawicon(((i == GUN_FIST && d->jumpstate == 2 && !ammohudhidemelee) ? HICON_FIST_OFF : HICON_FIST + i + ammohudhidemelee), HICON_X + ((healthbar ? 3 + ammohudoffset : 4 + ammohudoffset) * HICON_STEP) + (spa * HICON_SIZE / ammohudscale), HICON_Y, 120 / ammohudscale);
			}
		}
	}

	void drawhudicons(fpsent* d)
	{
		pushhudmatrix();
		hudmatrix.scale(2, 2, 1);
		flushhudmatrix();

		if (healthbar) drawhealth(d, m_insta);

		defformatstring(health, "%d", d->state == CS_DEAD ? 0 : d->health);
		bvec healthcolor = bvec::hexcolor(healthcolors && !m_insta ? (d->state == CS_DEAD ? 0x808080 : (d->health <= 400 ? 0xFF0000 : (d->health <= 600 ? 0xFF8000 : 0xFFFFFF))) : 0xFFFFFF);
		draw_text(health, (HICON_X + (healthbar ? 0 : (20 + HICON_SIZE + HICON_SPACE))) / 2, HICON_TEXTY / 2, healthcolor.r, healthcolor.g, healthcolor.b);
		if (d->state != CS_DEAD)
		{
			old_string ammocount = "INF";
			if (!m_bottomless) formatstring(ammocount, "%d", d->ammo[d->gunselect]);

			bvec ammocolor = bvec::hexcolor((delayammo && !(lastmillis - d->lastaction[d->gunselect] >= d->gunwait[d->gunselect])) ? 0x888888 : 0xFFFFFF);
			draw_text(ammocount, (HICON_X + (healthbar ? 1 : 2) * HICON_STEP + HICON_SIZE + HICON_SPACE) / 2, HICON_TEXTY / 2, ammocolor.r, ammocolor.g, ammocolor.b);
		}

		pophudmatrix();

		if (!healthbar) drawicon(HICON_HEALTH, HICON_X, HICON_Y);
		if (d->state != CS_DEAD)
		{
			if (d->gunselect == GUN_FIST)
			{
				drawicon((d->jumpstate == 2 ? HICON_FIST_OFF : HICON_FIST), HICON_X + (healthbar ? 1 : 2) * HICON_STEP, HICON_Y);
			}
			else drawicon(HICON_FIST + d->gunselect, HICON_X + (healthbar ? 1 : 2) * HICON_STEP, HICON_Y);

			if (ammohud) drawammohud(d);
		}
	}

	VARP(gameclock, 0, 1, 1);
	FVARP(gameclockscale, 0.1f, 0.5f, 2.0f);
	HVARP(gameclockcolor, 0, 0xFFFFFF, 0xFFFFFF);
	HVARP(gameclocklowcolor, 0, 0xFFC040, 0xFFFFFF);
	HVARP(gameclockcritcolor, 0, 0xFF0000, 0xFFFFFF);
	VARP(gameclockalpha, 0, 255, 255);
	VARP(gameclockalign, -1, 0, 1);
	FVARP(gameclockx, 0.0f, 0.50f, 1.0f);
	FVARP(gameclocky, 0.0f, 0.085f, 1.0f);

	void drawgameclock(int w, int h)
	{
		int secs = max(maplimit-lastmillis + 999, 0)/1000, mins = secs/60;
		secs %= 60;

		defformatstring(buf, "%d:%02d", mins, secs);
		int tw = 0, th = 0;
		text_bounds(buf, tw, th);

		vec2 offset = vec2(gameclockx, gameclocky).mul(vec2(w, h).div(gameclockscale));
		if(gameclockalign == 1) offset.x -= tw;
		else if(gameclockalign == 0) offset.x -= tw/2.0f;
		offset.y -= th/2.0f;

		pushhudmatrix();
		hudmatrix.scale(gameclockscale, gameclockscale, 1);
		flushhudmatrix();

		int color = mins < 1 ? secs < 10 ? gameclockcritcolor : gameclocklowcolor : gameclockcolor;
		if (m_timed) draw_text(buf, int(offset.x), int(offset.y), (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, gameclockalpha);

		pophudmatrix();
	}

	extern int hudscore;
	extern void drawhudscore(int w, int h);

	VARP(showphyscompass, 0, 0, 1);
	HVARP(physcompassvelcolor, 0, 0xFFFFFF, 0xFFFFFF);
	HVARP(physcompassinputcolor, 0, 0xFF0000, 0xFFFFFF);

	void drawphyscompassvector(int& color, float w, float yaw, float magnitude)
	{
		pushhudmatrix();
		hudmatrix.rotate_around_z(yaw);
		flushhudmatrix();
		gle::defvertex(2);
		gle::color(bvec((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF), 0xff);
		gle::begin(GL_TRIANGLE_STRIP);
		const float h = magnitude * 150.0f;
		const float x = -0.5f * w, y = 0.0f;
		gle::attribf(x, y);
		gle::attribf(x + w, y);
		gle::attribf(x, y + h);
		gle::attribf(x + w, y + h);
		gle::end();
		pophudmatrix();
	}

	void drawphyscompass(fpsent* d, int w, int h)
	{
		enabletexture(false);
		pushhudmatrix();
		hudmatrix.translate(vec(400.0f, 1400.0f, 0.0f));
		hudmatrix.rotate_around_z(90.0f * RAD);
		flushhudmatrix();

		const float velyaw = atan2f(d->vel.y, d->vel.x) - d->yaw * RAD;
		const float velmag = min(3.0f, d->vel.magnitude2() / d->maxspeed);
		drawphyscompassvector(physcompassvelcolor, 10.0f, velyaw, velmag);

		const float inputyaw = atan2f(d->fmove, d->fstrafe);
		const float inputmag = min(1.0f, vec2(d->fmove, d->fstrafe).magnitude());
		drawphyscompassvector(physcompassinputcolor, 7.5f, inputyaw, inputmag);

		pophudmatrix();
		enabletexture(true);
	}

	VARP(demohud, 0, 1, 1);

	VARP(showpointed, 0, 0, 1);
	fpsent* lastpointed = NULL;

	void gameplayhud(int w, int h)
	{
		pushhudmatrix();
		hudmatrix.scale(h/1800.0f, h/1800.0f, 1);
		flushhudmatrix();

		if (spectating(player1) || (demoplayback && demohud && spectating(player1)))
		{
			int pw, ph, tw, th, fw, fh;
			text_bounds("  ", pw, ph);
			text_bounds("SPECTATOR", tw, th);
			th = max(th, ph);
			fpsent *f = followingplayer();
			text_bounds(f ? colorname(f) : " ", fw, fh);
			fh = max(fh, ph);
			draw_text("SPECTATOR", w*1800/h - tw - pw, 1650 - th - fh);
			if(f)
			{
				int color = statuscolor(f, 0xFFFFFF);
				draw_text(teamcolorname(f), w*1800/h - fw - pw, 1650 - fh, (color>>16)&0xFF, (color>>8)&0xFF, color&0xFF);
			}
		}

		fpsent *d = hudplayer();
		if(d->state!=CS_EDITING)
		{
			if (d->state!=CS_SPECTATOR) drawhudicons(d);
			if (showphyscompass) drawphyscompass(d, w, h);
			if (cmode) cmode->drawhud(d, w, h);
		}

		pophudmatrix();

		if (!m_edit)
		{
			if (gameclock) drawgameclock(w, h);
			if (hudscore) drawhudscore(w, h);
			if (hudfragmessages == 1 || (hudfragmessages == 2 && !m_insta)) drawfragmessages(w, h);
		}

		fpsent* p = pointatplayer();
		if (p && player1->state != CS_DEAD && p->state != CS_EDITING) 
		{
			lastpointed = p;
		}
		else if (!p) lastpointed = NULL;

		if (lastpointed && (intermission || lastpointed != getclient(lastpointed->clientnum))) 
		{
			lastpointed = NULL;
		}

		if (lastpointed && player1->state != CS_SPECTATOR && showpointed) 
		{
			pushhudmatrix();
			hudmatrix.scale(h / 1800.0f, h / 1800.0f, 1);
			flushhudmatrix();

			int rpw, rph, pw, ph;
			old_string pointedname;
			formatstring(pointedname, "%s", colorname(lastpointed));
			text_bounds(pointedname, rpw, rph);
			text_bounds("  ", pw, ph);
			draw_textf("%s", w * 1800 / h - rpw - pw, 1650 - rph - 16, pointedname);
			pophudmatrix();
		}
	}

	int clipconsole(int w, int h)
	{
		if(cmode) return cmode->clipconsole(w, h);
		return 0;
	}

	VARP(teamcrosshair, 0, 1, 1);
	VARP(healthcrosshair, 0, 1, 1);
	VARP(delaycrosshair, 0, 1, 1);
	VARP(hitcrosshair, 0, 425, 1000);

	const char *defaultcrosshair(int index)
	{
		switch (index)
		{
			case 2: return "packages/crosshairs/default_hit.png";
			case 1: return "packages/crosshairs/default_teammate.png";
			default: return "packages/crosshairs/default.png";
		}
	}

	int selectcrosshair(vec &color)
	{
		fpsent *d = hudplayer();
		if(d->state==CS_SPECTATOR || d->state==CS_DEAD) return -1;

		if(d->state!=CS_ALIVE) return 0;

		int crosshair = 0;
		if (lasthit && lastmillis - lasthit < hitcrosshair && !(m_parkour && !parkourhitmarker))
		{
			crosshair = 2;
		}
		else if(teamcrosshair && !m_parkour)
		{
			dynent *o = intersectclosest(d->o, worldpos, d);
			if(o && o->type==ENT_PLAYER && isteam(((fpsent *)o)->team, d->team))
			{
				crosshair = 1;
				// TODO: team update
				color = !strcmp(d->team, "red") ? vec(1, 0, 0) : vec(0, 0, 1);
			}
		}

		if (crosshair != 1 && !editmode && !m_insta && healthcrosshair)
		{
			if (d->health <= 200) color = vec(1, 0, 0);
			else if (d->health <= 600) color = vec(1, (d->health - 200) / 400.0f, 0);
			else if (d->health <= 1000) color = vec(1, 1, (d->health - 600) / 400.0f);
			else if (d->health <= 2000) color = vec((2000 - d->health) / 1000.0f, 1, (2000 - d->health) / 1000.0f);
			else color = vec(0, 1, 0);
		}
		if (d->gunwait[d->gunselect] && delaycrosshair) color.mul(0.5f);
		return crosshair;
	}

	void lighteffects(dynent *e, vec &color, vec &dir)
	{
		// TODO: what is this?
#if 0
		fpsent *d = (fpsent *)e;
		if(d->state!=CS_DEAD && d->quadmillis)
		{
			float t = 0.5f + 0.5f*sinf(2*M_PI*lastmillis/1000.0f);
			color.y = color.y*(1-t) + t;
		}
#endif
	}

	int maxsoundradius(int n)
	{
		switch(n)
		{
			case S_JUMP:
			case S_LAND:
			case S_WEAPLOAD:
			case S_ITEMAMMO:
			case S_ITEMHEALTH:
			case S_ITEMPUP:
			case S_ITEMSPAWN:
			case S_NOAMMO:
			case S_PUPOUT:
				return 340;
			default:
				return 500;
		}
	}

	#define justified(elem,handleclick,dir) \
	{ \
		g->pushlist(); \
		if(dir) g->spring(); \
		g->pushlist(); /* get vertical list dir back, so mergehits works */ \
		int up = elem; \
		g->poplist(); \
		g->poplist(); \
		if(handleclick && up&G3D_UP) return true; \
	}

	#include "colors.h"

	bool serverinfostartcolumn(g3d_gui *g, int i)
	{
		static const char * const names[] = { "ping", "players", "mode", "map", "time", "master", "host", "port", "description" };
		static const float struts[] =       {      7,         7,  12.5f,    14,      7,        8,     14,      7,         24.5f };
		static const float alignments[] =   {      1,         1,      0,     0,      0,        0,      0,      1,             0 }; // 0 = left, 1 = right
		
		if (size_t(i) >= sizeof(names)/sizeof(names[0])) return false;

		if (i) g->space(2);
		g->pushlist();
		justified(g->text(names[i], COL_GRAY, NULL), false, alignments[i]);
		if(struts[i]) g->strut(struts[i]);
		g->mergehits(true);
		return true;
	}

	void serverinfoendcolumn(g3d_gui *g, int i)
	{
		g->mergehits(false);
		g->column(i);
		g->poplist();
	}

	const char *mastermodecolor(int n, const char *unknown)
	{
		return (n>=MM_START && size_t(n-MM_START)<sizeof(mastermodecolors)/sizeof(mastermodecolors[0])) ? mastermodecolors[n-MM_START] : unknown;
	}

	const char *mastermodeicon(int n, const char *unknown)
	{
		return (n>=MM_START && size_t(n-MM_START)<sizeof(mastermodeicons)/sizeof(mastermodeicons[0])) ? mastermodeicons[n-MM_START] : unknown;
	}

	SVAR(filterservers, "");

	bool serverinfoentry(g3d_gui *g, int i, const char *name, int port, const char *sdesc, const char *map, int ping, const vector<int> &attr, int np)
	{
		if (*filterservers) {
			if (!cubecasefind(sdesc, filterservers) && !cubecasefind(map, filterservers) &&
				(attr.length() < 2 || !cubecasefind(server::modename(attr[1], ""), filterservers)))
			{
				return false;
			}
		}		

		#define leftjustified(elem)   justified(elem,true,0)
		#define rightjustified(elem)  justified(elem,true,1)

		const char* pingcolor;
		if (attr.length() >= 4) {
			if (ping < 70) {
				pingcolor = "\f0";
			}
			else if (ping < 135) {
				pingcolor = "\f2";
			}
			else if (ping < 200) {
				pingcolor = "\f6";
			}
			else {
				pingcolor = "\f3";
			}
		}
		else {
			pingcolor = "";
		}

		// if server is invalid or incorrect protocol version
		if(ping < 0 || attr.empty() || attr[0]!=PROTOCOL_VERSION)
		{
			switch(i)
			{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					leftjustified(g->button(" ", COL_WHITE));
					break;
				case 7:
					leftjustified(g->buttonf("%s", COL_WHITE, NULL, name));
					break;
				case 8:
					if(ping < 0)
					{
						leftjustified(g->button(sdesc, COL_WHITE));
					}
					else leftjustified(g->buttonf("[%s version, %d]", COL_WHITE, NULL, attr.empty() ? "unknown" : (attr[0] < PROTOCOL_VERSION ? "older" : "newer"), attr.empty() ? 0 : attr[0]));
					break;
			}
			return false;
		}

		// fill out server info
		switch(i)
		{
			case 0:
			{
				rightjustified(g->buttonf("%s%d", COL_WHITE, NULL, pingcolor, ping));
				break;
			}

			case 1:
				if(attr.length()>=4)
				{
					rightjustified(g->buttonf(np >= attr[3] ? "\f3%d/%d" : "%d/%d", COL_WHITE, NULL, np, attr[3]));
				}
				else rightjustified(g->buttonf("%d", COL_WHITE, NULL, np));
				break;

			case 2:
				leftjustified(g->buttonf("%s", COL_WHITE, NULL, attr.length() >= 2 ? server::modename(attr[1], "") : ""));
				break;

			case 3:
				leftjustified(g->buttonf("%.25s", COL_WHITE, NULL, map));
				break;

			case 4:
				if(attr.length()>=3 && attr[2] > 0)
				{
					int secs = clamp(attr[2], 0, 59*60+59),
						mins = secs/60;
					secs %= 60;
					leftjustified(g->buttonf("%d:%02d", COL_WHITE, NULL, mins, secs));
				}
				else leftjustified(g->button(" ", COL_WHITE));
				break;

			case 5:
				leftjustified(g->buttonf("%s%s", COL_WHITE, NULL, attr.length() >= 5 ? mastermodecolor(attr[4], "") : "", attr.length() >= 5 ? server::mastermodename(attr[4], "") : ""));
				break;

			case 6:
				leftjustified(g->buttonf("%s", COL_WHITE, NULL, name));
				break;

			case 7:
				rightjustified(g->buttonf("%d", COL_WHITE, NULL, port));
				break;

			case 8:
				leftjustified(g->buttonf("%s", COL_WHITE, NULL, sdesc));
				break;
		}
		return false;
	}

	// any data written into this vector will get saved with the map data. Must take care to do own versioning, and endianess if applicable. Will not get called when loading maps from other games, so provide defaults.
	void writegamedata(vector<char> &extras) {}
	void readgamedata(vector<char> &extras) {}

	const char *savedconfig() { return "config.cfg"; }
	const char *restoreconfig() { return "restore.cfg"; }
	const char *defaultconfig() { return "data/defaults.cfg"; }
	const char *autoexec() { return "autoexec.cfg"; }
	const char *savedservers() { return "servers.cfg"; }

	ICOMMAND(islagged, "i", (int *cn),
	{
		fpsent *d = getclient(*cn);
		if(d) intret(d->state==CS_LAGGED ? 1 : 0);
	});

	ICOMMAND(isdead, "i", (int *cn),
	{
		fpsent *d = getclient(*cn);
		if(d) intret(d->state==CS_DEAD ? 1 : 0);
	});

	void loadconfigs()
	{
		execident("playsong");

		execfile("auth.cfg", false);
	}
}

