# ---------------------------------------------------------------------------
#   utils.tcl
#   =========
#   Handy utility methods.
#
# ---------------------------------------------------------------------------
proc orig { } {
    /game/lookat.txyz   0    0    0
    /game/lookat.rxyz  $::global(angulo)   45   0
    /game/camera.tz     5
}

proc incAngulo { } {
    set ::global(angulo) [expr $::global(angulo) + 5]
    if { $::global(angulo) < -80 } {
    	set ::global(angulo) -80
    	}
    if { $::global(angulo) > -10 } {
    	set ::global(angulo) -10
    	}    	
    /game/lookat.rx $::global(angulo)
}

proc decAngulo { } {
    set ::global(angulo) [expr $::global(angulo) - 5]
    if { $::global(angulo) < -80 } {
    	set ::global(angulo) -80
    	}
    if { $::global(angulo) > -10 } {
    	set ::global(angulo) -10
    	}    	
    /game/lookat.rx $::global(angulo)
}

proc random { args } {
    global _ran

    if { [llength $args] > 1 } {
	set _ran [lindex $args 1]
    } else {
	set period 233280
	if { [info exists _ran] } {
	    set _ran [expr { ($_ran*9301 + 49297) % $period }]
	} else {
	    set _ran [expr { [clock seconds] % $period } ]
	}
	return [expr { int($args*($_ran/double($period))) } ]
    }
}

proc PausarJuego { } {
	if { $::JuegoPausado } {
		/game.pause false
		set ::JuegoPausado false
	} else {
		/game.pause true
		set ::JuegoPausado true
	}

}

proc CambiarResolucion { ancho alto } {
	if { $::TipoVis ==  "full" } {
		Crear_Pantalla_Negra
		Mostrar_Pantalla_Negra
	}
	BorrarMenus
	DestruirSpritesVideos
	/game.update
	/sys/servers/gfx.setdisplaymode "-type($::TipoVis)-w($ancho)-h($alto)"
	/sys/servers/gfx.closedisplay
	/sys/servers/gfx.opendisplay
#	puts "Hemos cambiado la resolucion a $ancho x $alto"
	CargarMenus
	GenerarSpritesVideos
	GenerarSpritesMenus
	MostrarMenuActivo
	if { $::TipoVis ==  "full" } {
		Destruir_Pantalla_Negra
	}
	/sys/servers/gfx.showdisplay	
}


proc PonerMapeoMenuPartida { } {
	/sys/servers/input.beginmap

	/sys/servers/input.map mouse0:btn0.down         "script:AccionPrincipal"	
	/sys/servers/input.map keyb0:esc.down         	"script:TratarMenuPartida"
	
	# Para mostrar información de depuración	
	#/sys/servers/input.map keyb0:f1.down         	"script:/sys/servers/console.toggle"	

	/sys/servers/input.endmap
	
	/sys/servers/gfx.showcursor	
}

proc PonerMapeoPartida { } {
	/sys/servers/input.beginmap

	/sys/servers/input.map mouse0:btn0.down         "script:AccionPrincipal"
	/sys/servers/input.map mouse0:btn1.down         "script:AccionSecundaria"
	/sys/servers/input.map mouse0:btn2.pressed    	orbit
	/sys/servers/input.map mouse0:btn2.pressed    	pan
	#/sys/servers/input.map keyb0:space.down       	"script:orig"
	/sys/servers/input.map keyb0:esc.down         	"script:TratarMenuPartida"

	# Para mostrar información de depuración
	#/sys/servers/input.map keyb0:d.down       	"script:MostrarDebug"
	#/sys/servers/input.map keyb0:f.down		"script:TratarMenuFinalizacion"
	#/sys/servers/input.map keyb0:f1.down         	"script:/sys/servers/console.toggle"	

	/sys/servers/input.endmap

#	puts "Entrada Mapeada"
	
	/sys/servers/gfx.showcursor	
}

proc PonerMapeoMenus { } {
	/sys/servers/input.beginmap

	/sys/servers/input.map mouse0:btn0.down         "script:AccionPrincipal"
	
	# Para mostrar información de depuración	
	#/sys/servers/input.map keyb0:f1.down         	"script:/sys/servers/console.toggle"

	/sys/servers/input.endmap
	
	/sys/servers/gfx.showcursor		
}

proc PonerMapeoVideos { } {
	/sys/servers/input.beginmap
	
	/sys/servers/input.map keyb0:esc.down       	"script:OcultarVideoActual"
	/sys/servers/input.map keyb0:return.down       	"script:OcultarVideoActual"
	/sys/servers/input.map mouse0:btn0.down       	"script:OcultarVideoActual"
	
	# Para mostrar información de depuración	
	#/sys/servers/input.map keyb0:f1.down       	"script:/sys/servers/console.toggle"
	
	/sys/servers/input.endmap
	
	/sys/servers/gfx.hidecursor	
}