# ---------------------------------------------------------------------------
#   Gesti�n de la c�mara en el juego
# ---------------------------------------------------------------------------

new ncamera	/game/camaraPartida
sel /game/camaraPartida
	.setworld	/game/world
	.setnode	/game/scene/camera
	
	# Seguimos al jugador
	.settargetposition 0 0 -2
	.setstyle	"totalstationary"	

/game.setcamera	/game/camaraPartida