# ---------------------------------------------------------------------------
#   entity.tcl
#   ==========
#   Set up entities.
#
# ---------------------------------------------------------------------------

new n3dnode /game/scene/sombras

# ---------------------------------------------------------------------------
# Generamos las colisiones
# ---------------------------------------------------------------------------
runscript colisiones.tcl

#catch { unset ::entidades }

proc loadmodel { model } {
    sel /game/models
    set path [/sys/servers/file2.manglepath models:$model.tcl]
    source $path
    
MiniCheckPoint { 27 }
/game.update
}

proc generarCampesino { nombre tipo posx posy } {

#################################################
#
# 		Modelo
#
#################################################

	sel /game/models
	new n3dnode $tipo
	sel $tipo
		.preload
		new nMLoader mdl
			sel mdl
			.setFileName mdl:campesino.mdl
			.preload
			sel ..
		sel ..
		
#################################################
#
# 		Sombra
#
#################################################
		
	set sombra /game/scene/sombras/$nombre
	new n3dnode $sombra
	sel $sombra
		# puts "Creada la sombra $sombra"
		.preload
		.sxyz 1.1 1.0 1.1
		new nmeshnode mesh
		mesh.preload
		mesh.setfilename "meshes:sombra3.n3d"
		new nshadernode shader
		sel shader
			.setrenderpri 1
			.setnumstages 1
			.setcolorop 0 "add tex prev"
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
			.setalphaenable true
			.setalphatestenable true
			.setalphablend "srcalpha" "invsrcalpha"
			.setalphafunc "greaterequal"
			.setalpharef $::AlphaRef
			.setalphaop 0 "replace tex prev"	
			.setcolormaterial "diffuse"
			.setzwriteenable false
		sel ..
		new ntexarraynode tex
		sel tex
		    .preload
		    .settexture 0 "textures:sombra.png" "none"
		    .sethighquality 0 true
		sel ..
		
#################################################
#
# 		Entidad
#
#################################################		
		
	sel /game/world
		new ncampesino $nombre
		sel $nombre	
			.preload
			.setentityheight 0.0
			.setcollclass "humano"
			.setcollshape "meshes:hbbox2a.n3d"
			.settouchmethod colisionHumano
			.rxyz -90 180 0
			.sxyz 1 1 1
			.txyz 0 0.0 0	
			.setposition $posx $posy
			.setnodovis /game/models/$tipo
			sel ..
	sel /game
	
MiniCheckPoint { $::numCampesinos }
/game.update
# puts "Generado Campesino $nombre\n"	
}

proc GenerarEdificio {  tipo alturas id } {
sel /game/models
loadcolormesh cons$id pagoda$alturas.n3d pagoda2.png color$tipo.png

new nconstruccion /game/world/cons$id
sel /game/world/cons$id	
	.preload
	.setentityheight -100
	.setcollclass "oculto"
	.setcollshape "meshes:pagoda$alturas.n3d"
	.settouchmethod ignorarColision	
	.setvisnode /game/models/cons$id
	.setvisible false
	set pos [expr ($id * 2)]
	.txyz $pos -50 100
	.setid $id

MiniCheckPoint { 74 }
/game.update   	
}

proc generarPersonaje { nombre tipo posx posy } {
	sel /game/world
		new npersonaje $nombre
		sel $nombre	
			.preload
			.setentityheight 0.0
			.setcollclass "humano"
			.setcollshape "meshes:hbbox2a.n3d"
			.settouchmethod colisionHumano
			.rxyz -90 180 0
			.sxyz 1 1 1
			.txyz 0 0.0 0	
			.setposition $posx $posy
			.setnodovis /game/models/$tipo
			sel ..
	sel /game
/game.update	
# puts "Generado Personaje $nombre\n"	
}

proc generarModelo { tipo modelo } {

#################################################
#
# 		Modelo
#
#################################################

	sel /game/models
	new n3dnode $tipo	
	sel $tipo
		.preload
		.sxyz 0.02 0.02 0.02
		new nMLoader mdl
			sel mdl
			.preload
			.setFileName mdl:$modelo
			sel ..
		sel ..
	
#################################################
#
# 		Sombra
#
#################################################	
	
	set sombra /game/scene/sombras/$tipo
	new n3dnode $sombra
	sel $sombra
# 		puts "Creada la sombra $sombra"
		.preload
		.sxyz 1.1 1.0 1.1		
		.setactive false
		new nmeshnode mesh
		mesh.preload
		mesh.setfilename "meshes:sombra3.n3d"
		new nshadernode shader
		sel shader
			.setrenderpri 1
			.setnumstages 1
			.setcolorop 0 "add tex prev"
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
			.setalphaenable true
			.setalphatestenable true
			.setalphablend "srcalpha" "invsrcalpha"
			.setalphafunc "greaterequal"
			.setalpharef $::AlphaRef
			.setalphaop 0 "replace tex prev"	
			.setcolormaterial "diffuse"
			.setzwriteenable false
		sel ..
		new ntexarraynode tex
		sel tex
		    .preload
		    .settexture 0 "textures:sombra.png" "none"
		sel ..	
		
puts "Generado Modelo $tipo\n"
sel /game
MiniCheckPoint { 27 }
/game.update	
}

# ---------------------------------------------------------------------------
#   Generamos el mundo
# ---------------------------------------------------------------------------

new nworld /game/world
/game.setworld /game/world

/game.update

# ---------------------------------------------------------------------------
#   Create models
# ---------------------------------------------------------------------------
new n3dnode /game/models
sel /game/models
	loadmodel "ayuntamiento"
	loadmodel "casamoneda"
	loadmodel "casaplanos"	

generarModelo jugador0 campesino.mdl
generarModelo jugador1 campesino.mdl
generarModelo jugador2 campesino.mdl
generarModelo jugador3 campesino.mdl
generarModelo jugador4 campesino.mdl
generarModelo jugador5 campesino.mdl
generarModelo jugador6 campesino.mdl
generarModelo jugador7 campesino.mdl

generarModelo ladrona ladrona.mdl
generarModelo kunoichi kunoichi.mdl
generarModelo shogun shogun.mdl
generarModelo mercader mercader.mdl
generarModelo shugenja shugenja.mdl
generarModelo maestro maestro.mdl
generarModelo ingeniero ingeniero.mdl
generarModelo samurai samurai.mdl