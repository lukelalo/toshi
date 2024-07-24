#--------------------------------------------------------------------
#   logoToshi.tcl
#   ==================
#   An image projected on a wall of smoke.
#
#--------------------------------------------------------------------

proc DestructorVideoLogotipo { } {
delete /game/scene/video_logotipo
}

proc VideoLogotipo { } {
/game.playsong 0 "sounds:musica/musica_logo.mp3"
GenVideoLogotipo
}

proc GenVideoLogotipo { } {
sel /game/scene
new n3dnode video_logotipo
sel video_logotipo
	.setactive true
	.preload
	new n3dnode pos
	    sel pos

	    .txyz 10.000000 -8.000000 10.000000
	    .rxyz 0.000000 45.000000 0.000000
	    .sxyz 1 1 1

	#    new nipol ry
	#        sel ry
	#        .connect ry
	#        .addkey1f 0 0
	#        .addkey1f 20 360
	#    sel ..

	    new nspriterender sr
		sel sr

		# the particle emitter (emits particles from vertices of a vbuffer)
		new nmeshemitter pemit
		    sel pemit

		    # this mesh is just a line, so that the 'smoke' forms
		    # a projection walll
		    new nmeshnode mesh
			sel mesh
			#.setactive false
			.setreadonly true
			.setfilename "meshes:line.n3d"
		    sel ..

		    # particle emitter attributes... 
		    .settimes 0 0 0
		    .setlifetime 5
		    .setfreq 100
		    .setspeed 0
		    .setaccel -2 0 -2
		    .setmeshnode mesh
		sel ..

		# particle renderer attributes...
		.setemitter pemit
		.setspin 10
		.setspinaccel 0
		.beginkeys 3
		.setkey 0 2.0 0.0 0.0 0.0 0.0 1.0
		.setkey 1 3.0 0.0 0.8 0.8 0.8 1.0
		.setkey 2 2.0 0.0 0.0 0.0 0.0 1.0
		.endkeys
	    sel ..

	    # the shadernode is responsible to project the image
	    # on the smoke, we use 2 layers, the base layer defines
	    # the "particle", the second is the projected image
	    new nshadernode sn
		sel sn

		.setnumstages 2
		.setcolorop 0 "mul tex prev"
		.setcolorop 1 "mul tex prev"

		#.sxyz1 0.050000 0.050000 0.050000

		# rotate the projected texture
	#        new nipol rz
	#		    sel rz
	#    		.setreptype "oneshot"
	#    		.setchannel "time"
	#    		.setscale 1.000000
	#    		.connect "rxyz1"
	#    		.beginkeys 3 3
	#    		.setkey3f 0 0.000000 0.000000 0.000000 0.000000
	#    		.setkey3f 1 10.000000 90.000000 0.000000 360.000000
	#    		.setkey3f 2 20.000000 0.000000 0.000000 720.000000
	#    		.endkeys
	#	    sel ..
		new nipol ip
		sel ip
			.setreptype "oneshot"
			.setchannel "time"
			.setscale 1.000000
			.connect "sxyz1"
			.beginkeys 3 3
			.setkey3f 0 0.000000 5.000000 5.000000 5.000000
			.setkey3f 1 6.330000 0.04500 0.050000 0.050000
			.endkeys
		sel ..


		.begintunit 0
		    .setaddress wrap wrap
		    .setminmagfilter linear_mipmap_nearest linear
		    .settexcoordsrc uv0
		    .setenabletransform false
		.endtunit

		.begintunit 1
		    .setaddress wrap wrap
		    .setminmagfilter linear_mipmap_nearest linear
		    .settexcoordsrc eyespace
		    .setenabletransform true
		.endtunit

		.setlightenable false
		.setalphaenable true
		.setalphablend one one
		.setzwriteenable false
	    sel ..

	    new ntexarraynode tex
		sel tex
		.settexture 0 textures:toshiparticulas.bmp none
		.settexture 1 textures:logo.bmp none
sel /game/scene
}