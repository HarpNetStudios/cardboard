// creation of scoreboard
#include "game.h"
#include "colors.h"
#include "weaponstats.h"

namespace game
{
	VARP(scoreboard2d, 0, 1, 1);
	VARP(showservinfo, 0, 1, 1);
	VARP(showclientnum, 0, 0, 2);
	VARP(showpj, 0, 0, 1);
	VARP(showping, 0, 1, 2);
	VARP(showspectators, 0, 1, 1);
	VARP(showspectatorping, 0, 0, 1);
	VARP(highlightscore, 0, 1, 1);
	VARP(showconnecting, 0, 0, 1);
	VARP(hidefrags, 0, 1, 1);
	VARP(showdeaths, 0, 0, 1);
	VARP(showsuicides, 0, 0, 1);
	VARP(showflags, 0, 0, 1);
	VARP(showkdr, 0, 0, 1);
	VARP(showaccuracy, 0, 0, 2);
	VARP(showdamage, 0, 0, 2);
	VARP(showdamagereceived, 0, 0, 1);
	VARP(showracetime, 0, 1, 1);
	VARP(showracecheckpoints, 0, 1, 1);
	VARP(showracelaps, 0, 1, 1);
	VARP(showracerank, 0, 1, 1);

	static hashset<teaminfo> teaminfos;

	void clearteaminfo()
	{
		teaminfos.clear();
	}
	
	void setteaminfo(const char *team, int frags)
	{
		teaminfo *t = teaminfos.access(team);
		if(!t) { t = &teaminfos[team]; copystring(t->team, team, sizeof(t->team)); }
		t->frags = frags;
	}

	static inline bool playersort(const fpsent *a, const fpsent *b)
	{
		if(a->state==CS_SPECTATOR)
		{
			if(b->state==CS_SPECTATOR) return strcmp(a->name, b->name) < 0;
			else return false;
		}
		else if(b->state==CS_SPECTATOR) return true;
		if(m_ctf || m_collect)
		{
			if(a->flags > b->flags) return true;
			if(a->flags < b->flags) return false;
		}
		if(m_race)
		{
			if(a->racerank > b->racerank) return true;
			if(a->racerank < b->racerank) return false;
		}
		if(a->frags > b->frags) return true;
		if(a->frags < b->frags) return false;
		return strcmp(a->name, b->name) < 0;
	}

	void getbestplayers(vector<fpsent *> &best)
	{
		loopv(players)
		{
			fpsent *o = players[i];
			if(o->state!=CS_SPECTATOR) best.add(o);
		}
		best.sort(playersort);
		while(best.length() > 1 && best.last()->frags < best[0]->frags) best.drop();
	}

	void getbestteams(vector<const char *> &best)
	{
		if(cmode && cmode->hidefrags())
		{
			vector<teamscore> teamscores;
			cmode->getteamscores(teamscores);
			teamscores.sort(teamscore::compare);
			while(teamscores.length() > 1 && teamscores.last().score < teamscores[0].score) teamscores.drop();
			loopv(teamscores) best.add(teamscores[i].team);
		}
		else
		{
			int bestfrags = INT_MIN;
			enumerate(teaminfos, teaminfo, t, bestfrags = max(bestfrags, t.frags));
			if(bestfrags <= 0) loopv(players)
			{
				fpsent *o = players[i];
				if(o->state!=CS_SPECTATOR && !teaminfos.access(o->team) && best.htfind(o->team) < 0) { bestfrags = 0; best.add(o->team); }
			}
			enumerate(teaminfos, teaminfo, t, if(t.frags >= bestfrags) best.add(t.team));
		}
	}

	vector<scoregroup *> groups;
	vector<fpsent *> spectators;

	static inline bool scoregroupcmp(const scoregroup *x, const scoregroup *y)
	{
		if(!x->team)
		{
			if(y->team) return false;
		}
		else if(!y->team) return true;
		if(x->score > y->score) return true;
		if(x->score < y->score) return false;
		if(x->players.length() > y->players.length()) return true;
		if(x->players.length() < y->players.length()) return false;
		return x->team && y->team && strcmp(x->team, y->team) < 0;
	}

