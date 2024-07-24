#--------------------------------------------------------
#
# Guión que crea la parte del HUD
# 
# El hud se coloca en /game/scene/hud. En un
# futuro habrá que colocarlo en algún otro lugar.
# 
# Se crea el marco y la barra de energía.
# Habrá que mejorar las imagenes y ajustar las posiciones
# de los interpoladores. En concreto ahora mismo la barra
# con la energía disponible tiene espacio vacío alrededor
# (tiene el mismo tamaño que la textura del marco, y no
# tiene mucho sentido...)
#
#--------------------------------------------------------

set ::num_elem_HUD 18

#--------------------------------------------------------
# Creamos el HUD
#--------------------------------------------------------

proc GenerarHUD { } {



sel /game/scene
new n3dnode hud
sel hud
	new n3dnode fondoPersonaje
	sel fondoPersonaje
		.preload
		new nCSprite sprite
		sel sprite
			preload
			setautoscale false
			setsizef 0.21 0.25
			setpositionf 0.0 0.75
			setorder 10
		sel ..  				
		new ntexarraynode tex	
		tex.preload
		tex.settexture 0 "textures:personajefondo.png" "none" 
		tex.sethighquality 0 true
		sel ..

	MiniCheckPoint $::num_elem_HUD

	sel /game/scene/hud		
	new n3dnode fondoPergaminos
	sel fondoPergaminos
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.7 0.11
			setpositionf 0.28 0.86
			setorder 10
		sel ..  				
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:simbolosfondo.png" "none" 
		sel ..	

	MiniCheckPoint $::num_elem_HUD		

	sel /game/scene/hud		
	new n3dnode kunoichi
	sel kunoichi
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.14 0.08
			setpositionf -0.05 0.0
			setorder 10
		sel ..  				
		new ntexarraynode tex	
		tex.preload
		tex.settexture 0 "textures:SelKunoichi.png" "none" 
		sel ..

	MiniCheckPoint $::num_elem_HUD		

	sel /game/scene/hud		
	new n3dnode ladrona
	sel ladrona
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.14 0.08
			setpositionf -0.0495 0.085
			setorder 10
		sel ..  				
		new ntexarraynode tex		
		tex.preload
		tex.settexture 0 "textures:SelLadrona.png" "none" 
		sel ..			

	MiniCheckPoint $::num_elem_HUD

	sel /game/scene/hud
	new n3dnode shugenja
	sel shugenja
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.14 0.08
			setpositionf -0.045 0.17
			setorder 10
		sel ..  				
		new ntexarraynode tex	
		tex.preload
		tex.settexture 0 "textures:SelShugenja.png" "none" 
		sel ..			

	MiniCheckPoint $::num_elem_HUD

	sel /game/scene/hud
	new n3dnode shogun
	sel shogun
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.14 0.08
			setpositionf -0.051 0.255
			setorder 10
		sel ..  				
		new ntexarraynode tex		
		tex.preload
		tex.settexture 0 "textures:SelShogun.png" "none" 
		sel ..			

	MiniCheckPoint $::num_elem_HUD

	sel /game/scene/hud
	new n3dnode maestro
	sel maestro
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.14 0.08
			setpositionf -0.05 0.34
			setorder 10
		sel ..  				
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:SelMaestro.png" "none" 
		sel ..			

	MiniCheckPoint $::num_elem_HUD

	sel /game/scene/hud
	new n3dnode mercader
	sel mercader
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.14 0.08
			setpositionf -0.04 0.425
			setorder 10
		sel ..  				
		new ntexarraynode tex	
		tex.preload
		tex.settexture 0 "textures:SelMercader.png" "none" 
		sel ..			

	MiniCheckPoint $::num_elem_HUD

	sel /game/scene/hud
	new n3dnode ingeniero
	sel ingeniero
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.14 0.08
			setpositionf -0.043 0.51
			setorder 10
		sel ..  				
		new ntexarraynode tex		
		tex.preload
		tex.settexture 0 "textures:SelIngeniero.png" "none" 
		sel ..			

	MiniCheckPoint $::num_elem_HUD

	sel /game/scene/hud
	new n3dnode samurai
	sel samurai
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.14 0.08
			setpositionf -0.05 0.595
			setorder 10
		sel ..  				
		new ntexarraynode tex		
		tex.preload
		tex.settexture 0 "textures:SelSamurai.png" "none" 
		sel ..			

	MiniCheckPoint $::num_elem_HUD

	sel /game/scene/hud		
	new n3dnode reloj
	sel reloj
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.07 0.16
			setpositionf 0.1962 0.839
			setorder 9
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:dragon.png" "none"	
	sel ..

	MiniCheckPoint $::num_elem_HUD	

	sel /game/scene/hud	
	new n3dnode nombrepersonaje
	sel nombrepersonaje
		.preload
		new nCSprite sprite
		sel sprite
			preload
			setautoscale false
			setsizef 0.15 0.075
			setpositionf 0.83 0.78
			setorder 10
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:trans.png" "none"
		tex.sethighquality 0 true
	sel ..	

	MiniCheckPoint $::num_elem_HUD	

	sel /game/scene/hud
	new n3dnode cara
	sel cara
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.26 0.32
			setpositionf -0.05 0.68
			setorder 11
		sel ..
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:trans.png" "none"
		tex.sethighquality 0 true
	sel ..	

	MiniCheckPoint $::num_elem_HUD		

	sel /game/scene/hud		
	new n3dnode numero0
	sel numero0
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.03 0.035
			setpositionf 0.14 0.8
			setorder 12
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:trans.png" "none"
	sel ..	

	MiniCheckPoint $::num_elem_HUD	

	sel /game/scene/hud	
	new n3dnode numero1
	sel numero1
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.03 0.035
			setpositionf 0.154 0.8
			setorder 12
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:trans.png" "none"	
	sel ..

	MiniCheckPoint $::num_elem_HUD	

	sel /game/scene/hud	
	new n3dnode numero2
	sel numero2
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.03 0.035
			setpositionf 0.168 0.8
			setorder 12
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:numero0.png" "none"		
	sel ..	

	MiniCheckPoint $::num_elem_HUD	

	sel /game/scene/hud	
	new n3dnode tiempoTurno
	sel tiempoTurno
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.016 0.12
			setpositionf 0.216 0.862
			setorder 8
			new nipol dec2
				sel dec2
					.connect setpositionf 
					.setchannel tiempoTurno
					.addkey2f 0 0.216 0.982
					.addkey2f 1 0.216 0.862	
				sel ..				
			new nipol dec
				sel dec
					.connect setsizef 
					.setchannel tiempoTurno
					.addkey2f 0 0.016 0.0
					.addkey2f 1 0.016 0.12
				sel ..					
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:barra_roja.png" "none"
	sel ..	

	MiniCheckPoint $::num_elem_HUD	
	
	new n3dnode /game/scene/hresultados
	/game/scene/hresultados.setactive false
	
	sel /game/scene/hresultados	
	new n3dnode cab_acciones
	sel cab_acciones
		
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.12 0.06
			setpositionf 0.29 0.38
			setorder 52				
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:finacciones.png" "none"
	sel ..	

	MiniCheckPoint $::num_elem_HUD	
	
	sel /game/scene/hresultados	
	new n3dnode cab_construcciones
	sel cab_construcciones
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.12 0.06
			setpositionf 0.39 0.38
			setorder 52				
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:finconstruccion.png" "none"
	sel ..	

	MiniCheckPoint $::num_elem_HUD	
	
	sel /game/scene/hresultados	
	new n3dnode cab_distintivo
	sel cab_distintivo
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.12 0.06
			setpositionf 0.49 0.38
			setorder 52				
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:findistintivo.png" "none"
	sel ..	

	MiniCheckPoint $::num_elem_HUD	
	
	sel /game/scene/hresultados	
	new n3dnode cab_oro
	sel cab_oro
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.12 0.06
			setpositionf 0.59 0.38
			setorder 52
		sel ..
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:findinero.png" "none"
	sel ..	

	MiniCheckPoint $::num_elem_HUD	
	
	sel /game/scene/hresultados	
	new n3dnode cab_tiempo
	sel cab_tiempo
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.12 0.06
			setpositionf 0.69 0.38
			setorder 52				
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:fintiempo.png" "none"
	sel ..	

	MiniCheckPoint $::num_elem_HUD		
	
	sel /game/scene/hresultados	
	new n3dnode selRes
	sel selRes
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.95 0.12
			setpositionf 0.025 0.42
			setorder 52				
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:selec_ganador.png" "none"
	sel ..	
	
	MiniCheckPoint $::num_elem_HUD			

	sel /game/scene

}

