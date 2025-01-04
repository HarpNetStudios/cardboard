#include "game.h"

struct spawninfo { const extentity *e; float weight; };
extern float gatherspawninfos(dynent *d, int tag, vector<spawninfo> &spawninfos);

namespace game
{      
	vector<fpsent *> bestplayers;
	vector<const char *> bestteams;

	VARP(ragdoll, 0, 1, 1);
	VARP(ragdollmillis, 0, 10000, 300000);
	VARP(ragdollfade, 0, 1000, 300000);
	VARFP(playermodel, 0, 0, 5, changedplayermodel()); // change for new playermodel
	VARP(forceplayermodels, 0, 0, 1);
	VARP(hidedead, 0, 0, 2);

	vector<fpsent *> ragdolls;

	void saveragdoll(fpsent *d)
	{
		if(!d->ragdoll || !ragdollmillis || (!ragdollfade && lastmillis > d->lastpain + ragdollmillis)) return;
		fpsent *r = new fpsent(*d);
		r->lastupdate = ragdollfade && lastmillis > d->lastpain + max(ragdollmillis - ragdollfade, 0) ? lastmillis - max(ragdollmillis - ragdollfade, 0) : d->lastpain;
		r->edit = NULL;
		r->ai = NULL;
		r->attackchan = r->idlechan = -1;
		if(d==player1) r->playermodel = playermodel;
		ragdolls.add(r);
		d->ragdoll = NULL;   
	}

	void clearragdolls()
	{
		ragdolls.deletecontents();
	}

	void moveragdolls()
	{
		loopv(ragdolls)
		{
			fpsent *d = ragdolls[i];
			if(lastmillis > d->lastupdate + ragdollmillis)
			{
				delete ragdolls.remove(i--);
				continue;
			}
			moveragdoll(d);
		}
	}

	static const playermodelinfo playermodels[6] = // new player
	{
		{ "player/prototype", "player/prototype/blue", "player/prototype/red", "player/prototype/hudguns", NULL, "icon_sarah", "prototype_blue", "prototype_red", true },
		{ "player/prototype", "player/prototype/blue", "player/prototype/red", "player/prototype/hudguns", NULL, "icon_amber", "prototype_blue", "prototype_red", true },
		{ "player/prototype", "player/prototype/blue", "player/prototype/red", "player/prototype/hudguns", NULL, "icon_ashley", "prototype_blue", "prototype_red", true },
		{ "player/prototype", "player/prototype/blue", "player/prototype/red", "player/prototype/hudguns", NULL, "icon_bea", "prototype_blue", "prototype_red", true },
		{ "player/prototype", "player/prototype/blue", "player/prototype/red", "player/prototype/hudguns", NULL, "icon_savannah", "prototype_blue", "prototype_red", true },
		{ "player/prototype", "player/prototype/blue", "player/prototype/red", "player/prototype/hudguns", NULL, "icon_emily", "prototype_blue", "prototype_red", true },
	};

	int chooserandomplayermodel(int seed)
	{
		return (seed&0xFFFF)%(sizeof(playermodels)/sizeof(playermodels[0]));
	}

	const playermodelinfo *getplayermodelinfo(int n)
	{
		if(size_t(n) >= sizeof(playermodels)/sizeof(playermodels[0])) return NULL;
		return &playermodels[n];
	}

	const playermodelinfo &getplayermodelinfo(fpsent *d)
	{
		const playermodelinfo *mdl = getplayermodelinfo(d==player1 || forceplayermodels ? playermodel : d->playermodel);
		if(!mdl) mdl = getplayermodelinfo(playermodel);
		return *mdl;
	}

	void changedplayermodel()
	{
		if(player1->clientnum < 0) player1->playermodel = playermodel;
		if(player1->ragdoll) cleanragdoll(player1);
		loopv(ragdolls) 
		{
			fpsent *d = ragdolls[i];
			if(!d->ragdoll) continue;
			if(!forceplayermodels)
			{
				const playermodelinfo *mdl = getplayermodelinfo(d->playermodel);
				if(mdl) continue;
			}
			cleanragdoll(d);
		}
		loopv(players)
		{
			fpsent *d = players[i];
			if(d == player1 || !d->ragdoll) continue;
			if(!forceplayermodels)
			{
				const playermodelinfo *mdl = getplayermodelinfo(d->playermodel);
				if(mdl) continue;
			}
			cleanragdoll(d);
		}
	}

