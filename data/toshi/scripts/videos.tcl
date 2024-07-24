new nroot /data/videos
/game.setvideo /data/videos

#########################################################################
#
#	Carga de los videos
#
#########################################################################

runscript video_logo.tcl
runscript video_intro.tcl
runscript video_grupo.tcl
runscript video_creditos.tcl

#########################################################################
#
#	Procedimientos de los videos
#
#########################################################################

proc MostrarVideo { video tiempo } {
	/sys/servers/channel.setchannel1f time 0.0
	PonerMapeoVideos

	set ::VideoActual $video
	sel $video
	
	.agregartrigger $tiempo "OcultarVideoActual"	
	.mostrar
}

proc OcultarVideoActual { } {
	sel $::VideoActual
	.ocultar
	/sys/servers/channel.setchannel1f time 0.0	
}

proc ModificarVolumenPrincipal { volumen } {
	/sys/servers/audio.setmastervolume $volumen
}

proc AgregarTriggerVideo { video tiempo trigger } {
	$video.agregartrigger $tiempo $trigger
}

proc MostrarIntroduccion { } {
	OcultarMenuActivo
	MostrarMusicaMenus false
	
	MostrarVideo 				"/data/videos/grupo" 9.0
	
	EliminarTextoVersion
}

proc DestruirSpritesVideos { } {
	DestructorSpriteVideoIntro
	DestructorSpriteVideoCreditos
}

proc GenerarSpritesVideos { } {
	GenerarSpriteVideoIntro
	GenerarSpriteVideoCreditos
}

#########################################################################
#
#	Generación de las secuencias
#
#########################################################################

new nvideo /data/videos/grupo
sel /data/videos/grupo
	.setvideo "VideoGrupo"
	.setdestructorvideo "DestructorVideoGrupo"
	.setaccionfinal "MostrarVideo \"/data/videos/introduccion\" 104.0"

new nvideo /data/videos/introduccion
sel /data/videos/introduccion
	.setvideo "VideoIntroduccion"
	.setdestructorvideo "DestructorVideoIntroduccion"
	.setaccionfinal "MostrarVideo \"/data/videos/logotipo\" 13.0"
	
new nvideo /data/videos/logotipo
sel /data/videos/logotipo
	.setvideo "VideoLogotipo"
	.setdestructorvideo "DestructorVideoLogotipo"
	.setaccionfinal "MostrarMenuPrincipal"
	
new nvideo /data/videos/creditos
sel /data/videos/creditos
	.setvideo "VideoCreditos"
	.setdestructorvideo "DestructorVideoCreditos"
	.setaccionfinal "MostrarMenuPrincipal"
	
	