	int groupplayers()
	{
		int numgroups = 0;
		spectators.setsize(0);
		loopv(players)
		{
			fpsent *o = players[i];
			if(!showconnecting && !o->name[0]) continue;
			if(o->state==CS_SPECTATOR) { spectators.add(o); continue; }
			const char *team = m_teammode && o->team[0] ? o->team : NULL;
			bool found = false;
			loopj(numgroups)
			{
				scoregroup &g = *groups[j];
				if(team!=g.team && (!team || !g.team || strcmp(team, g.team))) continue;
				g.players.add(o);
				found = true;
			}
			if(found) continue;
			if(numgroups>=groups.length()) groups.add(new scoregroup);
			scoregroup &g = *groups[numgroups++];
			g.team = team;
			if(!team) g.score = 0;
			else if(cmode && cmode->hidefrags()) g.score = cmode->getteamscore(o->team);
			else { teaminfo *ti = teaminfos.access(team); g.score = ti ? ti->frags : 0; }
			g.players.setsize(0);
			g.players.add(o);
		}
		loopi(numgroups) groups[i]->players.sort(playersort);
		spectators.sort(playersort);
		groups.sort(scoregroupcmp, 0, numgroups);
		return numgroups;
	}

	int statuscolor(fpsent *d, int color)
	{
		if(d->privilege) color = d->privilege>=PRIV_ADMIN ? COL_ADMIN : (d->privilege>=PRIV_AUTH ? COL_AUTH : COL_MASTER);
		if(d->state==CS_DEAD) color = (color>>1)&0x7F7F7F;
		return color;
	}

	HVARP(sbhighlight, 0, 0xE0AEB7, 0xFFFFFF);
	VARP(pingcolor, 0, 1, 1);

