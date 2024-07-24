# ---------------------------------------------------------------------------
#   toshi.tcl
#   =========
#   Creación del juego. Crea todos los objetos necesarios para lanzar el
#   juego.
#
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Abrimos el Display
# ---------------------------------------------------------------------------

/sys/servers/gfx.opendisplay

set x [expr {($::AnchoRes - $::AnchoSplash) / 2}]
set y [expr {($::AltoRes - $::AltoSplash) / 2}]
wm geometry .splash +$x+$y

if { $::TipoVis ==  "full" } {
	.fondoSplash.c config -width $::AnchoRes -height $::AltoRes
	pack .fondoSplash.c
	raise .fondoSplash
}

raise .splash
update

# ---------------------------------------------------------------------------
#   Gestión de la cámara en los menús
# ---------------------------------------------------------------------------
new n3dnode /game/camera
new n3dnode /game/lookat

/game/camera.txyz	1	6	1
/game/lookat.txyz	0	0    	0
/game/camera.rxyz	-90	-135   	0

# ---------------------------------------------------------------------------
#   Carga de scripts que tienen procedimientos útiles
# ---------------------------------------------------------------------------
new n3dnode /game/scene
/game.setscene /game/scene

runscript ia.tcl
runscript generar.tcl 
runscript objetos.tcl
runscript utils.tcl                 
runscript mesh.tcl                  
runscript movimientos.tcl

gen_objeto_trans "seleccion" "destino.n3d" "seleccion3.png" "0" "-100" "0" "1" "1" "1" "false"
gen_objeto_trans_construccion "selcons" "seleccion_construccion.n3d" "efecto_nube.png" "0" "0" "0" "1.2" "1" "1.2" "true"

runscript partida.tcl

# puts "Procedimientos Utiles Cargados"

# ----------------------------------------------------------------------------
#  Generamos los sprites de los vídeos y de los menús
# ----------------------------------------------------------------------------

# Me cargo los menús después porque si intentas hacer un preload de las 
# texturas antes de abrir el gfxserver, casca
runscript videos.tcl
runscript menus.tcl

GenerarSpritesVideos
GenerarSpritesMenus


# puts "Videos y Menús cargados"

# ----------------------------------------------------------------------------
#  Generamos los cursores
# ----------------------------------------------------------------------------

/sys/servers/gfx.begincursors 7

/sys/servers/gfx.setcursor 0 textures:cursor0.png 6 1
/sys/servers/gfx.setcursor 1 textures:cursor2.png 6 1
/sys/servers/gfx.setcursor 2 textures:cursor1.png 6 1
/sys/servers/gfx.setcursor 3 textures:cursor3.png 6 1
/sys/servers/gfx.setcursor 4 textures:cursor4.png 6 1
/sys/servers/gfx.setcursor 5 textures:cursor5.png 6 1
/sys/servers/gfx.setcursor 6 textures:cursor6.png 6 1
/sys/servers/gfx.endcursors

/sys/servers/gfx.showcursor

# puts "Cursores Generados"

# ---------------------------------------------------------------------------
# Mostramos el Display
# ---------------------------------------------------------------------------

/sys/servers/gfx.showdisplay
/game.update

if { $::TipoVis ==  "full" } {
	Destruir_Pantalla_Negra
}

destroy .splash

# puts "Display Mostrado"

# ---------------------------------------------------------------------------
#   Video inicial del juego
# ---------------------------------------------------------------------------

MostrarIntroduccion