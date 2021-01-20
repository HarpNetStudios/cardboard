VARR(grapple, 0, 1, 1);
struct grapplestate
{
	fpsclient& cl;
	bool grappling;

	grapplestate(fpsclient& cl) : cl(cl), grappling(false)
	{
		CCOMMAND(usegrapple, "D", (grapplestate * self, int* down), { self->dograpple(*down != 0); });
	}

	void dograpple(bool on)
	{
		if (!on || !cl.grapple())
		{
			removegrapples(cl.player1, false);
			grappling = false;
		}
		else if (cl.player1->state == CS_ALIVE)
			grappling = true;
	}

	struct grappleobj
	{
		vec dir, o, to, offset;
		float speed;
		fpsent* owner;
		bool attached;
		physent* attachedto;
		bool local;
		int offsetmillis;
		int id;
		entitylight light;
	};
	vector<grappleobj> grapples;

	void reset() { grapples.setsizenodelete(0); grappling = false; }

	grappleobj& newgrapple(const vec& from, const vec& to, float speed, bool local, fpsent* owner)
	{
		grappleobj& g = grapples.add();
		g.dir = vec(to).sub(from).normalize();
		g.o = from;
		g.to = to;
		g.offset = cl.ws.hudgunorigin(0, from, to, owner);
		g.offset.sub(from);
		g.speed = speed;
		g.local = local;
		g.owner = owner;
		g.attached = false;
		g.attachedto = NULL;
		g.offsetmillis = weaponstate::OFFSETMILLIS;
		g.id = cl.lastmillis;
		return g;
	}

	bool hasgrapple(fpsent* d)
	{
		loopv(grapples)
			if (grapples[i].owner == d) return true;
		return false;
	}

	void removegrapples(physent* d, bool attached)
	{
		// can't use loopv here due to strange GCC optimizer bug
		int len = grapples.length();
		loopi(len) if (grapples[i].owner == d || (attached && grapples[i].attachedto == d))
		{
			grapples.remove(i--);
			len--;
		}
		if (d == cl.player1)
		{
			cl.cc.addmsg(SV_GRAPPLESTOP, "ri", (int)attached);
			grappling = false;
		}
	}

	void setgrapplepos(fpsent* d, const vec& pos)
	{
		loopv(grapples) if (grapples[i].owner == d)
		{
			grapples[i].o = pos;
			break;
		}
	}

	void setgrapplehit(fpsent* d, const vec& hit)
	{
		loopv(grapples) if (grapples[i].owner == d)
		{
			grapples[i].o = hit;
			grapples[i].to = hit;
			grapples[i].attached = true;
			break;
		}
	}

	void setgrappled(fpsent* d, fpsent* victim)
	{
		loopv(grapples) if (grapples[i].owner == d)
		{
			dynent* o = cl.freeze.owner2icecube(victim);
			if (o)
				grapples[i].attachedto = o;
			else
				grapples[i].attachedto = victim;
			grapples[i].attached = true;
			break;
		}
	}

	void sendgrappleclient(dynent* d, ucharbuf& p)
	{
		loopv(grapples)
		{
			grappleobj& g = grapples[i];
			if (grapples[i].owner == d && !g.attached)
			{
				putint(p, SV_GRAPPLEPOS);
				loopk(3) putint(p, int(g.o[k] * DMF));
				return;
			}
		}
	}

	bool grappleent(physent* o, grappleobj& g, vec& v)
	{
		if (o->state != CS_ALIVE) return false;
		if (!intersect(o, g.o, v)) return false;
		return true;
	}

