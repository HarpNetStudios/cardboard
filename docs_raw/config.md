---
title: Carmine Impact Docs — Configuration
---

## Before You Start

Take some time to read the rest of this document, as it describes options and commands that may be crucial to a successful
experience with Carmine Impact.
 
Binding keys, as well as performance and gameplay related settings are documented below, these commands can be used in
both the console and configuration scripts. In addition, you can also press <kbd>ESC</kbd> to go into the menu, which will provide
you with an interface for navigating some (not all) of the commands.

## Running Carmine

For Windows: Install and then run launcher.exe or launch from your software distribution platform (Itch, Steam, etc.).

For Linux: Extract and then run ./cardboard_unix. Needs a decent and compliant OpenGL implementation.

The game and engine derives its simplicity from some rather brute force rendering methods, and as such it needs 
a beefy machine to get good visual quality. It runs best with vertical sync (vsync) set to OFF and at high
refresh rates (otherwise, you may get excessive LOD/FPS variance).

## Command Line Options

### `-d`

This starts as a dedicated server. The default is a non-dedicated server with only a single client in graphical
mode. Dedicated servers run in the shell only (no graphics), with increased priority yet use very little cpu time and memory,
so you can run one in the background, or at the same time with a client if you want to host a game (which is usually better 
than using a listen server). Servers use the ports 35000 (UDP) and 35001 (UDP).

### `-dN`

If N=1, starts a listen server which allows it to simultaneously function as both a client and server. Note that a listen
server is limited by your in-game frame rate, so you should use a dedicated server if your graphics card is slow or you
have enabled frame rate limiting options such as "vsync". If N=2, it starts a dedicated server as for the "-d" command-line option.

### `-wN`

Sets the screen resolution width to N (default: 640).

### `-hN`

Sets the screen resolution height to N (default: 480).

### `-zN`

Sets the z-buffer precision to N bits. This should be at least 24 or you may experience depth problems.

### `-aN`

Sets FSAA (Full Scene AntiAliasing) to N samples, i.e. -a4 is 4xFSAA

### `-t`

Sets the game to run windowed.

### `-qS`

Sets S to your home directory. If set, the engine will look for files in your home directory as well as
the normal installation directory. However, all files will be written to your home directory, instead of the
normal installation directory.

### `-kS`

Adds the mod directory S to the list of directories the engine will search in for files. Directories will
be searched in the order listed on the command-line, and if not found, the engine looks in the installation
directory.

### `-gS`

Sets the log file to S. All console output is written to the log file.

### `-lS`

Loads map S on startup.

### `-xS`

Executes the script commands S on startup, note that this is done before the map loads, so if you wish them to
be executed later, you should enclose them in a sleep statement. Example: -x"sleep 1000 [ connect localhost ]"

### `-uN`

Sets the server upstream bandwidth to N bytes per second. Only set this parameter if you know what you are doing,
specifying a wrong value is worse than not specifying it at all.

### `-nN`

Sets the server description, for people pinging the server. usually does not need to be set if you have a descriptive
domain name already, but if you set it, keep it short as it may otherwise be truncated (example: -n"Bobs Instagib Server").

### `-cN`

Sets the max number of clients to N. The default is 4. If you want to set it higher, be aware that bandwidth usage almost
doubles with each extra client, so only do this if the server runs on a serious pipe (not your home DSL or Cable connection).

### `-iS`

Sets the server ip to S. This option is only useful for people running on machines with multiple network interfaces.

### `-jN`

Sets the server port to N. This option is only useful for people running on machines with multiple network interfaces. Note that both UDP ports N (game traffic) and N+1 (pings) must be available for this to work. If not specified, the default ports used are 35000 and 35001. Regardless of what ports are set, port 34999 must be available for pinging servers over a LAN to work.

### `-mS`

Sets the master server to use for either server (registering) and client (updating) to S. (default: master.harpnetstudios.com).

### `-pS`

Sets the server's administrative password to S.

### `-yS`

Locks the server so that password S must be provided to connect to it.

### `-oN`

Sets the openness of the server to N. N=1 disables mastermodes 2 (locked) and 3 (private), while N=0 (default) enables all mastermodes.  

## Console Commands
### `bind K A` { #bind }
### `editbind K A` { #editbind }

Binds key K to commands A (see console language below for what you can put in A). To find out what key names and their default
bindings are, look at data/keymap.cfg, then add bind commands to your autoexec.cfg. Editbind works much the same way, except
now the key is only available in edit mode (if no editbind is defined for a certain key, it will use the normal binding).