	void preloadplayermodel()
	{
		loopi(sizeof(playermodels)/sizeof(playermodels[0]))
		{
			const playermodelinfo *mdl = getplayermodelinfo(i);
			if(!mdl) break;
			if(i != playermodel && (!multiplayer(false) || forceplayermodels)) continue;
			if(m_teammode)
			{
				preloadmodel(mdl->blueteam);
				preloadmodel(mdl->redteam);
			}
			else preloadmodel(mdl->ffa);
			if(mdl->vwep) preloadmodel(mdl->vwep);
		}
	}

	VAR(testteam, 0, 0, 3);

	void renderplayer(fpsent *d, const playermodelinfo &mdl, int team, float fade, bool mainpass)
	{
		int lastaction = d->lastaction[d->gunselect], hold = mdl.vwep || d->gunselect==GUN_ARIFLE ? 0 : (ANIM_HOLD1+d->gunselect)|ANIM_LOOP, attack = ANIM_ATTACK1+d->gunselect, delay = mdl.vwep ? 300 : guns[d->gunselect].attackdelay+50;
		if(intermission && d->state!=CS_DEAD)
		{
			lastaction = 0;
			hold = attack = ANIM_LOSE|ANIM_LOOP;
			delay = 0;
			if(m_teammode ? bestteams.htfind(d->team)>=0 : bestplayers.find(d)>=0) hold = attack = ANIM_WIN|ANIM_LOOP;
		}
		else if(d->state==CS_ALIVE && d->lasttaunt && lastmillis-d->lasttaunt < 1000 && lastmillis-d->lastaction[d->gunselect] > delay)
		{
			lastaction = d->lasttaunt;
			hold = attack = ANIM_TAUNT;
			delay = 1000;
		}
		modelattach a[5];
		static const char* const vweps[] = { "vwep/fist", "vwep/arifle", "vwep/shotg", "vwep/rifle", "vwep/chaing", "vwep/rocket", "vwep/gl" };
		int ai = 0;
		if((!mdl.vwep || d->gunselect!=GUN_FIST) && d->gunselect<=GUN_GL)
		{
			int vanim = ANIM_VWEP_IDLE|ANIM_LOOP, vtime = 0;
			if(lastaction && d->lastattackgun==d->gunselect && lastmillis < lastaction + delay)
			{
				vanim = ANIM_VWEP_SHOOT;
				vtime = lastaction;
			}
			a[ai++] = modelattach("tag_weapon", mdl.vwep ? mdl.vwep : vweps[d->gunselect], vanim, vtime);
		}
		if(mainpass)
		{
			d->muzzle = vec(-1, -1, -1);
			a[ai++] = modelattach("tag_muzzle", &d->muzzle);
		}
		const char *mdlname = mdl.ffa;
		switch(testteam ? testteam-1 : team)
		{
			case 1: mdlname = mdl.blueteam; break;
			case 2: mdlname = mdl.redteam; break;
		}
		renderclient(d, mdlname, a[0].tag ? a : NULL, hold, attack, delay, lastaction, intermission && d->state!=CS_DEAD ? 0 : d->lastpain, fade, ragdoll && mdl.ragdoll);
	}

	VARP(teamskins, 0, 0, 1);

	// for testing spawns

	VAR(dbgspawns, 0, 0, 2);

	float hsv2rgb(float h, float s, float v, int n)
	{
		float k = fmod(n + h / 60.0f, 6.0f);
		return v - v * s * max(min(min(k, 4.0f - k), 1.0f), 0.0f);
	}

	vec hsv2rgb(float h, float s, float v)
	{
		return vec(hsv2rgb(h, s, v, 5), hsv2rgb(h, s, v, 3), hsv2rgb(h, s, v, 1));
	}