	void renderscoreboard(g3d_gui &g, bool firstpass)
	{
		g.space(.25f);

		const ENetAddress *address = connectedpeer();
		if(showservinfo && address)
		{
			cbstring hostname;
			if(enet_address_get_host_ip(address, hostname, sizeof(hostname)) >= 0)
			{
				if(servinfo[0]) g.titlef("%.50s", COL_WHITE, NULL, servinfo);
				else g.titlef("%s:%d", COL_WHITE, NULL, hostname, address->port);
			}
		}

		g.pushlist();
		g.spring();
		g.text(server::modename(gamemode), COL_WHITE);
		g.space(3);
		const char *mname = getclientmap();
		g.text(mname[0] ? mname : "[new map]", COL_WHITE);
		extern int gamespeed;
		if(m_timed && mname[0] && (maplimit >= 0 || intermission))
		{
			g.space(3);
			if(intermission) g.text("intermission", COL_WHITE);
			else
			{
				int secs = max(maplimit-lastmillis+999, 0)/1000, mins = secs/60;
				secs %= 60;
				g.pushlist();
				g.strut(mins >= 10 ? 4.5f : 3.5f);
				g.textf("%d:%02d", COL_WHITE, NULL, mins, secs);
				g.poplist();
			}
		}
		if(gamespeed != 100) { g.space(3); g.textf("%d.%02dx", COL_GRAY, NULL, gamespeed/100, gamespeed%100); }
		if(ispaused()) { g.space(3); g.text("paused", COL_GRAY); }
		g.spring();
		g.poplist();

		g.space(.75f);

		int numgroups = groupplayers();
		loopk(numgroups)
		{
			if((k%2)==0) g.pushlist(); // horizontal

			scoregroup &sg = *groups[k];
			int teamcolor = sg.team && m_teammode ? (strcmp(sg.team, "red") ? COL_BLUE : COL_RED) : 0;

			g.pushlist(); // vertical
			g.pushlist(); // horizontal

			#define loopscoregroup(o, b) \
				loopv(sg.players) \
				{ \
					fpsent *o = sg.players[i]; \
					b; \
				}

			#define rightjustified(b) \
				{ \
					g.pushlist(); \
					g.spring(); \
					b; \
					g.poplist(); \
				}

			#define fgcolor (o==hudplayer() && highlightscore && (multiplayer(false) || demoplayback || players.length() > 1) ? sbhighlight : COL_WHITE)

			g.pushlist();
			g.text("", 0, " ");
			loopscoregroup(o,
			{
				if (o == player1 && highlightscore && (multiplayer(false) || demoplayback || players.length() > 1))
				{
					g.pushlist();
					g.background(0x808080, numgroups > 1 ? 3 : 5);
				}
				const playermodelinfo& mdl = getplayermodelinfo(o);
				const char* icon = sg.team && m_teammode ? (strcmp(sg.team, "red") ? mdl.blueicon : mdl.redicon) : mdl.ffaicon;
				g.text("", 0, icon);
				if (o == player1 && highlightscore && (multiplayer(false) || demoplayback || players.length() > 1)) g.poplist();
			});
			g.poplist();

			if (sg.team && m_teammode)
			{
				g.pushlist(); // vertical
				if (sg.score >= 10000) g.textf("%s: WIN", teamcolor, NULL, sg.team);
				else g.textf("%s: %d", teamcolor, NULL, sg.team, sg.score);
				g.pushlist(); // horizontal
			}

			g.pushlist();
			g.text("name", COL_GRAY);
			loopscoregroup(o, g.text(colorname(o, NULL, "", "", NULL, true), statuscolor(o, fgcolor)););
			g.poplist();

			if ((m_collect || m_ctf || m_hold || m_protect) && showflags)
			{
				g.space(2);
				g.pushlist();
				rightjustified(g.text("flags", COL_GRAY))
					loopscoregroup(o, rightjustified(g.textf("%d", fgcolor, NULL, o->flags)));
				g.poplist();
			}

			if (!cmode || !cmode->hidefrags() || !hidefrags || m_parkour)
			{
				g.space(2);
				g.pushlist();
				rightjustified(g.text("frags", COL_GRAY))
					loopscoregroup(o, rightjustified(g.textf("%d", fgcolor, NULL, o->frags)));
				g.poplist();
			}

			if ((showdeaths && !m_parkour) || m_parkour)
			{
				g.space(2);
				g.pushlist();
				rightjustified(g.text("deaths", COL_GRAY))
					loopscoregroup(o, rightjustified(g.textf("%d", fgcolor, NULL, o->deaths)));
				g.poplist();
			}

			// REWORK LATER -Y
			if(m_collect)
			{
				g.pushlist();
				g.text("", 0x000000);
				loopscoregroup(o, {
					if(o->tokens) {
						g.textf("%d", 0xFFFFDD, NULL, o->tokens);
					} else {
						g.text("", 0x000000);
					}
				});
				g.poplist();

				g.pushlist();
				g.text("", 0x000000);
				loopscoregroup(o, {
					if(o->tokens) {
						g.text("", 0x000000, !isteam("red", o->team) ? "../hud/blip_red_skull.png" : "../hud/blip_blue_skull.png");
					} else {
						g.text("", 0x000000);
					}
				});
				g.poplist();
			}
			else if(m_ctf || m_protect || m_hold) 
			{
				g.pushlist();
				g.text("", 0x000000);
				loopscoregroup(o, {
					char *icon = NULL;
					if(m_ctf && !m_protect && !m_hold) {
						icon = !isteam("red", o->team) ? (char *)"../hud/blip_red_flag.png" : (char *)"../hud/blip_blue_flag.png";
					} else if(m_protect) {
						icon = !isteam("red", o->team) ? (char *)"..hud/blip_blue_flag.png" : (char *)"../hud/blip_red_flag.png";
					} else { // m_hold
						icon = (char *)"../hud/blip_neutral_flag.png";
					}

					if(icon && o->hasflag) {
						g.text("", 0x000000, icon);
					} else {
						g.text("", 0x000000);
					}
				})
				g.poplist();
			}

			if (showsuicides)
			{
				g.space(2);
				g.pushlist();
				rightjustified(g.text("suis", COL_GRAY))
					loopscoregroup(o, rightjustified(g.textf("%d", fgcolor, NULL, o->suicides)));
				g.poplist();
			}

			if (showkdr)
			{
				if (showpj || showping) g.space(1);
				g.space(2);
				g.pushlist();
				g.strut(3);
				rightjustified(g.text("kdr", COL_GRAY))
					loopscoregroup(o, rightjustified(g.textf("%.1f", fgcolor, NULL, (float)o->frags / max(1, o->deaths))));
				g.poplist();
			}

			if (showaccuracy)
			{
				g.space(2);
				g.pushlist();
				g.strut(4);
				rightjustified(g.text("acc", COL_GRAY))
				loopscoregroup(o,
					{
						extclient* ec = getextclient(o->clientnum);
						int acc = ec ? (ec)->accuracy : playeraccuracy(o);
						rightjustified(g.textf("%.0f%%", fgcolor, NULL, acc));
					});
				g.poplist();
			}

			if(m_race)
			{
				if (showracerank) 
				{
					g.space(2);
					g.pushlist();
					g.strut(4);
					rightjustified(g.text("rank", COL_GRAY))
					loopscoregroup(o, {
						switch (o->racestate) {
						case 0:
							rightjustified(g.textf("%s", 0xFFFFDD, NULL, ""));
							break;
						case 1:
						case 2:
							rightjustified(g.textf("%02d", 0xFFFFDD, NULL, o->racerank));
							break;
						}
					});
					g.poplist();
				}

				if (showracelaps) 
				{
					g.space(2);
					g.pushlist();
					g.strut(8);
					rightjustified(g.text("laps", COL_GRAY))
					loopscoregroup(o, {
						switch (o->racestate) {
						case 0:
							rightjustified(g.textf("%s", 0xFFFFDD, NULL, "start"));
							break;
						case 1:
							rightjustified(g.textf("%02d", 0xFFFFDD, NULL, o->racelaps));
							break;
						case 2:
							rightjustified(g.textf("%s", 0xFFFFDD, NULL, "finished"));
							break;
						}
					});
					g.poplist();
				}

				if (showracecheckpoints)
				{
					g.space(2);
					g.pushlist();
					g.strut(5);
					rightjustified(g.text("check", COL_GRAY))
					loopscoregroup(o, {
						if (o->racestate == 1) {
							rightjustified(g.textf("%02d", 0xFFFFDD, NULL, o->racecheckpoint));
						}
						else {
							rightjustified(g.textf("%s", 0xFFFFDD, NULL, ""));
						}
					});
					g.poplist();
				}
				// why is this broken -Y
				if (showracetime)
				{
					g.space(2);
					g.pushlist();
					g.strut(7);
					rightjustified(g.text("time", COL_GRAY))
					loopscoregroup(o, {
						if (o->racestate >= 1) {
							int secs = max(o->racetime, 0) / 1000;
							int mins = secs / 60;
							secs %= 60;
							rightjustified(g.textf("%d:%02d", 0xFFFFDD, NULL, mins, secs));
						}
						else {
							rightjustified(g.textf("%s", 0xFFFFDD, NULL, ""));
						}
					});
					g.poplist();
				}
			}
			
			if(!m_insta)
			{
				if(showdamage)
				{
					g.strut(4);
					rightjustified(g.text("dmg", COL_GRAY))
					loopscoregroup(o, {
						float dmg = (float) showdamage == 1 ? playerdamage(o, DMG_DEALT) : playernetdamage(o);
						const char *fmt = "%.0f";
						if(fabs(dmg) > 1000.0f) { fmt = "%.1fk"; dmg = dmg/1000.0f; }
						rightjustified(g.textf(fmt, fgcolor, NULL, dmg));
					});
					g.poplist();
				}

				if(showdamagereceived)
				{
					g.space(2);
					g.pushlist();
					g.strut(4);
					rightjustified(g.text("dr", COL_GRAY))
					loopscoregroup(o, {
						float dmg = (float) playerdamage(o, DMG_RECEIVED);
						const char *fmt = "%.0f";
						if(fabs(dmg) > 1000.0f) { fmt = "%.1fk"; dmg = dmg/1000.0f; }
						rightjustified(g.textf(fmt, fgcolor, NULL, dmg));
					});
					g.poplist();
				}
			}

			if(multiplayer(false) || demoplayback)
			{
				if(showping)
				{
					g.space(2);
					g.pushlist();
					
					rightjustified(g.text("ping", COL_GRAY))
					loopscoregroup(o,
					{
						fpsent *p = o->ownernum >= 0 ? getclient(o->ownernum) : o;
						if(!p) p = o;
						
						const char* pcolor;
						if(pingcolor) {
							
							if (p->ping < 70) {
								pcolor = "\f0";
							}
							else if (p->ping < 135) {
								pcolor = "\f2";
							}
							else if (p->ping < 200) {
								pcolor = "\f6";
							}
							else {
								pcolor = "\f3";
							}
						}
						else {
							pcolor = "";
						}

						if(!showpj && p->state==CS_LAGGED) rightjustified(g.text("LAG", fgcolor))
						else rightjustified(g.textf("%s%d", fgcolor, NULL, pcolor, p->ping))
					});
					g.poplist();
				}


				if(showpj)
				{
					g.space(2);
					g.pushlist();
					g.strut(2);
					rightjustified(g.text("pj", COL_GRAY))
					loopscoregroup(o,
					{
						fpsent* p = o->ownernum >= 0 ? getclient(o->ownernum) : o;
						if(!p) p = o;
						if(p==player1) rightjustified(g.text("0", fgcolor))
						else rightjustified(g.textf("%d", fgcolor, NULL, abs(33-p->plag)))
					});
					g.poplist();
				}
			}

			if(showclientnum || player1->privilege>=PRIV_MASTER)
			{
				g.space(2);
				g.pushlist();
				rightjustified(g.text("cn", COL_GRAY))
				loopscoregroup(o, rightjustified(g.textf("%d", fgcolor, NULL, o->clientnum)));
				g.poplist();
			}

			if(sg.team && m_teammode)
			{
				g.poplist(); // horizontal
				g.poplist(); // vertical
			}

			g.poplist(); // horizontal
			g.poplist(); // vertical

			if(k+1<numgroups && (k+1)%2) g.space(3);
			else
			{
				g.poplist(); // horizontal
				if(k+1<numgroups) g.space(.75f);
			}
		}

		if(showspectators && spectators.length())
		{
			#define loopspectators(o, b) \
				loopv(spectators) \
				{ \
					fpsent *o = spectators[i]; \
					b; \
				}

			g.space(.75f);

			if(showclientnum || showspectatorping || player1->privilege>=PRIV_MASTER)
			{
				g.pushlist();

				g.pushlist();
				g.text("spectator", COL_GRAY);
				loopspectators(o, g.text(colorname(o), statuscolor(o, fgcolor)));
				g.poplist();

				if((multiplayer(false) || demoplayback) && showspectatorping)
				{
					g.space(2);
					g.pushlist();
					rightjustified(g.text("ping", COL_GRAY))
					loopspectators(o,
					{
						fpsent *p = o->ownernum >= 0 ? getclient(o->ownernum) : o;
						if(!p) p = o;
						if(p->state==CS_LAGGED) rightjustified(g.text("LAG", fgcolor))
						else rightjustified(g.textf("%d", fgcolor, NULL, p->ping))
					});
					g.poplist();
				}

				if(showclientnum || player1->privilege>=PRIV_MASTER)
				{
					g.space(2);
					g.pushlist();
					rightjustified(g.text("cn", COL_GRAY))
					loopspectators(o, rightjustified(g.textf("%d", fgcolor, NULL, o->clientnum)));
					g.poplist();
				}

				g.poplist();
			}
			else
			{
				g.textf("%d spectator%s", COL_GRAY, NULL, spectators.length(), spectators.length()!=1 ? "s" : "");
				loopspectators(o,
				{
					if((i%3)==0)
					{
						g.pushlist();
						g.text("", fgcolor, 0);
					}
					g.text(colorname(o), statuscolor(o, fgcolor));
					if(i+1<spectators.length() && (i+1)%3) g.space(1);
					else g.poplist();
				});
			}
		}
		g.space(.25f);
	}

