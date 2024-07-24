#--------------------------------------------------------------------
#   video_intro.tcl
#   ==================
#   Lista de los personajes con su música y todo
#
#--------------------------------------------------------------------

proc DestructorVideoIntroduccion { } {
/game/scene/video_introduccion.setactive false
}

proc DestructorSpriteVideoIntro { } {
delete /game/scene/video_introduccion/fondo/tex
delete /game/scene/video_introduccion/fondo/sprite
}

proc GenerarSpriteVideoIntro { } {
new nCSprite /game/scene/video_introduccion/fondo/sprite
sel /game/scene/video_introduccion/fondo/sprite
	.preload
	setautoscale false
	setsizef 1.0 1.0
	setpositionf 0.0 0.0
	setorder 0
	sel ..	
new ntexarraynode tex
sel tex
	.settexture 0 "textures:v_1.png" "none"
	.settexture 1 "textures:v_1.png" "none"
	.preload
	sel ..	
}

proc VideoIntroduccion { } {

AgregarTriggerVideo  /data/videos/introduccion 8.0 "CambiarPersonaje \"textures:v_5.png\" "
AgregarTriggerVideo  /data/videos/introduccion 16.0  "CambiarPersonaje \"textures:v_2.png\" "	
AgregarTriggerVideo  /data/videos/introduccion 24.0 "CambiarPersonaje \"textures:v_3.png\" "
AgregarTriggerVideo  /data/videos/introduccion 32.0 "CambiarPersonaje \"textures:v_4.png\" "
AgregarTriggerVideo  /data/videos/introduccion 40.0 "CambiarPersonaje \"textures:v_kunoichi.png\" "
AgregarTriggerVideo  /data/videos/introduccion 48.0  "CambiarPersonaje \"textures:v_ladrona.png\" "	
AgregarTriggerVideo  /data/videos/introduccion 56.0 "CambiarPersonaje \"textures:v_shugenja.png\" "
AgregarTriggerVideo  /data/videos/introduccion 64.0 "CambiarPersonaje \"textures:v_shogun.png\" "
AgregarTriggerVideo  /data/videos/introduccion 72.0 "CambiarPersonaje \"textures:v_maestro.png\" "
AgregarTriggerVideo  /data/videos/introduccion 80.0 "CambiarPersonaje \"textures:v_mercader.png\" "
AgregarTriggerVideo  /data/videos/introduccion 88.0 "CambiarPersonaje \"textures:v_ingeniero.png\" "
AgregarTriggerVideo  /data/videos/introduccion 96.0 "CambiarPersonaje \"textures:v_samurai.png\" "

set tiempo [/sys/servers/channel.getchannel1f time]
/game.playsong 0 "sounds:musica/musica_intro.wav"
/game/scene/video_introduccion.setactive true

CambiarPersonaje "textures:v_1.png"
}

proc CambiarPersonaje { personaje } {
/game/scene/video_introduccion/fondo/tex.settexture 0 $personaje none
}

proc GenVideoIntroduccion { } {

sel /game/scene
new n3dnode video_introduccion
sel video_introduccion
	.preload
	.setactive false
	new n3dnode fondo
	sel fondo
		.preload
		new nshadernode shader
		sel shader
			.setrenderpri 1
			.setnumstages 2
			.setcolorop 0 "add tex prev"
			.setalphaop 0 "nop"
			.setconst 0 0.000000 0.000000 0.000000 1.000000
			.setconst 1 1.000000 1.000000 1.000000 1.000000
			.begintunit 0
			.setaddress clamp clamp
			.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
			.settexcoordsrc "uv0"
			.setenabletransform true
			.endtunit
			.setcolorop 1 "mul const prev"
			.setalphaop 1 "nop"
			.setconst 1 0.000000 0.000000 0.000000 1.000000
			.begintunit 1
			.setaddress clamp clamp
			.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
			.settexcoordsrc "uv0"
			.setenabletransform true
			.endtunit				
			.setlightenable true		
			new nipol ip
			sel ip
				.setreptype "loop"
				.setchannel "time"
				.setscale 1.0
				.connect "setconst0"
				.addkey4f 0.0 0.0 0.0 0.0 1.0
				.addkey4f 2.0 1.0 1.0 1.0 1.0
				.addkey4f 6.0 1.0 1.0 1.0 1.0
				.addkey4f 8.0 0.0 0.0 0.0 1.0							
}
GenVideoIntroduccion