	void renderspawn(const vec &o, int rating, float probability)
	{
		defformatstring(score, "%d", rating);
		defformatstring(percentage, "(%.2f%%)", probability * 100);
		bvec colorvec = bvec::fromcolor(hsv2rgb(rating * 1.2f, 0.8, 1));
		int color = (colorvec.r << 16) + (colorvec.g << 8) + colorvec.b;
		particle_textcopy(vec(o).addz(5), score, PART_TEXT, 1, color, 5.0f);
		particle_textcopy(vec(o).addz(1), percentage, PART_TEXT, 1, color, 4.0f);
	}

	void renderspawns()
	{
		vector<spawninfo> spawninfos;
		float ratingsum = gatherspawninfos(player1, 0, spawninfos);
		loopv(spawninfos) renderspawn(spawninfos[i].e->o, spawninfos[i].weight * 100, spawninfos[i].weight / ratingsum);
		if (dbgspawns > 1) { // render team spawns too
			ratingsum = gatherspawninfos(player1, 1, spawninfos);
			loopv(spawninfos) renderspawn(spawninfos[i].e->o, spawninfos[i].weight * 100, spawninfos[i].weight / ratingsum);
			ratingsum = gatherspawninfos(player1, 2, spawninfos);
			loopv(spawninfos) renderspawn(spawninfos[i].e->o, spawninfos[i].weight * 100, spawninfos[i].weight / ratingsum);
		}
	}

	VARP(statusbars, 0, 1, 2);
	FVARP(statusbarscale, 0, 1, 2);

	float renderstatusbars(fpsent *d, int team)
	{
		if(!statusbars || m_insta || (spectating(player1) ? statusbars <= 1 : team != 1) || (d->state != CS_ALIVE && d->state != CS_LAGGED)) return 0;
		vec p = d->abovehead().msub(camdir, 50/80.0f).msub(camup, 2.0f);
		float offset = 0;
		float scale = statusbarscale;
		int color = d->health<=250 ? 0xFF0000 : (d->health<=500 ? 0xFF8000 : (d->health<=1000 ? 0x40FF80 : 0x40C0FF));
		float size = scale*sqrtf(max(d->health, d->maxhealth)/100.0f);
		float fill = float(d->health)/d->maxhealth;
		offset += size;
		particle_meter(vec(p).madd(camup, offset), fill, PART_METER, 1, color, 0, size);
		return offset;
	}

	VARP(shownames, 0, 1, 1);

	void rendergame(bool mainpass)
	{
		if(mainpass) ai::render();

		if(intermission)
		{
			bestteams.shrink(0);
			bestplayers.shrink(0);
			if(m_teammode) getbestteams(bestteams);
			else getbestplayers(bestplayers);
		}

		startmodelbatches();

		fpsent *exclude = isthirdperson() ? NULL : followingplayer();
		loopv(players)
		{
			fpsent *d = players[i];
			if(d == player1 || d->state==CS_SPECTATOR || d->state==CS_SPAWNING || d->lifesequence < 0 || d == exclude || (d->state==CS_DEAD && hidedead)) continue;
			int team = 0;
			if(teamskins || m_teammode) team = strcmp(d->team, "red") ? 1 : 2;
			renderplayer(d, getplayermodelinfo(d), team, 1, mainpass);

			vec dir = vec(d->o).sub(camera1->o);
			float dist = dir.magnitude();
			dir.div(dist);
			if(d->state!=CS_EDITING && raycube(camera1->o, dir, dist, 0) < dist)
			{
				d->info[0] = '\0';
				continue;
			}

			if (shownames)
			{
				copystring(d->info, colorname(d));
				if (d->state != CS_DEAD)
				{
					float offset = renderstatusbars(d, team);
					if (d->state != CS_DEAD)
					{
						vec p = d->abovehead().madd(camup, offset);
						particle_text(p, d->info, PART_TEXT, 1, team ? (team == 1 ? 0x6496FF : 0xFF4B19) : 0x1EC850, 2.0f);
					}
				}
			}
		}

		loopv(ragdolls)
		{
			fpsent *d = ragdolls[i];
			int team = 0;
			if (teamskins || m_teammode) team = strcmp(d->team, "red") ? 1 : 2;
			float fade = 1.0f;
			if(ragdollmillis && ragdollfade) 
				fade -= clamp(float(lastmillis - (d->lastupdate + max(ragdollmillis - ragdollfade, 0)))/min(ragdollmillis, ragdollfade), 0.0f, 1.0f);
			renderplayer(d, getplayermodelinfo(d), team, fade, mainpass);
		}

		if (isthirdperson() && !followingplayer() && (player1->state != CS_DEAD || hidedead != 1))
		{
			renderplayer(player1, getplayermodelinfo(player1), teamskins || m_teammode ? (strcmp(player1->team, "red") ? 1 : 2) : 0, 1, mainpass);
		}

		entities::renderentities();
		renderbouncers();
		renderprojectiles();
		if(cmode) cmode->rendergame();

#if _DEBUG
		if (dbgspawns) renderspawns();
#else
		if (dbgspawns && m_edit) renderspawns();
#endif

		endmodelbatches();
	}

