// sound.cpp: basic positional sound using sdl_mixer

#include "engine.h"
#include <SDL3_mixer/SDL_mixer.h>

bool nosound = true;

struct soundsample
{
	char *name;
	MIX_Audio *audio;

	soundsample() : name(NULL), audio(NULL) {}
	~soundsample() { DELETEA(name); }

	void cleanup() { if(audio) { MIX_DestroyAudio(audio); audio = NULL; } }
	bool load(bool msg = false);
};

struct soundslot
{
	soundsample *sample;
	int volume;
};

struct soundconfig
{
	int slots, numslots;
	int maxuses;

	bool hasslot(const soundslot *p, const vector<soundslot> &v) const
	{
		return p >= v.getbuf() + slots && p < v.getbuf() + slots+numslots && slots+numslots < v.length(); 
	}

	int chooseslot(int flags) const
	{
		if(flags&SND_NO_ALT || numslots <= 1) return slots;
		if(flags&SND_USE_ALT) return slots + 1 + rnd(numslots - 1);
		return slots + rnd(numslots);
	}
};

struct soundchannel
{ 
	int id;
	bool inuse;
	vec loc; 
	soundslot *slot;
	extentity *ent; 
	int radius;
	float volume, pan;
	int flags;
	bool dirty;
	MIX_Track* track = NULL;
	MIX_Mixer* mixer = NULL;

	soundchannel(MIX_Mixer* mixer, int id) : id(id), mixer(mixer) { reset(); }

	bool hasloc() const { return loc.x >= -1e15f; }
	void clearloc() { loc = vec(-1e16f, -1e16f, -1e16f); }

	void reset()
	{
		if (!track) {
			track = MIX_CreateTrack(mixer);
		}

		inuse = false;
		clearloc();
		slot = NULL;
		ent = NULL;
		radius = 0;
		volume = 0.0f;
		pan = -1;
		flags = 0;
		dirty = false;
	}
};
vector<soundchannel> channels;
int maxchannels = 0;

soundchannel &newchannel(int n, MIX_Mixer* mixer, soundslot *slot, const vec *loc = NULL, extentity *ent = NULL, int flags = 0, int radius = 0)
{
	if(ent)
	{
		loc = &ent->o;
		ent->flags |= EF_SOUND;
	}
	while(!channels.inrange(n)) channels.add(soundchannel(mixer, channels.length()));
	soundchannel &chan = channels[n];
	chan.reset();
	chan.inuse = true;
	if(loc) chan.loc = *loc;
	chan.slot = slot;
	chan.ent = ent;
	chan.flags = 0;
	chan.radius = radius;
	return chan;
}

void freechannel(int n)
{
	if(!channels.inrange(n) || !channels[n].inuse) return;
	soundchannel &chan = channels[n];
	chan.inuse = false;
	if(chan.ent) chan.ent->flags &= ~EF_SOUND;
}

void syncchannel(soundchannel &chan)
{
	if(!chan.dirty) return;
	MIX_SetTrackGain(chan.track, chan.volume);
	MIX_StereoGains gains = { (255 - chan.pan) / 255.0f, chan.pan / 255.0f };
	MIX_SetTrackStereo(chan.track, &gains);
	chan.dirty = false;
}

void stopchannels()
{
	loopv(channels)
	{
		soundchannel &chan = channels[i];
		if(!chan.inuse) continue;
		MIX_StopTrack(chan.track, 0);
		freechannel(i);
	}
}

void setmusicvol(int musicvol);
extern int musicvol;
static int curvol = 0;
VARFP(soundvol, 0, 255, 255,
{
	if(!soundvol) { stopchannels(); setmusicvol(0); }
	else if(!curvol) setmusicvol(musicvol);
	curvol = soundvol;
});
VARFP(musicvol, 0, 128, 255, setmusicvol(soundvol ? musicvol : 0));

char *musicfile = NULL, *musicdonecmd = NULL;

MIX_Audio* music = NULL;
MIX_Track* musictrack = NULL;

SDL_IOStream *musicrw = NULL;
stream *musicstream = NULL;

MIX_Mixer* mixer = NULL;

void setmusicvol(int musicvol)
{
	if(nosound) return;
	if(musictrack) MIX_SetTrackGain(musictrack, (musicvol * 1.0f) / 255);
}

