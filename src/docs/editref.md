---
title: Carmine Impact Docs — Editing Reference
---

# Editing Reference

WARNING: **Important Note** When dealing with array attributes (getting a piece of information from a set of values)
it is important to know that most start at zero (0) and count upwards from there. This means that the first
value is 0, the second is 1, the third is 2 and so forth. Please keep this in mind, especially when dealing
with entity attributes, as zero is always considered to be the first/default value, not 1.

## Editing

### `edittoggle`

Switches between map edit mode and normal (default key: right alt). In map edit mode
you can select bits of the map by clicking or dragging your crosshair on the
cubes (using the "attack" command, normally MOUSE1), then use the commands
below to modify the selection. While in edit mode, physics & collision
don't apply (noclip), and key repeat is ON.

### `dragging 0/1` { #dragging }

Select cubes when set to 1. stop selection when set to 0

### `editdrag`

Select cubes and entities. (default: left mouse button)

### `selcorners`

Select the corners of cubes. (default: middle mouse button)

### `moving 0/1` { #moving }

set to 1 to turn on. when on, it will move the selection (cubes not included) to another position. 
the plane on which it will move on is dependent on which side of the selection your cursor was on when 
turned on. set to 0 to turn off moving. if cursor is not on selection when turned on, moving
will automatically be turned off.

### `editmovedrag`

if cursor is in current cube selection, holding will move selection. otherwise will create new selection.

### `cancelsel`

Cancels out any explicit selection you currently have (default: space).

### `editface D N` { #editface }

This is the main editing command. D is the direction of the action, -1 for
towards you, 1 for away from you (default: scroll wheel). N=0 to push all corners in the white box (hold F). N=1
create or destroy cubes (default). N=2 push or pull a corner you are pointing at (hold Q).       

### `gridpower N` { #gridpower }

Changes the size of the grid. (default: g + scrollwheel)

### `edittex D` { #edittex }

Changes the texture on the current selection by browsing through a list of textures
directly shown on the cubes. D is the direction you want to cycle the textures
in (1 = forwards, -1 = backwards) (default: y + scrollwheel).
The way this works is slightly strange at first, but allows for very fast
texture assignment. All textures are in a list. and each time a texture is
used, it is moved to the top of the list. So after a bit of editing, all your
most frequently used textures will come first, and the most recently used
texture is set immediately when you press the forward key for the type. These
lists are saved with the map.

### `settex N` { #settex }

Changes the texture on the current selection to the texture ID specified by N.

### `gettex`

Moves the texture on the current selection to the top of the texture list. Useful for quickly
texturing things using already textured geometry.

### `getcurtex`

Returns the most recent texture in the MRU list as a texture ID.

### `getseltex`

Returns the texture ID of the current selection.

### `selextend`

Extend current selection to include the cursor.

### `passthrough`

normally cubes of equal size to the grid are given priority when selecting.
passthrough removes this priority while held down so that the cube the cursor is directly on is selected.   
Holding down passthrough will also give priority to cube over entities. (default: alt)

### `reorient`

Change the side the white box is on to be the same as where you are currently
pointing. (default: shift)

### `flip`

Flip (mirror) the selected cubes front to back relative to the side of the
white box. (default: x)

### `rotate D` { #rotate }

Rotates the selection 90 degrees around the side of the white box.
Automatically squares the selection if it isn't already.
(default: r + scroll wheel)

### `undo`

Multi-level undo of the changes caused by the above operations (default: z [or u]).

### `redo`

Multi-level redo of the changes caused by the above undo (default: i).

### `copy`
### `paste`

Copy copies the current selection into a buffer, with the origin marked by the red square
at the bottom of the selection box. Upon pressing 'paste', a selection box will be created
to identify the location of the pasted cubes. Releasing the 'paste' button will actually
paste the cubes. You can combine this with the 'moving' command to easily place and clone
sets of cubes. If the current gridsize is different from the buffer, the pasted copy will
be scaled by the same factor.

### `editcopy`
### `editpaste`

Will copy cubes as normal copy, but also features entity copies. There are three 
different methods of use:

