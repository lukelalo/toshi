sel /game/scene

new n3dnode cielo
sel cielo

#.setminlod 0.000000
.txyz 0.000000 -100.000000 0.000000
.rxyz 0.000000 0.000000 0.000000
.sxyz 1.000000 1.000000 1.000000
.setlockviewer true

new nskydome domo
#sel domo
domo.setsunangle 290
# This nipol will control the dome's day/night factor
# set by the Sun angle.  Angle of 0 (or 360) is directly overhead.
# angle 70 = twilight, 100 = dark, 250 = dawn, 290 = day
#
# I imagine most will programatically connect this, but it demos 
# like this nicely
#new nipol nip
#nip.connect setsunangle
# nip.addkey1f 0  0   
# nip.addkey1f 10 360 
# nip.setscale 10
#sel ..

# The dome's "Cloud Factor" determines the speed of the cloud cover
#
# 1000 = default, 100 = REAL FAST, 10 = nauseating
domo.setcloudfactor 1300

new nshadernode shader
    sel shader

    new ntexarraynode tex
        sel tex
        .settexture 0 "textures:cielo.bmp" "none"         
        .settexture 1 "textures:nubes.tga" "none" 
    sel ..

    .setnumstages 2
    .setcolorop 0 "replace const prev"
    .setcolorop 1 "ipol tex prev tex.a"
#    .setcolorop 2 "mul primary prev"
 	
    .setconst 0 0.000000 0.000000 0.000000 1.000000
    .setconst 1 1.000000 0.000000 0.000000 1.000000
    .begintunit 0
    .setaddress "wrap" "wrap"
    .setminmagfilter linear_mipmap_nearest linear_mipmap_nearest
    .settexcoordsrc "uv0"
    .setenabletransform false
    .txyz 0.000000 0.000000 0.000000
    .rxyz 0.000000 0.000000 0.000000
    .sxyz 1.000000 1.000000 1.000000
    .endtunit

    .begintunit 1
    .setaddress "wrap" "wrap"
    .setminmagfilter linear_mipmap_nearest linear_mipmap_nearest
    .settexcoordsrc "uv1"
    .setenabletransform true
    .txyz 0.000000 0.000000 0.000000
    .rxyz 0.000000 0.000000 0.000000
    .sxyz 0.500000 0.500000 0.500000
    .endtunit

    .setdiffuse 1.000000 1.000000 1.000000 1.000000
    .setemissive 1.000000 1.000000 1.000000 1.000000
    .setambient 1.000000 1.000000 1.000000 1.0000000

#   A render pri of -10 should cause this to draw BEHIND everything else
#   thus scale shouldn't become a factor.
    .setrenderpri -10
    .setlightenable true	
    .setalphaenable true
    .setzwriteenable false 
    .setfogenable false
    .setalphablend srcalpha destalpha
#    .setzfunc "always"
    .setcullmode "none"
    .setcolormaterial "material"
sel ..

sel /game/scene