void stopmusic()
{
	if(nosound) return;
	DELETEA(musicfile);
	DELETEA(musicdonecmd);
	if(musictrack)
	{
		MIX_StopTrack(musictrack, 0);
		if(music) MIX_DestroyAudio(music);
	}
	if(musicrw) { SDL_CloseIO(musicrw); musicrw = NULL; }
	DELETEP(musicstream);
}

bool shouldinitaudio = true;
VARF(usesound, 0, 1, 1, { shouldinitaudio = true; initwarning("sound configuration", INIT_RESET, CHANGE_SOUND); });
VARF(soundchans, 1, 32, 128, initwarning("sound configuration", INIT_RESET, CHANGE_SOUND));
VARF(soundfreq, 0, 44100, 48000, initwarning("sound configuration", INIT_RESET, CHANGE_SOUND));
VARF(soundbufferlen, 128, 1024, 4096, initwarning("sound configuration", INIT_RESET, CHANGE_SOUND));

bool initaudio()
{
	if(SDL_Init(SDL_INIT_AUDIO)) return true;
	conoutf(CON_ERROR, "sound init failed: %s", SDL_GetError());
	return false;
}

void initsound()
{
	if(shouldinitaudio)
	{
		shouldinitaudio = false;
		if(SDL_WasInit(SDL_INIT_AUDIO)) SDL_QuitSubSystem(SDL_INIT_AUDIO);
		if(!usesound || !initaudio())
		{
			nosound = true;
			return;
		}
	}

	if (!MIX_Init())
	{
		nosound = true;
		conoutf(CON_ERROR, "sound init failed (SDL_mixer): %s", SDL_GetError());
		return;
	}

	const SDL_AudioSpec spec = { SDL_AUDIO_S16, 2, soundfreq };
	mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);

	if(mixer == NULL)
	{
		nosound = true;
		conoutf(CON_ERROR, "sound init failed (SDL_mixer): %s", SDL_GetError());
		return;
	}

	// TODO: what
	for (size_t i = 0; i < soundchans; i++)
	{
		channels.add(soundchannel(mixer, i));
	}
	maxchannels = soundchans;
	nosound = false;
}

void musicdone()
{
	if(musictrack) { MIX_StopTrack(musictrack, 0); MIX_DestroyAudio(music); music = NULL; }
	if(musicrw) { SDL_CloseIO(musicrw); musicrw = NULL; }
	DELETEP(musicstream);
	DELETEA(musicfile);
	if(!musicdonecmd) return;
	char *cmd = musicdonecmd;
	musicdonecmd = NULL;
	execute(cmd);
	delete[] cmd;
}

MIX_Audio *loadmusic(const char *name)
{
	if(!musicstream) musicstream = openzipfile(name, "rb");
	if(musicstream)
	{
		if(!musicrw) musicrw = musicstream->rwops();
		if(!musicrw) DELETEP(musicstream);
	}
	if(musicrw) music = MIX_LoadAudio_IO(mixer, musicrw, false, false);
	else music = MIX_LoadAudio(mixer, findfile(name, "rb"), false); 
	if(!music)
	{
		if(musicrw) { SDL_CloseIO(musicrw); musicrw = NULL; }
		DELETEP(musicstream);
	}
	return music;
}

void startmusic(char *name, char *cmd)
{
	if(nosound) return;
	stopmusic();
	if(soundvol && musicvol && *name)
	{
		defformatstring(file, "packages/%s", name);
		path(file);
		if(loadmusic(file))
		{
			DELETEA(musicfile);
			DELETEA(musicdonecmd);
			musicfile = newstring(file);
			if(cmd[0]) musicdonecmd = newstring(cmd);
			SDL_PropertiesID props = SDL_CreateProperties();
			SDL_SetNumberProperty(props, "MIX_PROP_PLAY_LOOPS_NUMBER", cmd[0] ? 0 : -1);
			MIX_SetTrackAudio(musictrack, music);
			MIX_PlayTrack(musictrack, props);
			MIX_SetTrackGain(musictrack, (musicvol * 1.0f) / 255.0f);
			intret(1);
		}
		else
		{
			conoutf(CON_ERROR, "could not play music: %s", file);
			conoutf(CON_ERROR, "SDL_mixer says: %s", SDL_GetError());
			intret(0); 
		}
	}
}

COMMANDN(music, startmusic, "ss");

