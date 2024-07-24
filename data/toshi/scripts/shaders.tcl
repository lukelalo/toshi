#--------------------------------------------------------------------
#   shaders.tcl
#   ===========
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
            .setminmagfilter linear_mipmap_nearest linear_mipmap_nearest
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