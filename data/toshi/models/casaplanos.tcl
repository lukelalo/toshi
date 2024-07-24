loadmesh casaplanos casaplanos.n3d casamoneda2.png
sel /game/models/casaplanos/shader
new nipol ip
    sel ip
    	.setreptype oneshot    
        .setchannel sel_cp
	.connect setconst0
	.addkey4f  0.0   0.5 0.5 0.5 1.0
	.addkey4f  0.3   0.0 0.0 0.0 1.0
    sel ..
sel /game/models
