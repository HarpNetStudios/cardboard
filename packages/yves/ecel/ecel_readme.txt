12/27/2003

Name: ecel textures  
Short Name: ecel
Author: Yves "evillair" Allaire 
Email: yves@evillair.net
URL: http://evillair.net

[Description]
Quake 3: Arena texture set in a cel shading industrial style.

=================================================================================
To add cel shading to your maps read the following.

Copy the text below into a text file and name it "cel.shader" and place in into your /scripts folder.

// [CelShading shader by ydnar]

//--------cel.shader copy START------------------//
// ink shader for maps
// to use, add "cel" to shaderlist.txt
// add a "_celshader" key to worldspawn entity with a value of "cel/ink"

textures/cel/ink
{
	qer_editorimage gfx/colors/black.tga
	
	q3map_notjunc
	q3map_nonplanar
	q3map_bounce 0.0
	q3map_shadeangle 120
	q3map_texturesize 1 1
	q3map_invert
	q3map_offset -2.0
	
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nomarks
	
	sort 16
	
	{
		map gfx/colors/black.tga
		rgbGen identity
	}
}
//--------cel.shader copy END------------------//

=================================================================================

[Instructions for Q3Radiant/GTKRadiant users]

Basic usage: (not recommended)
Extract the .zip file into your "baseq3" folder.

Add "ecel" (no quotes) to your shaderlist.txt in your \scripts folder to see them in Q3Radient/GTKRadient

Suggested usage:
I'd encourage you to make your own directories for my textures for your map like so ... 

"baseq3/textures/mymapname/" (place my textures here.)

"baseq3/scripts/mymapname.shader" (add my shaders here, making sure to edit the paths in the original shader file.)



[Thanks]
Everyone who had kind words to say about my other sets and
the people at the mapcenter forum and Quake3World.com Level Editing Forum.

Everyone who has used/will use my textures in their maps.
This is why I make them. :)



[Stuff]
If you use any of the textures it would be cool if you emailed me with the url to some screenshots if possible. 
I really would like to see what uses mappers have made of them.

[Copyright/Permissions]
-You may edit any shaders (.shader file) as you see fit.
-You may save textures that don't require an alpha channel as a .jpg for distrubution with your maps.
-You may change the color of any "texturename.blend" or hue to fit the style of your map.
-You may use these texture in your maps/mods/tc's as long as you give me credit.
-You may not edit, modify any textures within this archive unless given permission to do so by the author unless specified above.  
-You may convert these textures to other game formats but only with the author's permission (me).

[HarpNet Note]
Yves has since relicensed these textures under a different set of terms, see LICENSE.txt file for details.

QUAKE, QUAKE II and QUAKE3:ARENA are registered trademarks
of id Software, Inc.



