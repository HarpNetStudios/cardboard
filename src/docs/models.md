---
title: Carmine Impact Docs — Model Reference
---

## MD2 Format

MD2 files must be located in a directory in packages/models/, you must provide a skin
(either skin.png or skin.jpg) and the md2 itself (tris.md2).
Optionally you may provide a masks.png that holds a specmap in the R channel, a glowmap
in the G channel, and a chrome map in the B channel. The engine will apply it automatically.

If either of these files is not found, the engine will search the parent directory for them.
For example, if for the flags/red model, the tris.md2 is not found in
packages/models/flags/red/, then it will look for tris.md2 in packages/models/flags/.
This allows the sharing of skins and geometry.

It is expected that md2 format files use Quake-compatible scale and animations, unless you configure the model otherwise.

You may also supply a config file (md2.cfg) in your model directory, which allows you to
customize the model's animations. The following commands may be used in an md2.cfg:

### `md2anim N F M [S [P]]` { #md2anim }

N is the name of the animation to define. Any of the following names may be used:

* dying
* dead
* pain
* idle
* forward
* backward
* left
* right
* hold 1 ... hold 7
* attack 1 ... attack 7
* jump
* sink
* swim
* edit
* lag
* taunt
* win
* lose
* gun shoot
* gun idle
* vwep shoot
* vwep idle
* mapmodel
* trigger

F is the frame number the animation starts at. M is the number of frames in the
animation. S is the optional frames per second at which to run the anim. If none is specified,
10 FPS is the default. P specifies an optional priority for the animation (defaults to P=0).

For example, defining a "pain" animation starting at frame 50 with 6 frames and running at 30 frames per
second would look like: md2anim "pain" 50 6 30

## MD3 Format

MD3 file can be used in one of two ways.

### Without Configuration

Go this way if your md3 has no animations (static) and takes only one texture, they must be
located in a directory in packages/models/, and provide a skin (either skin.png
or skin.jpg) and the md3 itself (tris.md3). Optionally you may provide a
masks.png that holds a specmap in the R channel, a glowmap in the G channel, and a
chrome map in the B channel. The engine will apply it automatically.

If either of these files is not found, the engine will search the parent directory for them.
For example, if for the flags/red model, the tris.md3 is not found in
packages/models/flags/red/, then it will look for tris.md3 in
packages/models/flags/. This allows the sharing of skins and geometry.

### With Configuration

MD3 files with animations multiple skins, or if you want to make use of other configuration
possibilities, need to be set up this way. You must place a md3.cfg in a directory in
packages/models/. This file specifies which models should be loaded, linked, etc. The
following commands may be used in the md3.cfg:

### `md3load P` { #md3load }

This command loads the first part of your model. As an example, this could be the head.md3
of a playermodel. P is the path to the md3 file to load, its relative to the location of the
md3.cfg.

### `md3pitch S O M N` { #md3pitch }

This command controls how a model responds to the model's pitch. The pitch (in degrees) is scaled
by S, offset by O, and then clamped to the range M..N, i.e. clamp(pitch*S + O, M, N). By default, 
all model parts have S=1, O=0, M=-360, and N=360, such that the model part will pitch one-to-one.

### `md3skin H S M [E [F]]` { #md3skin }

This loads a texture and assigns it to a mesh of the last loaded model (md3load). H is the name
of the mesh you want to assign the texture to. S is the path to the texture, its recursive
to the location of the md3.cfg. The optional M sets a texture for spec (red channel)/glow
(green channel) maps as above. If E is non-zero, then the blue channel of the masks is
interpreted as a chrome map. E (maximum envmap intensity) and F (minimum envmap intensity, default: 0) are floating point 
values in the range of 0 to 1, and specify the range in which the envmapping intensity will vary based on a viewing angle
(a Fresnel term that is maximal at glancing angles, minimal when viewed dead-on). The intensity, 
after scaled into this range, is then multiplied by the chrome map.

### `md3bumpmap H N [S]` { #md3bumpmap }

