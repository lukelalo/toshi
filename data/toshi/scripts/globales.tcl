# ---------------------------------------------------------------------------
#   Variable que indican si se está en modo debug o no.
# ---------------------------------------------------------------------------
global debug
set debug true
#set debug false

# ---------------------------------------------------------------------------
#   Variables globales
# ---------------------------------------------------------------------------

set ::numCampesinos		2
set ::global(angulo)		-30
set ::global(incAngulo)		2
set ::global(tiempoAnterior)	0.0
set ::global(tiempoTurno)	0.0
set ::global(tiempoBusqueda)	0.0
set ::global(tiempoBusquedaA)	0.0
set ::edfSelect			""
set ::visDebug			false

set ::NumJugadores		1
set ::JuegoEnRed		false
set ::EsServidor		false
set ::VolMusica			9
set ::VolFx			9
set ::AyudaActiva		true

set ::MenuActivo		""
set ::JuegoPausado 		false

# ---------------------------------------------------------------------------
#   Variables globales
# ---------------------------------------------------------------------------

global AlphaRef
if {$::GFXServer == "nglserver"} {
	set ::AlphaRef 	0.0625
	set ::skybox "wrap"
	}

if {$::GFXServer == "nd3d8server"} {
	set ::AlphaRef 	0.9375
	set ::skybox "clamp"
	}
