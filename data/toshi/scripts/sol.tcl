new n3dnode /game/scene/escenario/overlay
new noverlayplane /game/scene/escenario/overlay/plane
new nshadernode /game/scene/escenario/overlay/shader

sel /game/scene/escenario/overlay	
	.preload
sel /game/scene/escenario/overlay/shader 
	.preload
	.setnumstages 1
	.setcolorop 0 "mul tex prev" 
	.begintunit 0
	.setaddress "wrap" "wrap"
	.setminmagfilter "nearest" "nearest"
	.settexcoordsrc "uv0"
	.setenabletransform false
	.endtunit
	.setlightenable false
	.setdiffuse 1.0 1.0 1.0 1.0
	.setemissive 0.0 0.0 0.0 0.0
	.setambient 1.0 1.0 1.0 1.0
	.setalphaenable true
	.setzwriteenable false
	.setzfunc "lessequal"
    
    
# Enabling this enables the "BLIND" color for
# lenseflares, but also wipes out line drawing
# since they aren't alpha-able
#   .setalphaenable(true);
    .setalphablend "srcalpha" "invsrcalpha"

new n3dnode /game/scene/escenario/lf1
sel /game/scene/escenario/lf1
.preload
.txyz 30 350 -1000
new n3dnode /game/scene/escenario/lf1/flare
sel flare
	.preload
	new nshadernode shad
		sel shad
		    .preload
		    .setrenderpri 40
		    .setnumstages 1
		    .setcolorop 0 "mul tex prev"
		    .setalphaop 0 "mul tex prev"
		    .setconst 0 0.000000 0.000000 0.000000 0.000000
		    .begintunit 0
		    .setaddress "clamp" "clamp"
		    .setminmagfilter "linear" "linear"
		    .settexcoordsrc "uv0"
		    .setenabletransform false
		    .txyz 0.000000 0.000000 0.000000
		    .rxyz 0.000000 0.000000 0.000000
		    .sxyz 1.000000 1.000000 1.000000
		    .endtunit
		    .setdiffuse 0.000000 0.000000 0.000000 0.000000
		    .setemissive 0.000000 0.000000 0.000000 0.000000
		    .setambient 0.000000 0.000000 0.000000 0.000000
		    .setlightenable false
		    .setalphaenable true
		    .setzwriteenable false
		    .setfogenable false
		    .setalphablend "one" "one"
		    .setzfunc "always"
		    .setcullmode "ccw"
		    .setcolormaterial "diffuse"
	sel ..

	new ntexarraynode tex
		sel tex
		.preload
		.settexture 0 "textures:glows.bmp" none
 	sel ..

	new nlenseflare2 lf
		sel lf
		.preload
		.beginflares 9
		.setblindcolor .9500000 1.0 .8600000 1.00000

		.setflaresizeat 0 0.800000
		.setflareposat 0 0.000000
		.setflarecolorat 0 1.000000 .400000 0.200000 0.00001

		.setflaresizeat 1 .400000
		.setflareposat 1 0.100000
		.setflarecolorat 1 0.07000 0.07000 0.00300 0.00001

		.setflaresizeat 2 .300000
		.setflareposat 2 0.300000
		.setflarecolorat 2 .0700000 0.0700000 .100000 0.00001

		.setflaresizeat 3 0.200000
		.setflareposat 3 0.500000
		.setflarecolorat 3 .1000000 0.0700000 .10000 0.00001

		.setflaresizeat 4 0.100000
		.setflareposat 4 0.700000
		.setflarecolorat 4 .1000000 0.100000 .0500000 0.00001

		.setflaresizeat 5 0.100000
		.setflareposat 5 0.200000
		.setflarecolorat 5 .07000000 0.0700000 .0300000 0.00001

		.setflaresizeat 6 0.200000
		.setflareposat 6 1.200000
		.setflarecolorat 6 .070000 0.0700000 .100000 0.00001

		.setflaresizeat 7 0.300000
		.setflareposat 7 1.400000
		.setflarecolorat 7 .1000000 0.070000 .100000 0.00001

		.setflaresizeat 8 0.100000
		.setflareposat 8 1.500000
		.setflarecolorat 8 .1000000 0.100000 .0500000 0.00001


		.endflares
	sel ..
sel ..

