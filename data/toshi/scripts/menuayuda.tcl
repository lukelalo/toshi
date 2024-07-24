proc MenuAyuda { } {
new nmenu /data/menus/menuAyuda
sel /data/menus/menuAyuda
	.setnodo "/game/scene/menus/MenuAyuda"
	.setimagenfondo "menus:MenuAyuda.png"

	set numopcion [.agregaropcion "SeleccionarVolverAnterior"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonVolver.png"
	.modificarimagenoveropcion $numopcion "menus:BotonVolverOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012	
	
	set numopcion [.agregaropcion "SeleccionarBotonAyuda"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonOk.png"
	.modificarimagenoveropcion $numopcion "menus:BotonOk.png"
	.modificarposicionopcion $numopcion 0.6665 0.519
	.modificardimensionesopcion $numopcion 0.027 0.037
}

# Menu Ayuda

proc SeleccionarBotonAyuda { } {
sel /data/menus/menuAyuda
	if { $::AyudaActiva } {
		set ::AyudaActiva false
		.modificarimagenprincipalopcion 1 "textures:trans.png"
		.modificarimagenoveropcion 1 "textures:trans.png"
	} else {
		set ::AyudaActiva true
		.modificarimagenprincipalopcion 1 "menus:BotonOk.png"
		.modificarimagenoveropcion 1 "menus:BotonOk.png"	
	}

	/game.mostrarayuda $::AyudaActiva
}