	VARP(hudgun, 0, 1, 1);
	VARP(hudgunsway, 0, 1, 1);
	VARP(teamhudguns, 0, 1, 1);
	VAR(testhudgun, 0, 0, 1);

	FVARP(swaystep, 1, 35.0f, 100);
	FVARP(swayside, 0, 0.04f, 1);
	FVARP(swayup, -1, 0.05f, 1);

	float swayfade = 0, swayspeed = 0, swaydist = 0;
	vec swaydir(0, 0, 0);

	void swayhudgun(int curtime)
	{
		fpsent *d = hudplayer();
		if(d->state != CS_SPECTATOR)
		{
			if(d->physstate >= PHYS_SLOPE)
			{
				swayspeed = min(sqrtf(d->vel.x*d->vel.x + d->vel.y*d->vel.y), d->maxspeed);
				swaydist += swayspeed*curtime/1000.0f;
				swaydist = fmod(swaydist, 2*swaystep);
				swayfade = 1;
			}
			else if(swayfade > 0)
			{
				swaydist += swayspeed*swayfade*curtime/1000.0f;
				swaydist = fmod(swaydist, 2*swaystep);
				swayfade -= 0.5f*(curtime*d->maxspeed)/(swaystep*1000.0f);
			}

			float k = pow(0.7f, curtime/10.0f);
			swaydir.mul(k);
			vec vel(d->vel);
			vel.add(d->falling);
			swaydir.add(vec(vel).mul((1-k)/(15*max(vel.magnitude(), d->maxspeed))));
		}
	}

	struct hudent : dynent
	{
		hudent() { type = ENT_CAMERA; }
	} guninterp;

	SVARP(hudgunsdir, "");

	void drawhudmodel(fpsent *d, int anim, float speed = 0, int base = 0)
	{
		if (d->gunselect > GUN_GL) return;

		vec sway;
		vecfromyawpitch(d->yaw, 0, 0, 1, sway);
		float steps = swaydist/swaystep*M_PI;
		sway.mul(swayside*cosf(steps));
		sway.z = swayup*(fabs(sinf(steps)) - 1);
		sway.add(swaydir).add(d->o);
		if(!hudgunsway) sway = d->o;

		const playermodelinfo &mdl = getplayermodelinfo(d);
		defformatstring(gunname, "%s/%s", hudgunsdir[0] ? hudgunsdir : mdl.hudguns, guns[d->gunselect].file);
		if ((m_teammode || teamskins) && teamhudguns)
		{
			concatstring(gunname, strcmp(d->team, "red") ? "/blue" : "/red");
		}
		else if (testteam > 1)
		{
			concatstring(gunname, testteam == 2 ? "/blue" : "/red");
		}
		modelattach a[2];
		d->muzzle = vec(-1, -1, -1);
		a[0] = modelattach("tag_muzzle", &d->muzzle);
		dynent *interp = NULL;
		if(d->gunselect==GUN_FIST)
		{
			anim |= ANIM_LOOP;
			base = 0;
			interp = &guninterp;
		}
		rendermodel(NULL, gunname, anim, sway, testhudgun ? 0 : d->yaw+90, testhudgun ? 0 : d->pitch, MDL_LIGHT|MDL_HUD, interp, a, base, (int)ceil(speed));
		if(d->muzzle.x >= 0) d->muzzle = calcavatarpos(d->muzzle, 12);
	}

