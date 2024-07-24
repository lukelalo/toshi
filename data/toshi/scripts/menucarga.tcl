proc MenuCarga { } {

#set ::ActCheck 0	
#set ::SigCheck 0
#set ::MiniCPActual 0
#set ::PorcenCarga 0

new nmenu /data/menus/menuCarga
sel /data/menus/menuCarga
	
	.setnodo "/game/scene/menus/MenuCarga"
	.setimagenfondo "menus:MenuCarga.png"
	
	set numopcion [.agregaropcion ""]
	.modificarimagenprincipalopcion $numopcion "menus:BarraEstado.png"
	.modificarimagenoveropcion $numopcion "menus:BarraEstado.png"
	.modificarimagenseleccionopcion $numopcion "menus:BarraEstado.png"
	.modificarposicionopcion $numopcion 0.39 0.631
	.modificardimensionesopcion $numopcion 0.0 0.037
}

# Menu Carga

proc CheckPoint { siguiente definicion } {
set ::ActCheck $::SigCheck	
set ::SigCheck $siguiente
set ::MiniCPActual 0.0
set ::RangoCheck [expr ($::SigCheck - $::ActCheck) ]

set ::PorcenCarga "$::ActCheck%"

set estado [expr ($::ActCheck + 0.0) / 100.0 ]

ActualizarBarra $estado $::ActCheck

/game/scene/textos/defcarga/texto.settext "$definicion..."
}

proc ActualizarBarra { estado siguiente } {

/game/scene/textos/carga/texto.settext $::PorcenCarga

sel /data/menus/menuCarga	
	.modificardimensionesopcion 0 [expr $estado * 0.276] 0.037	
	
}

proc MiniCheckPoint { total } {

set sumasig [expr ($::RangoCheck + 0.0) / ($total + 0.0) ]
set ::MiniCPActual [expr $::MiniCPActual + 0.0  + $sumasig]
set siguiente [expr int ($::ActCheck + $::MiniCPActual)]

set ::PorcenCarga "$siguiente%"

set estado [expr ($siguiente + 0.0) / 100.0 ]

ActualizarBarra $estado $siguiente
}

proc OcultaMenuCarga { } {
	CheckPoint 100 "Comenzando la partida"
	MiniCheckPoint 1
	/game/scene/textos/carga.setactive false
	/game/scene/textos/defcarga.setactive false
	OcultarMenuActivo
	MostrarMusicaMenus false
	BorrarMenusEnJuego
	
	/game.stopsong 0
	/game.playsong 1 "sounds:musica/musica_juego.mp3"
	/game.playsong 2 "sounds:ambiental/viento.ogg"
	/game.setmusicvolume 2 0.1
	/game.playsong 3 "sounds:ambiental/mar.ogg"
	
	/game.update
	/game.setiniciado true
	PonerMapeoPartida
}

proc RestaurarValoresInicialesCarga { } {
	set ::ActCheck 0	
	set ::SigCheck 0
	set ::MiniCPActual 0
	set ::PorcenCarga ""
}

proc RestaurarVolumenMusicaEfectos { } {
	/game/scene/sonidos.setactive true
}

proc PararVolumenMusicaEfectos { } {
	/game/scene/sonidos.setactive false
}