new n3dnode /game/scene/escenario/lf2
sel /game/scene/escenario/lf2
.preload
.txyz 600 100 -1000
new n3dnode /game/scene/escenario/lf2/flare
sel flare
	.preload
	new nshadernode shad
		sel shad
		    .preload
		    .setrenderpri 40
		    .setnumstages 1
		    .setcolorop 0 "mul tex prev"
		    .setalphaop 0 "mul tex prev"
		    .setconst 0 0.000000 0.000000 0.000000 0.000000
		    .begintunit 0
		    .setaddress "clamp" "clamp"
		    .setminmagfilter "linear" "linear"
		    .settexcoordsrc "uv0"
		    .setenabletransform false
		    .txyz 0.000000 0.000000 0.000000
		    .rxyz 0.000000 0.000000 0.000000
		    .sxyz 1.000000 1.000000 1.000000
		    .endtunit
		    .setdiffuse 0.000000 0.000000 0.000000 0.000000
		    .setemissive 0.000000 0.000000 0.000000 0.000000
		    .setambient 0.000000 0.000000 0.000000 0.000000
		    .setlightenable false
		    .setalphaenable true
		    .setzwriteenable false
		    .setfogenable false
		    .setalphablend "one" "one"
		    .setzfunc "always"
		    .setcullmode "ccw"
		    .setcolormaterial "diffuse"
	sel ..

	new ntexarraynode tex
		sel tex
		.preload
		.settexture 0 "textures:glows.bmp" none
 	sel ..

	new nlenseflare2 lf
		sel lf
		.preload
		.beginflares 9
		.setblindcolor .9500000 1.0 .8600000 1.00000

		.setflaresizeat 0 0.800000
		.setflareposat 0 0.000000
		.setflarecolorat 0 1.000000 .400000 0.200000 0.00001

		.setflaresizeat 1 .400000
		.setflareposat 1 0.100000
		.setflarecolorat 1 0.07000 0.07000 0.00300 0.00001

		.setflaresizeat 2 .300000
		.setflareposat 2 0.300000
		.setflarecolorat 2 .0700000 0.0700000 .100000 0.00001

		.setflaresizeat 3 0.200000
		.setflareposat 3 0.500000
		.setflarecolorat 3 .1000000 0.0700000 .10000 0.00001

		.setflaresizeat 4 0.100000
		.setflareposat 4 0.700000
		.setflarecolorat 4 .1000000 0.100000 .0500000 0.00001

		.setflaresizeat 5 0.100000
		.setflareposat 5 0.200000
		.setflarecolorat 5 .07000000 0.0700000 .0300000 0.00001

		.setflaresizeat 6 0.200000
		.setflareposat 6 1.200000
		.setflarecolorat 6 .070000 0.0700000 .100000 0.00001

		.setflaresizeat 7 0.300000
		.setflareposat 7 1.400000
		.setflarecolorat 7 .1000000 0.070000 .100000 0.00001

		.setflaresizeat 8 0.100000
		.setflareposat 8 1.500000
		.setflarecolorat 8 .1000000 0.100000 .0500000 0.00001


		.endflares
	sel ..
sel ..

new n3dnode /game/scene/escenario/lf3
sel /game/scene/escenario/lf3
.preload
.txyz -900 120 -1000
new n3dnode /game/scene/escenario/lf3/flare
sel flare
	.preload
	new nshadernode shad
		sel shad
		    .preload
		    .setrenderpri 40
		    .setnumstages 1
		    .setcolorop 0 "mul tex prev"
		    .setalphaop 0 "mul tex prev"
		    .setconst 0 0.000000 0.000000 0.000000 0.000000
		    .begintunit 0
		    .setaddress "clamp" "clamp"
		    .setminmagfilter "linear" "linear"
		    .settexcoordsrc "uv0"
		    .setenabletransform false
		    .txyz 0.000000 0.000000 0.000000
		    .rxyz 0.000000 0.000000 0.000000
		    .sxyz 1.000000 1.000000 1.000000
		    .endtunit
		    .setdiffuse 0.000000 0.000000 0.000000 0.000000
		    .setemissive 0.000000 0.000000 0.000000 0.000000
		    .setambient 0.000000 0.000000 0.000000 0.000000
		    .setlightenable false
		    .setalphaenable true
		    .setzwriteenable false
		    .setfogenable false
		    .setalphablend "one" "one"
		    .setzfunc "always"
		    .setcullmode "ccw"
		    .setcolormaterial "diffuse"
	sel ..

	new ntexarraynode tex
		sel tex
		.preload
		.settexture 0 "textures:glows.bmp" none
 	sel ..

	new nlenseflare2 lf
		sel lf
		.preload
		.beginflares 9
		.setblindcolor .9500000 1.0 .8600000 1.00000

		.setflaresizeat 0 0.800000
		.setflareposat 0 0.000000
		.setflarecolorat 0 1.000000 .400000 0.200000 0.00001

		.setflaresizeat 1 .400000
		.setflareposat 1 0.100000
		.setflarecolorat 1 0.07000 0.07000 0.00300 0.00001

		.setflaresizeat 2 .300000
		.setflareposat 2 0.300000
		.setflarecolorat 2 .0700000 0.0700000 .100000 0.00001

		.setflaresizeat 3 0.200000
		.setflareposat 3 0.500000
		.setflarecolorat 3 .1000000 0.0700000 .10000 0.00001

		.setflaresizeat 4 0.100000
		.setflareposat 4 0.700000
		.setflarecolorat 4 .1000000 0.100000 .0500000 0.00001

		.setflaresizeat 5 0.100000
		.setflareposat 5 0.200000
		.setflarecolorat 5 .07000000 0.0700000 .0300000 0.00001

		.setflaresizeat 6 0.200000
		.setflareposat 6 1.200000
		.setflarecolorat 6 .070000 0.0700000 .100000 0.00001

		.setflaresizeat 7 0.300000
		.setflareposat 7 1.400000
		.setflarecolorat 7 .1000000 0.070000 .100000 0.00001

		.setflaresizeat 8 0.100000
		.setflareposat 8 1.500000
		.setflarecolorat 8 .1000000 0.100000 .0500000 0.00001


		.endflares
	sel ..
sel ..

# ---
# EOF