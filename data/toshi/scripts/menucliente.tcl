proc MenuCliente { } {
new nmenu /data/menus/menuCliente
sel /data/menus/menuCliente
	.setnodo "/game/scene/menus/MenuCliente"
	.setimagenfondo "menus:MenuCliente.png"

	set numopcion [.agregaropcion "SeleccionarVolverAnteriorClienteRed"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonVolver.png"
	.modificarimagenoveropcion $numopcion "menus:BotonVolverOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012	
	
	set numopcion [.agregaropcion "SeleccionarConectar"]
	.modificarimagenprincipalopcion $numopcion "menus:LineaConectar.png"
	.modificarimagenoveropcion $numopcion "menus:LineaConectarOver.png"
	.modificarposicionopcion $numopcion 0.1041 0.62
	.modificardimensionesopcion $numopcion 0.1 0.055
	
}

# Menu Cliente

proc SeleccionarVolverAnteriorClienteRed { } {
PonerMapeoMenus
EliminarTextoClienteRed
SeleccionarVolverAnterior
}

proc SeleccionarConectar { } {
/game/scene/textoconexion/texto.settext "Intentando conectar con $::TextoIp"
/game.update
SonidoClick
set ::JuegoEnRed		true
set ::EsServidor		false

runscript net.tcl

clienteRed $::TextoIp 5050

/game/scene/textoconexion/texto.settext "Conexion Fallida"
}

proc AgregarTextoIp { texto } {
/game/scene/textoconexion/texto.settext ""
if { $::NumCaracteresIp < 15 } {
	set ::TextoIp [join [list $::TextoIp $texto] ""]
	set ::NumCaracteresIp [expr $::NumCaracteresIp + 1]
	/game/scene/textoip/texto.settext "$::TextoIp"
	}
}

proc BorrarTextoIp { } {
/game/scene/textoconexion/texto.settext ""
if { $::NumCaracteresIp > 0 } {
	set ::NumCaracteresIp [expr $::NumCaracteresIp - 1]
	set ::TextoIp [string range $::TextoIp 0 [expr ( $::NumCaracteresIp - 1 )] ]
	/game/scene/textoip/texto.settext "$::TextoIp"
	}
}

proc GenerarTextoClienteRed { } {
new n3dnode /game/scene/textoip
new nLabel /game/scene/textoip/texto
sel /game/scene/textoip/texto
	.loadfont fonts:toshi1$::ResSel.png
	.settext ""
	.setpositionf 0.535 0.525	
	.setalign center
	.setorder 52

new n3dnode /game/scene/textoconexion
new nLabel /game/scene/textoconexion/texto
sel /game/scene/textoconexion/texto
	.loadfont fonts:toshi1$::ResSel.png
	.settext ""
	.setpositionf 0.535 0.64	
	.setalign center
	.setorder 52	
}

proc EliminarTextoClienteRed { } {
delete /game/scene/textoip
delete /game/scene/textoconexion
}

proc PonerMapeoClienteRed { } {
	/sys/servers/input.beginmap

	/sys/servers/input.map mouse0:btn0.down         "script:AccionPrincipal"
	
	# Para introducir la ip activamos los números,
	# los puntos y el borrar
	
	/sys/servers/input.map keyb0:0.down		"script:AgregarTextoIp 0"
	/sys/servers/input.map keyb0:1.down		"script:AgregarTextoIp 1"
	/sys/servers/input.map keyb0:2.down		"script:AgregarTextoIp 2"
	/sys/servers/input.map keyb0:3.down		"script:AgregarTextoIp 3"
	/sys/servers/input.map keyb0:4.down		"script:AgregarTextoIp 4"
	/sys/servers/input.map keyb0:5.down		"script:AgregarTextoIp 5"
	/sys/servers/input.map keyb0:6.down		"script:AgregarTextoIp 6"
	/sys/servers/input.map keyb0:7.down		"script:AgregarTextoIp 7"
	/sys/servers/input.map keyb0:8.down		"script:AgregarTextoIp 8"
	/sys/servers/input.map keyb0:9.down		"script:AgregarTextoIp 9"
	
	/sys/servers/input.map keyb0:a.down		"script:AgregarTextoIp a"
	/sys/servers/input.map keyb0:b.down		"script:AgregarTextoIp b"
	/sys/servers/input.map keyb0:c.down		"script:AgregarTextoIp c"
	/sys/servers/input.map keyb0:d.down		"script:AgregarTextoIp d"
	/sys/servers/input.map keyb0:e.down		"script:AgregarTextoIp e"
	/sys/servers/input.map keyb0:f.down		"script:AgregarTextoIp f"
	/sys/servers/input.map keyb0:g.down		"script:AgregarTextoIp g"
	/sys/servers/input.map keyb0:h.down		"script:AgregarTextoIp h"
	/sys/servers/input.map keyb0:i.down		"script:AgregarTextoIp i"
	/sys/servers/input.map keyb0:j.down		"script:AgregarTextoIp j"
	/sys/servers/input.map keyb0:k.down		"script:AgregarTextoIp k"
	/sys/servers/input.map keyb0:l.down		"script:AgregarTextoIp l"
	/sys/servers/input.map keyb0:m.down		"script:AgregarTextoIp m"
	/sys/servers/input.map keyb0:n.down		"script:AgregarTextoIp n"
	/sys/servers/input.map keyb0:o.down		"script:AgregarTextoIp o"
	/sys/servers/input.map keyb0:p.down		"script:AgregarTextoIp p"
	/sys/servers/input.map keyb0:q.down		"script:AgregarTextoIp q"
	/sys/servers/input.map keyb0:r.down		"script:AgregarTextoIp r"
	/sys/servers/input.map keyb0:s.down		"script:AgregarTextoIp s"
	/sys/servers/input.map keyb0:t.down		"script:AgregarTextoIp t"
	/sys/servers/input.map keyb0:u.down		"script:AgregarTextoIp u"
	/sys/servers/input.map keyb0:v.down		"script:AgregarTextoIp v"
	/sys/servers/input.map keyb0:w.down		"script:AgregarTextoIp w"
	/sys/servers/input.map keyb0:x.down		"script:AgregarTextoIp x"
	/sys/servers/input.map keyb0:y.down		"script:AgregarTextoIp y"
	/sys/servers/input.map keyb0:z.down		"script:AgregarTextoIp z"
	
	/sys/servers/input.map keyb0:decimal.down	"script:AgregarTextoIp ."
	/sys/servers/input.map keyb0:space.down		"script:AgregarTextoIp ."	
	/sys/servers/input.map keyb0:bs.down		"script:BorrarTextoIp"
	/sys/servers/input.map keyb0:delete.down	"script:BorrarTextoIp"
	
	# Para mostrar información de depuración	
	/sys/servers/input.map keyb0:f1.down         	"script:/sys/servers/console.toggle"

	/sys/servers/input.endmap
	
	GenerarTextoClienteRed
	set ::TextoIp ""
	set ::NumCaracteresIp 0
}

proc ConectadoAlServidor { } {
EliminarTextoClienteRed
ActualizarComenzar
}