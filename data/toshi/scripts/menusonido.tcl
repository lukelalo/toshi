proc MenuSonido { } {
new nmenu /data/menus/menuSonido
sel /data/menus/menuSonido

	.setnodo "/game/scene/menus/MenuSonido"
	.setimagenfondo "menus:MenuSonido.png"
	
	set numopcion [.agregaropcion "SeleccionarVolverAnterior"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonVolver.png"
	.modificarimagenoveropcion $numopcion "menus:BotonVolverOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012		
	
	set numopcion [.agregaropcion ""]
	.modificarimagenprincipalopcion $numopcion "menus:BarraEstado.png"
	.modificarimagenoveropcion $numopcion "menus:BarraEstado.png"
	.modificarposicionopcion $numopcion 0.39 0.519
	.modificardimensionesopcion $numopcion [expr $::VolMusica * 0.0276 / 0.9] 0.037
	
	set numopcion [.agregaropcion "SeleccionarMenosVolMusica"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonMenos.png"
	.modificarimagenoveropcion $numopcion "menus:BotonMenos.png"
	.modificarposicionopcion $numopcion 0.3605 0.519
	.modificardimensionesopcion $numopcion 0.027 0.037	
	
	set numopcion [.agregaropcion "SeleccionarMasVolMusica"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonMas.png"
	.modificarimagenoveropcion $numopcion "menus:BotonMas.png"
	.modificarposicionopcion $numopcion 0.6665 0.519
	.modificardimensionesopcion $numopcion 0.027 0.037
	
	set numopcion [.agregaropcion ""]
	.modificarimagenprincipalopcion $numopcion "menus:BarraEstado.png"
	.modificarimagenoveropcion $numopcion "menus:BarraEstado.png"
	.modificarposicionopcion $numopcion 0.39 0.631
	.modificardimensionesopcion $numopcion [expr $::VolFx * 0.0276 / 0.9] 0.037
	
	set numopcion [.agregaropcion "SeleccionarMenosVolFx"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonMenos.png"
	.modificarimagenoveropcion $numopcion "menus:BotonMenos.png"
	.modificarposicionopcion $numopcion 0.3605 0.631
	.modificardimensionesopcion $numopcion 0.027 0.037	
	
	set numopcion [.agregaropcion "SeleccionarMasVolFx"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonMas.png"
	.modificarimagenoveropcion $numopcion "menus:BotonMas.png"
	.modificarposicionopcion $numopcion 0.6665 0.631
	.modificardimensionesopcion $numopcion 0.027 0.037	
}

# Menu Sonido

proc AplicarSonido { } {
	set volumen [expr ($::VolMusica / 9.0)]
	/game.setmusicvolume 0 $volumen
	/game.setmusicvolume 1 $volumen	
	set volumen [expr ($::VolFx / 9.0)]
	/game.setsoundvolume 0 $volumen	
	/game.setsoundvolume 1 $volumen	
	/game.setsoundvolume 2 $volumen		
}

proc SeleccionarVolverAnterior { } {
SonidoClick
MostrarMenu "$::MenuAnterior" -1.0 ""
}

proc SeleccionarMenosVolMusica { } {
sel /data/menus/menuSonido

if { $::VolMusica != 0 } {
	set ::VolMusica [expr $::VolMusica - 1]
}
	
.modificardimensionesopcion 1 [expr $::VolMusica * 0.0276 / 0.9] 0.037
AplicarSonido
}

proc SeleccionarMasVolMusica { } {
sel /data/menus/menuSonido

if { $::VolMusica != 9 } {
	set ::VolMusica [expr $::VolMusica + 1]
}

.modificardimensionesopcion 1 [expr $::VolMusica * 0.0276 / 0.9] 0.037

AplicarSonido
}

proc SeleccionarMenosVolFx { } {
sel /data/menus/menuSonido

if { $::VolFx != 0 } {
	set ::VolFx [expr $::VolFx - 1]
}

.modificardimensionesopcion 4 [expr $::VolFx * 0.0276 / 0.9] 0.037

AplicarSonido
}

proc SeleccionarMasVolFx { } {
sel /data/menus/menuSonido

if { $::VolFx != 9 } {
	set ::VolFx [expr $::VolFx + 1]
}

.modificardimensionesopcion 4 [expr $::VolFx * 0.0276 / 0.9] 0.037

AplicarSonido
}