// TODO: completely rewrite the soundtrack system, this shit sucks so much
SVARFP(soundtrack, "adwh", {
	execfile("data/streset.cfg", false);
	defformatstring(st_arch, "packages/music/%s", soundtrack);
	if (addzip(st_arch, "packages/music", NULL, true)) {
		defformatstring(st_cfgname, "packages/music/%s/soundtrack.cfg", soundtrack);
		execfile(st_cfgname, false);
		execident("playsong");
	}
	});
SVARP(_soundtrack_title, "");
SVARP(_soundtrack_author, "");
SVARP(_soundtrack_url, "");

static MIX_Audio *loadwav(const char *name)
{
	MIX_Audio *c = NULL;
	stream *z = openzipfile(name, "rb");
	if(z)
	{
		SDL_IOStream *rw = z->rwops();
		if(rw)
		{
			c = MIX_LoadAudio_IO(mixer, rw, true, true);
		}
		delete z;
	}
	if(!c) c = MIX_LoadAudio(mixer, findfile(name, "rb"), true);
	return c;
}

bool soundsample::load(bool msg)
{
	if(audio) return true;
	if(!name[0]) return false;

	static const char * const exts[] = { "", ".wav", ".ogg" };
	old_string filename;
	loopi(sizeof(exts)/sizeof(exts[0]))
	{
		formatstring(filename, "packages/sounds/%s%s", name, exts[i]);
		if(msg && !i) renderprogress(0, filename);
		path(filename);
		audio = loadwav(filename);
		if(audio) return true;
	}

	conoutf(CON_ERROR, "failed to load sample: packages/sounds/%s", name);
	conoutf(CON_ERROR, "SDL_mixer says: %s", SDL_GetError());
	return false;
}

static hashnameset<soundsample> samples;

static void cleanupsamples()
{
	enumerate(samples, soundsample, s, s.cleanup());
}

static struct soundtype
{
	vector<soundslot> slots;
	vector<soundconfig> configs;

	int findsound(const char *name, int vol)
	{
		loopv(configs)
		{
			soundconfig &s = configs[i];
			loopj(s.numslots)
			{
				soundslot &c = slots[s.slots+j];
				if(!strcmp(c.sample->name, name) && (!vol || c.volume==vol)) return i;
			}
		}
		return -1;
	}

	int addslot(const char *name, int vol)
	{
		soundsample *s = samples.access(name);
		if(!s)
		{
			char *n = newstring(name);
			s = &samples[n];
			s->name = n;
			s->audio = NULL;
		}
		soundslot *oldslots = slots.getbuf();
		int oldlen = slots.length();
		soundslot &slot = slots.add();
		// soundslots.add() may relocate slot pointers
		if(slots.getbuf() != oldslots) loopv(channels)
		{
			soundchannel &chan = channels[i];
			if(chan.inuse && chan.slot >= oldslots && chan.slot < &oldslots[oldlen])
				chan.slot = &slots[chan.slot - oldslots];
		}
		slot.sample = s;
		slot.volume = vol ? vol : 100;
		return oldlen;
	}

	int addsound(const char *name, int vol, int maxuses = 0)
	{
		soundconfig &s = configs.add();
		s.slots = addslot(name, vol);
		s.numslots = 1;
		s.maxuses = maxuses;
		return configs.length()-1;
	}

	void addalt(const char *name, int vol)
	{
		if(configs.empty()) return;
		addslot(name, vol);
		configs.last().numslots++;
	}

	void clear()
	{
		slots.setsize(0);
		configs.setsize(0);
	}

	void reset()
	{
		loopv(channels)
		{
			soundchannel &chan = channels[i];
			if(chan.inuse && slots.inbuf(chan.slot))
			{
				MIX_StopTrack(chan.track, 0);
				freechannel(i);
			}
		}
		clear();
	}

	void preloadsound(int n)
	{
		if(nosound || !configs.inrange(n)) return;
		soundconfig &config = configs[n];
		loopk(config.numslots) slots[config.slots+k].sample->load(true);
	}

	bool playing(const soundchannel &chan, const soundconfig &config) const
	{
		return chan.inuse && config.hasslot(chan.slot, slots);
	}
} gamesounds, mapsounds;

void registersound(char *name, int *vol) { intret(gamesounds.addsound(name, *vol, 0)); }
COMMAND(registersound, "si");

