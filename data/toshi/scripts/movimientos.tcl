#----------------------------------------------------------
# Procedimientos para el control del movimiento del jugador
#----------------------------------------------------------

proc MostrarDebug { } {
	if { $::visDebug } {
		set ::visDebug false
		/data/mapa.mostrardebug false
	} else {
		set ::visDebug true
		/data/mapa.mostrardebug true	
	}
}

proc AccionPrincipal { } {
     /game.manejadorbotonprincipal     
}

proc AccionSecundaria { } {     	
     /game.manejadorbotonsecundario     
}