This command enables bumpmapping for a given mesh in the last loaded model (md3load). H is the name
of the mesh you want to assign bumpmapping textures to. S is the path to a diffuse skin texture which is (if specified)
used instead of the skin supplied with the "md3skin" command only if the user's 3D card supports bumpmapping, 
otherwise the skin supplied with "md3skin" takes precedence and no bumpmapping is done. These
two diffuse skins may be the same. However a diffuse skin intended for bumpmapping should generally have
little to no directional shading baked into it, whereas flat diffuse skins (no bumpmapping) generally should,
and this command allows you to provide a separate skin for the bumpmapping case. N is a normal map texture which is 
used to shade the supplied diffuse skin texture.

### `md3spec MESH S` { #md3spec }

MESH specifies the name of the mesh this setting applies to. S is the specular intensity (not given or 0 gives the default of 100, good for metal/plastics and anything shiny, use lower values like 50 for wood etc, -1 means off entirely).
### `md3alphatest MESH T` { #md3alphatest }

MESH specifies the name of the mesh this setting applies to. Controls the cut-off threshold, T, at which alpha-channel skins will discard pixels where alpha is less than T.
T is a floating point value in the range of 0 to 1 (Default: 0.9). 

### `md3alphablend MESH B` { #md3alphablend }

MESH specifies the name of the mesh this setting applies to. Controls whether a model with an alpha-channel skin will alpha blend (Default: 1). 

### `md3shader MESH S` { #md3shader }

MESH specifies the name of the mesh this setting applies to. S is the name of the shader to use for rendering the model (Default: "stdmodel"). 

### `md3glow MESH G` { #md3glow }

MESH specifies the name of the mesh this setting applies to. G is the glowmap scale (not given or 0 gives the default of 300, -1 means off entirely), such that
the glow is G percent of the diffuse skin color.

### `md3glare MESH S G` { #md3glare }

MESH specifies the name of the mesh this setting applies to. S and G are floating point values that scale the amount of glare generated by specular light and glare, respectively (Default: 1 1).

### `md3envmap MESH P` { #md3envmap }

MESH specifies the name of the mesh this setting applies to. Sets the environment map used for the model, where P is a pathname in the same syntax as the
"loadsky" command. If this is not specified, the mesh will use the closest "envmap" entity,
or skybox, if none is available (unless overridden by "mdlenvmap").

### `md3ambient MESH A` { #md3ambient }

MESH specifies the name of the mesh this setting applies to.  A is the percent of the ambient light that should be used for shading. Not given or 0 gives the
default of 30%, -1 means no ambient.

### `md3fullbright MESH N` { #md3fullbright }

MESH specifies the name of the mesh this setting applies to. Uses a constant lighting level of N instead of the normal lighting. N is a floating-point value on a scale of 0 to 1.

### `md3anim A F N [S [P]]` { #md3anim }

This assigns a new animation to the last loaded model (md3load). A is the name of the animation
to define. Any of the following names may be used:

* dying
* dead
* pain
* idle
* forward
* backward
* left
* right
* hold 1 ... hold 7
* attack 1 ... attack 7
* jump
* sink
* swim
* edit
* lag
* taunt
* win
* lose
* gun shoot
* gun idle
* vwep shoot
* vwep idle
* mapmodel
* trigger

F is the frame number the animation starts at. N is the number of frames in the animation.
S is frames per second at which to run the anim. If none is specified or S=0, 10 FPS is the default.
P specifies an optional priority for the animation (defaults to P=0).

A character model will have up to 2 animations simultaneously playing - a primary animation
and a secondary animation. If a character model defines the primary animation, it will be used,
otherwise the secondary will be used if it is available. Primary animations are:

* dying
* dead
* pain
* hold 1 ... hold 7
* attack 1 ... attack 7
* edit
* lag
* taunt
* win
* lose

Secondary animations are:

* idle
* forward
* backward
* left
* right
* jump
* sink
* swim