void mapsound(char *name, int *vol, int *maxuses) { intret(mapsounds.addsound(name, *vol, *maxuses < 0 ? 0 : max(1, *maxuses))); }
COMMAND(mapsound, "sii");

void altsound(char *name, int *vol) { gamesounds.addalt(name, *vol); }
COMMAND(altsound, "si");

void altmapsound(char *name, int *vol) { mapsounds.addalt(name, *vol); }
COMMAND(altmapsound, "si");

ICOMMAND(numsounds, "", (), intret(gamesounds.configs.length()));
ICOMMAND(nummapsounds, "", (), intret(mapsounds.configs.length()));

void soundreset()
{
	gamesounds.reset();
}
COMMAND(soundreset, "");

void mapsoundreset()
{
	mapsounds.reset();
}
COMMAND(mapsoundreset, "");

void resetchannels()
{
	loopv(channels) {
		if (channels[i].inuse) freechannel(i);
	}
	channels.shrink(0);
}

void clear_sound()
{
	closemumble();
	if(nosound) return;
	stopmusic();

	cleanupsamples();
	gamesounds.clear();
	mapsounds.clear();
	samples.clear();
	MIX_DestroyMixer(mixer);
	resetchannels();
}

void stopmapsounds()
{
	loopv(channels)
	{
		soundchannel& chan = channels[i];
		if (chan.inuse && chan.ent)
		{
			MIX_StopTrack(chan.track, 0);
			freechannel(i);
		}
	}
}

void clearmapsounds()
{
	stopmapsounds();
	mapsounds.clear();
}

void stopmapsound(extentity* e)
{
	loopv(channels)
	{
		soundchannel& chan = channels[i];
		if(chan.inuse && chan.ent == e)
		{
			MIX_StopTrack(chan.track, 0);
			freechannel(i);
		}
	}
}

void checkmapsounds()
{
	const vector<extentity*>& ents = entities::getents();
	loopv(ents)
	{
		extentity &e = *ents[i];
		if(e.type!=ET_SOUND) continue;
		if(camera1->o.dist(e.o) < e.attr2)
		{
			if(!(e.flags&EF_SOUND)) playsound(e.attr1, NULL, &e, SND_MAP, -1);
		}
		else if(e.flags&EF_SOUND) stopmapsound(&e);
	}
}

VAR(stereo, 0, 1, 1);

bool updatechannel(soundchannel &chan)
{
	if(!chan.slot) return false;
	float volf = 1.0f, panf = 0.5f;
	if(chan.hasloc())
	{
		vec v;
		float dist = chan.loc.dist(camera1->o, v);
		int rad = 0;
		if(chan.ent)
		{
			rad = chan.ent->attr2;
			if(chan.ent->attr3)
			{
				rad -= chan.ent->attr3;
				dist -= chan.ent->attr3;
			}
		}
		else if(chan.radius > 0) rad = chan.radius;
		if(rad > 0) volf -= clamp(dist/rad, 0.0f, 1.0f); // simple mono distance attenuation
		if(stereo && (v.x != 0 || v.y != 0) && dist>0)
		{
			v.rotate_around_z(-camera1->yaw*RAD);
			panf = 0.5f - 0.5f*v.x/v.magnitude2(); // range is from 0 (left) to 1 (right)
		}
	}
	int vol = clamp(int(volf*soundvol*chan.slot->volume*(1.0f/float(255*255)) + 0.5f), 0, 1);
	int pan = clamp(int(panf*255.9f), 0, 255);
	if(vol == chan.volume && pan == chan.pan) return false;
	chan.volume = vol;
	chan.pan = pan;
	chan.dirty = true;
	return true;
}  

void reclaimchannels()
{
	loopv(channels)
	{
		soundchannel &chan = channels[i];
		if(chan.inuse && !MIX_TrackPlaying(chan.track)) freechannel(i);
	}
}

void syncchannels()
{
	loopv(channels)
	{
		soundchannel &chan = channels[i];
		if(chan.inuse && chan.hasloc() && updatechannel(chan)) syncchannel(chan);
	}
}

VARP(minimizedsounds, 0, 0, 1);

