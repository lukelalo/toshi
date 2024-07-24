loadmesh casamoneda casamoneda.n3d casamoneda2.png
sel /game/models/casamoneda/shader
new nipol ip
    sel ip
    	.setreptype oneshot
        .setchannel sel_cm
	.connect setconst0
	.addkey4f  0.0   0.5 0.5 0.5 1.0
	.addkey4f  0.3   0.0 0.0 0.0 1.0
    sel ..
sel /game/models