### `alias N A` { #alias }

Binds name N to commands A, after which you can use N as a shortcut for A. You 
may re-bind N as often as you like, call it recursively etc. You may use N = A as a shortcut for alias.

### `push N A B` { #push }

Pushes the old value of name N onto its stack, and then aliases N to A, and executes B, after which the old value of N is restored from the stack.

### `local N1 [N2...]` { #local }

Makes the supplied variables local variables, such that their old values are saved onto the stack and restored at the end of the current block. They are initialized to null.

### `quit`

Quits without asking.

### `forward`
### `backward`
### `left`
### `right`

Move you in said direction (default key: up down left right, w s a d).

### `attack`

Fires the current weapon (default key: left mouse).

### `jump`

Triggers a jump (default key: space, right mouse).

### `zoom N` { #zoom }

Zooms in if N is -1, zooms out if N is 1, and turns off zoom if N is 0.

### `taunt`

Plays your player model's taunt animation.

### `floatspeed N` { #floatspeed }

Sets the speed at which your character floats in edit mode or spectator mode (default: 100).

### `invmouse B` { #invmouse }

Toggles mouse between regular and inverted (flight sim) mode (default: 0 (uninverted)).

### `sensitivity S` { #sensitivity }

Sets mouse sensitivity (default: 3). The feel of which should be a similar number to Quake.

### `sensitivityscale S` { #sensitivityscale }

Scales the effect of an increase or decrease in the sensitivity value. The larger the value, the smaller the difference
between sensitivity values (default: 1).

### `maxfps N` { #maxfps }

Sets the max number of FPS (Frames per Second). Useful if you have a recent machine that can run the engine at close to 1000FPS,
or if you want to test. If you set maxfps to exactly or a simple ratio of your monitor's refresh rate, you may experience tearing.
Be aware that maxfps is not a replacement for vsync (default: 200).

### `fov N` { #fov }

Sets your FOV to 10..150.

### `zoomfov N` { #zoomfov }

Sets your zoomed-in FOV to N.

### `zoominvel N` { #zoominvel }

Sets the number of milliseconds over which zoom-in happens. Zoom-in is instant if N is 0. (default: 250)

### `zoomoutvel N` { #zoomoutvel }

Sets the number of milliseconds over which zoom-out happens. Zoom-out is instant if N is 0. (default: 100)

### `zoomsens S` { #zoomsens }

Sets mouse sensitivity when zoomed-in (default: 1).

### `zoomautosens B` { #zoomautosens }

Toggles whether mouse sensitivity should be automatically scaled to match the zoomed-in FOV relative to FOV. This overrides "zoomsens" if enabled. (default: 1)

### `hudgun N` { #hudgun }

Sets hudguns either on (1) or off (0).

### `hudgunsway N` { #hudgunsway }

Sets hudgun sway either on (1) or off (0).

### `grass N` { #grass }

Sets grass 0..1 generated by autograss textures to be drawn (default: 1).

### `grassdist N` { #grassdist }

Sets the maximum distance 0..10000 at which grass is drawn (default: 128). Higher numbers mean more grass will be drawn and be more expensive.

### `grassstep N` { #grassstep }

Sets the step distance between rows of grass to 0.5..8 (default: 2). Higher step distances will look sparser, but render faster.

### `shaderdetail N` { #shaderdetail }

This sets the level of detail shaders should use, for N=3 is highest, and N=0 is lowest. Lower settings will give speedups
on slower hardware.

### `maxtexsize N` { #maxtexsize }

This sets the maximum allow size of a texture in texture memory. If any texture's width or height is greater than N, the texture will be scaled down to be no more than NxN. This variable is not saved, and is reset on each run with the 3D card's texture size limit, so set this in your "autoexec.cfg" if you want to override it. Set this to a small value like 128 if your card has very little texture memory (i.e. 32 MB or less).

### `texcompress N` { #texcompress }

Any texture whose width or height is greater than or equal to N will be compressed using S3 texture compression, if available on your card. The default is set to 1024, so set this lower if your card supports S3TC and you have very little texture memory (i.e. 64 MB or less).

### `watersubdiv N` { #watersubdiv }

Determines the subdivision of the water surface in maps, which can greatly 
affect fps on slow machines. Must be a power of 2: 4 is the default, 8 is 
recommended for people on slow machines, 2 is nice for fast machines, and 1 is 
quite OTT. See the edit reference for more information on how to add the water material to your levels.

