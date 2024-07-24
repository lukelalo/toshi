proc MenuServidor { } {
new nmenu /data/menus/menuServidor
sel /data/menus/menuServidor
	.setnodo "/game/scene/menus/MenuServidor"
	.setimagenfondo "menus:MenuServidor.png"

	set numopcion [.agregaropcion "SeleccionarVolverAnteriorServidorRed"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonVolver.png"
	.modificarimagenoveropcion $numopcion "menus:BotonVolverOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012	
	
	set numopcion [.agregaropcion ""]
	.modificarimagenprincipalopcion $numopcion "textures:trans.png"
	.modificarimagenoveropcion $numopcion "textures:trans.png"
	.modificarposicionopcion $numopcion 0.6665 0.519
	.modificardimensionesopcion $numopcion 0.025 0.035
	
	set numopcion [.agregaropcion "SeleccionarComenzarPartidaServidor"]
	.modificarimagenprincipalopcion $numopcion "menus:LineaComenzarPartida.png"
	.modificarimagenoveropcion $numopcion "menus:LineaComenzarPartidaOver.png"
	.modificarposicionopcion $numopcion 0.1041 0.62
	.modificardimensionesopcion $numopcion 0.2 0.055	
}

# Menu Servidor

proc SeleccionarVolverAnteriorServidorRed { } {
SeleccionarVolverAnterior
}

proc SeleccionarComenzarPartidaServidor { } {
SonidoClick
set ::JuegoEnRed		true
set ::EsServidor		true
#AgregarTriggerMenu $::MenuActivo 1.0 "ActualizarComenzar"

runscript net.tcl

servidorRed 5050

sel /data/menus/menuServidor

.modificarimagenprincipalopcion 1 "textures:numero0.png"
.modificarimagenoveropcion 1 "textures:numero0.png"

.modificaraccionopcion 0  ""
.modificarimagenprincipalopcion 0 "textures:trans.png"
.modificarimagenoveropcion 0 "textures:trans.png"

.modificaraccionopcion 2  ""
.modificarimagenprincipalopcion 2 "textures:trans.png"
.modificarimagenoveropcion 2 "textures:trans.png"

set ::NumClientes 0
}


proc ConectadoCliente { } {

sel /data/menus/menuServidor

.modificaraccionopcion 0 "SeleccionarVolverAnteriorServidorRed"
.modificarimagenprincipalopcion 0 "menus:BotonVolver.png"
.modificarimagenoveropcion 0 "menus:BotonVolverOver.png"
.modificarposicionopcion 0 0.35 0.88
.modificardimensionesopcion 0 0.335 0.1012	

.modificaraccionopcion 1 ""
.modificarimagenprincipalopcion 1 "textures:trans.png"
.modificarimagenoveropcion 1 "textures:trans.png"
.modificarposicionopcion 1 0.6665 0.519
.modificardimensionesopcion 1 0.025 0.035

.modificaraccionopcion 2 "SeleccionarComenzarPartidaServidor"
.modificarimagenprincipalopcion 2 "menus:LineaComenzarPartida.png"
.modificarimagenoveropcion 2 "menus:LineaComenzarPartidaOver.png"
.modificarposicionopcion 2 0.1041 0.62
.modificardimensionesopcion 2 0.2 0.055

set ::NumClientes [expr $::NumClientes + 1]
	if { $::NumClientes == 1 } {
		ActualizarComenzar
	}
}