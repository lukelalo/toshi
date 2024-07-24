proc MenuPrincipal { } {

new nmenu /data/menus/menuPrincipal
sel /data/menus/menuPrincipal
	
	.setnodo "/game/scene/menus/MenuPrincipal"
	.setimagenfondo "menus:MenuPrincipal.png"
	
	set numopcion [.agregaropcion "SeleccionarUnJugador"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonUnJugador.png"
	.modificarimagenoveropcion $numopcion "menus:BotonUnJugadorOver.png"
	.modificarposicionopcion $numopcion 0.35 0.39
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
	set numopcion [.agregaropcion "SeleccionarMultijugador"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonMultijugador.png"
	.modificarimagenoveropcion $numopcion "menus:BotonMultijugadorOver.png"
	.modificarposicionopcion $numopcion 0.35 0.5
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
	set numopcion [.agregaropcion "SeleccionarOpciones"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonOpciones.png"
	.modificarimagenoveropcion $numopcion "menus:BotonOpcionesOver.png"
	.modificarposicionopcion $numopcion 0.35 0.61
	.modificardimensionesopcion $numopcion 0.335 0.1012	
	
	set numopcion [.agregaropcion "SeleccionarCreditos"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonCreditos.png"
	.modificarimagenoveropcion $numopcion "menus:BotonCreditosOver.png"
	.modificarposicionopcion $numopcion 0.35 0.72
	.modificardimensionesopcion $numopcion 0.335 0.1012	
	
	set numopcion [.agregaropcion "SeleccionarSalir"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonSalir.png"
	.modificarimagenoveropcion $numopcion "menus:BotonSalirOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
}

# Menu Principal

proc MostrarMenuPrincipal { } {
	GenerarTextoVersion
	MostrarMenu "/data/menus/menuPrincipal" 60.0 "MostrarIntroduccion"
}

proc GenerarTextoVersion { } {
	new n3dnode /game/scene/textoversion
	new nLabel /game/scene/textoversion/texto
	sel /game/scene/textoversion/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext "Version 1.00"
		.setpositionf 0.01 0.96	
		.setalign left
		.setorder 52
}

proc EliminarTextoVersion { } {
	delete /game/scene/textoversion
}

proc SeleccionarUnJugador { } {
	SonidoClick
	EliminarTextoVersion
	MostrarMenu "/data/menus/menuJugar" -1.0 ""
}

proc SeleccionarMultijugador { } {
	SonidoClick
	EliminarTextoVersion
	MostrarMenu "/data/menus/menuRed" -1.0 ""
}

proc SeleccionarOpciones { } {
	SonidoClick
	EliminarTextoVersion
	MostrarMenu "/data/menus/menuOpciones" -1.0 ""
}

proc SeleccionarCreditos { } {
	SonidoClick
	EliminarTextoVersion
	OcultarMenuActivo
	MostrarMusicaMenus false	
	MostrarVideo "/data/videos/creditos" 320.0
}

proc SeleccionarSalir { } {
	SonidoClick
	AgregarTriggerMenu $::MenuActivo 1.0 "PararJuego"
}

proc PararJuego { } {
	/game.stop
}