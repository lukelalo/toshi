#loadmesh humano minoS1.n3d minotauro.bmp
new n3dnode humano
sel humano
	new nMLoader mdl
		sel mdl
		.setFileName models/clan_killer.mdl
		sel ..
	loadmesh malla hbbox.n3d minotauro.bmp
	new nshadernode shader
		sel shader
		source shader.n
		sel ..
	
sel /game/models