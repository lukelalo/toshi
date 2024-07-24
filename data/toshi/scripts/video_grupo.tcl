#--------------------------------------------------------------------
#   video_grupo.tcl
#   ==================
#   Vídeo que muestra el logotipo del grupo
#
#--------------------------------------------------------------------

proc DestructorVideoGrupo { } {
delete /game/scene/video_grupo
}

proc VideoGrupo { } {
/game.playsong 0 "sounds:musica/musica_grupo.wav"
GenVideoGrupo
}

proc GenVideoGrupo { } {
new n3dnode /game/scene/video_grupo
sel /game/scene/video_grupo

# Switching off the ambient light
new nlightnode amb
amb.settype    ambient
amb.setcolor   0 0 0 0

# The two lightsources rotating
new n3dnode dlight    
    sel dlight
    new nipol roty 
        sel roty
        .connect ry
        .addkey1f 0 0
        .addkey1f 6 360
    sel ..
    new nipol rotx
        sel rotx
        .connect rx
        .addkey1f  0   0
        .addkey1f  7  60
        .addkey1f 13   0
    sel ..

sel /game/scene/video_grupo
    new n3dnode light1
      	sel light1
        new nlightnode light1
            sel light1
		    .setattenuation 1   0   0 
			.settype        point
			.setcolor       0.1 0.5 1 1

            new nipol col
                sel col
                .connect setcolor
                .addkey4f 0   1 1 1 0
                .addkey4f 7   1 1 1 1
             
sel /game/scene/video_grupo

    new n3dnode light2
      	sel light2
        txyz -2   0   -2
        sxyz  0.25 0.25  0.25

        new nlightnode li2
            sel li2
		    .setattenuation 1 0 0 
			.settype  point
			.setcolor 0.1 0.5 1 1

            new nipol col
                sel col
                .connect setcolor
                .addkey4f 0   1 1 1 0
                .addkey4f 7   1 1 1 1

sel /game/scene/video_grupo

new n3dnode logogrupo
sel logogrupo
.txyz -1.0 0 3.0
.rxyz 0 225 0
.sxyz 0.4 0.4 0.4
.preload
new nmeshnode mesh
sel mesh
	.preload
	.setfilename "meshes:plano.n3d"
	.setcastshadow false
sel ..

new nshadernode shader
sel shader
	.preload
	.setnumstages 1
	.setconst 0 0.0 0.0 0.0 1.0
	.setcolorop 0 "mul tex prev"
	.begintunit 0
	.setaddress         clamp   clamp
	.setminmagfilter    linear_mipmap_nearest linear_mipmap_nearest
	.settexcoordsrc     uv0
	.setenabletransform false
	.endtunit	
	.setalphaenable true
	.setalphablend "srcalpha" "invsrcalpha"
	.setzfunc "lessequal"
	.setlightenable true
	.setdiffuse  0.2 0.2 0.2 1
	.setemissive 0.8 0.8 0.8 1
	.setambient  0.0 0.0 0.0 1
	new nipol fade
	sel fade
		.connect setemissive
		.setreptype oneshot
		.addkey4f  0  	0 0 0 1     
		.addkey4f  5.5 	0 0 0 1
		.addkey4f  7.0	1.0 1.0 1.0 1	
	sel .. 

	new nipol fade2
	sel fade2
		.connect setdiffuse
		.setreptype oneshot
		.addkey4f  0  	0 0 0 1
		.addkey4f  5.5 	0 0 0 1        
		.addkey4f  7.0	1.0 1.0 1.0 1	
	sel .. 	
sel ..

new ntexarraynode texture
sel texture
    .preload
    .settexture 0 "textures:LogoStudioBlanco2.png" "none"
sel ..



sel /game/scene/video_grupo

new n3dnode shuriken
sel shuriken
    txyz 2.5 0 -0.5
    rxyz 0 0 0
    sxyz 1.3 1.3 1.3
new nmeshnode mesh
mesh.setfilename meshes:shuriken.n3d

    new nipol rotz
        sel rotz
        .connect rz
        .setreptype oneshot        
        .addkey1f  0 	0
        .addkey1f  7.0 	670
    sel ..
    new nipol rotx
        sel rotx
        .connect rx
        .setreptype oneshot
        .addkey1f  0  	670
        .addkey1f  7.0	0
    sel ..
    
    new nipol roty
        sel roty
        .connect ry
        .setreptype oneshot
        .addkey1f  0  	0
        .addkey1f  7.0	310
    sel ..    

new nshadernode shader
    sel shader
    .setnumstages 1
    .setcolorop 0 "mul tex prev"

    .begintunit 0
	.setaddress         wrap   wrap
	.setminmagfilter    linear linear
	.settexcoordsrc     uv0
	.setenabletransform false
    .endtunit

    .setlightenable true
    .setdiffuse  0.2 0.2 0.2 1
    .setemissive 0.5 0.5 0.5 1
    .setambient  0.0 0.0 0.0 1
    .setalphaenable false
    new nipol fade
        sel fade
        .connect setemissive
        .setreptype oneshot
        .addkey4f  0  	0 0 0 1    
        .addkey4f  1  	0 0 0 1 
        .addkey4f  7.0	0.5 0.5 0.5 1
    sel .. 
    
    new nipol fade2
        sel fade2
        .connect setdiffuse
        .setreptype oneshot
        .addkey4f  0  	0 0 0 1
        .addkey4f  1  	0 0 0 1        
        .addkey4f  7.0	0.8 0.8 0.8 1
    sel ..   
sel ..
new ntexarraynode tex
    sel tex
    .settexture 0 textures:metal.png none
}