	void drawhudgun()
	{
		fpsent *d = hudplayer();
		if(d->state==CS_SPECTATOR || d->state==CS_EDITING || !hudgun || editmode) 
		{ 
			d->muzzle = player1->muzzle = vec(-1, -1, -1);
			return;
		}

		int rtime = guns[d->gunselect].attackdelay;
		if(d->lastaction && d->lastattackgun==d->gunselect && lastmillis - d->lastaction[d->gunselect] < rtime)
		{
			drawhudmodel(d, ANIM_GUN_SHOOT | ANIM_SETSPEED, rtime/17.0f, d->lastaction[d->gunselect]);
		}
		else
		{
			drawhudmodel(d, ANIM_GUN_IDLE | ANIM_LOOP);
		}
	}

	void renderavatar()
	{
		drawhudgun();
	}

	void renderplayerpreview(int model, int team, int weap)
	{
		static fpsent *previewent = NULL;
		if(!previewent)
		{
			previewent = new fpsent;
			previewent->light.color = vec(1, 1, 1);
			previewent->light.dir = vec(0, -1, 2).normalize();
			loopi(NUMGUNS - 1) previewent->ammo[i + 1] = 1;
		}
		float height = previewent->eyeheight + previewent->aboveeye,
			  zrad = height/2;
		vec2 xyrad = vec2(previewent->xradius, previewent->yradius).max(height/4);
		previewent->o = calcmodelpreviewpos(vec(xyrad, zrad), previewent->yaw).addz(previewent->eyeheight - zrad);
		previewent->gunselect = clamp(weap, int(GUN_FIST), int(GUN_GL));
		previewent->light.millis = -1;
		const playermodelinfo *mdlinfo = getplayermodelinfo(model);
		if(!mdlinfo) return;
		renderplayer(previewent, *mdlinfo, team >= 0 && team <= 2 ? team : 0, 1, false);
	}

	vec hudgunorigin(int gun, const vec &from, const vec &to, fpsent *d)
	{
		if(d->muzzle.x >= 0) return d->muzzle;
		vec offset(from);
		if(d!=hudplayer() || isthirdperson())
		{
			vec front, right;
			vecfromyawpitch(d->yaw, d->pitch, 1, 0, front);
			offset.add(front.mul(d->radius));
			if(d->type!=ENT_AI)
			{
				offset.z += (d->aboveeye + d->eyeheight)*0.75f - d->eyeheight;
				vecfromyawpitch(d->yaw, 0, 0, -1, right);
				offset.add(right.mul(0.5f*d->radius));
				offset.add(front);
			}
			return offset;
		}
		offset.add(vec(to).sub(from).normalize().mul(2));
		if(hudgun)
		{
			offset.sub(vec(camup).mul(1.0f));
			offset.add(vec(camright).mul(0.8f));
		}
		else offset.sub(vec(camup).mul(0.8f));
		return offset;
	}

	void preloadweapons()
	{
		const playermodelinfo &mdl = getplayermodelinfo(player1);
		loopi(NUMGUNS)
		{
			const char *file = guns[i].file;
			if(!file) continue;
			old_string fname;
			if((m_teammode || teamskins) && teamhudguns)
			{
				formatstring(fname, "%s/%s/blue", hudgunsdir[0] ? hudgunsdir : mdl.hudguns, file);
				preloadmodel(fname);
				formatstring(fname, "%s/%s/red", hudgunsdir[0] ? hudgunsdir : mdl.hudguns, file);
				preloadmodel(fname);
			}
			else
			{
				formatstring(fname, "%s/%s", hudgunsdir[0] ? hudgunsdir : mdl.hudguns, file);
				preloadmodel(fname);
			}
			formatstring(fname, "vwep/%s", file);
			preloadmodel(fname);
		}
	}

	void preloadsounds()
	{
		// preload all sounds, we aren't in 2003 anymore
		for (int i = S_JUMP; i <= S_SRV_DISCONNECT; i++) preloadsound(i);
	}

	void preload()
	{
		if(hudgun) preloadweapons();
		preloadbouncers();
		preloadplayermodel();
		preloadsounds();
		entities::preloadentities();
	}
}
