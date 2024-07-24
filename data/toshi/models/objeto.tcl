#loadmesh humano minoS1.n3d minotauro.bmp
sel /game/scene
new n3dnode arbol1
sel arbol1
	.rxyz -90 0 0
	.sxyz 0.3 0.3 0.3
	.txyz 0 0.7 0		
	new nMLoader mdl
		sel mdl
		.setFileName mdl:arch_japan.mdl
		sel ..