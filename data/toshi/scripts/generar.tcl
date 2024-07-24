#--------------------------------------------------------------------
#   generar.tcl
#   ===========
#--------------------------------------------------------------------

proc gen_objeto {nombre_objeto mesh_objeto textura_objeto posx posy posz sx sy sz sombras} {
sel /game/scene
    loadmesh $nombre_objeto $mesh_objeto $textura_objeto
    sel $nombre_objeto
    .txyz $posx $posy $posz 
    sel ..

}

proc Generar_Marca_Ayto_Antigua { posx posy posz } {
sel /game/scene
    new n3dnode marca_ayuntamiento
	sel marca_ayuntamiento
	.txyz $posx $posy $posz
	.sxyz 0.25 0.25 0.25
	new nipol roty
	sel roty
	.connect ry
	.setreptype loop       
	.addkey1f  0 	0
	.addkey1f  2.0 	360
	sel ..
	
	new nipol tray
	sel tray
	.connect ty
	.setreptype loop
	.addkey1f  0  	[expr $posy + 0]
	.addkey1f  0.15	[expr $posy + 0.4]
	.addkey1f  0.3	[expr $posy + 0.6]
	.addkey1f  0.45	[expr $posy + 0.7]
	.addkey1f  0.6	[expr $posy + 0.6]
	.addkey1f  0.75	[expr $posy + 0.4]
	.addkey1f  0.9	[expr $posy + 0]
	sel ..        

	.preload
	new nmeshnode mesh
	mesh.preload
	mesh.setfilename "meshes:shuriken.n3d"
	mesh.setcastshadow false

	new nshadernode shader
	    sel shader
	    .preload
	    .setnumstages 1
	    .setlightenable true
	    .setcolorop 0 "add tex prev"
	    .begintunit 0
		.setaddress "wrap" "wrap"
		.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
		.settexcoordsrc "uv0"
		.setenabletransform false
	    .endtunit	    
	    .setdiffuse 0.2 0.2 0.2 0.0
	    .setemissive 0.0 0.0 0.0 0.0
	    .setambient 0.0 0.0 0.0 0.0
	    .setlightenable true
	    .setalphaenable false
	    .setzwriteenable true
	    .setfogenable true
	    .setalphablend "srcalpha" "invsrcalpha"
	    .setzfunc "lessequal"
	sel ..

	new ntexarraynode texture
	sel texture
	    .preload
	    .settexture 0 "textures:metal.png" "none"
	sel ..
    sel ..
}

proc Generar_Marca_Ayto { posx posy posz roty } {
sel /game/scene/escenario
#    puts "posx = $posx, posy = $posy, posz = $posz, angulo = $roty"
    new n3dnode marca_ayuntamiento
	sel marca_ayuntamiento
	.txyz $posx $posy $posz
	.sxyz 1 1 1
	.ry [expr $roty-90]

	.preload
	
	new n3dnode light1
	sel light1
		.ty 3.0
		new nlightnode light
		sel light
		        .setattenuation 0 0.2 0 
			.settype point
			.setcolor 0.0 0.2 0.0 1.0
			sel ..	
		sel ..	
	
	new nmeshnode mesh
	mesh.preload
	mesh.setfilename "meshes:poste.n3d"
	mesh.setcastshadow false

	new nshadernode shader
	    sel shader
	    .preload
	    .setnumstages 1
	    .setlightenable true
	    .setcolorop 0 "add tex prev"
	    .begintunit 0
		.setaddress "wrap" "wrap"
		.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
		.settexcoordsrc "uv0"
		.setenabletransform false
	    .endtunit	    
	    .setdiffuse 0.2 0.2 0.2 0.0
	    .setemissive 0.0 0.0 0.0 0.0
	    .setambient 0.0 0.0 0.0 0.0
	    .setlightenable true
	    .setalphaenable false
	    .setzwriteenable true
	    .setfogenable true
	    .setalphablend "srcalpha" "invsrcalpha"
	    .setzfunc "lessequal"
	sel ..

	new ntexarraynode texture
	sel texture
	    .preload
	    .settexture 0 "textures:driftwood.png" "none"
	sel ..
    sel ..
}

proc Generar_Barca { num posx posz roty } {
sel /game/scene/escenario
    new n3dnode barca$num
	sel barca$num
	.txyz $posx -1.4 $posz
	.ry $roty

	# Movimiento de subida y bajada de las barcas, con el mar
	new nipol ty
	sel ty
		.connect ty
		.setreptype loop
		.addkey1f 0   -1.4
		.addkey1f 2   -1.35
		.addkey1f 4   -1.4
	sel ..
	
	# Movimiento de bamboleo hacia los lados
	new nipol rx
	sel rx
		.connect rx
		.setreptype loop
		.addkey1f 0   0
		.addkey1f 1   5
		.addkey1f 2   0
		.addkey1f 3   -5
		.addkey1f 4   0
	sel ..	

	.preload
	new nmeshnode mesh
	mesh.preload
	mesh.setfilename "meshes:barca.n3d"
	mesh.setcastshadow false

	new nshadernode shader
	    sel shader
	    .preload
	    .setnumstages 1
	    .setlightenable true
	    .setcolorop 0 "add tex prev"
	    .begintunit 0
		.setaddress "wrap" "wrap"
		.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
		.settexcoordsrc "uv0"
		.setenabletransform false
	    .endtunit	    
	    .setdiffuse 0.2 0.2 0.2 0.0
	    .setemissive 0.0 0.0 0.0 0.0
	    .setambient 0.0 0.0 0.0 0.0
	    .setlightenable true
	    .setalphaenable false
	    .setzwriteenable true
	    .setfogenable true
	    .setalphablend "srcalpha" "invsrcalpha"
	    .setzfunc "lessequal"
	sel ..

	new ntexarraynode texture
	sel texture
	    .preload
	    .settexture 0 "textures:oldwood.png" "none"
	sel ..
    sel ..
}