### `waterlod N` { #waterlod }

Sets the precision level of the water LOD (Level of Detail) from 0..3, where 0 is the lowest and 3 is the highest setting
(default: 1).

### `waterreflect B` { #waterreflect }

Enables or disables water reflections (default: 1).

### `waterenvmap B` { #waterenvmap }

Enables or disables water environment-mapping (cheaper fake reflections). By default, this is used when "waterreflect" is turned off.

### `waterrefract B` { #waterrefract }

Enables or disables water refractions. (default: 1).

### `waterfade B` { #waterfade }

Enables or disables soft fading of water edges. This option only works if "waterrefract" is enabled. (default: 1).

### `waterfallrefract B` { #waterfallrefract }

Enables or disables refractive waterfalls. (default: 0).

### `waterfallenv B` { #waterfallenv }

Enables or disables environment-mapped waterfall reflections. (default: 1).

### `reflectdist N` { #reflectdist }

Sets the maximum distance geometry/objects can be at to be in a reflection from 0..10000 (default: 2000).

### `reflectsize N` { #reflectsize }

Sets the resolution of reflected surfaces (where the dimensions would measure 2^N x 2^N), from 6..10 (default: 8). Lowering this value results
in a grainier image, whereas increasing it results in a finer and mroe detailed image. This too should be used to fine tune the speed of
reflection rendering.

### `maxreflect N` { #maxreflect }

Sets the maximum number of reflection surfaces that can be visible at once from 1..8 (default: 1). Any surfaces exceeding this
limit will not be given reflections on their surfaces.

### `reflectmms B` { #reflectmms }

Toggles whether map models are rendered in reflections (default: 1).

### `envmapsize N` { #envmapsize }

Sets the resolution of generated environment maps to 2^Nx2^N (default: 7).

### `maxparticledistance N` { #maxparticledistance }

The maximimum visibility distance for particle effects from 256..4096 (default: 512).

### `particlesize N` { #particlesize }

The size of default particle effects from 20..500 (default: 100).

### `emitfps N` { #emitfps }

The rate at which paricles can be emitted from 1..200 fps (default: 60).

### `flarelights B` { #flarelights }

Enables or disables automatic lens flares for all lights (default: 0). Requires very careful placement of lights to be effective, and most maps are not currently designed with this in mind.
Therefore it is recomended that lens flares instead be placed manually by using particles entities.

### `flarecutoff N` { #flarecutoff }

The maximum visiblity distance of a lens flare source from 0..10000 (default: 1000).  Note that visibility also depends on the viewers angle to the flare source.

### `flaresize N` { #flaresize }

The size of the lens flare effect from 20..500 (default: 100).

### `setpostfx [N [P]]` { #setpostfx }

Sets your favourite fullscreen shader. Active permanently once set. Use without N
argument to turn it off. Certain shaders like bloom require you to specify a P for the intensity.
Current example shaders: bw, invert, gbr, sobel, bloom

### `screenshot`

Writes out "screenshotN.bmp", where N is the number of milliseconds since the game was launched (default key: F12).

### `musicvol N` { #musicvol }
### `soundvol N` { #soundvol }

Sets the music/sound volume from 0..255.

### `soundbufferlen N` { #soundbufferlen }

Sets the sound buffer length in bytes, tweak this if you experience stuttering or lagging sound (default: 512).

### `scr_w N` { #scr_w }

Sets the screen resolution's width to N. This variable can also be used to check the current screen resolution width.

### `scr_h N` { #scr_h }

Sets the screen resolution's height to N. This variable can also be used to check the current screen resolution height.

### `screenres W H` { #screenres }

Sets the screen resolution's width to W and height to H. On supported platforms, this can change the screen resolution immediately without requiring OpenGL to reset.

### `gamma N` { #gamma }

Sets the hardware gamma value to N percent (default: 100). May not work if your card/driver doesn't support it.

### `hidestats B` { #hidestats }

Toggles display of fps/rendering stats on the HUD (default: 1).

### `hidehud B` { #hidehud }

Toggles the entire hud display, which includes rendering stats, ammo, and echoed text (default: 0).

### `maxroll N` { #maxroll }

Sets the maximum value your display will roll on strafing from 0..20 (default: 3)

### `crosshairsize N` { #crosshairsize }

Sets the size of your crosshair, 0 being off (default: 3).

