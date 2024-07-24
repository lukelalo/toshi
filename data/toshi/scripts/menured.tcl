proc MenuRed { } {
new nmenu /data/menus/menuRed
sel /data/menus/menuRed
	
	.setnodo "/game/scene/menus/MenuRed"
	.setimagenfondo "menus:MenuMultijugador.png"
	
	set numopcion [.agregaropcion "SeleccionarServidor"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonCrearPartida.png"
	.modificarimagenoveropcion $numopcion "menus:BotonCrearPartidaOver.png"
	.modificarposicionopcion $numopcion 0.35 0.39
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
	set numopcion [.agregaropcion "SeleccionarCliente"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonClientePartida.png"
	.modificarimagenoveropcion $numopcion "menus:BotonClientePartidaOver.png"
	.modificarposicionopcion $numopcion 0.35 0.5
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
	set numopcion [.agregaropcion "SeleccionarVolver"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonVolver.png"
	.modificarimagenoveropcion $numopcion "menus:BotonVolverOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012	
}

# Menu Red

proc SeleccionarServidor { } {
SonidoClick
set ::MenuAnterior "/data/menus/menuRed"
MostrarMenu "/data/menus/menuServidor" -1.0 ""
}

proc SeleccionarCliente { } {
SonidoClick
set ::MenuAnterior "/data/menus/menuRed"
MostrarMenu "/data/menus/menuCliente" -1.0 ""
PonerMapeoClienteRed
}

proc SeleccionarVolver { } {
SonidoClick
MostrarMenuPrincipal
}
