# ---------------------------------------------------------------------------
#   skybox.tcl
#   ==========
#   Provides method to create a skybox
#
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
#   Create a skybox plane, unable to share nshadernode
# ---------------------------------------------------------------------------
proc createskyboxplane { name texture } {
    new n3dnode $name
    sel $name
	.preload
        new nmeshnode mesh
        mesh.preload
        mesh.setfilename "meshes:cielo.n3d"

        new nshadernode shader
        sel shader
            .preload
            .setrenderpri -10
            .setnumstages 1
            .setcolorop 0 "replace tex.c"
            .begintunit 0
                .setaddress "clamp" "clamp"
                .setminmagfilter "linear_mipmap_linear" "linear_mipmap_linear"
                .settexcoordsrc "uv0"
                .setenabletransform false
            .endtunit        
            .setlightenable false
            .setalphaenable false
            .setzwriteenable false
            .setfogenable false
            .setalphablend "srcalpha" "invsrcalpha"
            .setzfunc "always"
            .setcullmode "ccw"
            .setcolormaterial "material"
        sel ..
        new ntexarraynode texture
        texture.preload
        texture.settexture 0 "textures:skybox/$texture" "none"
        texture.setgenmipmaps 0 true
        texture.sethighquality 0 true
    sel ..
}

# ---------------------------------------------------------------------------
#   Create a skybox
# ---------------------------------------------------------------------------
proc createskybox { name } {
    new n3dnode $name
    sel $name

        .setlockviewer true

        createskyboxplane front ft.bmp  
        sel front
            .txyz 0.0 0.0 -1.0
            .rxyz 90 0 0
        sel ..

        createskyboxplane back bk.bmp  
        sel back
            .txyz 0.0 0.0 1.0
            .rxyz 90.0 180 0.0
        sel ..

        createskyboxplane left lt.bmp 
        sel left
            .txyz -1.0 0.0 0.0
            .rxyz 90.0 90.0 0.0
        sel ..

        createskyboxplane right rt.bmp 
        sel right
            .txyz 1.0 0.0 0.0
            .rxyz 90.0 -90.0 0.0
        sel ..

        #createskyboxplane up up.bmp
        #sel up
        #    .txyz 0.0 1.0 0.0
        #    .rxyz 180.0 90.0 0.0
        #sel ..


    sel ..
}

