# ---------------------------------------------------------------------------
#   scene.tcl
#   =========
#   Set up static scene.
#
# ---------------------------------------------------------------------------

# Setup for shadowcasting
/sys/servers/shadow.setcastshadows false
new n3dnode /game/scene/escenario
sel /game/scene/escenario

    # Create skybox
    runscript skybox.tcl
    #runscript skydome.tcl
    createskybox sky

    # Create sun with directional light
    # Static at 30 degrees east of north and 30 degrees up
sel /game/scene/escenario   
    new n3dnode sun
    sel sun
   	.preload
    	.txyz 30 350 -1000
    #    .txyz 900 200 -866
        .rxyz 0 180 0

        # Create sun
	#sel /game/scene/escenario/sun    
        new nlightnode light
        sel light
            .preload
            .settype directional
            .setcastshadows true
            .setcolor 0.5 0.5 0.8 1
        sel ..      
        runscript sol.tcl 
sel /game/scene/escenario
    new nlightnode ambient
    sel ambient
        .preload
        .settype "ambient"
        .setcolor 0.5 0.5 0.5 1.0
    sel ..

#new nfognode fog
#	sel fog
#	.setmode exp
#        .setcolor 1.0 1.0 1.0 0.5
#        .setdensity 0.005
#sel ..

sel /game/scene/escenario
new n3dnode humo
	sel humo
	.preload
	txyz -0.5 0 0.5
	sxyz 0.5 0.2 0.5
	setactive false
	runfisica humo.psys
sel /game/scene/escenario   
new n3dnode polvo
	sel polvo
	.preload
	txyz 3 0 3
	sxyz 0.5 0.1 0.5
	setactive false
	runfisica polvo.psys

# Grupo 1 de barcas, al norte	
Generar_Barca 1 20 1 30
Generar_Barca 2 33 0 170
Generar_Barca 3 14 0 90

# Grupo 2 de barcas, al sur
Generar_Barca 4 50 78 139
Generar_Barca 5 30 79 56

# Grupo 3 de barcas, al este
Generar_Barca 6 1 58 245
Generar_Barca 7 0 36 128

# Grupo 4 de barcas, al oeste
Generar_Barca 8 80 65 342
Generar_Barca 9 79 23 25