proc GenerarInformacion { } {

	new n3dnode /game/scene/hud/infoContextual
	sel /game/scene/hud/infoContextual
		.setactive false
		.preload

	new nLabel /game/scene/hud/infoContextual/texto
	sel /game/scene/hud/infoContextual/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext ""
		.setpositionf 0.0 0.0	
		.setalign center
		.setorder 105
		.setcolor 1.0 1.0 0.0

	new n3dnode /game/scene/hud/lineaInfo
	sel /game/scene/hud/lineaInfo
		.setactive false
		.preload

	new nLabel /game/scene/hud/lineaInfo/texto
	sel /game/scene/hud/lineaInfo/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext ""
		.setpositionf 0.275 0.82	
		.setalign left
		.setorder 102
		.setcolor 1.0 1.0 0.0

	new n3dnode /game/scene/hud/info
	sel /game/scene/hud/info
		.setactive false
		.preload
		new nCSprite fondo
		sel fondo
			.preload
			setautoscale false
			setsizef 0.5 0.33
			setpositionf 0.25 0.33
			setorder 100				
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:fondo_info.png" "none"
	sel ..	

	MiniCheckPoint $::num_elem_HUD
	
	new n3dnode /game/scene/hud/info/foto
	sel /game/scene/hud/info/foto
		.preload
		new nCSprite sprite
		sel sprite
			.preload
			setautoscale false
			setsizef 0.06 0.08
			setpositionf 0.28 0.42
			setorder 101				
		sel ..			
		new ntexarraynode tex
		tex.preload
		tex.settexture 0 "textures:pergamino42.png" "none"
	sel ..	

	MiniCheckPoint $::num_elem_HUD		

	new n3dnode /game/scene/hud/info/info_titulo
	new nLabel /game/scene/hud/info/info_titulo/texto
	sel /game/scene/hud/info/info_titulo/texto
		.loadfont fonts:toshi2$::ResSel.png
		.settext ""
		.setpositionf 0.5 0.37	
		.setalign center
		.setorder 102
		.setcolor 1.0 1.0 0.0

	new n3dnode /game/scene/hud/info/info_accion1
	new nLabel /game/scene/hud/info/info_accion1/texto
	sel /game/scene/hud/info/info_accion1/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext ""
		.setpositionf 0.36 0.43
		.setalign left
		.setorder 102

	new n3dnode /game/scene/hud/info/info_accion2
	new nLabel /game/scene/hud/info/info_accion2/texto
	sel /game/scene/hud/info/info_accion2/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext ""
		.setpositionf 0.36 0.47
		.setalign left
		.setorder 102

	new n3dnode /game/scene/hud/info/info_especial1
	new nLabel /game/scene/hud/info/info_especial1/texto
	sel /game/scene/hud/info/info_especial1/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext ""
		.setpositionf 0.28 0.53
		.setalign left
		.setorder 102

	new n3dnode /game/scene/hud/info/info_especial2
	new nLabel /game/scene/hud/info/info_especial2/texto
	sel /game/scene/hud/info/info_especial2/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext ""
		.setpositionf 0.28 0.57
		.setalign left
		.setorder 102		

	new n3dnode /game/scene/hud/info/info_mensaje1
	new nLabel /game/scene/hud/info/info_mensaje1/texto
	sel /game/scene/hud/info/info_mensaje1/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext ""
		.setpositionf 0.5 0.43
		.setalign center
		.setorder 102

	new n3dnode /game/scene/hud/info/info_mensaje2
	new nLabel /game/scene/hud/info/info_mensaje2/texto
	sel /game/scene/hud/info/info_mensaje2/texto
		.loadfont fonts:toshi1$::ResSel.png
		.settext ""
		.setpositionf 0.5 0.47
		.setalign center
		.setorder 102		
}

GenerarHUD

# Etiqueta de información acerca de lo que tenemos seleccionado
GenerarInformacion
runscript info.tcl
	
		
	