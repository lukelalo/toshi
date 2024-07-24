#
#   Std-Lights, used by most of the demos
#
#   upd.:   30-Jul-99
#
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#

sel /game/scene

new n3dnode dlight
	sel dlight

	new n3dnode light1
		sel light1
		new nlightnode light
			sel light
		        .setattenuation 1 0 0 
			.settype  point
			.setcolor 1.0 1.0 1 1
		sel ..	
		.txyz -50 5 50
	sel ..

	new n3dnode light2
		sel light2
		new nlightnode light
			sel light
			.setattenuation 1 0 0 
			.settype  point
			.setcolor 1.0 1.0 1 1
		sel ..	
		.txyz 50 5 -30
	sel ..
sel ..