This allows, for example, a torso part to "shoot" (a primary animation) while a leg part
simultaneously runs "forward" (a secondary animation). In the event that a secondary animation
other than "idle" is playing and there is no primary animation, then the secondary animation
will behave as if it were primary, and the secondary animation will effectively be "idle".
This allows parts that would not normally participate in a certain animation to do so in the
"idle" case, or otherwise simply be "idle" if this is not desired. However, if you want to override
which animation is primary for a specific part, you can set an explicit priority for that animation,
and the animation with the highest priority is chosen, regardless of which is primary and 
which is secondary. So, for example, you could give an animation a -1 priority so that all animations
with the default 0 priority are chosen first, or you could give an animation a 1 or greater priority
so that it is always chosen before animations with the default 0 priority.

For example, to define a "punch" animation starting at frame 131 with 6 frames and running at
15 frames per second: md3anim "punch" 131 6 15

### `md3link P C T [X Y Z]` { #md3link }

This links two models together. Every model you load with md3load has an ID. The first model
you load has the ID 0, the second has the ID 1, and so on, those ID's are now used to identify
the models and link them together. P the ID of the parent model. C to ID of the
child model. T name of the tag that specifies at which vertex the models should be linked. 
X, Y, and Z are optional translation for this link.

Hint: Make sure you understand the difference between the parent and child model.
Rendering starts at model 0 (first loaded model) and then continues with model 0's children,
etc. Imagine a tree, model 0 is the root if it.

```
md3load lower.md3 // model no. 0
md3skin l_lower ./default_l.png

md3anim dying           0   30  20
md3anim dead            30  1   25
md3anim "forward|backward|left|right|swim" 114 10  18
md3anim idle            164 30  30
md3anim "jump|lag|edit" 147	8	15

md3load upper.md3 // model no. 1
md3skin u_torso ./default.png

md3anim dying           0   30  20
md3anim dead            30  1   25
md3anim "lag|edit"      91  40  18
md3anim "attack *"      131 6   15
md3anim idle            152 1   15
md3anim pain            152 1   15

md3load head.md3 // model no. 2
md3skin h_head ./default_h.png

md3link 0 1 tag_torso
md3link 1 2 tag_head

mdlspec 50
mdlscale 20         // 20 percent of the original size
mdltrans 0 0 -1.5   // lower the model a bit
```

## OBJ Format

The Wavefront OBJ format is configured almost identically to how MD3s are used. The only differences are that OBJ specific commands are prefixed with "obj" instead of "md3" (i.e. "objskin" instead of "md3skin") which are specified in "obj.cfg" instead of "md3.cfg", it looks for "tris.obj" instead of "tris.md3" by default, and there is no support for animation ("objanim") or linking ("objlink"). Group names are used to identify meshes within the model.

## MD5 Format

MD5 models require a proper configuration to function; make sure your exporter properly exports mesh names in the MD5 file so that these can be referenced in the configuration file (the Blender exporter does not export these, but a fixed  Blender MD5 exporter can be gotten from the Cube wiki).

Make sure no more than 4 blend weights are used per vertex, any extra blend weights will be dropped silently. The skeleton should use no more than 256 bones, and less than 70 or so bones should be used if you wish the model to be skeletally animated on the GPU. To optimize animation of the model on both CPU and GPU, keep the number of blend weights per vertex to a minimum. Also, similar combinations of blend weights are cached while animating, especially on the CPU, such that if two or more vertices use the same blend weights, blending calculations only have to be done once for all the vertices - so try and minimize the number of distinct combinations of blend weights if possible. 

When animating skeletal models, you should model the animations as a single piece for the entire body (unlike for MD3 which requires splitting the mesh). In the configuration file, you can choose a bone at which to split the model into an upper and lower part (via "md5animpart"), which allows, for example, the upper body movement of one animation to be combined with the lower body movement of another animation automatically. The bone at which you split the animation up should ideally be a root bone, of which the upper body and lower body are separate sub-trees. Rigging the model in this way also allows for pitch animation (which also requires selecting a bone to pitch at) to take place such as bending at the waist, which similarly requires the upper body to be a sub-tree of the bone at which the pitch animation will occur.