void updatesounds()
{
	updatemumble();
	if(nosound) return;
	if(minimized && !minimizedsounds) stopsounds();
	else
	{
		reclaimchannels();
		if(mainmenu) stopmapsounds();
		else checkmapsounds();
		//syncchannels();
	}
	if(music)
	{
		if(!MIX_TrackPlaying(musictrack)) musicdone();
		else if(MIX_TrackPaused(musictrack)) MIX_ResumeTrack(musictrack);
	}
}

VARP(maxsoundsatonce, 0, 7, 100);

VAR(dbgsound, 0, 0, 1);

void preloadsound(int n)
{
	gamesounds.preloadsound(n);
}

void preloadmapsound(int n)
{
	mapsounds.preloadsound(n);
}

void preloadmapsounds()
{
	const vector<extentity *> &ents = entities::getents();
	loopv(ents)
	{
		extentity &e = *ents[i];
		if(e.type==ET_SOUND) mapsounds.preloadsound(e.attr1);
	}
}
 
int playsound(int n, const vec *loc, extentity *ent, int flags, int loops, int fade, int chanid, int radius, int expire)
{
	if(nosound || !soundvol || (minimized && !minimizedsounds)) return -1;

	soundtype &sounds = ent || flags&SND_MAP ? mapsounds : gamesounds;
	if(!sounds.configs.inrange(n)) { conoutf(CON_WARN, "unregistered sound: %d", n); return -1; }
	soundconfig &config = sounds.configs[n];

	if(loc)
	{
		// cull sounds that are unlikely to be heard
		int maxrad = game::maxsoundradius(n);
		if(radius <= 0 || maxrad < radius) radius = maxrad;
		if(camera1->o.dist(*loc) > 1.5f*radius)
		{
			if(channels.inrange(chanid) && sounds.playing(channels[chanid], config))
			{
				MIX_StopTrack(channels[chanid].track, 0);
				freechannel(chanid);
			}
			return -1;    
		}
	}

	if(chanid < 0)
	{
		if(config.maxuses)
		{
			int uses = 0;
			loopv(channels) if(sounds.playing(channels[i], config) && ++uses >= config.maxuses) return -1;
		}

		// avoid bursts of sounds with heavy packetloss and in sp
		static int soundsatonce = 0, lastsoundmillis = 0;
		if(totalmillis == lastsoundmillis) soundsatonce++; else soundsatonce = 1;
		lastsoundmillis = totalmillis;
		if(maxsoundsatonce && soundsatonce > maxsoundsatonce) return -1;
	}

	if(channels.inrange(chanid))
	{
		soundchannel &chan = channels[chanid];
		if(sounds.playing(chan, config))
		{
			if(loc) chan.loc = *loc;
			else if(chan.hasloc()) chan.clearloc();
			return chanid;
		}
	}
	if(fade < 0) return -1;

	soundslot &slot = sounds.slots[config.chooseslot(flags)];
	if(!slot.sample->audio && !slot.sample->load()) return -1;

	if(dbgsound) conoutf(CON_DEBUG, "sound: %s", slot.sample->name);
 
	chanid = -1;
	loopv(channels) if(!channels[i].inuse) { chanid = i; break; }
	if(chanid < 0 && channels.length() < maxchannels) chanid = channels.length();
	if(chanid < 0) loopv(channels) if(!channels[i].volume) { chanid = i; break; }
	if(chanid < 0) return -1;

	soundchannel &chan = newchannel(chanid, mixer, &slot, loc, ent, flags, radius);
	updatechannel(chan);
	int playing = -1;

	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetNumberProperty(props, "MIX_PROP_PLAY_LOOPS_NUMBER", loops);
	if(expire >= 0) SDL_SetNumberProperty(props, "MIX_PROP_PLAY_MAX_MILLISECONDS_NUMBER", expire);
	if(fade) SDL_SetNumberProperty(props, "MIX_PROP_PLAY_MAX_MILLISECONDS_NUMBER", fade);
	 
	MIX_SetTrackGain(chan.track, chan.volume / 255.0f);
	MIX_SetTrackAudio(chan.track, slot.sample->audio);
	if(MIX_PlayTrack(chan.track, props)) playing = chanid;

	if(playing >= 0) syncchannel(chan); 
	else freechannel(chanid);
	return playing;
}

void stopsounds()
{
	loopv(channels) if(channels[i].inuse)
	{
		MIX_StopTrack(channels[i].track, 0);
		freechannel(i);
	}
}