### `crosshairfx B` { #crosshairfx }

Toggles crosshair effects (default: 1). When on, the crosshair will go grey when the weapon is reloading, orange when health <= 50,
or red when <=25.

### `exec S` { #exec }

Executes all commands in config file S.

### `history N` { #history }

Executes the Nth command in the command line history. For example, binding "history 0" to a key allows you to quickly repeat the
last command typed in (useful for placing many identical entities etc.)

### `keymap K N A` { #keymap }

Sets up the keymap for key code K with name N and default action A. You should 
never have to use this command manually, use "bind" instead.

## GUI Commands

### `gui2d B` { #gui2d }

Sets whether menus should be shown in 2D, or 3D if disabled (default 0).

### `menudistance D` { #menudistance }

Sets the distance at which the menu is created in front of the player (default 40).

### `menuautoclose D` { #menuautoclose }

Sets the distance at which the menu is automatically closed when the player moves away from it (default 120).

### `guiautotab H` { #guiautotab }

Sets the height of the gui before tabs are automatically generated.

### `cleargui`

Hides the menu.

### `showgui N` { #showgui }

Displays the menu with name N previously defined, and allows the user to pick and manipulate items with the cursor. Pressing ESC will cancel the menu.

### `newgui S` { #newgui }

Creates a new menu with name S. All the following 'gui' menuitem commands will apply to this menu.
See "data/menus.cfg" for defaults.

### `guilist '[' ... ']'`  { #guilist }

Defines a menu item that is a group of menu items. The layout of each nested group will alternate - the top level (newgui) is laid out vertically, 
the first guilist will be laid out horizontally, the second guilist vertically, etc. This allows tables and lists to be created. The 
layout direction also influences the display of items such as sliders, bars, progress bars, and struts.

### `guispring [W]` { #guispring }

A list containing springs will fill its parent list along its layout direction by dividing any needed space among the springs. The spring is weighted by a positive integer W, otherwise defaults to W=1.

### `guititle S` { #guititle }

Creates a menu item with horizontally centered title S.

### `guibutton [S] A [I]` { #guibutton }

Creates a menu button with the name S and icon I, which will execute A when selected. If you only specify one parameter, then it will be used for both.
The icon used will be I if supplied, otherwise a blue-button if A contains showgui, otherwise it is a green button. Clicking a button will close the menu.

### `guiimage P A S [O]` { #guiimage }

Creates a menu image tile from the path P, which will execute A when selected. Clicking the image will close the menu. Uses image scale S and O=1 if an overlay is required.

### `guitext S` { #guitext }

Creates a menu item consisting of the text line S and an information icon.

### `guitextbox S W [H] [C]` { #guitextbox }

Creates a text box showing the text S, with width W, height H, and color C. The default is a single line of white text. Note that the height will increase automatically so as to enclose all the text.

### `guibar`

Creates a vertical/horizontal menu bar. The orientation depends on the layout direction.

### `guislider V [MIN MAX A]` { #guislider }

Creates a vertical/horizontal menu slider that is bound to a variable V. The min/max bounds of the slider will be as defined by the data model of the variable unless explicitly specified. 
An action A can also be specified for every slider change.
The orientation of the slider depends on the layout direction.

### `guicheckbox N V [ON OFF A]` { #guicheckbox }

Creates a menu checkbox with a label N that is bound to a variable V. The default value are 0/1 unless supplied by ON/OFF. An action A can also be specified for every checkbox change.

### `guiradio N V VAL [A]` { #guiradio }

Creates a menu radio button with a label N that is bound to a variable V. The radio button is only ticked when the variable has value VAL, and 
presing the button will update the variable to the value VAL.
An action A can also be specified for every radio button change.

### `guitab S` { #guitab }

Creates a new menu tab with title S. Note: has no effect if used within a guilist.

### `guifield A N [O [U]]` { #guifield }

Creates a field which accepts editable input, the value of which is taken from and updated
to (upon change) the alias A, and can have at most N characters. If specified, O is executed
when the user presses enter. If U is also specified, it will execute it every every frame
(unless it is currently being edited) to allow updating the field. Note that all fields that
share the same A alias, also share the same edit info, if the user moves outside the field the
changes are not committed, if they select another field, changes will be lost.