The included MD5 support allows for two methods of attaching models to another: via tags (by assigning a tag name to a bone with "md5tag"), or by animating multiple models against a common, named skeleton that will be shared among all of them (useful for modeling clothing attachments and similar items). To use a shared skeleton, you simply export all the models with the same skeleton. Animations only need to be specified for the base model. A name for the skeleton is specified via the "md5load" command, for both the model exporting the skeleton and the models using it. When one of the models is attached to the one supplying the skeleton internally, the tag setup is instead ignored and the skeleton/animations of the base model are used to animate the attachment as if it were a sub-mesh of the base model itself.


You must place a md5.cfg in a directory in packages/models/.
The following commands may be used in the md5.cfg:

### `md5load P [S]` { #md5load }

This command loads the first part of your model. As an example, this could be the head.md5mesh
of a playermodel. P is the path to the md5mesh file to load, its relative to the location of the
md5.cfg. S is an optional name that can be assigned to the skeleton specified in the md5mesh
for skeleton sharing, but need not be specified if you do not wish to share the skeleton. This skeleton 
name must be specified for both the model supplying a skeleton and an attached model intending to use the skeleton.

### `md5pitch B S O M N` { #md5pitch }

This command controls how a model responds to the model's pitch. B is the name of the bone which
the pitch animation is applied to, as well as all bones in the sub-tree below it. The pitch (in degrees) 
is scaled by S, offset by O, and then clamped to the range M..N, i.e. clamp(pitch*S + O, M, N). By default,
all model parts have S=1, O=0, M=-360, and N=360, such that the model part will pitch one-to-one.

### `md5skin H S M [E [F]]` { #md5skin }

This loads a texture and assigns it to a mesh of the last loaded model (md5load). H is the name
of the mesh you want to assign the texture to. S is the path to the texture, its recursive
to the location of the md5.cfg. The optional M sets a texture for spec (red channel)/glow
(green channel) maps as above. If E is non-zero, then the blue channel of the masks is
interpreted as a chrome map. E (maximum envmap intensity) and F (minimum envmap intensity, default: 0) are floating point
values in the range of 0 to 1, and specify the range in which the envmapping intensity will vary based on a viewing angle
(a Fresnel term that is maximal at glancing angles, minimal when viewed dead-on). The intensity,
after scaled into this range, is then multiplied by the chrome map.

### `md5bumpmap H N [S]` { #md5bumpmap }

This command enables bumpmapping for a given mesh in the last loaded model (md5load). H is the name
of the mesh you want to assign bumpmapping textures to. S is the path to a diffuse skin texture which is
used (if specified) instead of the skin supplied with the "md5skin" command only if the user's 3D card supports bumpmapping,
otherwise the skin supplied with "md5skin" takes precedence and no bumpmapping is done. These
two diffuse skins may be the same. However a diffuse skin intended for bumpmapping should generally have
little to no directional shading baked into it, whereas flat diffuse skins (no bumpmapping) generally should,
and this command allows you to provide a separate skin for the bumpmapping case. N is a normal map texture which is
used to shade the supplied diffuse skin texture.

### `md5spec MESH S` { #md5spec }

MESH specifies the name of the mesh this setting applies to. S is the specular intensity (not given or 0 gives the default of 100, good for metal/plastics and anything shiny, use lower values like 50 for wood etc, -1 means off entirely).
### `md5alphatest MESH T` { #md5alphatest }

MESH specifies the name of the mesh this setting applies to. Controls the cut-off threshold, T, at which alpha-channel skins will discard pixels where alpha is less than T.
T is a floating point value in the range of 0 to 1 (Default: 0.9).

### `md5alphablend MESH B` { #md5alphablend }

MESH specifies the name of the mesh this setting applies to. Controls whether a model with an alpha-channel skin will alpha blend (Default: 1).

### `md5shader MESH S` { #md5shader }