bool stopsound(int n, int chanid, int fade)
{
	if(!gamesounds.configs.inrange(n) || !channels.inrange(chanid) || !channels[chanid].inuse || !gamesounds.playing(channels[chanid], gamesounds.configs[n])) return false;
	if(dbgsound) conoutf(CON_DEBUG, "stopsound: %s", channels[chanid].slot->sample->name);
	if(!fade || !MIX_StopTrack(channels[chanid].track, MIX_TrackMSToFrames(channels[chanid].track, fade)))
	{
		MIX_StopTrack(channels[chanid].track, 0);
		freechannel(chanid);
	}
	return true;
}

int playsoundname(const char *s, const vec *loc, int vol, int flags, int loops, int fade, int chanid, int radius, int expire) 
{ 
	if(!vol) vol = 100;
	int id = gamesounds.findsound(s, vol);
	if(id < 0) id = gamesounds.addsound(s, vol);
	return playsound(id, loc, NULL, flags, loops, fade, chanid, radius, expire);
}

ICOMMAND(sound, "i", (int *n), playsound(*n));

ICOMMAND(entsoundname, "i", (int* id),
	if (mapsounds.slots.inrange(*id))
	{
		defformatstring(s, "%s", mapsounds.slots[*id].sample->name); result(s);
	}
);

void writemapsounds(stream *f)
{
	loopv(mapsounds.slots)
	{
		f->printf("mapsound \"%s\" %d // %d\n", mapsounds.slots[i].sample->name, mapsounds.slots[i].volume, i);
	}
}

void resetsound()
{
	clearchanges(CHANGE_SOUND);
	if(!nosound) 
	{
		cleanupsamples();
		if(music)
		{
			MIX_StopTrack(musictrack, 0);
			MIX_DestroyAudio(music);
		}
		if(musicstream) musicstream->seek(0, SEEK_SET);
		MIX_DestroyMixer(mixer);
	}
	initsound();
	resetchannels();
	if(nosound)
	{
		DELETEA(musicfile);
		DELETEA(musicdonecmd);
		music = NULL;
		cleanupsamples();
		return;
	}
	if(music && loadmusic(musicfile))
	{
		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetNumberProperty(props, "MIX_PROP_PLAY_LOOPS_NUMBER", musicdonecmd ? 0 : -1);
		MIX_SetTrackAudio(musictrack, music);
		MIX_PlayTrack(musictrack, props);
		MIX_SetTrackGain(musictrack, (musicvol * 1.0f) / 255);
	}
	else
	{
		DELETEA(musicfile);
		DELETEA(musicdonecmd);
	}
}

COMMAND(resetsound, "");

// Mumble positional audio
// TODO: Check if this works, fix context to reflect connected server.

