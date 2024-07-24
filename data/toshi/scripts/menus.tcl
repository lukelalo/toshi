#--------------------------------------------------------
# MENÚS DEL JUEGO
#--------------------------------------------------------
new n3dnode /game/scene/menus
			
proc SonidoClick { } {
	/game.playsound 0 "sounds:sonidos/click.ogg"
}

proc CrearTextos { } {		
new n3dnode /game/scene/textos			
new n3dnode /game/scene/textos/carga
/game/scene/textos/carga.setactive false
new nLabel /game/scene/textos/carga/texto
sel /game/scene/textos/carga/texto
	.loadfont fonts:toshi1$::ResSel.png
	.settext "0%"
	.setpositionf 0.535 0.64		
	.setalign center
	.setorder 52

new n3dnode /game/scene/textos/defcarga
/game/scene/textos/defcarga.setactive false
new nLabel /game/scene/textos/defcarga/texto
sel /game/scene/textos/defcarga/texto
	.loadfont fonts:toshi1$::ResSel.png
	.settext ""
	.setpositionf 0.535 0.55	
	.setalign center
	.setorder 52		
}

proc MostrarTextos { } {
/game/scene/textos/carga.setactive true
/game/scene/textos/defcarga.setactive true
}

proc BorrarTextos { } {
delete /game/scene/textos
}
		
sel /game/scene


new nroot /data/menus
/game.setmenu /data/menus

# ----------------------------------------------------------------------------
#  Generamos los menus
# ----------------------------------------------------------------------------

set ::MusicaMenusActiva false

runscript defmenus.tcl


proc MostrarMenu { menu tiempo acciontemporal } {
	/sys/servers/gfx.setcurrentcursor 0 true
	/sys/servers/channel.setchannel1f time 0.0
	
	OcultarMenuActivo
	PonerMapeoMenus

	if { $acciontemporal != "" } {
		AgregarTriggerMenu $menu $tiempo $acciontemporal
		}
	$menu.mostrar

	set ::MenuActivo $menu
	if { $::MusicaMenusActiva == false } {
		MostrarMusicaMenus true
		}
}

proc OcultarMenuActivo { } {
	#/game/scene/menus/click.setactive false
	if { $::MenuActivo != "" } {
		$::MenuActivo.ocultar
		$::MenuActivo.setactive false
	}	
}

proc RecargarMenus { } {
	BorrarMenus
	CargarMenus
	
	MostrarMenuActivo
}

proc MostrarMenuActivo { } { 
	MostrarMenu $::MenuActivo -1 ""
}

proc BorrarMenus { } {
	delete /data/menus/menuPrincipal
	delete /data/menus/menuJugar
	delete /data/menus/menuRed
	delete /data/menus/menuCarga
	delete /data/menus/menuOpciones
	delete /data/menus/menuGraficos
	delete /data/menus/menuSonido
	delete /data/menus/menuPartida
	delete /data/menus/menuResultados
	delete /data/menus/menuFinalizacion	
	delete /data/menus/menuAyuda
	delete /data/menus/menuCliente
	delete /data/menus/menuServidor
		
	delete /game/scene/menus/MenuPrincipal
	delete /game/scene/menus/MenuJugar
	delete /game/scene/menus/MenuRed
	delete /game/scene/menus/MenuCarga
	delete /game/scene/menus/MenuOpciones
	delete /game/scene/menus/MenuGraficos
	delete /game/scene/menus/MenuSonido
	delete /game/scene/menus/MenuPartida
	delete /game/scene/menus/MenuResultados	
	delete /game/scene/menus/MenuFinalizacion	
	delete /game/scene/menus/MenuAyuda
	delete /game/scene/menus/MenuCliente
	delete /game/scene/menus/MenuServidor
}

proc CargarMenus { } {
	MenuPrincipal
	MenuJugar
	MenuRed
	MenuCarga
	MenuOpciones
	MenuGraficos
	MenuSonido
	MenuPartida
	MenuResultados
	MenuFinalizacion
	MenuAyuda
	MenuCliente
	MenuServidor
}

proc BorrarMenusEnJuego { } {
	delete /data/menus/menuPrincipal
	delete /data/menus/menuJugar
	delete /data/menus/menuRed
	delete /data/menus/menuOpciones
	delete /data/menus/menuGraficos	
	delete /data/menus/menuCliente
	delete /data/menus/menuServidor
		
	delete /game/scene/menus/MenuPrincipal
	delete /game/scene/menus/MenuJugar
	delete /game/scene/menus/MenuRed
	delete /game/scene/menus/MenuOpciones
	delete /game/scene/menus/MenuGraficos
	delete /game/scene/menus/MenuCliente
	delete /game/scene/menus/MenuServidor
}

proc CargarMenusEnJuego { } {
	MenuPrincipal
	MenuJugar
	MenuRed
	MenuOpciones
	MenuGraficos
	MenuCliente
	MenuServidor
	
	/data/menus/menuPrincipal.generarsprite
	/data/menus/menuJugar.generarsprite
	/data/menus/menuRed.generarsprite
	/data/menus/menuOpciones.generarsprite
	/data/menus/menuGraficos.generarsprite
	/data/menus/menuCliente.generarsprite
	/data/menus/menuServidor.generarsprite		
}

proc MostrarMusicaMenus { mostrar } {
	if { $mostrar } {
		/game.playsong 0 "sounds:musica/musica_menu.wav"
		set ::MusicaMenusActiva true	
	} else {
		set ::MusicaMenusActiva false		
	}
}	

proc AgregarTriggerMenu { menu tiempo trigger } {
	$menu.agregartrigger $tiempo $trigger
}

proc GenerarSpritesMenus { } {
	/data/menus/menuPrincipal.generarsprite
	/data/menus/menuJugar.generarsprite
	/data/menus/menuRed.generarsprite
	/data/menus/menuCarga.generarsprite
	/data/menus/menuOpciones.generarsprite
	/data/menus/menuGraficos.generarsprite
	/data/menus/menuSonido.generarsprite
	/data/menus/menuPartida.generarsprite
	/data/menus/menuResultados.generarsprite
	/data/menus/menuFinalizacion.generarsprite
	/data/menus/menuAyuda.generarsprite
	/data/menus/menuCliente.generarsprite
	/data/menus/menuServidor.generarsprite	
}