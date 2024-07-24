for { set i 0 } { $i < 8 } { incr i } {
	sel /game/models
	loadmesh ayuntamiento$i ayuntamiento.n3d ayuntamiento.bmp
	sel /game/models/ayuntamiento$i/shader	
	new nipol ip
	    sel ip
		.setreptype oneshot	    
		.setchannel sel_ay$i
		.connect setconst0
		.addkey4f  0.0     0.5 0.5 0.5 1.0
		.addkey4f  0.3     0.0 0.0 0.0 1.0
	    sel ..	
}
sel /game/models
