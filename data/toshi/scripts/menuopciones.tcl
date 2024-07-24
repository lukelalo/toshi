proc MenuOpciones { } {
new nmenu /data/menus/menuOpciones
sel /data/menus/menuOpciones

	.setnodo "/game/scene/menus/MenuOpciones"
	.setimagenfondo "menus:MenuOpciones.png"
	
	set numopcion [.agregaropcion "SeleccionarGraficos"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonGraficos.png"
	.modificarimagenoveropcion $numopcion "menus:BotonGraficosOver.png"
	.modificarposicionopcion $numopcion 0.35 0.39
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
	set numopcion [.agregaropcion "SeleccionarSonido"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonSonido.png"
	.modificarimagenoveropcion $numopcion "menus:BotonSonidoOver.png"
	.modificarposicionopcion $numopcion 0.35 0.5
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
	set numopcion [.agregaropcion "SeleccionarAyuda"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonAyuda.png"
	.modificarimagenoveropcion $numopcion "menus:BotonAyudaOver.png"
	.modificarposicionopcion $numopcion 0.35 0.61
	.modificardimensionesopcion $numopcion 0.335 0.1012	
	
	set numopcion [.agregaropcion "SeleccionarVolver"]
	.modificarimagenprincipalopcion $numopcion "menus:BotonVolver.png"
	.modificarimagenoveropcion $numopcion "menus:BotonVolverOver.png"
	.modificarposicionopcion $numopcion 0.35 0.88
	.modificardimensionesopcion $numopcion 0.335 0.1012
	
}

# Menu Opciones

proc SeleccionarGraficos { } {
SonidoClick
set ::MenuAnterior "/data/menus/menuOpciones"
MostrarMenu "/data/menus/menuGraficos" -1.0 ""
}

proc SeleccionarSonido { } {
SonidoClick
set ::MenuAnterior "/data/menus/menuOpciones"
MostrarMenu "/data/menus/menuSonido" -1.0 ""
}

proc SeleccionarAyuda { } {
SonidoClick
set ::MenuAnterior "/data/menus/menuOpciones"
MostrarMenu "/data/menus/menuAyuda" -1.0 ""
}