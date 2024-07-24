#--------------------------------------------------------------------
#   video_creditos.tcl
#   ==================
#   Lista de los créditos
#
#--------------------------------------------------------------------

proc DestructorVideoCreditos { } {
/game/scene/video_creditos.setactive false
}

proc DestructorSpriteVideoCreditos { } {
delete /game/scene/video_creditos/texto_creditos
delete /game/scene/video_creditos/fondo/tex
delete /game/scene/video_creditos/fondo/sprite
}

proc GenerarSpriteVideoCreditos { } {
new nCSprite /game/scene/video_creditos/fondo/sprite
sel /game/scene/video_creditos/fondo/sprite
	.preload
	setautoscale false
	setsizef 1.0 1.0
	setpositionf 0.0 0.0
	setorder 0
	sel ..
new ntexarraynode tex
sel tex
	.settexture 0 "textures:creditos1.png" "none"
	.settexture 1 "textures:creditos1.png" "none"
	.preload
	sel ..	
	
CargarFicheroCreditos	
}

proc VideoCreditos { } {

AgregarTriggerVideo  /data/videos/creditos 80.0   "CambiarFondoCreditos \"textures:creditos2.png\" "	
AgregarTriggerVideo  /data/videos/creditos 160.0  "CambiarFondoCreditos \"textures:creditos3.png\" "
AgregarTriggerVideo  /data/videos/creditos 240.0  "CambiarFondoCreditos \"textures:creditos4.png\" "

set tiempo [/sys/servers/channel.getchannel1f time]
/game.playsong 0 "sounds:musica/musica_creditos.mp3"
/game/scene/video_creditos.setactive true
CambiarFondoCreditos "textures:creditos1.png"
}

proc CambiarFondoCreditos { fondo } {
/game/scene/video_creditos/fondo/tex.settexture 0 $fondo none
}

proc CargarFicheroCreditos { } {

    sel /game/scene/video_creditos
    new n3dnode texto_creditos
    sel texto_creditos

    set path [/sys/servers/file2.manglepath text:creditos.txt]
    set file $path
    set posy 1.0
    
    set tam_creditos  0.0

    set fileID [open $file RDONLY]

    # Hacemos una prelectura del fichero para averiguar su tamaño
    
    while { [gets $fileID line ] >= 0 } {
    	set tam_creditos [expr $tam_creditos + 0.05]
    }
    
    close $fileID  

    # Hacemos otra lectura de los elementos
    
    set fileID [open $file RDONLY]    

    set i 0
    while { [gets $fileID line ] >= 0 } {
        # Si la línea contiene blancos, la tomamos como un salto de posición

        set trimLine [string trim $line]
        if { ![string length $line] } {
        	set posy [expr $posy + 0.05]
		continue            
        }
        
        # Como no es una línea en blanco, nos creamos un n3dnode para cada línea de texto
        new n3dnode linea_texto$i
        sel linea_texto$i
		new nLabel texto
		sel texto
			# Si el primer carácter es una @ lo tratamos como un título

			set line [string range $line 0 100]
			set char [string index $line 0]

			if { [string equal $char "@"] } {
				set colList [split $line {@}]
				set line [lindex $colList 1]
				.loadfont fonts:titulo_creditos$::ResSel.png
			} else {
				.loadfont fonts:creditos$::ResSel.png
			}
			
			.settext "$line"
			.setpositionf 0.5 $posy
			.setalign center
			.setorder 52
			new nipol movimiento
			sel movimiento
				connect setpositionf
				setreptype oneshot
				addkey2f 0	0.5 $posy
				addkey2f 240	0.5 [expr $posy - $tam_creditos - 0.8]
			sel ..
			
		sel ..
	sel ..

        set posy [expr $posy + 0.05]       
       	set i [expr $i+1]
    }
    
    close $fileID    
}


proc GenVideoCreditos { } {

sel /game/scene
new n3dnode video_creditos
sel video_creditos
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
				.addkey4f 78.0 1.0 1.0 1.0 1.0
				.addkey4f 80.0 0.0 0.0 0.0 1.0
}

GenVideoCreditos