/*
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <fcntl.h> // For O_* constants
#endif // _WIN32

struct LinkedMem {
#ifdef _WIN32
	UINT32	uiVersion;
	DWORD	uiTick;
#else
	uint32_t uiVersion;
	uint32_t uiTick;
#endif
	float	fAvatarPosition[3];
	float	fAvatarFront[3];
	float	fAvatarTop[3];
	wchar_t	name[256];
	float	fCameraPosition[3];
	float	fCameraFront[3];
	float	fCameraTop[3];
	wchar_t	identity[256];
#ifdef _WIN32
	UINT32	context_len;
#else
	uint32_t context_len;
#endif
	unsigned char context[256];
	wchar_t description[2048];
};

#ifdef WIN32
static HANDLE mumblelink = NULL;
static LinkedMem *mumbleinfo = NULL;
#define VALID_MUMBLELINK (mumblelink && mumbleinfo)
#elif defined(_POSIX_SHARED_MEMORY_OBJECTS)
static int mumblelink = -1;
static MumbleInfo* mumbleinfo = (MumbleInfo*)-1;
#define VALID_MUMBLELINK (mumblelink >= 0 && mumbleinfo != (MumbleInfo *)-1)
#endif

VARFP(mumble, 0, 1, 1, { if (mumble) initmumble(); else closemumble(); });

void initmumble() {
	if (!mumble) return;
	#ifdef _WIN32
		mumblelink = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"MumbleLink");
		if (mumblelink == NULL) return;

		mumbleinfo = (LinkedMem*)MapViewOfFile(mumblelink, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkedMem));
		if (mumbleinfo == NULL) {
			CloseHandle(mumblelink);
			mumblelink = NULL;
			return;
		}
	#else
		char memname[256];
		snprintf(memname, 256, "/MumbleLink.%d", getuid());

		mumblelink = shm_open(memname, O_RDWR, S_IRUSR | S_IWUSR);

		if (mumblelink < 0) {
			return;
		}

		mumbleinfo = (LinkedMem*)(mmap(NULL, sizeof(struct LinkedMem), PROT_READ | PROT_WRITE, MAP_SHARED, mumblelink, 0));

		if (mumbleinfo == (void*)(-1)) {
			mumbleinfo = NULL;
			return;
		}
	#endif
}

void closemumble()
{
#ifdef WIN32
	if (mumbleinfo) { UnmapViewOfFile(mumbleinfo); mumbleinfo = NULL; }
	if (mumblelink) { CloseHandle(mumblelink); mumblelink = NULL; }
#elif defined(_POSIX_SHARED_MEMORY_OBJECTS)
	if (mumbleinfo != (MumbleInfo*)-1) { munmap(mumbleinfo, sizeof(MumbleInfo)); mumbleinfo = (MumbleInfo*)-1; }
	if (mumblelink >= 0) { close(mumblelink); mumblelink = -1; }
#endif
}

static inline vec mumblevec(const vec& v, bool pos = false)
{
	// change from X left, Z up, Y forward to X right, Y up, Z forward
	// 8 cube units = 1 meter
	vec m(-v.x, v.z, v.y);
	if (pos) m.div(8);
	return m;
}

void updatemumble() {
	if (!mumbleinfo) return;

	if (mumbleinfo->uiVersion != 2) {
		wcsncpy(mumbleinfo->name, L"Cardboard", 256);
		wcsncpy(mumbleinfo->description, L"Mumble positional audio support for the Cardboard Engine.", 2048);
		mumbleinfo->uiVersion = 2;
	}
	mumbleinfo->uiTick++;

	// Left handed coordinate system.
	// X positive towards "right".
	// Y positive towards "up".
	// Z positive towards "front".
	//
	// 1 unit = 1 meter

	// Unit vector pointing out of the avatar's eyes aka "At"-vector.
	vec front = mumblevec(vec(RAD*player->yaw, RAD*player->pitch));
	mumbleinfo->fAvatarFront[0] = front.x;
	mumbleinfo->fAvatarFront[1] = front.y;
	mumbleinfo->fAvatarFront[2] = front.z;

	// Unit vector pointing out of the top of the avatar's head aka "Up"-vector (here Top points straight up).
	vec top = mumblevec(vec(RAD*player->yaw, RAD*(player->pitch+90)));
	mumbleinfo->fAvatarTop[0] = top.x;
	mumbleinfo->fAvatarTop[1] = top.y;
	mumbleinfo->fAvatarTop[2] = top.z;

	// Position of the avatar (here standing slightly off the origin)
	vec pos = mumblevec(player->o, true);
	mumbleinfo->fAvatarPosition[0] = pos.x;
	mumbleinfo->fAvatarPosition[1] = pos.y;
	mumbleinfo->fAvatarPosition[2] = pos.z;

	// Same as avatar but for the camera.
	vec campos = mumblevec(camera1->o, true);
	mumbleinfo->fCameraPosition[0] = campos.x;
	mumbleinfo->fCameraPosition[1] = campos.y;
	mumbleinfo->fCameraPosition[2] = campos.z;

	vec camfront = mumblevec(vec(RAD*camera1->yaw, RAD*camera1->pitch));
	mumbleinfo->fCameraFront[0] = camfront.x;
	mumbleinfo->fCameraFront[1] = camfront.y;
	mumbleinfo->fCameraFront[2] = camfront.z;

	vec camtop = mumblevec(vec(RAD*camera1->yaw, RAD*(camera1->pitch+90)));
	mumbleinfo->fCameraTop[0] = camtop.x;
	mumbleinfo->fCameraTop[1] = camtop.y;
	mumbleinfo->fCameraTop[2] = camtop.z;

	// Identifier which uniquely identifies a certain player in a context (e.g. the ingame name).
	fpsent *plr = (fpsent *)player;

	wchar_t plrname[30];
	size_t plrlen = strlen(plr->name);

	mbstowcs_s(&plrlen, plrname, ((fpsent *)player)->name, plrlen);
	wcsncpy(mumbleinfo->identity, plrname, 256);
	// Context should be equal for players which should be able to hear each other positional and
	// differ for those who shouldn't (e.g. it could contain the server+port and team)
	defformatstring(mumblecontext, "TODO:SERVER_%s", plr->team);
	memcpy(mumbleinfo->context, "ContextBlob\x00\x01\x02\x03\x04", 16); // "127.0.0.1:35000_red"
	mumbleinfo->context_len = 16;
}
*/

