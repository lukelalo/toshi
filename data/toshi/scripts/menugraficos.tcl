proc MenuGraficos { } {
new nmenu /data/menus/menuGraficos
sel /data/menus/menuGraficos
	.setnodo "/game/scene/menus/MenuGraficos"
	.setimagenfondo "menus:MenuGraficos.png"
	
	set numopcion [.agregaropcion "SeleccionarAplicarGraficos"]
	.modificarimagenprincipalopcion $numopcion "menus:LineaAplicarCambios.png"
	.modificarimagenoveropcion $numopcion "menus:LineaAplicarCambiosOver.png"
	.modificarposicionopcion $numopcion 0.1041 0.62
	.modificardimensionesopcion $numopcion 0.2 0.0613
	
	set numopcion [.agregaropcion "SeleccionarVolverAnterior"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonVolver.png"
	.modificarimagenoveropcion $numopcion "menus:BotonVolverOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
	set numopcion [.agregaropcion ""]
	.modificarimagenprincipalopcion $numopcion "menus:BarraEstado.png"
	.modificarimagenoveropcion $numopcion "menus:BarraEstado.png"
	.modificarposicionopcion $numopcion 0.39 0.519
	.modificardimensionesopcion $numopcion 0.276 0.037
	.modificardimensionesopcion $numopcion [expr ($::ResSel-1) * 0.138] 0.037
	
	set numopcion [.agregaropcion ""]
	.modificarimagenprincipalopcion $numopcion "menus:res$::ResSel.png"
	.modificarimagenoveropcion $numopcion "menus:res$::ResSel.png"
	.modificarimagenseleccionopcion $numopcion "menus:res$::ResSel.png"
	.modificarposicionopcion $numopcion 0.495 0.515
	.modificardimensionesopcion $numopcion 0.06 0.042
	
	set numopcion [.agregaropcion "SeleccionarMenosResolucion"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonMenos.png"
	.modificarimagenoveropcion $numopcion "menus:BotonMenos.png"
	.modificarposicionopcion $numopcion 0.3605 0.519
	.modificardimensionesopcion $numopcion 0.027 0.037	
	
	set numopcion [.agregaropcion "SeleccionarMasResolucion"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonMas.png"
	.modificarimagenoveropcion $numopcion "menus:BotonMas.png"
	.modificarposicionopcion $numopcion 0.6665 0.519
	.modificardimensionesopcion $numopcion 0.027 0.037
}

# Menu Graficos

proc SeleccionarAplicarGraficos { } {
SonidoClick
OcultarMenuActivo
switch -exact -- $::ResSel {
		1 {
		CambiarResolucion 640 480
		}

		2 { 
		CambiarResolucion 800 600	
		}

		3 { 
		CambiarResolucion 1024 768	
		}

		4 { 
		CambiarResolucion 1280 1024	
		}
	}
MostrarMenuActivo
}

proc SeleccionarVolverOpciones { } {
SonidoClick
MostrarMenu "/data/menus/menuOpciones" -1.0 ""
}

proc SeleccionarMenosResolucion { } {
sel /data/menus/menuGraficos

if { $::ResSel != 1 } {
	set ::ResSel [expr $::ResSel - 1]
}
	
.modificarimagenprincipalopcion 3 "menus:res$::ResSel.png"
.modificarimagenoveropcion 3 "menus:res$::ResSel.png"
.modificarimagenseleccionopcion 3 "menus:res$::ResSel.png"

.modificardimensionesopcion 2 [expr ($::ResSel-1) * 0.138] 0.037	
}

proc SeleccionarMasResolucion { } {
sel /data/menus/menuGraficos

if { $::ResSel != 3 } {
	set ::ResSel [expr $::ResSel + 1]
}

.modificarimagenprincipalopcion 3 "menus:res$::ResSel.png"
.modificarimagenoveropcion 3 "menus:res$::ResSel.png"
.modificarimagenseleccionopcion 3 "menus:res$::ResSel.png"

.modificardimensionesopcion 2 [expr ($::ResSel-1) * 0.138] 0.037	
}