	struct scoreboardgui : g3d_callback
	{
		bool showing;
		vec menupos;
		int menustart;

		scoreboardgui() : showing(false) {}

		void show(bool on)
		{
			if(!showing && on)
			{
				menupos = menuinfrontofplayer();
				menustart = starttime();
			}
			showing = on;
		}

		void gui(g3d_gui &g, bool firstpass)
		{
			g.start(menustart, 0.03f, NULL, false);
			renderscoreboard(g, firstpass);
			g.end();
		}

		void render()
		{
			if(showing) g3d_addgui(this, menupos, (scoreboard2d ? GUI_FORCE_2D : GUI_2D | GUI_FOLLOW) | GUI_BOTTOM);
		}

	} scoreboard;

	void g3d_gamemenus()
	{
		scoreboard.render();
	}

	VARFN(scoreboard, showscoreboard, 0, 0, 1, scoreboard.show(showscoreboard!=0));

	void showscores(bool on)
	{
		showscoreboard = on ? 1 : 0;
		scoreboard.show(on);
	}
	ICOMMAND(showscores, "D", (int *down), showscores(*down!=0));

	VARP(hudscore, 0, 1, 1);
	FVARP(hudscorescale, 1e-3f, 0.85f, 1e3f);
	VARP(hudscorealign, -1, 0, 1);
	FVARP(hudscorex, 0, 0.50f, 1);
	FVARP(hudscorey, 0, 0.03f, 1);
	HVARP(hudscoreplayercolor, 0, 0xFFFFFF, 0xFFFFFF);
	HVARP(hudscoreenemycolor, 0, 0xFF4040, 0xFFFFFF);
	VARP(hudscorealpha, 0, 255, 255);
	VARP(hudscoresep, 0, 20, 1000);
	VARP(hudscoreself, 0, 0, 1);

