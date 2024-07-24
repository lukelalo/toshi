sel /game/scene
	new n3dnode sonidos
	sel /game/scene/sonidos
		set active false
		.preload
		new n3dnode mar
		sel mar
			.preload
			new nsoundnode sound
			sel sound
				.preload
				.setambient true
				.setlooping true
				.setvolume 0.0
				.setfrequency 1.0	
				.setdoppler false
                        	.setfile "sounds:mar2.ogg"
                        	sel ..   	
sel /game/scene

