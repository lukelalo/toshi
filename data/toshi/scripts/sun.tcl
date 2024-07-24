# ---
# $parser:ntclserver$ $class:n3dnode$ $ver:Mon Jul 16 22:11:12 2001$
# ---

# position sun to match sky textures

new nlenseflare flare
    sel flare

    # I think there might be a bug, I can't get it to show
    # more than 3 flares (could just be me).
    .beginflares 3

    # This is the color the screen will fade to when
    # you look at the flare.
    .setblindcolor 1.000000 1.000000 0.800000 1.000000

    # This sets the flares colour, position, and size in
    # relation to the flare pos and centre of the screen
    .setflaresizeat 0 1.300000
    .setflareposat 0 0.000000
    .setflarecolorat 0 0.400000 0.400000 0.350000 0.000000
    .setflaresizeat 1 0.225000
    .setflareposat 1 0.700000
    .setflarecolorat 1 0.300000 0.230000 0.230000 0.000000
    .setflaresizeat 2 0.725000
    .setflareposat 2 1.400000
    .setflarecolorat 2 0.300000 0.300000 0.250000 0.000000
    .endflares
sel ..

new nshadernode s
    sel s
    .setrenderpri 50
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

new ntexarraynode t
    sel t
    .settexture 0 "textures:flare.bmp" "none"
    .setgenmipmaps 0 true
sel ..

new n3dnode sun2
    sel sun2
    .txyz 300.000000 0.000000 0.000000
    .rxyz 0.000000 0.000000 0.000000

    new nlenseflare flare
        sel flare
        .beginflares 3
        .setblindcolor 0.000000 0.000000 0.000000 0.000000
        .setflaresizeat 0 0.300000
        .setflareposat 0 0.400000
        .setflarecolorat 0 0.400000 0.400000 0.250000 0.000000
        .setflaresizeat 1 0.725000
        .setflareposat 1 1.000000
        .setflarecolorat 1 0.400000 0.330000 0.230000 0.000000
        .setflaresizeat 2 0.425000
        .setflareposat 2 1.600000
        .setflarecolorat 2 0.200000 0.200000 0.150000 0.000000
        .endflares
    sel ..

    new nshadernode s
        sel s
        .setrenderpri 50
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

    new ntexarraynode t
        sel t
        .settexture 0 "textures:glow.bmp" "none"
        .setgenmipmaps 0 true
    sel ..
sel ..