	void movegrapples(int time)
	{
		loopv(grapples)
		{
			grappleobj& g = grapples[i];
			fpsent* d = g.owner;
			if (!g.attached)
			{
				g.offsetmillis = max(g.offsetmillis - time, 0);
				vec v;
				float dist = g.to.dist(g.o, v);
				float dtime = dist * 1000 / g.speed;
				if (time > dtime) dtime = time;
				v.mul(time / dtime);
				v.add(g.o);
				loopj(cl.numdynents())
				{
					dynent* o = cl.iterdynents(j);
					if (!o || d == o || o->o.reject(v, 10.0f)) continue;
					if (grappleent(o, g, v))
					{
						g.attachedto = o;
						g.attached = true;
						break;
					}
				}
				if (!g.attached)
				{
					if (dist < 4)
					{
						if (g.o != g.to) // if original target was moving, reevaluate endpoint
						{
							if (raycubepos(g.o, g.dir, g.to, 0, RAY_CLIPMAT | RAY_ALPHAPOLY) >= 4) continue;
						}
						g.attached = true;
					}
				}
				if (!g.attached)
					g.o = v;
				else if (d == cl.player1)
				{
					physent* o = g.attachedto;
					if (o != NULL && (o->type == ENT_PLAYER || (o = cl.freeze.icecube2owner(o)) != NULL))
					{
						cl.cc.addmsg(SV_GRAPPLED, "ri", ((fpsent*)o)->clientnum);
					}
					else
					{
						cl.cc.addmsg(SV_GRAPPLEHIT, "ri3", int(g.o.x * DMF), int(g.o.y * DMF), int(g.o.z * DMF));
					}
				}
			}
			else
			{
				vec to(g.attachedto ? g.attachedto->o : g.to);
				vec dir(to);
				dir.sub(d->o);
				if (dir.squaredlen() > GRAPPLESEPERATION * GRAPPLESEPERATION)
				{
					dir.normalize();
					if (g.attachedto)
					{
						physent* a = g.attachedto;
						dir.mul(GRAPPLEPULLSPEED * 0.5);
						a->vel.sub(dir);
						a->moving = true;
					}
					else
					{
						dir.mul(GRAPPLEPULLSPEED);
						g.owner->vel.add(dir);
					}
					g.owner->moving = true;
				}
			}
		}
	}

	void shootgrapplev(vec& from, vec& to, fpsent* d, bool local)
	{
		newgrapple(from, to, GRAPPLETHROWSPEED * 4, local, d);
	}

	void shootgrapple(fpsent* d, vec& targ)
	{
		if (d == cl.player1 && !grappling) return;
		if (hasgrapple(d)) return;

		vec from = d->o;
		vec to = targ;

		vec unitv;
		float dist = to.dist(from, unitv);
		unitv.div(dist);
		float barrier = raycube(d->o, unitv, dist, RAY_CLIPMAT | RAY_ALPHAPOLY);
		if (barrier < dist)
		{
			to = unitv;
			to.mul(barrier);
			to.add(from);
		}

		shootgrapplev(from, to, d, true);

		if (d == cl.player1)
		{
			cl.cc.addmsg(SV_GRAPPLE, "rii6", cl.lastmillis - cl.maptime,
				(int)(from.x * DMF), (int)(from.y * DMF), (int)(from.z * DMF),
				(int)(to.x * DMF), (int)(to.y * DMF), (int)(to.z * DMF));
		}

		d->gunwait = GRAPPLEGUNDELAY;
	}

	void rendergrapples()
	{
		float yaw, pitch;
		loopv(grapples)
		{
			grappleobj& g = grapples[i];
			vec pos(g.o), to(g.to);
			if (g.attachedto)
			{
				to = g.attachedto->o;
				to.z += (g.attachedto->aboveeye - g.attachedto->eyeheight) / 2;
			}
			pos.add(vec(g.offset).mul(g.offsetmillis / float(weaponstate::OFFSETMILLIS)));
			vec v(to);
			if (g.attached || pos == g.to)
			{
				v.sub(g.owner->o);
				v.normalize();
				vectoyawpitch(v, yaw, pitch);
				v = to;
			}
			else
			{
				v.sub(pos);
				v.normalize();
				vectoyawpitch(v, yaw, pitch);
				yaw += 45;
				v.mul(3);
				v.add(pos);
			}
			rendermodel(&g.light, "grapple", ANIM_MAPMODEL | ANIM_LOOP, v, yaw, pitch, MDL_CULL_VFC | MDL_CULL_OCCLUDED | MDL_LIGHT);
			particle_trail(1, 1, cl.ws.hudgunorigin(0, g.owner->o, v, g.owner), v);
		}
	}
};
