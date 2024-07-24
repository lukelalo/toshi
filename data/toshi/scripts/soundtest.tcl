sel /game/scene
	new nlistenernode listener
	sel listener
	    setdopplerfactor 1.0
	    setrollofffactor 1.0
	sel ..
	
	new n3dnode sonidos
	sel /game/scene/sonidos                 
		new n3dnode ambiente
		sel ambiente                      
			new nsoundnode sound
			sel sound
				.setambient true
				.setlooping true
				.setvolume 0.8
				.setfrequency 1.0	
				.setvelocitychannel velocity
				.setdoppler false
				.settriggerchannel soundtrigger
                        	.setfile "sounds:mar.ogg"
                        	sel ..   	
	sel /game/scene/sonidos                 
		new n3dnode ambiente2
		sel ambiente2                      
			new nsoundnode sound
			sel sound
				.setambient true
				.setlooping true
				.setvolume 0.85
				.setfrequency 1.0	
				.setvelocitychannel velocity
				.setdoppler false
				.settriggerchannel soundtrigger
                        	.setfile "sounds:yingyang.ogg"
                        	sel ..                        	
                        	

#				
#sel /game/scene

