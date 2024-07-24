# ---------------------------------------------------------------------------
#   Setup entities
# ---------------------------------------------------------------------------
for { set i 0 } { $i < $::numCampesinos } { incr i } {
	set campesino "campesino$i"
	generarCampesino $campesino $campesino [expr $i * 10] [expr  50]
}