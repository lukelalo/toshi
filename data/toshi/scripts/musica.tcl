sel /game/scene/sonidos                 
	new n3dnode musica
	sel musica
		.preload
		new nsoundnode sound
		sel sound
			.preload
			.setambient true
			.setlooping true
			.setvolume 0.85
			.setfrequency 1.0
			.setdoppler false
			.setfile "sounds:cancion1_toshi.ogg"
			sel ..                        	

sel /game/scene

# puts "Musica Inicializada"