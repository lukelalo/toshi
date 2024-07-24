#--------------------------------------
# Procedimientos para la carga del mapa
#--------------------------------------

# ---------------------------------------------------------------------------
#   Load a terrain with default attributes
# ---------------------------------------------------------------------------
proc loadterrain { texturemap } {
    new n3dnode terrain
    sel terrain
    	.preload
    	
        new nmapnode map
		sel map
		.preload
		.setmap "/data/terreno"
		.setblocksize 15
		.seterror 1
		.setdetailsize 1.0
		#0.00625
        sel ..
        new ntexarraynode tex
        sel tex
            .preload
            .settexture 0 $texturemap "none"   
            #.settexture 1 "textures:seleccion3.png" "none" 
        sel ..
        new nshadernode sn
        sel sn
		.preload
		.setrenderpri -3
		.setnumstages 2
		.setcolorop 0 "add tex prev"
		.setconst 0 0.000000 0.000000 0.000000 0.000000
		.begintunit 0
		.setaddress "wrap" "wrap"
		.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
		.settexcoordsrc "uv0"
		.setenabletransform true            
		.sxyz 16 16 16               
		.endtunit	

		#.setdiffuse 0.2 0.2 0.2 0.2
		#.setemissive 0 0 0 0
		#.setambient 0 0 0 0
		#.setlightenable true
		#.setalphaenable false
		#.setzwriteenable true
		#.setfogenable true
		
		
		#.preload
		#.setrenderpri -2
		#.setnumstages 2
		.setcolorop 1 "replace tex prev"
		.setalphaop 1 "replace tex prev"
		.setconst 0 0.000000 0.000000 0.000000 0.000000
		.begintunit 1
		.setaddress "clamp" "clamp"
		.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
		.settexcoordsrc "uv0"
		.setenabletransform true            
		.sxyz 8 8 8 
		.txyz -4 -4 0
		.endtunit	

		.setdiffuse 0.8 0.8 0.8 0.8
		.setemissive 0 0 0 0
		.setambient 0 0 0 0
		.setlightenable true
		.setalphaenable true
		.setzwriteenable true
		.setfogenable true		

        sel ..
    sel ..
/game.update      
}

# ---------------------------------------------------------------------------
#   Load a terrain with default attributes and alpha channel
# ---------------------------------------------------------------------------
proc loadterrainalpha { } {
    new n3dnode terrainalpha
    sel terrainalpha
    	.preload
    	txyz 0 0.01 0
        new nmapnode map
		sel map
		.setmap "/data/terrenoalpha"		
		.preload
		.setblocksize 15
		.seterror 1
		.setdetailsize 1.0
		#0.00625
        sel ..
        new ntexarraynode tex
        sel tex
            .preload
            .settexture 0 "textures:efecto_nube.png" "none"              
        sel ..
        new nshadernode sn
        sel sn
		.preload
		.setrenderpri -2
		.setnumstages 2
		.setcolorop 0 "mul tex prev"
		.setalphaop 0 "mul tex prev"
		.setconst 0 0.000000 0.000000 0.000000 0.000000
		.begintunit 0
		.setaddress "wrap" "wrap"
		.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
		.settexcoordsrc "uv0"
		.setenabletransform true            
		.sxyz 1 1 1 
		.endtunit	

		.setlightenable false
		.setalphaenable true
		.setzwriteenable false
		.setfogenable false
		.setalpharef $::AlphaRef
		.setalphablend "one" "one"
		.setcullmode "ccw"
		.setcolormaterial "diffuse"
		
		new nipol movimiento_hierba
		sel movimiento_hierba
			.connect txyz0
			.setreptype loop
			.addkey3f 0  0 0 0
			.addkey3f 10 1 0 0
		sel ..
        sel ..
    sel ..
/game.update      
}

proc CrearMar { } {

	new n3dnode /game/scene/escenario/mar
	new nmeshnode /game/scene/escenario/mar/mesh
	new nshadernode /game/scene/escenario/mar/shader
	new ntexarraynode /game/scene/escenario/mar/tex

	sel /game/scene/escenario/mar/
		txyz 40.0 -1.8 40.0
		
		new nipol ty
		sel ty
			.connect ty
			.setreptype loop
			.addkey1f 0   -1.8
			.addkey1f 5   -1.7
			.addkey1f 20  -1.8
		sel ..

	sel /game/scene/escenario/mar/mesh
		.preload
		setfilename meshes:mar.n3d

	sel /game/scene/escenario/mar/tex
		.preload
		settexture 0 "terrain:aguareflejo2.png" "none"

	sel /game/scene/escenario/mar/shader
		setrenderpri -5
		setlightenable true
		setnumstages 1
		begintunit 0 
			setconst0 0.2 0.2 0.2 0.1 
			setcolorop 0 "add tex prev"	
			setalphaop 0 "add tex prev"		
			setaddress "clamp" "clamp"
			setminmagfilter "linear" "linear"
			settexcoordsrc "uv0"
			setenabletransform false
		endtunit
		setdiffuse 0.2 0.2 0.2 0.0
		setemissive 0.0 0.0 0.0 0.0
		setambient 0.0 0.0 0.0 0.0
		setalphaenable true
		setzwriteenable true
		setfogenable true
		
	new n3dnode /game/scene/escenario/olas
	new nmeshnode /game/scene/escenario/olas/mesh
	new nshadernode /game/scene/escenario/olas/shader
	new ntexarraynode /game/scene/escenario/olas/tex

	sel /game/scene/escenario/olas/
		.txyz 40.0 -1.75 40.0
		new nipol roty
		sel roty
			.connect ry
			.setreptype loop
			.addkey1f 0   0
			.addkey1f 100 360
		sel ..
			
		new nipol ty
		sel ty
			.connect ty
			.setreptype loop
			.addkey1f 0   -1.75
			.addkey1f 5  -1.65
			.addkey1f 20  -1.75			
		sel ..

	sel /game/scene/escenario/olas/mesh
		.preload
		setfilename meshes:mar.n3d

	sel /game/scene/escenario/olas/tex
		.preload
		settexture 0 "terrain:olas2.png" "none"

	sel /game/scene/escenario/olas/shader
		setrenderpri -5
		setlightenable true
		setnumstages 1
		begintunit 0 
			setconst0 0.2 0.2 0.2 0.1 
			setcolorop 0 "add tex prev"	
			setalphaop 0 "add tex prev"		
			setaddress "clamp" "clamp"
			setminmagfilter "linear" "linear"
			settexcoordsrc "uv0"
			setenabletransform false
		endtunit
		setdiffuse 0.2 0.2 0.2 0.0
		setemissive 0.0 0.0 0.0 0.0
		setambient 0.0 0.0 0.0 0.0
		setalphaenable true
		setzwriteenable true
		setfogenable true		
}



proc CrearTerreno { } {

	new nmap /data/terreno
	sel /data/terreno
		.setheightrange -2 0
		.setgridinterval 0.3	
		.setheightmap "terrain:alturas2.bmp"

	sel /game/models
		loadterrain "terrain:terreno2.png"

	sel /game/world
		setterrain /game/models/terrain
		/game.update	

	new nmapa /data/mapa
	/data/mapa.setmap [expr $::NumJugadores + 1]

}

CrearMar
CrearTerreno

#new nmap /data/terrenoalpha
#sel /data/terrenoalpha
#	.setheightrange -2 0
#	.setgridinterval 0.3	
#	.setheightmap "toshi:terrain/alturas2.bmp"
#
#sel /game/scene
#	loadterrainalpha