MESH specifies the name of the mesh this setting applies to. S is the name of the shader to use for rendering the model (Default: "stdmodel").

### `md5glow MESH G` { #md5glow }

MESH specifies the name of the mesh this setting applies to. G is the glowmap scale (not given or 0 gives the default of 300, -1 means off entirely), such that
the glow is G percent of the diffuse skin color.

### `md5glare MESH S G` { #md5glare }

MESH specifies the name of the mesh this setting applies to. S and G are floating point values that scale the amount of glare generated by specular light and glare, respectively (Default: 1 1).

### `md5envmap MESH P` { #md5envmap }

MESH specifies the name of the mesh this setting applies to. Sets the environment map used for the model, where P is a pathname in the same syntax as the
"loadsky" command. If this is not specified, the mesh will use the closest "envmap" entity,
or skybox, if none is available (unless overridden by "mdlenvmap").

### `md5ambient MESH A` { #md5ambient }

MESH specifies the name of the mesh this setting applies to.  A is the percent of the ambient light that should be used for shading. Not given or 0 gives the
default of 30%, -1 means no ambient.

### `md5fullbright MESH N` { #md5fullbright }

MESH specifies the name of the mesh this setting applies to. Uses a constant lighting level of N instead of the normal lighting. N is a floating-point value on a scale of 0 to 1.

### `md5anim A F [S] [P] [START] [END]` { #md5anim }

This assigns a new animation to the current animation part of the last loaded model (md5load). A is the name of the animation
to define. Any of the following names may be used:

* dying
* dead
* pain
* idle
* forward
* backward
* left
* right
* hold 1 ... hold 7
* attack 1 ... attack 7
* jump
* sink
* swim
* edit
* lag
* taunt
* win
* lose
* gun shoot
* gun idle
* vwep shoot
* vwep idle
* mapmodel
* trigger

F is the file name of an md5 animation file. S is frames per second at which to run the anim. If none is specified or S=0, 10 FPS is the default. P specifies an optional priority for the animation (defaults to P=0). START is an optional start frame offset within the animation, where a positive values is an offset from the first frame, and a negative value is an offset from end. END is an optional animation length in frames, where positive values specify the length, or negative values merely subtract off from the length.

A character model will have up to 2 animations simultaneously playing - a primary animation
and a secondary animation. If a character model defines the primary animation, it will be used,
otherwise the secondary will be used if it is available. Primary animations are:

* dying
* dead
* pain
* hold 1 ... hold 7
* attack 1 ... attack 7
* edit
* lag
* taunt
* win
* lose

Secondary animations are:

* idle
* forward
* backward
* left
* right
* jump
* sink
* swim

### `md5animpart B` { #md5animpart }

Starts a new animation part that will include bone B and all its sub-bones. This effectively splits animations up at the bone B, such that each animation part animates as if it were a separate model. Note that a new animation part has no animations (does not inherit any from the previous animation part). After an "md5load", an implicit animation part is started that involves all bones not used by other animation parts. Each model currently may only have 2 animation parts, including the implicit default part, so this command may only be used once and only once per md5mesh loaded. However, you do not need to specify any animation parts explicitly and can just use the default part for all animations, if you do not wish the animations to be split up/blended together.

### `md5tag B T [X Y Z] [RX RY RZ]` { #md5tag }

Assigns the tag name T to the bone B, for either use with "md5link" or attachment of other models via tags. X, Y, and Z are an optional translation, whereas RX, RY, and RZ are optional rotations in degrees.

### `md5link P C T [X Y Z]` { #md5link }

This links two models together. Every model you load with md5load has an ID. The first model
you load has the ID 0, the second has the ID 1, and so on, those ID's are now used to identify
the models and link them together. P the ID of the parent model. C to ID of the
child model. T name of the tag that specifies at which vertex the models should be linked.
X, Y, and Z are an optional translation for this link.

### `md5adjust BONE YAW [PITCH] [ROLL] [X Y Z]` { #md5adjust }

