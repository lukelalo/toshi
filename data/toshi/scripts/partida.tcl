proc BorrarPartida { } {
delete /game/world
if { $::JuegoEnRed } {
delete /game/nets
delete /sys/share/network
} else {
delete /game/ias
}
delete /game/models
delete /game/scene/escenario
delete /game/scene/hud
delete /game/scene/sombras
delete /data/mapa
delete /data/terreno
delete /game/camaraPartida

/game/scene/seleccion.setactive false
/game/scene/selcons.setactive false
/game.setiniciado false
/game.stopallsounds
/game.stopsong 7
/game.stopsong 2
/game.stopsong 1
MostrarMusicaMenus true
ActualizarBarra 0 0
}


proc ComenzarPartida { } {

CrearTextos
MostrarTextos
AplicarSonido

# ---------------------------------------------------------------------------
#   Gestión de la cámara
# ---------------------------------------------------------------------------

runscript camara.tcl  
# puts "Cámara Generada"

CheckPoint 8 "Cargando las entidades"

# ---------------------------------------------------------------------------
#   Gestión de las entidades
# ---------------------------------------------------------------------------
runscript entity.tcl

# puts "Entidades Generadas"

CheckPoint 9 "Cargando los jugadores"
/game.update

# ---------------------------------------------------------------------------
#   Mostramos a los jugadores
# ---------------------------------------------------------------------------
if { $::JuegoEnRed == false} {

	set ::numCampesinos [expr ($::NumJugadores + 1) * 2]
	/game.update
} else {
	set ::numCampesinos 0
}

CheckPoint 10 "Cargando la escena"
/game.update

# ----------------------------------------------------------------------------
#  Generamos la escena
# ----------------------------------------------------------------------------
runscript scene.tcl

# puts "Escena Generada"

CheckPoint 12 "Cargando los campesinos"
/game.update

# ----------------------------------------------------------------------------
#  Generamos los campesinos
# ----------------------------------------------------------------------------
runscript campesinos.tcl

# puts "Campesinos Generados"

CheckPoint 15 "Cargando el interfaz de usuario"
/game.update

# ----------------------------------------------------------------------------
#  Generamos el HUD
# ----------------------------------------------------------------------------
sel /game/scene
runscript hud.tcl

# puts "HUD Generado"

CheckPoint 99 "Cargando el mapa"
/game.update
# ----------------------------------------------------------------------------
#  Generamos el Mapa
# ----------------------------------------------------------------------------
runscript carga_mapa.tcl

# puts "Mapa Cargado"

# ---------------------------------------------------------------------------
#   Generamos la partida
# ---------------------------------------------------------------------------

if { $::JuegoEnRed } {
	/game.comenzarpartidared $::EsServidor
} else {
	/game.comenzarpartidalocal [expr $::NumJugadores + 1] 
	}

if { $::JuegoEnRed == false } {
	CheckPoint 100 "Generando inteligencia artificial"	
} else {
	CheckPoint 100 "Generando interfaz de red"	
}
/game.update

# ---------------------------------------------------------------------------
#   Creación del IA
# ---------------------------------------------------------------------------
if { $::JuegoEnRed == false } {
	new nroot /game/ias
	/game.setia /game/ias

	for { set i 1 } { $i < [expr $::NumJugadores + 1] } { incr i } {
		new niajugador /game/ias/jugador$i
		}

# 	puts "IA Cargada"
 
	MiniCheckPoint 1
	/game.update

}

# ----------------------------------------------------------------------------
#  ¡Comienza la partida!
# ----------------------------------------------------------------------------
AgregarTriggerMenu $::MenuActivo 3.0 "OcultaMenuCarga"

/game.update
}