- If no entity is explicitly selected, `editcopy` will copy the selected cube, just like 
  normal the normal [`copy`](#copy) command.
- If one or more entities are selected, `editcopy` will copy the last entity selected. 
  `Editpaste` will create a new entity using copy as the template if no entities are selected,
  otherwise it will overwrite all selected entities with the copied ent.
- If there are both entity and cube selections, `editcopy` will copy the entire selection.
  In other words, when `editpaste` is used it will paste the cube selection along with all the
  entities that were selected.

### `replacetex OLD NEW B`

Replaces every texture with the texture ID of OLD with NEW. B=1 to only target the current
selection's textures, not the entire map.

### `replacelasttex B`

Repeats the last texture edit across the whole map. Only those faces with textures matching the 
one that was last edited will be replaced. B=1 to only target the current selection's textures, not
the entire map.

### `editmat MAT [FILTER]` { #editmat }

Changes the type of material left behind when a cube is deleted to MAT.
If FILTER is specified, then only cubes with that material named by FILTER are changed to 
MAT. MAT may also be blank, indicating that only those parts of the material mask matching 
FILTER will be cleared, as opposed to setting MAT to `air`, which would clear the entire
material mask. FILTER may alternatively be one of `empty`, `notempty`, `solid`, and 
`notsolid` which will then only affect cubes containing such geometry.

Currently, the following types of materials are supported:

- `air`: the default material, has no effect. Overwrites other volume materials.
- `water`: acts as you would expect. Renders the top as a reflection/refraction and the 
  sides as a waterfall if it isn't contained. Should be placed with a floor at the bottom 
  to contain it. Shows blue volume in edit mode. Overwrites other volume materials.
- `glass`: a clip-like material with a blended/reflective surface. Glass also stops bullets.
  Will reflect the closest [envmap](#ent-envmap) entity, or if none is in range, the skybox. 
  Shows cyan volume in edit mode. Overwrites other volume materials.
- `lava`: renders the top as a glowing lava flow and the sides as lavafalls if it isn't 
  contained. It kills any players who enter it. Shows orange volume in edit mode.
  Overwrites other volume materials.
- `clip`: an invisible wall that blocks players movement but not bullets. Is ideally used
  to keep players "in bounds" in a map. Can be used sparingly to smooth out the flow around 
  decoration. Shows red volume in edit mode. Overwrites other clip materials.
- `noclip`: cubes with this material are always treated as empty in physics.
  Shows green volume in edit mode. Overwrites other clip materials.
- `gameclip`: a game mode specific clip material. Currently, it can stop flags from being 
  picked up in areas in CTF/protect modes, and it can stop capturing of bases in areas in
  capture modes. Overwrites other clip materials.
- `death`: causes the player to die if they are inside the material. Shows black volume in
  edit mode.
- `alpha`: all faces of cubes with this material are rendered transparently. Use the [`valpha`](#valpha)
  and [`texalpha`](#texalpha) commands to control the transparency of front and back faces.
  Shows pink volume in edit mode.
- `jumpreset`: causes the player's ability to double jump to be reset when inside the material.

### `recalc`

Recalculates scene geometry. This also will regenerate any envmaps to reflect the changed geometry,
and fix any geometry with `bumpenv*` shaders to use the closest available envmaps. This command is also
implicitly used by calclight.

### `havesel`

Returns the number of explicitly selected cubes for scripting purposes. 
Returns 0 if the cubes are only implicitly selected.

### `gotosel`

Goes to the position of the currently selected cube or entity.

### `saveprefab B` { #saveprefab }

Saves the current selection as a prefab named B.

### `pasteprefab B` { #pasteprefab }

Pastes the prefab named B.

## Heightmaps

Heightmaps are very different from what most people are probably used to; they are just 
normal cubes and are NOT special meshes. Really, it's just another way of editing the same
geometry. First, instead of selecting cubes, you select a brush (B+wheel) and textures
(middle mouse button while in heightmap mode to toggle). Once this is done, you can apply
the brush to all cubes that match the textures you've selected. Making hills and valleys
can be quite fast when using this feature. By default, all textures are automatically
selected. 

### `hmapedit 0/1` { #hmapedit }

Set to 1 to turn on heightmap mode (default: hold LCTRL or use H to toggle on and off). 
In heightmap mode the cursor will turn bright green when hilighting heightmap cubes and 
the editface command will now use brushes to edit heightmap cubes. In order to avoid 
accidental edits only heightmap cubes are editable in this mode. If a cubic selection 
is present, then only cubes within the column of the selection will be modifiable.

### `hmapselect`

Selects the texture and orientation of the highlighted cube (default: mouse buttons while in 
heightmap mode, or H key). If `hmapselall` is set to 1, then all textures are automatically selected,
and this command will simply select the orientation. All cubes, of equal or larger size,
that match the selection will be considered part of the heightmap.

### `hmapcancel`

Return the heightmap texture selection to default (ie: select all textures).

### `selectbrush D` { #selectbrush }

Switches between the various height map brushes (default: hold B + wheel).

### `clearbrush`

This resets the current brush that is used during heightmap mode editing.

### `brushvert x y depth ` { #brushvert }

A brush is a 2D map that describes the depth that the editface commands should
push into the cubes at various points. The first two parameters of brushvert are the X and Y
coordinates, respectively, of a vert on this 2D map. The last parameter
is used to set the depth. NOTE: if all the brush verts are 0, then a smoothing filter will be 
applied instead of the brush. This filter will affect the same square sized region as the brush.

### `brushx`
### `brushy`

Along with the 2D map, all brushes also have a handle. This handle is a reference
point on the 2D map which defines where the brush is relative to the editing cursor.
These two variables define the brush handle's coordinates.

## Entity

### `newent type value1 value2 value3 value4` { #newent }

Adds a new entity where (x,y) is determined by the current selection (the red
dot corner) and z by the camera height, of said type. Type is a string giving
the type of entity, such as "light", and may optionally take values (depending
on the entity). The types are defines below in the [Entity Types](#entity-types) section.

### `delent`

Deletes the selected entities.

### `entflip`

Flip the selected entities. cube selection serves as both reference point and 
orientation to flip around.

### `entpush D` { #entpush }

Push the selected entities. cube selection serves as orientation to push towards.

### `entrotate D` { #entrotate }

Rotate the selected entities in relation to the cube selection.

### `entmoving 0/1/2` { #entmoving }

Set to 1 to turn on. If an entity is under the cursor when turned on, the
entity will get toggled selected/unselected (set to 2 to add to selection instead of toggle). 
if selected, one can move the
entity around using the cursor. if multiple entities are selected, they will 
also move. the plane on which the entity will be moved is
dependent on the orientation of the cube surrounding the entity. set to 0 to turn off moving.
if no entity is under the cursor when turned on, it will automatically turn off.

### `entdrop N` { #entdrop }

Variable controlling where entities created with [`newent`](#newent) will be placed.
- N=0 places entities at your current eye position.
- N=1 drops entities to the floor beneath you. Lights, however, will be placed at your
  current eye position as for N=0.
- N=2 places entities at the center of the currently selected cube face. If a corner
  is selected, the entity will be placed at the corner vertex.
- N=3 behaves as with N=2, except all entities, including lights, will then be dropped 
  from that position to whatever floor lies beneath. This mode is useful for placing 
  objects on selected floors. Lights are also dropped to the floor, unlike for N=1.

### `dropent`

Positions the selected entity according to the [`entdrop`](#entdrop) variable.

### `trigger T N` { #trigger }

Sets the state of all locked triggers with tag T to N.

### `entselect X` { #entselect }

Takes a boolean expression as argument. Selects all entities that evaluate to true for the given expression.

- `entselect insel`: select all entities in blue selection box
- `entselect [ strcmp (et) "shells" ]`: select all shells in map

### `entloop X` { #entloop }

Loops through and executes the given expression for all selected entities.
Note that most of the entity commands are already vector based and will 
automatically do this. Therefore, they don't need to be explicitly executed within an `entloop`. 
`entloop` is to be used when more precise custom instructions need to be executed on a selection. 

Another property of `entloop` is that entity commands within it, that are normally executed 
on the entire selection, will only be done on the current entity iterator.
In other words, the two following examples are equivalent:

- `entset light 120 0 0 0`
- `entloop [ entset light 120 0 0 0 ]`

The [`entset`](#entset) in the second statement will NOT be applied n squared times. An `entloop` can be nested.

### `entcancel`

Deselect all entities.

### `enthavesel`

Returns the number of entities in the current selection.

### `entget`

Returns a string in the form of "type value1 value2 value3 value4".
This string is the definition of the current selected entity. 
For example, the following statement will display the values of
all the entities within the current selection:

- `entloop [ echo (entget) ]`

Outside an [`entloop`](#entloop), the last entity selected will be returned. 
Normally, if an entity is highlighted by the cursor, it will
only be considered as in the selection if an explicit one does not exist 
(like cubic selections). However, [`entget`](#entget) is special in that it considers
the highlighted entity as being the last entity in the selection.

### `insel`

Returns true if the selected entity is inside the cube selection

### `et`

Cuts out the 'type' field from [`entget`](#entget).

### `ea N` { #ea }

Cuts out the given 'value' field from [`entget`](#entget). Attributes are numbered 0 to 3.

### `entset type value1 value2 value3 value4` { #entset }

Change the type and attributes of the selected entity.
To quickly bring up the `entset` command in the console press '.' (default: period). 
It will come pre-filled with the values of the current entity selection (from [`entget`](#entget)).

### `entproperty P A` { #entproperty }

Changes property P (0..3) of the selected entities by amount A. For example
`entproperty 0 2` when executed near a light entity would increase its radius by 2.

### `entfind type value1 value2 value3 value4` { #entfind }

Select all entities matching given values. '*' and blanks are wildcard.
All entities that match the pattern will be ADDED to the selection.

### `clearents type` { #clearents }

Deletes all entities of said type.

### `replaceents type value1 value2 value3 value4` { #replaceents }

Replaces the given values for the selected entity and all entities that are
equal to the selected entity. 
To quickly bring up the `replaceents` command in the console press ',' (default: comma).
It will come pre-filled with the values of the current entity selection.

### `entautoview N` { #entautoview }

Centers view on selected entity. Increment through selection by N.

For example, N = 1 => next, N = -1 => previous. 

`entautoviewdist N` sets the distance from entity.

## Settings

### `undomegs N` { #undomegs }

Sets the number of megabytes used for the undo buffer (default 8, max 1000).
Undo works for any size areas, so the amount of undo steps per megabyte is
more for small areas than for big ones.

### `showsky B` { #showsky }

This variable controls whether explicit sky polygons are outlined (in purple) in edit mode.

Default = 1.

### `outline B` { #outline }

This variable controls whether geometry boundaries (outlines) are shown.

Default = 0.

### `wireframe 0/1` { #wireframe }

Turns on wireframe drawing of the map.

### `allfaces 0/1` { #allfaces }

When on, causes texture commands to apply the new texture to all sides of the selected cubes
rather than just the selected face.

### `showmat N` { #showmat }

This variable controls whether volumes are shown for invisible material surfaces in edit mode.
Material volumes may also be selected while this is enabled.

If N=2, volumes are shown outside of edit mode. This feature will eventually be limited to 
editing modes and parkour.

Default = 1.

### `optmats 0/1` { #optmats }

This variable controls whether material rendering should be optimized by grouping materials
into the largest possible surfaces. This will always make rendering faster, so the only reason
to disable it is for testing.

Default = 1.

### `entselradius N` { #entselradius }

Sets the 'handle' size of entities when trying to select them. 
Larger sizes means it should be easier to select entities.

### `entselsnap 0/1` { #entselsnap }

Turns on snap-to-grid while dragging entities. (default hotkey: 6)

### `entitysurf 0/1` { #entitysurf }

When on, you will move with the entity as you push it with the scroll wheel. 
Of course, in order to push an entity, you must be holding it.

### `selectionsurf 0/1` { #selectionsurf }

When on, you will move with the selection box as you push it with the scroll wheel. 
Of course, in order to push a selection box, you must be holding it.

## Map and Config

### `map name` { #map }

Loads up map "name" in the gamemode set previously by [`mode`](game.html#mode). A map given as
"blah" refers to "packages/maps/blah.cmr", "mypackage/blah" refers to
"packages/mypackage/blah.cmr". The menu has a set of maps that can be loaded.
See also [`map`](game.html#mode) in the gameplay reference.

At every map load, "data/default_map_settings.cfg" is loaded which sets up all
texture definitions, map models, etc. Everything defined in there can be overridden
per package or per map by creating or editing the "mapname.cfg" which contains
whatever you want to do differently from the default. It can also set up
triggers scripts per map etc.

### `sendmap`

Saves the current map (without lightmaps) and sends it to the server so other clients may download it.
Only works in coop edit game mode.

### `getmap`

Gets a map from the server if one is available. Automatically loads the map when done.
Only works in coop edit game mode.

### `savemap name` { #savemap }
### `savecurrentmap`

Saves the current map, using the same naming scheme as [`map`](#map). Makes a versioned backup to
"mapname_N.BAK" if a map by that name already exists, so you can never accidentally lose a map.
With `savemap`, if you leave out the "name" argument, it is saved under the current map name.
With `savecurrentmap`, the map is saved with the name determined by the current game.

### `newmap size` { #newmap }

Creates a new map of size^2 cubes (on the smallest grid size). 10 is a small
map, 12 is a large map, however it goes up to 16 (not recommended).

### `mapenlarge`

Doubles the dimensions of the current map.

### `maptitle "Title by Author"` { #maptitle }

Sets the map title, which will be displayed when the map loads. Either use the
above format, or simply "by Author" if the map has no particular title (always
displayed after the map load msg).

### `loadsky NAME [SPIN]` { #loadsky }

loads the skybox described by NAME, where NAME is a file name relative
to the "packages/" directory. The engine will look for 6 sky box images:
NAME_up.png, NAME_dn.png, NAME_lf.png, NAME_rt.png, NAME_ft.png, NAME_bk.png.
These represent the skybox in the up, down, left, right, front, and back
directions, respectively. If a .png file is not found, it will attempt to
search for the files with a .jpg extension. SPIN, if specified, is floating point value that
specifies, in degrees per second, the rate at which to spin/yaw the skybox.

NOTE: This is an alias for the "skybox" and "spinsky" commands.

### `texturereset`

Sets the texture slot to 0 for the subsequent "texture" commands.

### `materialreset`

Resets the material texture slots for subsequent "texture" commands.

### `texture TYPE FILENAME ROT X Y SCALE` { #texture }
### `autograss FILENAME` { #autograss }

Automatically generates grass for the current texture slot on any upward facing surface.
The grass texture FILENAME is used to texture the grass blades.

### `grassscale N` { #grassscale }

Sets the scaling of all grass textures to N where N=1..64 (default: 2).

### `grasscolor R G B` { #grasscolor }

The color of the grass, specified as R G B values from 0..255 (default: 255 255 255).

### `grassalpha A` { #grassalpha }

Sets the opacity of all grass textures to A where floating point value A=0..1 (default: 1).

### `texscroll X Y` { #texscroll }

Scrolls the current texture slot at X and Y Hz, along the X and Y axes of the texture respectively.

### `texrotate N` { #texrotate }

Rotates the current texture slot by N\*90 degrees for N=0..3.
N=4 flips along the X axis, and N=5 flips along the Y axis. 

### `texoffset X Y` { #texoffset }

Offsets the current texture slot by X and Y texels along the X and Y axes of the texture respectively.

### `texscale N` { #texscale }

Scales the current texture slot such that it is N times its normal size.

### `texalpha F B` { #texalpha }

Sets the alpha transparency of the front faces to F and back faces to B,
where F and B are floating point values in the range 0.0 to 1.0.
F defaults to 0.5, and B defaults to 0 (invisible).

### `texcolor R G B` { #texcolor }

Sets the color multiplier of the current texture slot to the color R G B,
where R, G, and B are floating point values in the range 0.0 to 1.0.
The default is white, i.e. (1.0 1.0 1.0).

### `vrotate N` { #vrotate }

Rotates all textures in the current selection as if by the [`texrotate`](#texrotate) command.

### `voffset X Y` { #voffset }

Offsets all textures in the current selection as if by the [`texoffset`](#texoffset) command.

### `vscale N` { #vscale }

Scales all textures in the current selection as if by the [`texscale`](#texscale) command.

### `valpha F B` { #valpha }

Sets the alpha transparency of all textures in the current selection as if by the [`texalpha`](#texalpha) command.

### `vcolor R G B` { #vcolor }

Sets the color multiplier of all textures in the current selection as if by the [`texcolor`](#texcolor) command.

### `vreset`

Resets the texture configuration of all textures in the current selection to the defaults defined in the map cfg file.

### `vdelta BODY` { #vdelta }

Executes all the "v\*" commands in BODY such that they now only add to the current values
for the textures in the current selection, rather than simply setting them. 
For example, `vdelta [vrotate 1]` would add 1 to the current rotation value for the textures,
rather than just setting their rotation value to 1. This affects the [`vrotate`](#vrotate) (adds),
[`voffset`](#voffset) (adds), [`vscale`](#vscale) (multiplies),
[`vshaderparam`](#vshaderparam) (overrides), and [`vcolor`](#vcolor) (multiplies) commands.

### `fog N` { #fog }

Sets fog distance to N (default: 4000). You can do this for tweaking the visual effect of the fog,
or if you are on a slow machine, setting the fog to a low value can also be a very effective way
to increase fps (if you are geometry limited). Try out different values on big maps or maps
which give you low fps.

### `fogcolor R G B` { #fogcolor }

The color of the fog, specified as R G B values from 0..255 (default: 128 153 179).

### `waterspec N` { #waterspec }

This sets the percentage of light water shows as specularity (default: 150).

### `waterfog N` { #waterfog }

Sets the distance beneath the surface of water at which it fogs, from 1..10000 (default: 150).

### `watercolor R G B` { #watercolor }

Sets the color of fog inside the water to the specified R G B value from 0..255 (default: 20 70 80). Used to give water some color. Setting to the values 0 0 0 will cause it to reset to the default watercolor.

### `waterfallcolor R G B` { #waterfallcolor }

Sets the color used for waterfall turbulence to the specified R G B value from 0..255 (default: 0 0 0). If the default of 0 0 0 is specified, waterfalls will use the color supplied via "watercolor" instead.

### `lavafog N` { #lavafog }

Sets the distance beneath the surface of lava at which it fogs, from 1..10000 (default: 50).

### `lavacolor R G B` { #lavacolor }

Sets the color of fog inside the lava to the specified R G B value from 0..255 (default: 255 64 0). Used to give lava some color. Setting to the values 0 0 0 will cause it to reset to the default watercolor.

### `shader TYPE NAME VS PS` { #shader }

Defines a shader NAME with vertex shader VS and pixel shader PS (both in ARB OpenGL 1.5 assembly format).
See data/stdshader.cfg for examples. These definitions can be put in map cfg files or anywhere else,
and will only be compiled once. TYPE indicates what resources the shader provides, or what backup method
should be used if the graphics card does not support shaders. TYPE is either 0 for default shader,
or 1 for normal-mapped world shaders. Requires DX9 / shader 2 class hardware
(Radeon 9500 or better, GeForce 5200 or better) to run (older hardware will default to basic rendering).

### `fastshader NICE FAST N` { #fastshader }

Associates shader FAST so that it will run in place of shader NICE if `shaderdetail` is less than or equal to N.

### `setshader NAME` { #setshader }

Sets a previously defined shader as the current shader. Any following texture slots 
(see [`texture`](#texture) command) will have this shader attached to them. Any pixel
or vertex parameters are reset to the shader's defaults when this command is used.

### `setshaderparam NAME X Y Z W` { #setshaderparam }

Overrides a uniform parameter for the current shader. Any following texture slots will
use this pixel parameter until its value is set/reset by subsequent commands. NAME is
the name of a defined parameter of the current shader. Its value is set to the vector
(X, Y, Z, W). Coordinates that are not specified default to 0.

### `vshaderparam NAME X Y Z W` { #vshaderparam }

Overrides a uniform parameter for the shaders of all textures in the current selection,
as if by the [`setshaderparam`](#setshaderparam) command.

### `setpixelparam INDEX X Y Z W`  { #setpixelparam }

Overrides a pixel parameter for the current shader. Any following texture slots will use
this pixel parameter until its value is set/reset by subsequent commands. INDEX is the
index of a program environment parameter (`program.env[10+INDEX]`) to the pixel program of
the current shader. Its value is set to the vector (X, Y, Z, W).
Coordinates that are not specified default to 0.

### `setvertexparam INDEX X Y Z W` { #setvertexparam }

Overrides a vertex parameter for the current shader. Any following texture slots will use
this vertex parameter until its value is set/reset by subsequent commands. INDEX is the
index of a program environment parameter (`program.env[10+INDEX]`) to the vertex program of
the current shader. Its value is set to the vector (X, Y, Z, W).
Coordinates that are not specified default to 0.

### `setuniformparam NAME X Y Z W` { #setuniformparam }

Overrides a uniform parameter for the current shader. Any following texture slots will use
this pixel parameter until its value is set/reset by subsequent commands. NAME is the name
of a uniform variable in the current GLSL shader. Its value is set to the vector
(X, Y, Z, W). Coordinates that are not specified default to 0.

### `music name [ondone]` { #music }

Plays song `name` (with "packages" as base dir).
This command is best used from map cfg files or triggers. Evaluates ondone 
when the song is finished, or just keeps looping the song if ondone is missing.
Example: music "songs/music.ogg" [ echo "Song done playing!" ]

### `N = registersound name V` { #registersound }

Registers sound "name" (see for example data/sounds.cfg). This
command returns the sound number N, which is assigned from 0 onwards, and which
can be used with "sound" command below. if the sound was already registered,
its existing index is returned. registersound does not actually load the sound,
this is done on first play. V is volume adjustment; if not specified (0), it is the default 100, valid
range is 1-255.

### `sound N` { #sound }

Plays sound N, see data/sounds.cfg for default sounds, and use registersound to
register your own. for example, sound 0 and sound (registersound
"aard/jump") both play the standard jump sound.

### `mapsound name V N` { #mapsound }

Registers sound "name" as a map-specific sounds. These map-specific sounds may currently only
be used with "sound" entities within a map. The first map sound registered in a map has index 0, 
and increases afterwards (the second sound in the config is 1 and the third is 2, and so on).
V is volume adjustment; if not specified (0), it is the default 100, valid
range is 1-255. N is the maximum number instances of this sound that are allowed to play simultaneously;
the default is only 1 instance. If N is -1, then an unlimited number of instances may play simultaneously.
### `mmodel N` { #mmodel }

Registers a mapmodel that can be placed in maps using newent
mapmodel (see newent). N is the name of a folder inside packages/models folder, e.g.
"hudguns/rifle". Loaded from that folder are: tris.md2 and skin.png (and if not available,
skin.jpg, or the same from the parent folder to allow skin sharing). set additional properties of a mapmodel in its .cfg

### `shadowmapambient N` { #shadowmapambient }

Specifies a color to use for the ambient light value of shadows created by shadowmapping, where N
is a hexadecimal color value of the form "0xRRGGBB". Note that any value of 255 or less are treated
as gray-scale. If N is 0 or unset, this value is determined by the "ambient" variable and the "skylight"
command. (Default: 0)

### `shadowmapangle N` { #shadowmapangle }

Specifies the angle in degrees at which shadows created by shadowmapping point. If N is 0 or unset,
this value is guessed based on any radius 0 lights in the map.

### `causticscale N` { #causticscale }

Specifies the scale, as a percent, to multiply the size of water caustics by.

### `causticmillis N` { #causticmillis }

Specifies the speed at which water caustics play, in milliseconds per frame.

### `skybox NAME` { #skybox }

Loads the skybox described by NAME, where NAME is a file name relative
to the "packages/" directory. The engine will look for 6 sky box images:
NAME_up.png, NAME_dn.png, NAME_lf.png, NAME_rt.png, NAME_ft.png, NAME_bk.png.
These represent the skybox in the up, down, left, right, front, and back
directions, respectively. If a .png file is not found, it will attempt to
search for the files with a .jpg extension.

### `spinsky SPIN` { #spinsky }

A floating point value that specifies, in degrees per second, the rate at which to 
spin/yaw the skybox.

### `yawsky YAW` { #yawsky }

Specifies in degrees a constant yaw rotation to apply
to the skybox.

### `cloudbox NAME` { #cloudbox }

Loads the cloudbox described by NAME, similar to the "skybox" command. The cloudbox
should have an alpha channel which is used to blend it over the normal skybox.

### `spinclouds SPIN` { #spinclouds }

A floating point value that specifies, in degrees per second, the rate at which to 
spin/yaw the cloudbox.

### `yawclouds YAW` { #yawclouds }

Specifies in degrees a constant yaw rotation to apply
to the cloudbox.

### `cloudclip CLIP` { #cloudclip }

Specifies a vertical offset at which to clip the cloudbox, a floating point value between 0 and 1.
This defaults to 0.5, meaning the bottom half of the cloudbox is clipped away.

### `cloudlayer NAME` { #cloudlayer }

Loads the cloud layer described by NAME, where NAME is a file name relative
to the "packages/" directory. The engine will look for either "packages/NAME.png"
or "packages/NAME.jpg".  The cloud layer should have an alpha channel which is used
to blend it onto the skybox. The cloud layer is mapped onto a horizontal circle 
that fades into the edges of the skybox.

### `cloudscrollx N` { #cloudscrollx }

Specifies the rate, a floating-point value in Hz, at which the cloud layer scrolls in the X direction.

### `cloudscrolly N` { #cloudscrolly }

Specifies the rate, a floating-point value in Hz, at which the cloud layer scrolls in the Y direction.

### `cloudscale N` { #cloudscale }

Specifies the scale as a floating-point value telling how much to multiply the size of the cloud layer.
(Default: 1)

### `cloudheight N` { #cloudheight }

Specifies the height of the cloud layer as a floating-point value, where -1 corresponds to the bottom of
the skybox, 0 corresponds to the middle of the skybox, and 1 corresponds to the top of the skybox. Intermediate
values place the cloud layer at intermediate heights of those. (Default: 0.2)

### `cloudfade N` { #cloudfade }

Specifies the offset towards the center of the cloud layer at which the cloud layer will start fading into the skybox.
This is a floating-point value between 0 and 1, where 0 corresponds to the edge of the cloud layer, and 1 corresponds
to the center. (Default: 0.2)

### `cloudcolor N` { #cloudcolor }

Specifies a color multiplier for the cloud layer, where N is a hexadecimal color value in the form of "0xRRGGBB".
(Default: 0xFFFFFF, white)

### `cloudalpha A` { #cloudalpha }

Specifies an opacity for the cloud layer, where A is a floating-point value between 0 and 1.
(Default: 1, solid)

### `fogdomeheight N` { #fogdomeheight }

Specifies the height of the fog dome as a floating-point value, where -1 corresponds to the bottom of
the skybox, 0 corresponds to the middle of the skybox, and 1 corresponds to the top of the skybox. Intermediate
values place the fog dome at intermediate heights of those. (Default: -0.5)

### `fogdomemin A` { #fogdomemin }

Specifies a minimum opacity for the fog dome, where A is a floating-point value between 0 and 1.
(Default: 0, invisible)

### `fogdomemax A` { #fogdomemax }

Specifies a maximum opacity for the fog dome, where A is a floating-point value between 0 and 1.
(Default: 0, invisible)

### `fogdomecap B` { #fogdomecap }

Specifies whether the bottom of the fog dome should be capped, where B is 0 or 1 (Default: 1, on).

### `fogdomeclip Z` { #fogdomeclip }

Specifies whether the top of the fog dome should be clipped off at a relative size Z, where Z is a floating-point value between 0 and 1 (Default: 1, not clipped).

### `fogdomecolor R G B` { #fogdomecolor }

The color of the fog dome, specified as R G B values from 0..255 (default: 0 0 0). If the color is 0, then the value of fogcolor is used instead of fogdomecolor.

### `skytexture B` { #skytexture }

Specifies whether or not to enable rendering of sky-textured surfaces. If set to 0, sky-textured surfaces are not rendered,
allowing sky texture to be used as a "don't render this" surface. Disabling this also allows the skybox to be rendered last
after the scene, which yields speedups on some video cards, so disable this if possible in your map, even though it defaults
to on. (Default: 1)

### `writeobj N` { #writeobj }

Writes out the current map as N.obj, so you could use the engine as a generic modeller
with any program/engine that uses meshes. The meshes aren't very optimal and don't
have texture/lighting information.

### `flipnormalmapy D N` { #flipnormalmapy }

Normalmaps generally come in two kinds, left-handed or righ-handed coordinate systems. If you are
trying to use normalmaps authored for other engines, you may find that the lighting goes the wrong
way along one axis, this can be fixed by flipping the Y coordinate of the normal. This command loads normalmap
N (MUST be 24bit .tga), and writes out a flipped normalmap as D (also tga).

### `mergenormalmaps H N` { #mergenormalmaps }

Normalmaps authored for Quake 4 often come as a base normal map, with seperate height offset file *_h.tga.
This is NOT a height file as used for parallax, instead its detail to be blended onto the normals.
This command takes normalmap N and a _h file H (both MUST be 24bit .tga), and outputs a combined normalmap N
(it *overwrites* N).  

## Lighting

### `ambient R [G B]` { #ambient }

This sets the level of ambient light (default: 25), where R G B are color values 0..255. If only R is specified, it is interpreted as a grayscale light value. This is the minimum amount of light that a surface will get, even when no light entities reach the surface.

### `edgetolerance N` { #edgetolerance }

This controls how far an intersection with some geometry has to be from the surface of the actual triangle in question before it counts as a shadow (default: 4), where N is 1..8; the distance immediately in front of the triangle along the edge where it ignores shadows.


When lighting a map, each lightmap pixel is the result of many samples from a grid-pattern, as a result the sampling pattern may go over the edge of one triangle but behind another. Increasing this variable helps alleviate this, especially in terrain heavy maps.
 
### `sunlight R [G B]` { #sunlight }

This sets the color of a simple directional sunlight. R G B are values in the range 0..255 (default: 0). If only R, is specified it is interpreted as a grayscale light value. Use "sunlight 0" to disable.

### `sunlightyaw YAW` { #sunlightyaw }

This sets the yaw of the directional sunlight to YAW.

### `sunlightpitch PITCH` { #sunlightpitch }

This sets the pitch of the directional sunlight to PITCH.

### `sunlightscale F` { #sunlightscale }

This sets the color scale of the direction sunlight to the floating-point value F (default: 1.0).

### `skylight R [G B]` { #skylight }

This enables the skybox to "emit" light, that can be occluded by geometry or models in the map. A surface will cast
a number of rays (currently 17), and any of them that hit the skybox will contribute a portion of the R G B 
light value above the "ambient" level to the surface (1/17th of the light). Effectively, the light will vary between 
the "ambient" value and the "skylight" value depending on how much of the skybox is visible. R G B are values in the 
range 0..255 (default: 0). If only R, is specified it is interpreted as a grayscale light value. Use "skylight 0" to disable.

### `lmshadows N` { #lmshadows }

This controls the level of shadowing used when "calclight" or "patchlight" are not given a quality setting, where N is:

### `lmaa N` { #lmaa }

This controls the level of anti-aliasing used when "calclight" or "patchlight" are not given a quality setting, where N is:

### `calclight Q` { #calclight }

This calculates all lightmaps. Usually takes only a few seconds, depending on
map size and settings. If you "savemap", the lightmap will be stored along with
it. Q is these predefined quality settings:

### `patchlight Q` { #patchlight }

This will calculate lightmaps for any newly created cubes. This will generally
be much quicker than doing a "calclight", and so is very useful when editing.
However, it will make very inefficient use of lightmap textures, and any new
cubes will not properly cast shadows on surfaces that are already lit. It is
recommended you do a "calclight" on your map before you publish it with
"savemap". A quality setting Q may be supplied, which behaves the same as for
"calclight".

### `lightthreads N` { #lightthreads }

This controls the number of threads (N) used by the "calclight" and "patchlight" commands
You should set this variable to the number of processor cores you have to get a speed-up. 
By default (N=1), no multi-threading is used. 

### `fullbright B` { #fullbright }

This variable controls whether the map will be shown with lighting disabled.
Fullbright 1 will disable lighting, whereas 0 will enable lighting. (Default =
0)

### `lerpangle A` { #lerpangle }

Default = 44. This variable controls whether surface normals are interpolated for
lighting. Normals are sampled at each vertex of the surface. If the angle between
two surfaces' normals (which meet at a vertex) is less than A, then the resulting
normal will be the average of the two. Normals are then later interpolated between
the normals at the vertexes of a surface.

### `lerpsubdiv N` { #lerpsubdiv }

Default = 2. This allows more normals to be sampled at points along an edge between
two vertexes of a surface. 2^N-1 extra normals will be sampled along the edge, i.e.
the edge is split in half for every increment of N.

### `lerpsubdivsize N` { #lerpsubdivsize }

Default = 4. This sets the minimum size to which an edge may be subdivided. Edges
smaller than N or edge sections smaller than N will not be sampled.

### `lightprecision P` { #lightprecision }

Default = 32. This is the most important variable for tweaking the lighting, it
determines what the resolution of the lightmap is. As such has a BIG effect on
calculation time, video memory usage, and map file size. The default is good
for most maps, you may go as low as 16 if you are lighting a really small map
and love hard shadows, and for bigger maps you may need to set it to 64 or so
to get reasonable memory usage.

The number to watch out for is the number of lightmaps generated which are
shown on the HUD (and also as output after a calclight). 1 or 2 lightmap
textures is very good, over 10 lightmap textures is excessive.

The map file size is 90% determined by the lightmaps, so tweak this value
to get an acceptable quality to size ratio. Look at the size of the map files,
sometimes a slightly higher lightprecision can halve the size of your .cmr.

Every surface matters, even though the engine attempts to compress surfaces
with a uniform lightvalue, it is always a good ideas to delete parts of the
world that are not part of your map. lightprecision, lighterror, and lightlod
are stored as part of map files.

### `lighterror E` { #lighterror }

There should be little reason to tweak this. If in your map you can see visible
polygon boundaries caused by lighting, you can try stepping this down to 6 or 4
to improve quality at the expense of lightmap space. If you have an insanely
large map and looking for ways to reduce file size, increasing error up to 16
may help. (Default = 8)

### `lightlod D` { #lightlod }

Default = 0. This will double the resolution of lightmaps (cut the
lightprecision in half) if size of the surface being lit is smaller than 2^D
units. This allows large maps to have pockets of detailed lighting without
using a high resolution over everything.

NOTE: If you feel like using this, test it thoroughly. On medium or small sized
detailed maps, this command wastes space, use lightlod 0. Lightlod > 0 is
only useful for huge maps

### `blurlms N` { #blurlms }

This variable controls whether to apply a blur filter to the lightmap after
they are generated, as a post-pass. For N=0, no blur is applied. For N=1, a
3x3 blur filter is used. For N=2, a 5x5 blur filter is used. 

NOTE: This can cause lightmaps to mismatch at surface boundaries in complex scenes.

### `blurskylight N` { #blurskylight }

This variable controls whether to apply a blur filter to the ambient skylight
(enabled via the "skylight" command) before it is combined with the other 
lighting to create the final lightmap. This variable is useful for softening
the skylight and making it appear more scattered. For N=0, no blur is applied. 
For N=1, a 3x3 blur filter is used. For N=2, a 5x5 blur filter is used.

### `dumplms`

Dumps all lightmaps to a set of .bmps. Mostly interesting for developers, but
mappers may find it interesting too.

## Texture Blending

Texture blending in maps is accomplished by smoothly blending two textures together to create variations on architecture and terrain.


Texture blending can use any two textures that are already set in your map.cfg, but you need to define the textures normally first. Texture thumbnails that have an additional texture layer to blend will have the texture they are set to blend with in one of the corners in the texture browser (F2).

To set up textures to blend, in your map cfg, under the texture that you want to have a second texture layer, you would add:

### `texlayer N` { #texlayer }

N is the index of the texture slot you want to use as the bottom texture layer to blend with. Texture slots start at 0, which is the first slot, 1 is the second slot, 2 is the third slot, etc. If N is a negative number, it will reference N slots back from the current slot, i.e. -1 references the previous texture slot.

### `vlayer N` { #vlayer }

Sets the bottom texture layer for all textures in the current selection, as if by "texlayer" command. However, negative values of N are not supported.


There will be a number of brushes already included and set up in the default "data/brush.cfg". If you want to set up your own brush, create a grayscale PNG file to use as a pattern. Keep in mind that the size of the brushes are relatively large, and not meant for super detailed work. So a 16 x 16 size image will make a brush that covers a very large patch of architecture.

### `addblendbrush N F` { #addblendbrush }

Where N is the name of your brush, that you will use later to load the brush, and F is the file name, relative to the data directory. So if the brush you want to add is called "mybrush.png", then you would add the following line to your map.cfg file: addblendbrush mybrush "mybrush.png"". Note that the filename of the brush image is relative to the root directory.

### `setblendbrush N` { #setblendbrush }

Where N is the name of your brush that you defined in the cfg, or the name of an existing brush. So to set the brush to the one that was defined above, you would open the console with the tilde key and type: /setblendbrush mybrush

### `nextblendbrush N` { #nextblendbrush }

Selects the next brush after the current one if N is not specified, or is 1. Otherwise, it will advance the current brush selected by N over the list of brushes. N may be negative, in which case it will go backwards in the list of brushes. By default this is bound to the scroll wheel.

### `rotateblendbrush`

Rotates the current blend brush. By default this is bound to MOUSE2 (right mouse button).


First, texture the surfaces that you will want to paint with the textures that have had an extra texture layer defined for them in your map.cfg. You can only paint on textures that have had a second layer defined in the cfg. Now, you need to turn on a painting mode with the following command:

### `blendpaintmode N` { #blendpaintmode }

Where N is a number from 0 to 5, which defines how the painting will be done. Setting it to 0 turns paint mode off. Usually you should paint with mode 2. By default each of these modes are bound to their corresponding number on the numeric keypad.


Experiment with the numbers to understand what they do. Finally, in order to paint on to the surface, you would open the console and type

### `paintblendmap`

If you've done everything right, you should see your first blended textures. By default this is bound to MOUSE1 (left mouse button).

### `showblendmap`

If for some reason the blendmap gets messed up while editing, you can use this command cause the blendmap to reshow without doing a full calclight.

There is NO UNDO for texture blending. If you've screwed something up, you can do one of two things. Open the console with the tilde key and type

### `clearblendmap`

This will delete ALL of the texture blending for the ENTIRE level.

### `clearblendmapsel`

This will clear the texture blending on the selected geometry.


The amount of rendering passes the engine is required to make doubles on areas where the textures have been blended.

## Entity Types

Entities are shown in editmode by blue sparklies, and the closest one is
indicated on the HUD.

### `"light" radius r g b` { #ent-light }

If G and B are 0 the R value will be taken as brightness for a white light. A
good radius for a small wall light is 64, for a middle sized room 128... for a
sun probably more like 1000. Lights with a radius of 0 do not attenuate and may
be more appropriate for simulating sunlight or ambient light; however, this
comes at the cost of slightly greater map file sizes. See the 
lighting commands for an indepth list of all lighting related commands.

### `"spotlight" radius` { #ent-spotlight }

Creates a spotlight with the given "radius" (in degrees, 0 to 90). A 90 degree spotlight will
be a full hemisphere, whereas 0 degrees is simply a line. These will attach to the nearest "light"
entity within 100 units of the spotlight. The spotlight will shine in the direction of the spotlight,
relative to the "light" entity it is attached to. It inherits the sphere of influence (length of the spotlight) and 
color values from the attached light as well. Do not move these very far from the light they're or attached to or
you risk them detaching or attaching to the wrong lights on a map load!

### `"envmap" [radius]` { #ent-envmap }

Creates an environment map reflecting the geometry around the entity. The optional radius
overrides the maximum distance within which glass or geometry using the "bumpenv*" shaders will reflect from this environment map. If none is specified,
the default is taken from the variable "envmapradius" (which defaults to 128 units), which may also be set in 
map cfgs. Environment maps are generated on a map load, or can be regenerated while editing using the "recalc" command.

Please use the absolute minimum number of these possible. Each one uses up a decent amount of texture memory. 
For instance, rather than using two environment maps on each side of a window, use only one in the middle of the pane
of glass. If you have a wall with many windows, place only one environment map in the middle of the wall geometry, and 
it should work just fine for all the windows.

### `"sound" N radius [size]` { #ent-sound }

Will play map-specific sound N so long as the player is within the radius. However, only up to the max uses allowed for N (specified in the mapsound command) will play, even if the player is within the radius of more N sounds than the max. 

By default (size 0), the sound is a point source. Its volume is maximal at the entity's location, and tapers off to 0 at the radius. If size is specified, the volume is maximal within the specified size, and only starts tapering once outside this distance. Radius is always defined as distance from the entity's location, so a size greater than or equal to the radius will just make a sound that is always max volume within the radius, and off outside.

### `"playerstart" [Y] [T]` { #ent-playerstart }

Spawn spot, yaw Y is taken from the current camera yaw (should not be explicitly specified to "newent"). If T is specified, then the playerstart is used as a team-spawn for CTF modes ONLY, where team T may be either 1 or 2, matching the parameter supplied to "flag" entities. For all other modes, team T should either be 0 or simply not specified at all! Note that normal playerstarts are never used for CTF, and CTF playerstarts are never used for spawns in other modes.

### `"flag" [Y] T` { #ent-flag }

A team flag for CTF maps ONLY. Yaw Y is taken from the current camera yaw (should not be explicitly specified to "newent"). Team T may be either 1 or 2. Playerstarts with a matching team will be chosen in CTF mode. 

### `"base" [ammo [N]]` { #ent-base }

A base for capture mode. If N is specified, the alias "base_N" will be looked up, and its value used for the name of the base, or otherwise a default name will be assigned. If ammo is specified, the base will always produce that type of ammo. If ammo is unspecified or 0, the server will randomly choose a type of ammo to produce at the start of the match. If ammo is negative, then it will pick a random type, but will match all other bases with the same negative ammo value. Ammo types are:

### `"ammo"` { #ent-ammo }
### `"health"` { #ent-health }

A variety of pickup-able items, see here.

### `"teleport" N [M]` { #ent-teleport }
### `"teledest" [Y] N [P]` { #ent-teledest }

Creates a teleport connection, teleports are linked to a teledest with the same
N (of which there should be exactly one). N can be 0..255. Y is the yaw of the destination and is
initially taken from the current camera yaw, it cannot be specified when creating the entity. If M
is 0 or not specified, the default teleporter model is used. If M is -1, no model is rendered for
the teleporter. If M is a value 1 or greater, the corresponding mapmodel slot is used as the teleporter's
model. If P is 1, the teledest preserves the velocity of the player, otherwise the player's velocity is
reset upon teleporting.

### `"jumppad" Z [Y] [X]` { #ent-jumppad }

A jumppad entity which gives you a push in the direction specified. For example, "jumppad 30 5" makes
you bounce up quite a bit and also pushes you forward a bit (so it is easier to land on a higher platform).
This entity does not render anything, so you are responsible for creating something that looks logical below
this entity.

### `"mapmodel" [Y] N T R` { #ent-mapmodel }

A map model, i.e. an object rendered as md2/md3 which you collide against, cast
shadows etc. Y is the yaw of the model and is initially taken from the current camera yaw,
it cannot be specified when creating the entity. N determines which mapmodel you want, this
depends on "mapmodel" declarations in the maps cfg file. T specifies mapmodel behaviour such
as triggers, see table below. R is the trigger number, 0 means no trigger. This number specifies
what trigger to activate, and in addition, the alias
"level_trigger_Trigger"
will be executed, where Trigger is substituted accordingly (this allows you to script
additional actions upon a trigger, i.e. put this into your map cfg file to print
a message: alias level_trigger_1 "echo A door opened nearby"). The alias
"triggerstate" will hold a value of -1, 0, or 1 indicating how the trigger was activated.


Be careful when using "switch many" for thing that affect gameplay, such as opening
doors, as it can be confusing. Best is to reserve a particular model to mean "many"
and others "once". All types >0 are snapped to 15 degree angles for orientation.

### `"box" [Y] N W` { #ent-box }
### `"barrel" [Y] N W H` { #ent-barrel }

Like a mapmodel, except that damage done to it will push the entity around. Y is the yaw of the model and is initially taken from the current camera yaw, it cannot be specified when creating the entity. N determines which mapmodel you want, this depends on "mapmodel" declarations in the maps cfg file. W is the weight of the box or barrel where the heavier it is the less it moves; if W is not specified or 0, it defaults to 25. Barrels, unlike boxes, will explode if more than H damage is done to them; if H is not specified or 0, it defaults to 50.

### `"platform" [Y] N T S` { #ent-platform }
### `"elevator" [Y] N T S` { #ent-elevator }

Like a mapmodel, except it moves around and carries players, or other dynamic entities. Y is the yaw of the model and is initially taken from the current camera yaw, it cannot be specified when creating the entity. N determines which mapmodel you want, this depends on "mapmodel" declarations in the maps cfg file. A platform will travel horizontally back and forth along the direction of its yaw, while an elevator will travel only up and down. When they hit an obstacle such as geometry, they will reverse their direction. T is a tag that may be used to start or stop the elevator with the "platform" command; if a non-zero tag is specified, then the platform or elevator will stop upon hitting an obstacle, rather than reversing direction. S is the speed at which the entity moves, in units per second; if S is not specified or 0, it defaults to to 8.

### `"respawnpoint"` { #ent-respawnpoint }

A respawnpoint for classic SP mode (see "SP Respawning"), when the player dies, they will repsawn at the last
one of these they touched, otherwise they start at the playerstart entity.

### `"particles" type value1 value2 value3 value4` { #ent-particles }

A particle emitter. Particles includes many of the effects as seen for weapons, explosions, and lens flares.

## PVS Culling
Cardboard Engine provides a precomputed visibility culling system as described in the technical paper "Conservative Volumetric Visibility with Occluder Fusion" by Schaufler et al (see paper for technical details). Basically, it divides the world into small cube-shaped "view cells" of empty space that the player might possibly occupy, and for each of these view cells calculates what other parts of the octree might be visible from it. Since this is calculated ahead of time, the engine can cheaply look up at runtime whether some part of the octree is possibly visible from the player's current view cell. Once pre-calculated, this PVS (potential visibility set) data is stored within your map and saved along with it, so that it may be reused during gameplay. This data is only valid for a particular map/octree, and if you change your map, you must recalculate it or otherwise expect culling errors. It is recommended you do this only after you are sure you are finished working on your map and ready to release it, as it can take a very long time to compute this data. If you have a multi-core processor or multi-processor system, it can use multiple threads to speed up the pre-calculation (essentially N processors/cores will calculate N times faster).

The number of pre-calculated view cells stored with your map will show up in the edit HUD stats under the "pvs:" stat. It is recommended you keep this number to less than 10,000, or otherwise the amount of storage used for the PVS data in your map can become excessive. For very large SP maps, up to 15,000 view cells is acceptable. The number of view cells is best controlled by use of the "clip" material, or by setting the view cell size (default is 32, equal to a gridpower 5 cube). View cell sizes of 64 or 128 are worth trying if your map still has an excessive number of view cells, but try to use the default view cell size of 32 if it stays reasonable. Note that if you have a map with a lot of open space, there will be a lot of view cells, and so the initial pre-calculation may take a long time. You can use the "clip" material, if necessary, to mark empty space the player can't go into, and the PVS calculation will skip computing view cells for these areas. Filling places the player can't go with solid cubes/sealing the map will similarly reduce the number of possible view cells.

Visibility from a view cell, to some other part of the octree, is determined by looking for large square or block-shaped surfaces and seeing if they block the view from the view cell to each part of the octree. So surfaces like large walls, ceilings, solid buildings, or even mountains and hills, that have large solid cross-sections to them will make the best occluders, and allow the PVS system to cull away large chunks of the octree that are behind them, with respect to the current view cell. Avoid putting holes running entirely through these structures, or this will prevent large cross-section of them from being used as an occluder (since the player could possibly see through them). You can use the "testpvs" command to check how well your occluders are working while building them. If your map is an open arena-style map, then using the PVS system will have little to no effect, since few things are blocking visibility, and it is not worth using the PVS system for such maps.

Note that there is already an occlusion culling system based on hardware occlusion queries, in addition to the PVS system, so the main function of the PVS system is to provide occlusion culling for older 3D hardware that does not support occlusion queries, and also to speed up occlusion queries by reducing the amount of such queries (which can be expensive themselves) even for 3D hardware that supports them. If PVS is used effectively (a map with lots of good occluders), it should always provide some speed-up regardless of whether or not the 3D hardware supports occlusion queries. However, if you are doing open arena-style maps for which there are few good occluders, then it is recommended you skip using the PVS system (as it will just take up memory without providing a speedup) and rely upon the hardware occlusion queries instead.

### `pvs N` { #pvs }

Toggles PVS culling, where N=1 enables it, and N=0 disables it. This is mostly useful for testing the performance effect of the PVS system and should usually be left on.

### `pvsthreads N`{ #pvsthreads }

Sets the number of threads (N) that will be used for calculating PVS info with the "genpvs" command. By default, only 1 thread (N=1) is used. If you have N processor or N processor cores, then set this variable to N to make the pre-calculation effectively N times faster. Setting this variable higher than the number of processors/cores will not make it any faster, but setting it lower will not utilize all of them, so try and set it to the exact number.

### `genpvs [N]`{ #genpvs }

Pre-calculates PVS data for the current version of the map. N is the size of the view cell used for calculation. If N is not specified or 0, then the default view cell size of 32 is used. Try to always use the default view cell size where reasonable.

### `clearpvs`

Clears the PVS data for the map. Use this to clear away stale PVS data if you are editing a map for which PVS data was already pre-calculated to avoid culling errors (i.e. stuff being invisible that should otherwise be visible).

### `lockpvs N` { #lockpvs }

If N=1, this locks the view cell used by the PVS culling to the current view cell, even if you happen to move outside of it. Everything that was occluded/invisible from that view cell will still be so, even if you move outside of it. This is useful for seeing all the things that are being culled from a current vantage point by the PVS system. If N=0, the view cell is unlocked and PVS will function as normal again.

### `testpvs [N]` { #testpvs }

Generates PVS data for only the current view cell you are inside (of size N, or default 32 if not specified) and locks the view cell to it as if "lockpvs 1" were used. This allows you to quickly test the effectiveness of occlusion in your map without generating full PVS data, so that you can more easily optimize your map for PVS before the actual expensive pre-calculation is done. Use "lockpvs 0" to release the lock on the view cell when you are done testing. Note that this will not overwrite any existing PVS data already calculated for the map. 

### `pvsstats`

Prints out some useful info about the PVS data stored with the map, such as the number of view cells, the total amount of storage used for all the view cells, and the average amount of storage used for each individual view cell.

## Waypoints
Cardboard Engine provides waypoints to aid bot navigation, and without these bots are not able to calculate safe paths from place to place on the map.

Waypoints are entities in a map that tell where it is safe for bots to move to. Each waypoint can link to other waypoints, telling a bot how to get from one waypoint to another. To set up waypoints, first either add a bot (which causes you to drop waypoints as a side-effect) or set the "dropwaypoints" variable to 1. Run around the level through all valid paths, making sure to run through teleporters and touch all items and playerstarts. Waypoints should be dropped at ground level in an even grid across the map, i.e. only jump where it is necesssary to jump. Avoid any movement tricks like weapon jumping that might confuse the AI. 

Set the "showwaypoints" variable to 1 so you can see the waypoints you are dropping, and you will see blue lines representing the links between these waypoints. When you are done, use the "savewaypoints" command to save the waypoints for your map; they will be saved to a file named "yourmap.wpt" for a map named "yourmap" in the same directory as your map. It also helps to add some bots before you save and make sure there are no bots stuck at playerstarts without waypoints to guide them.

Note waypoints are not loaded until required, i.e. until a "loadwaypoints" or "addbot" command is issued.

### `showwaypoints 0/1` { #showwaypoints }

Toggles showing of waypoints, where 1 enables it, and 0 disables it. This is mostly useful when laying waypoints so as to see the possible paths and ensure good coverage. 

### `dropwaypoints 0/1` { #dropwaypoints }

Toggles dropping of waypoints, where 1 enables it, and 0 disables it.  By default the player is dropping waypoints whilst playing against bots, this enables bots to "learn" from the player. Note that if enabled, waypoints will be saved automatically once the map is changed, and the variable will then be reset back to 0.

### `loadwaypoints [filename]` { #loadwaypoints }

Loads the waypoints for the current map (or specified file).

### `savewaypoints [filename]` { #savewaypoints }

Saves the waypoints for the current map (or specified file), e.g. as "<mapname>.wpt".

### `clearwaypoints`

Removes all waypoints.

### `delselwaypoints`

In editing mode this will remove waypoints within the selection region.

## The HUD

On the bottom left of the screen are a bunch of stats. You'll find out what they mean below.

### `hidestats 0/1` { #hidestats }

Turn on to hide the above stats

### `hidehud 0/1` { #hidehud }

Turn on to hide all HUD elements

## Shader Reference

### `stdworld`

**Slots:** c

The default lightmapped world shader.

### `decalworld`

**Slots:** c, d

Like [`stdworld`](#stdworld), except alpha blends decal texture on diffuse texture.

### `glowworld`

**Slots:** c, g

**Params:**

- `glowcolor`: **Rk**, **Gk**, **Bk** - multiplies the glow map color by the factors Rk, Gk, Bk

Like [`stdworld`](#stdworld), except adds light from glow map.

### `bumpworld`

**Slots:** c, n

Normal-mapped shader without specularity (diffuse lighting only).

| Shader                         | Params                                                                                                                                                                                                                                     | Slots         | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
|--------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `stdworld`                     |                                                                                                                                                                                                                                            | c             |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
| `decalworld`                   |                                                                                                                                                                                                                                            | c, d          | Like stdworld, except alpha blends decal texture on diffuse texture.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
| `glowworld`                    | `glowcolor`: Rk, Gk, Bk - multiplies the glow map color by the factors Rk, Gk, Bk                                                                                                                                                          | c, g          | Like stdworld, except adds light from glow map.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
| `bumpworld`                    |                                                                                                                                                                                                                                            | c, n          | Normal-mapped shader without specularity (diffuse lighting only).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
| `bumpglowworld`                | `glowcolor`: Rk, Gk, Bk - multiplies the glow map color by the factors Rk, Gk, Bk                                                                                                                                                          | c, n, g       | Normal-mapped shader with glow map and without specularity.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| `bumpspecworld`                | `specscale`: Rk, Gk, Bk - multiplies the specular light color by the factors Rk, Gk, Bk                                                                                                                                                    | c, n          | Normal-mapped shader with constant specularity factor.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| `bumpspecmapworld`             | same as above                                                                                                                                                                                                                              | c, n, s       | Normal-mapped shader with specularity map.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |
| `bumpspecglowworld`            | `glowcolor`: Rk, Gk, Bk - multiplies the glow map color by the factors Rk, Gk, Bk<br>`specscale`: Rk, Gk, Bk - multiplies the specular light color by the factors Rk, Gk, Bk                                                               | c, n, g       | Normal-mapped shader with constant specularity factor and glow map.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
| `bumpspecmapglowworld`         | same as above                                                                                                                                                                                                                              | c, n, s, g    | Normal-mapped shader with specularity map and glow map.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| `bumpparallaxworld`            | `parallaxscale`: Scale, Bias - Scales the heightmap offset                                                                                                                                                                                 | c, n, z       | Normal-mapped shader with height map and without specularity.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| `bumpspecparallaxworld`        | `specscale`: Rk, Gk, Bk - multiplies the specular light color by the factors Rk, Gk, Bk<br>`parallaxscale`: Scale, Bias - Scales the heightmap offset                                                                                      | c, n, z       | Normal-mapped shader with constant specularity factor and height map.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |
| `bumpspecmapparallaxworld`     | same as above                                                                                                                                                                                                                              | c, n, s, z    | Normal-mapped shader with specularity map and height map.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| `bumpparallaxglowworld`        | `glowcolor`: Rk, Gk, Bk - multiplies the glow map color by the factors Rk, Gk, Bk<br>`parallaxscale`: Scale, Bias - Scales the heightmap offset                                                                                            | c, n, z, g    | Normal-mapped shader with height and glow maps, and without specularity.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
| `bumpspecparallaxglowworld`    | `glowcolor`: Rk, Gk, Bk - multiplies the glow map color by the factors Rk, Gk, Bk<br>`specscale`: Rk, Gk, Bk - multiplies the specular light color by the factors Rk, Gk, Bk<br>`parallaxscale`: Scale, Bias - Scales the heightmap offset | c, n, z, g    | Normal-mapped shader with constant specularity factor, and height and glow maps.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| `bumpspecmapparallaxglowworld` | same as above                                                                                                                                                                                                                              | c, n, s, z, g | Normal-mapped shader with specularity, height, and glow maps.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| `bumpenv*`                     | `envscale`: Rk, Gk, Bk - multiplies the environment map color by the factors Rk, Gk, Bk                                                                                                                                                    |               | Any of the above bump* shader permutations may replace "bump" with "bumpenv" (i.e. bumpenvspecmapworld), and will then reflect the closest envmap entity (or the skybox if necessary). They support all their usual texture slots and pixel params, in addition to the envmap multiplier pixel param. If a specmap is present in the given shader, the raw specmap value will be scaled by the envmap multipliers (instead of the specmap ones), to determine how much of the envmap to reflect. A calclight (if it has not been done before) or recalc (thereafter) is also needed by this shader to properly setup its engine state. |