Adjusts bone BONE with the specified rotations, in degrees, on any animations loaded after this command is specified. X, Y, and Z are an optional translation.

## SMD Format

The Half-Life SMD format is configured almost identically to how MD5s are used. The only differences are that SMD specific commands are prefixed with "smd" instead of "md5" (i.e. "smdskin" instead of "md5skin") which are specified in "smd.cfg" instead of "md5.cfg".

## IQM Format

The Inter-Quake Model (IQM) format is configured almost identically to how MD5s are used. The only differences are that IQM specific commands are prefixed with "iqm" instead of "md5" (i.e. "iqmskin" instead of "md5skin") which are specified in "iqm.cfg" instead of "md5.cfg".

## Common Commands
### `mdlcollide N` { #md1collide }

If N is 0, collisions with the model are disabled (Default: 1).

### `mdlellipsecollide N` { #mdlellipsecollide }

If N is 1, the model uses an elliptical collision volume instead of a box (Default: 0). This setting is good for barrels, trees, etc.

### `mdlbb R H [E]` { #md1bb }

Sets the model's bounding box to radius R and height H. If this is not set, or R and H are 0, 
a bounding box is automatically generated from the model's geometry. E is fraction of the
model's height to be used as the eyeheight (Default: 0.9).

### `mdlcullface N` { #md1cullface }

If N is 0, backface culling is disabled for this model. Otherwise, backface culling is enabled
(Default: 1).

### `mdlspec S` { #md1spec }

S is the specular intensity (not given or 0 gives the default of 100, good for metal/plastics
and anything shiny, use lower values like 50 for wood etc, -1 means off entirely).

### `mdlalphatest T` { #md1alphatest }

Controls the cut-off threshold, T, at which alpha-channel skins will discard pixels where alpha is less than T.
T is a floating point value in the range of 0 to 1 (Default: 0.9).

### `mdlalphablend B` { #md1alphablend }

Controls whether a model with an alpha-channel skin will alpha blend (Default: 1).

### `mdlglow G` { #md1glow }

G is the glowmap scale (not given or 0 gives the default of 300, -1 means off entirely), such that 
the glow is G percent of the diffuse skin color.

### `mdlglare S G` { #md1glare }

S and G are floating point values that scale the amount of glare generated by specular light and glare, respectively (Default: 1 1).

### `mdlshader S` { #md1shader }

S is the name of the shader to use for rendering the model (Default: "stdmodel").

### `mdlambient A` { #md1ambient }

A is the percent of the ambient light that should be used for shading. Not given or 0 gives the
default of 30%, -1 means no ambient.

### `mdlscale S` { #md1scale }

Scale the model's size to be S percent of its default size.

### `mdltrans X Y Z` { #md1trans }

Translate the model's center by (X, Y, Z), where X/Y/Z are in model units (may use floating
point).

### `mdlyaw Y` { #md1yaw }

Offsets the model's yaw by Y degrees.

### `mdlpitch P` { #md1pitch }

Offsets the model's pitch by P degrees.

### `mdlspin Y` { #md1spin }

Simple spin animation that yaws the model by Y degrees per second.

### `mdlenvmap E F [P]` { #md1envmap }

Sets the environment map used for the model, where P is a pathname in the same syntax as the
"loadsky" command. If this is not specified, the model will use the closest "envmap" entity,
or skybox, if none is available. If E is non-zero, then the blue channel of the masks is
interpreted as a chrome map. E (maximum envmap intensity) and F (minimum envmap intensity, default: 0) are floating point
values in the range of 0 to 1, and specify the range in which the envmapping intensity will vary based on a viewing angle
(a Fresnel term that is maximal at glancing angles, minimal when viewed dead-on). The intensity,
after scaled into this range, is then multiplied by the chrome map.

### `mdlfullbright N` { #md1fullbright }

Uses a constant lighting level of N instead of the normal lighting. N is a floating-point value on a scale of 0 to 1.

### `mdlshadow B` { #md1shadow }

Controls whether a mapmodel will cast shadows (Default: 1).
