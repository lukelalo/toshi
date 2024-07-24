#--------------------------------------------------------------------
#   shadernode.tcl
#   ==============
#   Demonstrate the new shadernodes which replace the obsolete
#   nmatnode objects.
#
#   Please note that some shaders may not work on all host systems.
#   Please note also that some shaders cannot be dynamically lighted
#   without requiring an additional "post-diffuse-lighting" operation.
#
#   23-Oct-00   floh    created
#   17-Jan-04   eric    rotaion fixed with timefreeze from pe2
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#   Lets define a few cool shaders...
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#   modulate_shader
#   Most simple shader, simply defines one texture layer which is
#   modulated with the polygons base color.
#--------------------------------------------------------------------

proc modulate_shader {nombre_textura} {
    new nshadernode sn
	sel sn
	    .setrenderpri 1
	    .setnumstages 1
	    .setlightenable true
	    .setcolorop 0 "mul tex prev"
	    .begintunit 0
		.setaddress "wrap" "wrap"
		.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
		.settexcoordsrc "uv0"
		.setenabletransform false
	    .endtunit
	    .setdiffuse 1.000000 1.000000 1.000000 1.000000
	    .setemissive 0.000000 0.000000 0.000000 0.000000
	    .setambient 0.5 0.5 0.5 0.5
	    .setalphaenable true
	    .setzwriteenable true
	sel ..
    
	new ntexarraynode tex
	sel tex
	.settexture 0 textures:$nombre_textura none
        .setgenmipmaps 0 true
	sel ..
}

proc terrain_shader {nombre_textura} {
	new nshadernode sn
        sel sn
            .setrenderpri 0
            .setnumstages 1
            .setcolorop 0 "mul tex prev"
            .setalphaop 0 "nop"
            .setconst 0 0.000000 0.000000 0.000000 0.000000
            .begintunit 0
                .setaddress wrap wrap
                .setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
                .settexcoordsrc "uv0"
                .setenabletransform false
            .endtunit
            .setcolorop 1 "mul tex prev"
            .setalphaop 1 "nop"
            .setconst 1 0.000000 0.000000 0.000000 0.000000
            .setdiffuse 0.2 0.2 0.2 0.2
            .setemissive 0.8 0.8 0.8 0.8
            .setambient 0.6 0.6 0.6 0.6
            .setlightenable true
            .setalphaenable false
            .setzwriteenable true
            .setfogenable true
            .setalphablend "srcalpha" "invsrcalpha"
            .setzfunc "lessequal"
        sel ..
    
	new ntexarraynode tex
	sel tex
	.settexture 0 terrain:$nombre_textura none
	sel ..
}


proc seleccion_shader {} {
    new nshadernode sn
        sel sn
	.setrenderpri 1
        .setnumstages 1
        .setcolorop 0 "mul tex prev"
        
        .begintunit 0
            .setaddress wrap wrap
            .setminmagfilter linear linear
            .settexcoordsrc uv0
            .setenabletransform false
        .endtunit

        .setlightenable true
        .setdiffuse  1 1 1 1
        .setemissive 1 1 1 1
        .setambient  1 1 1 1
        .setalphaenable true

    sel ..

    new ntexarraynode tex
        sel tex
        .settexture 0 textures:seleccion.png seleccion.png
    sel ..
}


#--------------------------------------------------------------------
#   gen_visual()
#   A simple visual hierarchy using the new nmeshnode object
#   and nshadernode objects. The shader object is always
#   generated under the name of 'pos/sn' so that it can be
#   replaced later.
#--------------------------------------------------------------------

proc gen_objeto {nombre_objeto mesh_objeto textura_objeto posx posy posz sx sy sz sombras} {
sel /game/scene
    loadmesh $nombre_objeto $mesh_objeto $textura_objeto
    sel $nombre_objeto
    .txyz $posx $posy $posz 
    sel ..
# puts "Generado Objeto '$nombre_objeto' \n"
/game.update   	    

}

proc gen_objeto_trans {nombre_objeto mesh_objeto textura_objeto posx posy posz sx sy sz sombras} {
sel /game/scene
    loadmesh $nombre_objeto $mesh_objeto $textura_objeto
    sel $nombre_objeto
    .setactive false
    .txyz $posx $posy $posz
    .sxyz $sx $sy $sz
    sel shader
	.setrenderpri 1
	.setnumstages 1
	.setcolorop 0 "add tex prev"
	.setconst 0 0.000000 0.000000 0.000000 0.000000
	.begintunit 0
	.setaddress "clamp" "clamp"
	.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
	.settexcoordsrc "uv0"
	.setenabletransform false
	.txyz 0.000000 0.000000 0.000000
	.rxyz 0.000000 0.000000 0.000000
	.sxyz 1.000000 1.000000 1.000000
	.endtunit			
	.setalphaenable true
	.setalphatestenable true
	.setalphablend "srcalpha" "invsrcalpha"
	.setalphafunc "greaterequal"
	.setalpharef $::AlphaRef
	.setalphaop 0 "add tex prev"	
	.setcolormaterial "material"
	.setzwriteenable true
sel /game/scene

# puts "Generado Objeto Transparente '$nombre_objeto' \n"
/game.update 
}

proc gen_terreno {nombre_objeto mesh_objeto textura_objeto posx posy posz sx sy sz sombras} {
sel /game/scene
    loadtmesh $nombre_objeto $mesh_objeto $textura_objeto
        sel $nombre_objeto
        .txyz $posx $posy $posz
        .sxyz $sx $sy $sz
    sel ..
# puts "Generado Terreno '$nombre_objeto' \n"
/game.update     
}

proc gen_luz {nombre_luz posx posy posz cx cy cz} {
	sel /game/scene
	new n3dnode dLuz
		sel dLuz
		.preload
		new n3dnode $nombre_luz
			sel $nombre_luz
			.preload
			new nlightnode light
				sel light
				.preload
				.setattenuation 0 0.01 0 
				.settype directional
				.setcolor $cx $cy $cz 1
				.setcastshadows true
			sel ..	
			.txyz $posx $posy $posz
		sel ..
    sel ..
# puts "Generada Luz '$nombre_luz' \n"
/game.update         
}