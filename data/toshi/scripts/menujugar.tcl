proc MenuJugar { } {

new nmenu /data/menus/menuJugar
sel /data/menus/menuJugar
	
	.setnodo "/game/scene/menus/MenuJugar"
	.setimagenfondo "menus:MenuUnJugador.png"
	
	set numopcion [.agregaropcion "SeleccionarComenzar"]
	.modificarimagenprincipalopcion $numopcion "menus:LineaJugar.png"
	.modificarimagenoveropcion $numopcion "menus:LineaJugarOver.png"
	.modificarposicionopcion $numopcion 0.1041 0.62
	.modificardimensionesopcion $numopcion 0.0764 0.0613
	
	set numopcion [.agregaropcion "SeleccionarVolver"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonVolver.png"
	.modificarimagenoveropcion $numopcion "menus:BotonVolverOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012				
	
	set numopcion [.agregaropcion ""]
	.modificarimagenprincipalopcion $numopcion "menus:BarraEstado.png"
	.modificarimagenoveropcion $numopcion "menus:BarraEstado.png"
	.modificarposicionopcion $numopcion 0.39 0.519
	.modificardimensionesopcion $numopcion 0.276 0.037
	.modificardimensionesopcion $numopcion [expr $::NumJugadores * 0.0394] 0.037		
	
	set numopcion [.agregaropcion ""]
	.modificarimagenprincipalopcion $numopcion "textures:numero$::NumJugadores.png"
	.modificarimagenoveropcion $numopcion "textures:numero$::NumJugadores.png"
	.modificarimagenseleccionopcion $numopcion "textures:numero$::NumJugadores.png"
	.modificarposicionopcion $numopcion 0.512 0.519
	.modificardimensionesopcion $numopcion 0.027 0.03
	
	set numopcion [.agregaropcion "SeleccionarMenosJugadores"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonMenos.png"
	.modificarimagenoveropcion $numopcion "menus:BotonMenos.png"
	.modificarposicionopcion $numopcion 0.3605 0.519
	.modificardimensionesopcion $numopcion 0.027 0.037	
	
	set numopcion [.agregaropcion "SeleccionarMasJugadores"]	
	.modificarimagenprincipalopcion $numopcion "menus:BotonMas.png"
	.modificarimagenoveropcion $numopcion "menus:BotonMas.png"
	.modificarposicionopcion $numopcion 0.6665 0.519
	.modificardimensionesopcion $numopcion 0.027 0.037
	
}

# Menu Jugar

proc SeleccionarComenzar { } {
SonidoClick
ActualizarComenzar
}

proc ActualizarComenzar { } {
OcultarMenuActivo
RestaurarValoresInicialesCarga
MostrarMenu "/data/menus/menuCarga" 1.0 "ComenzarPartida"
}


proc SeleccionarVolver { } {
SonidoClick
MostrarMenuPrincipal
}

proc SeleccionarMenosJugadores { } {
sel /data/menus/menuJugar

if { $::NumJugadores != 1 } {
	set ::NumJugadores [expr $::NumJugadores - 1]
}
	
.modificarimagenprincipalopcion 3 "textures:numero$::NumJugadores.png"
.modificarimagenoveropcion 3 "textures:numero$::NumJugadores.png"
.modificarimagenseleccionopcion 3 "textures:numero$::NumJugadores.png"	

.modificardimensionesopcion 2 [expr $::NumJugadores * 0.0394] 0.037	
}

proc SeleccionarMasJugadores { } {
sel /data/menus/menuJugar

if { $::NumJugadores != 7 } {
	set ::NumJugadores [expr $::NumJugadores + 1]
}

.modificarimagenprincipalopcion 3 "textures:numero$::NumJugadores.png"
.modificarimagenoveropcion 3 "textures:numero$::NumJugadores.png"
.modificarimagenseleccionopcion 3 "textures:numero$::NumJugadores.png"	

.modificardimensionesopcion 2 [expr $::NumJugadores * 0.0394] 0.037	
}