proc gen_objeto_trans {nombre_objeto mesh_objeto textura_objeto posx posy posz sx sy sz sombras} {
sel /game/scene
    loadmesh $nombre_objeto $mesh_objeto $textura_objeto
    sel $nombre_objeto
    .setactive false
    .txyz $posx $posy $posz
    .sxyz $sx $sy $sz
    sel shader
	.setrenderpri 1
	.setnumstages 1
	.setcolorop 0 "add tex prev"
	.setconst 0 0.000000 0.000000 0.000000 0.000000
	.begintunit 0
	.setaddress "clamp" "clamp"
	.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
	.settexcoordsrc "uv0"
	.setenabletransform false
	.txyz 0.000000 0.000000 0.000000
	.rxyz 0.000000 0.000000 0.000000
	.sxyz 1.000000 1.000000 1.000000
	.endtunit			
	.setalphaenable true
	.setalphatestenable true
	.setalphablend "srcalpha" "invsrcalpha"
	.setalphafunc "greaterequal"
	.setalpharef $::AlphaRef
	.setalphaop 0 "add tex prev"	
	.setcolormaterial "material"
	.setzwriteenable true
sel /game/scene
}

proc gen_objeto_trans_construccion {nombre_objeto mesh_objeto textura_objeto posx posy posz sx sy sz alta_definicion} {
sel /game/scene
	new n3dnode $nombre_objeto          
	sel $nombre_objeto
	.setactive false	
	.txyz $posx $posy $posz
	.sxyz $sx $sy $sz
	new nipol roty
	sel roty
		.connect ry
		.setreptype loop       
		.addkey3f  0 	0	0	0
		.addkey3f  10.0	360	0	0		
	sel ..	        
	.preload
	new nmeshnode mesh
	mesh.preload
	mesh.setfilename "meshes:$mesh_objeto"
	mesh.setcastshadow false

	new nshadernode shader
	sel shader
		.preload
		.setrenderpri 1
		.setnumstages 1
		.setcolorop 0 "mul tex prev"
		.setalphaop 0 "mul tex prev"
		.setconst 0 0.000000 0.000000 0.000000 0.000000
		.begintunit 0
		.setaddress "wrap" "wrap"
		.setminmagfilter "linear" "linear"
		.settexcoordsrc "uv0"
		.setenabletransform true
		.txyz 0.000000 0.000000 0.000000
		.rxyz 0.000000 0.000000 0.000000
		.sxyz 100.000000 2.00000 1.000000
		.endtunit
		.setdiffuse 0.000000 0.000000 0.000000 0.000000
		.setemissive 0.000000 0.000000 0.000000 0.000000
		.setambient 0.000000 0.000000 0.000000 0.000000
		.setlightenable false
		.setalphaenable true
		.setzwriteenable false
		.setfogenable false
		.setalpharef $::AlphaRef
		.setalphablend "one" "one"
		.setcullmode "ccw"
		.setcolormaterial "diffuse"
		new nipol rotx
		sel rotx
			.connect txyz0
			.setreptype loop       
			.addkey3f  0 	0	0	0
			.addkey3f  10.0	1	0	0		
			sel ..	   		
		sel ..
	
	new ntexarraynode texture
	sel texture
	    .preload
	    .settexture 0 "textures:$textura_objeto" "none"
	sel .. 

sel /game/scene
}

proc gen_terreno {nombre_objeto mesh_objeto textura_objeto posx posy posz sx sy sz rx ry rz sombras renderpri} {
sel /game/scene
    loadtmesh $nombre_objeto $mesh_objeto $textura_objeto $renderpri
        sel $nombre_objeto
        .txyz $posx $posy $posz
        .sxyz $sx $sy $sz
        .rxyz $rx $ry $rz
    sel ..
}

proc gen_suelo {nombre_objeto mesh_objeto textura_objeto posx posy posz sx sy sz rx ry rz sombras renderpri} {
sel /game/scene
    cargasuelo $nombre_objeto $mesh_objeto $textura_objeto $renderpri
        sel $nombre_objeto
        .txyz $posx $posy $posz
        .sxyz $sx $sy $sz
        .rxyz $rx $ry $rz
    sel ..
}


proc gen_luz {nombre_luz posx posy posz cx cy cz} {
	sel /game/scene
	new n3dnode dLuz
		sel dLuz

		new n3dnode $nombre_luz
			sel $nombre_luz
			new nlightnode light
				sel light
				.setattenuation 0 0.01 0 
				.settype directional
				.setcolor $cx $cy $cz 1
				.setcastshadows true
			sel ..	
			.txyz $posx $posy $posz
		sel ..
    sel ..
}