## Server Configuration Commands
The following commands may be placed in the file "server-init.cfg" in the root Carmine directory to configure a dedicated server, or they may be used inside a running client to config
ure a listen server:
### `startlistenserver [MASTER]` { #startlistenserver }
Starts a listen server from within a running game client. If MASTER=1, then the server will report to the master server, otherwise if MASTER=0 or not supplied the server will not report to the master server. The various server configuration commands can be used before this command to setup properties of the listen server.
### `stoplistenserver`
Stops a listen server running from within a game client.
### `serverip S` { #serverip }
Sets the IP the server should bind/listen to S. This is only useful if your server is running on a host with multiple interfaces.
### `serverport N` { #serverport }
Sets the port the server should bind/listen to N. By default, server's listen on port 35000.
### `maxclients N` { #maxclients }
Sets the maximum number of clients that can connect to the server to N.
### `serverbotlimit N` { #serverbotlimit }
Sets the maximum number of bots a master can add to N. Admins are unaffected by this limit.
### `publicserver B` { #publicserver }
Toggles whether a server is considered a "public" server when B=1: can only gain master by "auth" or admin and doesn't allow locked/private mastermodes. B=0 allows "setmaster 1" and locked/private mastermodes (for coop-editing and such).
### `serverdesc S` { #serverdesc }
Sets the description shown for the server in the server browser to S.
### `serverpass S` { #serverpass }
Sets the password required to connect to the server to S. This option is only useful if you don't want anyone connecting to your server.
### `adminpass S` { #adminpass }
Sets the password to gain admin access to a server to S, as well as the password that may be used to override the private mastermode when connecting.
### `servermotd S` { #servermotd }
Sets the "message of the day", a message shown to users when they connect to the server, to S.
### `updatemaster B` { #updatemaster }
Toggles whether or not the server should report to the masterserver. B=1 enables (default) and B=0 disables.
### `mastername S` { #mastername }
Sets the IP of the master server the server reports to. This defaults to "master.harpnetstudios.com" and generally should not be changed.
### `restrictdemos B` { #restrictdemos }
Toggles whether admin is required to start recording a demo. B=1 enables (default) and B=0 disables.
### `maxdemos N` { #maxdemos }
The maximum number of demos the server will store (default: 5).
### `maxdemosize N` { #maxdemosize }
The maximum size a demo is allowed to grow to in megabytes (default: 16).
### `ctftkpenalty B` { #ctftkpenalty }
Toggles whether teamkilling the flag runner in CTF modes should disallow the teamkiller from stealing the flag (default: 1).

## CubeScript

Cube's console language is similar to console languages of other games (e.g. Quake), but is a bit more powerful in that it is a
full programming language.

What is similar to quake is the basic command structure: commands consist of 
the command itself, followed by any number of arguments seperated by 
whitespace. you can use "" to quote strings with whitespace in them (such as 
the actions in bind/alias), and whereever a command is required you can also 
use ; to sequence multiple commands in one.

What is new compared to quake is that you can evaluate aliases and expressions. 
You can substitute the value of an alias as an argument by prefixing it with a 
"$" sign, (example: echo The current value of x is $x). You can even 
substitute the values of console variables this way, i.e $fov gives the current 
fov. Some aliases are set automatically, for example $arg1 to $argN are set (as if by the "push" command)
if you supply arguments when you execute an alias, and are popped when the alias finishes executing.

There are two alternatives to "" for quoting a string: () and []. They work in
the same way as "", with the difference that they can be nested infinitely, and
that they may contain linefeeds (useful for larger scripts). () is different
from [] in that it evaluates the commands contained in it _before_ it evaluates
the surrounding command, and substitutes the results. () bracketed strings are
called expressions, while [] bracketed strings may be thought of as blocks.

An alternative to $x is @x, which uses an alias as a macro. The difference is that @x 
can be substituted inside [] forms before they have ever been evaluated (at parse time), 
which makes them useful for composing strings or creating code on the fly. The @x form will be
substituted using the value of x at the time the enclosing [] is evaluated. You can add more @ prefixes
to move up more levels of []s, so @@x will move up two levels of []s and so on.
Example: `x = 0; if $cond [x = 1; [x = 2; echo @@x]]` will echo 0, since it uses the value of x at two levels up. 

The form @(body) is similar to @x, except that body contains commands executed at parse time.
The result value after body executes is substituted in for @(body). 
You may use multiple @ prefixes as with the @x form.
Example: @(result "Hello, World!") will substitute itself with Hello, World!

@[body] is also similar to @(body), except that it does not execute its contents and instead expands macros to use as the alias name for the final macro substitution.

