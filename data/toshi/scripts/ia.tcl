#proc bajaTiempoJugador { } { 
#	set ::global(tiempoTurno) [expr [/sys/servers/channel.getchannel1f time] - $::global(tiempoAnterior)]
#	set tiempo [/sys/servers/channel.getchannel1f tiempoJugador]	
#	if { $tiempo <= 0.0 } {
#		/sys/servers/channel.setchannel1f tiempoJugador 0.0
#		set posicion [/game/world/personaje.getposition]
#		/game/world/personaje.setnodovis /game/models/campesino		
#		/game/scene/hud/nombrepersonaje/tex.settexture 0 "textures:trans.png" "none"
#	} else {
#		# Por el paso del tiempo:
#		set tiempo [ expr (180 - $::global(tiempoTurno))/180]
#		/sys/servers/channel.setchannel1f tiempoJugador $tiempo
#	}
#}

