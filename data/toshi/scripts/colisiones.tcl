# ---------------------------------------------------------------------------
#   colisiones.tcl
#   ==============
#   Set up collisions.
#
# ---------------------------------------------------------------------------
# ----------------------------------------------------------------------------
#  Generamos el servidor de colisiones
# ----------------------------------------------------------------------------
new nopcodeserver       /sys/servers/collide

sel /sys/servers/collide
	.begincollclasses
	.addcollclass "humano"
	.addcollclass "edificio"
	.addcollclass "oculto"
	.endcollclasses

# ----------------------------------------------------------------------------
#  Definimos las colisiones
# ----------------------------------------------------------------------------
sel /sys/servers/collide

	.begincolltypes
	.addcolltype "humano" "humano" "exact"
	.addcolltype "humano" "edificio" "exact"
	.addcolltype "edificio" "humano" "exact"
	.addcolltype "edificio" "edificio" "ignore"

	.addcolltype "oculto" "edificio" "ignore"
	.addcolltype "oculto" "humano" "ignore"
	.addcolltype "oculto" "oculto" "ignore"
	.addcolltype "humano" "oculto" "ignore"
	.addcolltype "edificio" "oculto" "ignore"
	
	.endcolltypes

proc ignorarColision { } { }

proc colisionHumano { } {
	processcollision
}