The following commands are available for programming:

### `+ A B` { #op-add }
### `- A B` { #op-sub }
### `* A B` { #op-mul }
### `div A B` { #op-div }
### `mod A B` { #op-mod }

(add, substract, multiply, divide, modulo): these all work like the integer 
operators from other languages (example: echo x squared is (* $x $x)).

### `= A B` { #op-eql }
### `< A B` { #op-lt }
### `> A B` { #op-gt }
### `strcmp A B` { #strcmp }

(equals, lessthan, greaterthan, stringcompare): comparison operators that return 1 for true and 0 for false.

### `! A` { #op-not }
### `&& A B` { #op-and }
### `|| A B` { #op-or }
### `^ A B` { #op-xor }

(not, and, or, xor): boolean operators. 0 is false, everything else is true.
The AND and OR operators are implemented to shortcut.

### `strstr S N` { #strstr }

Searches the string S for the substring N and returns its starting position.
This function is case sensitive, when N is not found it returns -1.
(example: echo (strstr "for example this sentence" "this" ) - would return 14 ).

### `substr S A B` { #substr }

Returns a part from the string S starting from A for B chars
(example: echo (substr "grab this part here" 11 4 ) - would return "here" ).

### `strreplace S A B` { #strreplace }

This searches the string S for substring A and replaces it with another substring B.
(example: echo ( strreplace "this is serious" "serious" "fun" ) - would return "this is fun" ).

### `strlen S ` { #strlen }

Returns the length of the string S
(example: echo (strlen "long sentence") - would return 13 ).

### `min A B ` { #min }

This results the lower number of the 2 Strings A and B

### `max A B ` { #max }

This results the higher number of the 2 Strings A and B

### `rnd N ` { #rnd }

Grabs a random number from 0 to N, and results it.

### `if cond true false` { #if }

Executes the true or false part depending on wether cond is "0" or 
something else (example: if (< $x 10) [ echo "x is" $x ] [ echo "x is too big" ]).

### `loop I N body` { #loop }

Evaluates body N times, and sets the alias I from 0 to N-1 for every iteration (example: loop i 10 [ echo $i ]).

### `while cond body` { #while }

Evaluates body while cond evaluates to true. Note that cond here has to have [], otherwise it would only be evaluated once
(example: i = 0; while [ (< $i 10) ] [ echo $i; i = (+ $i 1) ]).

### `concat S...` { #concat }

Concatenates all the arguments and returns the result

### `concatword S...` { #concatword }

Same as concat but without spaces between the elements.

### `format F V1..Vn` { #format }

Substitutes the V values into the format string F and returns the result. The format strings %1 through %9 are substituted with V1 through V9, respectively, and may be
used multiple times. %% will generate a single % character (example: format "%1 bottles of %2 on the %3, %1 bottles of %2!" 99 beer wall).

### `at S N` { #at }

Grabs the Nth word out of string S and returns the result

### `listlen L` { #listlen }

Returns the number of items in the list L

### `listclients N` { #listclients }

Gives a list of all playernumbers if N is 1
if N is 0 it lists all players except you.

### `onrelease A` { #onrelease }

Only executes A if the command is executed on the release of a key/button (must be in an action in a bind or an alias in a bind).

### `result V` { #result }

Normally the result of a [] block is the result of the last command in the block. If you want the result to be a particular
variable or value, you can use e.g. "result $i" etc.

## Variables that are only really useful when used as value:

### `$editing`  { #editing }

This is true when in edit mode.

### `getalias V` { #getalias }

Returns the alias from the variable V.

### `getbind B` { #getbind }
### `geteditbind B` { #geteditbind }
### `getspecbind B` { #getspecbind }

Returns the game, edit or spectator bind B

### `getclientnum N` { #getclientnum }
### `getclientname N` { #getclientname }
### `getclientteam N` { #getclientteam }

This returns the client number, name or team of the player N.

### `getname` { #getname }
### `getteam` { #getteam }
### `getweapon` { #getweapon }
### `getfps` { #getfps }

Tells your own Name Team Weapon or FPS

### `gettex` { #gettex }

Adds the current selected texture to the front of your texture list in Y+scroll
Needs Allfaces set to 0 so it works.

### `gettexname` { #gettexname }

Results the current Texture name

### `isconnected N` { #isconnected }

If the Player N is online this is 1 (true)

### `isspectator N` { #isspectator }

If the Player N is spectator this is 1 (true)

