# ---------------------------------------------------------------------------
#   mesh.tcl
#   ========
#   Handy utility methods relating to mesh or terrain.
#
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
#   Load a mesh with default attributes
# ---------------------------------------------------------------------------

proc loadcolormesh { name mesh texture color} {
    new n3dnode $name
        sel $name
	.preload
	new nmeshnode mesh
	mesh.preload
	mesh.setfilename "meshes:$mesh"
	mesh.setcastshadow false

	new nshadernode shader
	    sel shader
	    runshader bicolor.n
	sel ..

	new ntexarraynode texture
	sel texture
	.preload
	.settexture 0 "textures:$texture" "none"
	.settexture 1 "textures:$color" "none"

#	sel ..
    sel ..
/game.update
}

proc loadmesh { name mesh texture } {
    new n3dnode $name
        sel $name
	.preload
	new nmeshnode mesh
	mesh.preload
	mesh.setfilename "meshes:$mesh"
	mesh.setcastshadow false

	new nshadernode shader
	    sel shader
	    .preload
	    .setnumstages 1
	    .setrenderpri 1
	    .setlightenable true
	    .setcolorop 0 "add tex prev"
	    .begintunit 0
	    	.setaddress "wrap" "wrap"
		.setminmagfilter "linear_mipmap_nearest" "linear_mipmap_nearest"
		.settexcoordsrc "uv0"
		.setenabletransform false
	    .endtunit	    
            .setdiffuse 0.2 0.2 0.2 0.0
            .setemissive 0.0 0.0 0.0 0.0
            .setambient 0.0 0.0 0.0 0.0
            .setlightenable true
            .setalphaenable false
            .setzwriteenable true
            .setfogenable true
            .setalphablend "srcalpha" "invsrcalpha"
            .setzfunc "lessequal"	
            
            runshader seleccion.n
	sel ..
	
	new ntexarraynode texture
	sel texture
	    .preload
	    .settexture 0 "textures:$texture" "none"
	    .settexture 1 "textures:$texture" "none"
	sel ..
    sel ..
/game.update    
}

proc seleccionaEdificio { ruta } {

	sel $ruta
	
}

proc loadtmesh { name mesh texture renderpri} {
    new n3dnode $name
        sel $name
	.preload
	new nmeshnode mesh
	mesh.preload
	mesh.setfilename "meshes:$mesh"
	mesh.setcastshadow false

	new nshadernode shader
	    sel shader
	    .preload
	    .setrenderpri -3
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
		.sxyz 1 1 1
		.endtunit			
		.setalphaenable true
		.setalphatestenable true
		.setalphablend "srcalpha" "invsrcalpha"
		.setalphafunc "greaterequal"
		.setalpharef $::AlphaRef
		.setalphaop 0 "replace tex prev"	
		.setcolormaterial "diffuse"
		.setzwriteenable true
		.setzfunc "lessequal"
	sel ..

	new ntexarraynode texture
	sel texture
	    .preload
	    .settexture 0 "terrain:$texture" "none"
	sel ..
    sel ..
/game.update        
}

proc cargasuelo { name mesh texture renderpri} {
    new n3dnode $name
        sel $name
	.preload
	new nmeshnode mesh
	mesh.preload
	mesh.setfilename "meshes:$mesh"
	mesh.setcastshadow false

	new nshadernode shader
	    sel shader
	    .preload
	    .setrenderpri -3
		.setnumstages 2
		.setcolorop 0 "add tex prev"
		.setcolorop 1 "add tex prev"		
		.setconst 0 0.000000 0.000000 0.000000 0.000000
		.begintunit 0
		.setaddress "wrap" "wrap"
		.setminmagfilter "linear_mipmap_linear" "linear_mipmap_linear"
		.settexcoordsrc "uv0"
		.setenabletransform true
		.txyz 0.000000 0.000000 0.000000
		.rxyz 0.000000 0.000000 0.000000
		.sxyz 16 16 16
		.endtunit
		
		.begintunit 1
		.setaddress "clamp" "clamp"
		.setminmagfilter "linear" "linear"
		.settexcoordsrc "uv0"
		.setenabletransform true
		.txyz 20.000000 20.000000 20.000000
		.rxyz 0.000000 0.000000 0.000000
		.sxyz 0.5 0.5 0.5
		.endtunit
		
		.setalphaenable true
		.setalphatestenable true
		.setalphablend "srcalpha" "invsrcalpha"
		.setalphafunc "greaterequal"
		.setalpharef $::AlphaRef
		.setalphaop 0 "replace tex prev"	
		.setcolormaterial "diffuse"
		.setzwriteenable false
		.setzfunc "lessequal"
		
	sel ..

	new ntexarraynode texture
	sel texture
	    .preload
	    .settexture 0 "terrain:$texture" "none"
	    .settexture 1 "textures:seleccion3.png" "none"	    
	sel ..
    sel ..
/game.update      
}