	void drawhudscore(int w, int h)
	{
		int numgroups = groupplayers();
		if(!numgroups) return;
		
		fpsent * p = followingplayer();
		if(!p) p = player1;
		scoregroup * g = groups[0];
		int score = INT_MIN, score2 = 0;
		if(hudscoreself) score2 = INT_MIN;
		if(m_teammode)
		{
			score = g->score;
			if(numgroups > 1) score2 = groups[1]->score;
		}
		else
		{
			if(hudscoreself) score = m_parkour ? p->deaths : p->frags;
			else 
			{
				score = m_parkour ? p->deaths : p->frags;
				if(g->players.length() > 1) 
				{
					if(p != g->players[0]) score2 = m_parkour ? g->players[0]->deaths : g->players[0]->frags;
					else score2 = m_parkour ? g->players[1]->deaths : g->players[1]->frags;
				}
			}
		}
		
		score = clamp(score, -999, 9999);
		defformatstring(buf, "%d", score);
		int tw = 0, th = 0;
		text_bounds(buf, tw, th);
		
		cbstring buf2;
		int tw2 = 0, th2 = 0;
		if(score2 > INT_MIN)
		{
			score2 = clamp(score2, -999, 9999);
			formatstring(buf2, "%d", score2);
			text_bounds(buf2, tw2, th2);
		}
		
		int fw = 0, fh = 0;
		text_bounds("000", fw, fh);
		fw = max(fw, max(tw, tw2));
		
		vec2 offset = vec2(hudscorex, hudscorey).mul(vec2(w, h).div(hudscorescale));
		if(hudscorealign == 1 && score2 > INT_MIN) offset.x -= 2 * fw + hudscoresep;
		else if(hudscorealign == 0 && score2 > INT_MIN) offset.x -= (2 * fw + hudscoresep) / 2.0f;
		vec2 offset2 = offset;
		if(score2 > INT_MIN) offset.x += (fw - tw) / 2.0f;
		else offset.x -= tw / 2.0f;
		offset.y -= th / 2.0f;
		offset2.x += fw + hudscoresep + (fw - tw2) / 2.0f;
		offset2.y -= th2 / 2.0f;
		
		pushhudmatrix();
		hudmatrix.scale(hudscorescale, hudscorescale, 1);
		flushhudmatrix();
		
		int color = hudscoreplayercolor;
		int color2 = hudscoreenemycolor;
		if(m_teammode) 
		{
			color = COL_BLUE;
			color2 = COL_RED;
			if(!strcmp(g->team, "red")) swap(color, color2);
		}
		
		draw_text(buf, int(offset.x), int(offset.y), (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, hudscorealpha);
		if(score2 > INT_MIN) draw_text(buf2, int(offset2.x), int(offset2.y), (color2 >> 16) & 0xFF, (color2 >> 8) & 0xFF, color2 & 0xFF, hudscorealpha);
		
		pophudmatrix();
	}
}


