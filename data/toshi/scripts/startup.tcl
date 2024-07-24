# ---------------------------------------------------------------------------
#   startup.tcl
#   ===========
#   Este es el primer script llamado. Crea todos los objetos servidores,
# y lanza el juego.
# ---------------------------------------------------------------------------


# ---------------------------------------------------------------------------
#   Variables globales
# ---------------------------------------------------------------------------

global GFXServer
global AnchoRes
global AltoRes
global BPP
global ZBUF

#set ::TipoVis  win
set ::TipoVis  full
set ::ResSel  3
set ::AnchoRes 1024
set ::AltoRes 768
set ::BPP 32
set ::ZBUF 16
set ::SBUF 8

#set ::GFXServer nglserver
set ::GFXServer nd3d8server

# ---------------------------------------------------------------------------
#   Ventana SPLASH
# ---------------------------------------------------------------------------

set     nebula_home    [/sys/share/assigns/home.gets]
set     home           $nebula_home

source $nebula_home/data/toshi/scripts/loadtk.tcl

proc Crear_Pantalla_Negra { } {
wm withdraw .
toplevel .fondoSplash
wm overrideredirect .fondoSplash 1
canvas .fondoSplash.c -highlightt 0 -border 0 -background black

set wscreen [winfo screenwidth .fondoSplash]
set hscreen [winfo screenheight .fondoSplash]
.fondoSplash.c config -width $wscreen -height $hscreen
set x 0
set y 0
wm geometry .fondoSplash +$x+$y
pack .fondoSplash.c
}

proc Mostrar_Pantalla_Negra { } {
raise .fondoSplash
}

proc Destruir_Pantalla_Negra { } {
destroy .fondoSplash
}

proc Splash_Init {imgfile { path "" } {delay 0}} \
{

if { $::TipoVis ==  "full" } {
	Crear_Pantalla_Negra
}


wm withdraw .
toplevel .splash
wm overrideredirect .splash 1
canvas .splash.c -highlightt 0 -border 0
cd $path
if {[catch {image create photo splash_img -format gif -file $imgfile}]} \
{ error "image $imgfile not found" }
.splash.c create image 0 0 -anchor nw -image splash_img
foreach {- - width height} [.splash.c bbox all] break
.splash.c config -width $width -height $height
set wscreen [winfo screenwidth .splash]
set hscreen [winfo screenheight .splash]
if { $::TipoVis ==  "full" } {
	set x [expr {($::AnchoRes - $width) / 2}]
	set y [expr {($::AltoRes - $height) / 2}]
} else {
	set x [expr {($wscreen - $width) / 2}]
	set y [expr {($hscreen - $height) / 2}]
}
set ::AnchoSplash $width
set ::AltoSplash $height
wm geometry .splash +$x+$y
pack .splash.c

if { $::TipoVis ==  "full" } {
	Mostrar_Pantalla_Negra
}

raise .splash
update

if {$delay > 0} \
{ after $delay { destroy .splash; wm deiconify . } }
}

Splash_Init "splash.gif" "$nebula_home/data/toshi/textures"

# ---------------------------------------------------------------------------
#   Creacion de los servidores
# ---------------------------------------------------------------------------

new     $::GFXServer       /sys/servers/gfx

new     ninputserver    /sys/servers/input
# To use Direct Input (Joystick Support etc.), replace previous line with
# new     ndxinputserver    /sys/servers/input

# basic servers 
new nscenegraph2        /sys/servers/sgraph2
new nsbufshadowserver   /sys/servers/shadow
new nchannelserver      /sys/servers/channel
new nmathserver         /sys/servers/math
new nparticleserver     /sys/servers/particle
new nspecialfxserver    /sys/servers/specialfx
new nnpkserver          /sys/servers/file2
new nprimitiveserver    /sys/servers/primitive
new nMDLRepository	/sys/servers/mdlRepository
new nFontServer 	/sys/servers/fontserver

/sys/servers/file2.setassign "scripts"	"toshi:scripts/"
/sys/servers/file2.setassign "meshes"	"toshi:meshes/"
/sys/servers/file2.setassign "models"	"toshi:models/"
/sys/servers/file2.setassign "mdl"	"toshi:mdl/"
/sys/servers/file2.setassign "textures"	"toshi:textures/"
/sys/servers/file2.setassign "sounds"	"toshi:sounds/"
/sys/servers/file2.setassign "terrain"	"toshi:terrain/"
/sys/servers/file2.setassign "levels"	"toshi:levels/"
/sys/servers/file2.setassign "fonts"	"toshi:fonts/"
/sys/servers/file2.setassign "shaders"	"toshi:shaders/"
/sys/servers/file2.setassign "fisica"	"toshi:fisica/"
/sys/servers/file2.setassign "text"	"toshi:text/"
/sys/servers/file2.setassign "menus"	"toshi:menus/"

/sys/servers/channel.setchannel1f tiempoJugador 1.0

proc runshader { shader } {
    set path [/sys/servers/file2.manglepath shaders:$shader]
    source $path
}

proc runfisica { fisica } {
    set path [/sys/servers/file2.manglepath fisica:$fisica]
    source $path
}


proc runscript { script } {
    set path [/sys/servers/file2.manglepath scripts:$script]
    source $path
}

runscript globales.tcl

# Ajustamos el tamaño de la pantalla inicial

/sys/servers/gfx.setdisplaymode "dev(0)-type($::TipoVis)-w($::AnchoRes)-h($::AltoRes)-bpp($::BPP)-zbuf($::ZBUF)-sbuf($::SBUF)"
/sys/servers/gfx.setviewvolume  -0.1 +0.1 -0.075 +0.075 +0.1 +2500

# Ajustamos el color de fondo( RGBA 0 - 1 )

/sys/servers/gfx.setclearcolor   0.0 0.0 0.0 0.0

# Creamos la consola TCL
runscript console.tcl

# Creamos el objeto del juego, lo inicializamos, y lo lanzamos.
new ngame /game
/game.init scripts:toshi.tcl

/game.run
