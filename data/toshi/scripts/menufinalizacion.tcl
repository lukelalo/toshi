proc MenuFinalizacion { } {
new nmenu /data/menus/menuFinalizacion
sel /data/menus/menuFinalizacion
	.setnodo "/game/scene/menus/MenuFinalizacion"
	.setimagenfondo "menus:MenuFinalizacion.png"
	
	set numopcion [.agregaropcion "SeleccionarContinuar"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonContinuar.png"
	.modificarimagenoveropcion $numopcion "menus:BotonContinuarOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012
}

proc SeleccionarContinuar { } {
	SonidoClick
	MostrarMenu "/data/menus/menuResultados" -1.0 ""
	MostrarTextoMenuResultados true
}

proc TratarMenuFinalizacion { } {
	/game.stopallsounds
	/game.stopsong 7
	/game.stopsong 2
	/game.stopsong 1
	MostrarMenu "/data/menus/menuFinalizacion" -1 ""
	PonerMapeoMenus
	MostrarMusicaMenus true
	/game/scene/hud.setactive false
	PausarJuego
}