#ifdef WIN32

#include <wchar.h>

#else

#include <unistd.h>

#ifdef _POSIX_SHARED_MEMORY_OBJECTS
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <wchar.h>
#endif

#endif

#if defined(WIN32) || defined(_POSIX_SHARED_MEMORY_OBJECTS)
struct MumbleInfo
{
	int version, timestamp;
	vec pos, front, top;
	wchar_t name[256];
};
#endif

#ifdef WIN32
static HANDLE mumblelink = NULL;
static MumbleInfo *mumbleinfo = NULL;
#define VALID_MUMBLELINK (mumblelink && mumbleinfo)
#elif defined(_POSIX_SHARED_MEMORY_OBJECTS)
static int mumblelink = -1;
static MumbleInfo *mumbleinfo = (MumbleInfo *)-1; 
#define VALID_MUMBLELINK (mumblelink >= 0 && mumbleinfo != (MumbleInfo *)-1)
#endif

#ifdef VALID_MUMBLELINK
VARFP(mumble, 0, 1, 1, { if(mumble) initmumble(); else closemumble(); });
#else
VARFP(mumble, 0, 0, 1, { if(mumble) initmumble(); else closemumble(); });
#endif

void initmumble()
{
	if(!mumble) return;
#ifdef VALID_MUMBLELINK
	if(VALID_MUMBLELINK) return;

	#ifdef WIN32
		mumblelink = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MumbleLink");
		if(mumblelink)
		{
			mumbleinfo = (MumbleInfo *)MapViewOfFile(mumblelink, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(MumbleInfo));
			if(mumbleinfo) wcsncpy(mumbleinfo->name, L"Cardboard", 256);
		}
	#elif defined(_POSIX_SHARED_MEMORY_OBJECTS)
		defformatstring(shmname, "/MumbleLink.%d", getuid());
		mumblelink = shm_open(shmname, O_RDWR, 0);
		if(mumblelink >= 0)
		{
			mumbleinfo = (MumbleInfo *)mmap(NULL, sizeof(MumbleInfo), PROT_READ|PROT_WRITE, MAP_SHARED, mumblelink, 0);
			if(mumbleinfo != (MumbleInfo *)-1) wcsncpy(mumbleinfo->name, L"Cardboard", 256);
		}
	#endif
	if(!VALID_MUMBLELINK) closemumble();
#else
	conoutf(CON_ERROR, "Mumble positional audio is not available on this platform.");
#endif
}

void closemumble()
{
#ifdef WIN32
	if(mumbleinfo) { UnmapViewOfFile(mumbleinfo); mumbleinfo = NULL; }
	if(mumblelink) { CloseHandle(mumblelink); mumblelink = NULL; }
#elif defined(_POSIX_SHARED_MEMORY_OBJECTS)
	if(mumbleinfo != (MumbleInfo *)-1) { munmap(mumbleinfo, sizeof(MumbleInfo)); mumbleinfo = (MumbleInfo *)-1; } 
	if(mumblelink >= 0) { close(mumblelink); mumblelink = -1; }
#endif
}

static inline vec mumblevec(const vec &v, bool pos = false)
{
	// change from X left, Z up, Y forward to X right, Y up, Z forward
	// 8 cube units = 1 meter
	vec m(-v.x, v.z, v.y);
	if(pos) m.div(8);
	return m;
}

void updatemumble()
{
#ifdef VALID_MUMBLELINK
	if(!VALID_MUMBLELINK) return;

	static int timestamp = 0;

	mumbleinfo->version = 1;
	mumbleinfo->timestamp = ++timestamp;

	mumbleinfo->pos = mumblevec(player->o, true);
	mumbleinfo->front = mumblevec(vec(RAD*player->yaw, RAD*player->pitch));
	mumbleinfo->top = mumblevec(vec(RAD*player->yaw, RAD*(player->pitch+90)));
#endif
}