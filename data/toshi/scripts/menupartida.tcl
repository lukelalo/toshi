proc MenuPartida { } {

new nmenu /data/menus/menuPartida
sel /data/menus/menuPartida
	.setnodo "/game/scene/menus/MenuPartida"
	.setimagenfondo "menus:MenuPartida.png"
	
	set numopcion [.agregaropcion "SeleccionarVolverPartida"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonContinuarPartida.png"
	.modificarimagenoveropcion $numopcion "menus:BotonContinuarPartidaOver.png"
	.modificarposicionopcion $numopcion 0.35 0.39
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
	set numopcion [.agregaropcion "SeleccionarSonidoPartida"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonSonido.png"
	.modificarimagenoveropcion $numopcion "menus:BotonSonidoOver.png"
	.modificarposicionopcion $numopcion 0.35 0.5
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
	set numopcion [.agregaropcion "SeleccionarAyudaPartida"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonAyuda.png"
	.modificarimagenoveropcion $numopcion "menus:BotonAyudaOver.png"
	.modificarposicionopcion $numopcion 0.35 0.61
	.modificardimensionesopcion $numopcion 0.335 0.1012	
	
	set numopcion [.agregaropcion "SeleccionarSalirPartida"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonSalir.png"
	.modificarimagenoveropcion $numopcion "menus:BotonSalirOver.png"
	.modificarposicionopcion $numopcion 0.35 0.72
	.modificardimensionesopcion $numopcion 0.335 0.1012	
}

# Funciones

proc TratarMenuPartida { } {
	if { $::JuegoPausado } {
		OcultarMenuActivo
		PonerMapeoPartida
		MostrarMusicaMenus false
		/game.stopsong 0
		/game.pausesong 1 false
		# /game.playsong 1 "sounds:musica/musica_juego.mp3"
		/game.playsong 2 "sounds:ambiental/viento.ogg"
		/game.playsong 3 "sounds:ambiental/mar.ogg"
		/game.pauseallsounds false		
		/game/scene/hud.setactive true
		
	} else {
		MostrarMenu "/data/menus/menuPartida" -1 ""
		PonerMapeoMenuPartida
		/game.pausesong 1 true
		/game.stopsong 2
		/game.stopsong 3
		/game.pauseallsounds true
		MostrarMusicaMenus true
		/game/scene/hud.setactive false
	}
	
	PausarJuego
}

proc SeleccionarVolverPartida { } {
	SonidoClick
	TratarMenuPartida
}

proc SeleccionarAyudaPartida { } {
	SonidoClick
	set ::MenuAnterior "/data/menus/menuPartida"
	MostrarMenu "/data/menus/menuAyuda" -1.0 ""
}

proc SeleccionarSonidoPartida { } {
	SonidoClick
	set ::MenuAnterior "/data/menus/menuPartida"
	MostrarMenu "/data/menus/menuSonido" -1.0 ""
}

proc SeleccionarSalirPartida { } {
	SonidoClick
	FinalizarPartida
}

proc FinalizarPartida { } {
	PausarJuego
	CargarMenusEnJuego	
	BorrarPartida
	MostrarMenuPrincipal
}
