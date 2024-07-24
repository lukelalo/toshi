proc MenuResultados { } {
new nmenu /data/menus/menuResultados
sel /data/menus/menuResultados
	.setnodo "/game/scene/menus/MenuResultados"
	.setimagenfondo "menus:MenuResultados.png"
	
	set numopcion [.agregaropcion "SeleccionarSalirPartidaFin"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonSalir.png"
	.modificarimagenoveropcion $numopcion "menus:BotonSalirOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012
}

proc SeleccionarSalirPartidaFin { } {
	MostrarTextoMenuResultados false
	SeleccionarSalirPartida
}

proc MostrarTextoMenuResultados { mostrar } {

	set sepy 0.05

	set mejorPuntuacion -1
	set posyPuntuacion 0.41
	set posyActual 0.41
	if { $mostrar } {
		CrearTextoResultadosPartida
	}
	
	for { set i 0 } { $i < [expr $::NumJugadores + 1] } { incr i } {
		# Nombre de los jugadores
		/game/scene/textos/nombre$i.setactive $mostrar
		set jugadorPrincipal [/game.esjugadorprincipal $i]
		set numJugador [expr $i+1]
		
		if { $jugadorPrincipal } {
			/game/scene/textos/nombre$i/texto.settext "Jugador Humano"
		} else {
			/game/scene/textos/nombre$i/texto.settext "Jugador CPU $numJugador"	
		}

		# Puntos por acciones
		/game/scene/textos/accion$i.setactive $mostrar
		set ppA [/game.getpuntosporaccion $i]
		/game/scene/textos/accion$i/texto.settext "$ppA"

		# Puntos por construcciones
		/game/scene/textos/construccion$i.setactive $mostrar
		set ppC [/game.getpuntosporconstruccion $i]
		set ppNC [/game.getnumconstrucciones $i]
		/game/scene/textos/construccion$i/texto.settext "$ppC/$ppNC"	

		# Puntos por distintivo
		/game/scene/textos/distintivo$i.setactive $mostrar
		set ppD [/game.getpuntospordistintivo $i]
		/game/scene/textos/distintivo$i/texto.settext "$ppD"

		# Puntos por oro
		/game/scene/textos/oro$i.setactive $mostrar
		set ppO [/game.getpuntospororo $i]
		/game/scene/textos/oro$i/texto.settext "$ppO"	

		# Puntos por tiempo
		/game/scene/textos/tiempo$i.setactive $mostrar
		set ppT [/game.getpuntosportiempo $i]
		/game/scene/textos/tiempo$i/texto.settext "$ppT"	

		# Total de puntos
		/game/scene/textos/puntos$i.setactive $mostrar		
		set pT [/game.getpuntostotales $i]
		/game/scene/textos/puntos$i/texto.settext "$pT"

		if { $pT > $mejorPuntuacion } {
			set mejorPuntuacion $pT
			set posyPuntuacion $posyActual
		}
		set posyActual [expr $posyActual+$sepy]
	
	}
	
	/game/scene/hresultados/selRes/sprite.setpositionf 0.025 $posyPuntuacion
	
	/game/scene/hresultados.setactive $mostrar
	/game/scene/textos/tpuntos.setactive $mostrar
}

proc CrearTextoResultadosPartida { } {

set posyinicial 0.46
set sepy 0.05

for { set i 0 } { $i < 8 } { incr i } {
	# Nombre de los jugadores
	new n3dnode /game/scene/textos/nombre$i
	/game/scene/textos/nombre$i.setactive false
	new nLabel /game/scene/textos/nombre$i/texto
	sel /game/scene/textos/nombre$i/texto
		.loadfont fonts:toshi2$::ResSel.png
		set posy [expr $posyinicial + ($sepy*$i) - 0.002]
		.setpositionf 0.1 $posy
		.setalign left  
		.setorder 52
		
	# Puntos por acciones
	new n3dnode /game/scene/textos/accion$i
	/game/scene/textos/accion$i.setactive false
	new nLabel /game/scene/textos/accion$i/texto
	sel /game/scene/textos/accion$i/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext "0"
		set posy [expr $posyinicial + ($sepy*$i)]
		.setpositionf 0.35 $posy
		.setalign center
		.setorder 52
		
	# Puntos por construcciones
	new n3dnode /game/scene/textos/construccion$i
	/game/scene/textos/construccion$i.setactive false
	new nLabel /game/scene/textos/construccion$i/texto
	sel /game/scene/textos/construccion$i/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext "0"
		set posy [expr $posyinicial + ($sepy*$i)]
		.setpositionf 0.45 $posy
		.setalign center
		.setorder 52
		
	# Puntos por distintivo
	new n3dnode /game/scene/textos/distintivo$i
	/game/scene/textos/distintivo$i.setactive false
	new nLabel /game/scene/textos/distintivo$i/texto
	sel /game/scene/textos/distintivo$i/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext "0"
		set posy [expr $posyinicial + ($sepy*$i)]
		.setpositionf 0.55 $posy
		.setalign center
		.setorder 52
		
	# Puntos por oro
	new n3dnode /game/scene/textos/oro$i
	/game/scene/textos/oro$i.setactive false
	new nLabel /game/scene/textos/oro$i/texto
	sel /game/scene/textos/oro$i/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext "0"
		set posy [expr $posyinicial + ($sepy*$i)]
		.setpositionf 0.65 $posy
		.setalign center
		.setorder 52
		
	# Puntos por tiempo
	new n3dnode /game/scene/textos/tiempo$i
	/game/scene/textos/tiempo$i.setactive false
	new nLabel /game/scene/textos/tiempo$i/texto
	sel /game/scene/textos/tiempo$i/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext "0"
		set posy [expr $posyinicial + ($sepy*$i)]
		.setpositionf 0.75 $posy
		.setalign center
		.setorder 52
		
	# Total de puntos
	new n3dnode /game/scene/textos/puntos$i
	/game/scene/textos/puntos$i.setactive false
	new nLabel /game/scene/textos/puntos$i/texto
	sel /game/scene/textos/puntos$i/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext "0"
		set posy [expr $posyinicial + ($sepy*$i)]
		.setpositionf 0.9 $posy
		.setalign center
		.setorder 52
	}
	
	# Texto de total
	new n3dnode /game/scene/textos/tpuntos
	/game/scene/textos/tpuntos.setactive false
	new nLabel /game/scene/textos/tpuntos/texto
	sel /game/scene/textos/tpuntos/texto
		.loadfont fonts:toshi2$::ResSel.png
		.settext "Total"
		.setpositionf 0.9 0.4
		.setalign center
		.setorder 52		
}