proc OcultarInformacion { } {
/game/scene/hud/info.setactive false
}

proc SetTituloInfo { titulo } {
/game/scene/hud/info/info_titulo/texto.settext "$titulo"
/game/scene/hud/info/info_titulo/texto.loadfont "fonts:toshi2$::ResSel.png"
}

proc SetAccion1Info { accion } {
/game/scene/hud/info/info_accion1/texto.settext "$accion"
/game/scene/hud/info/info_accion1/texto.loadfont "fonts:toshi1$::ResSel.png"
}

proc SetAccion2Info { accion } {
/game/scene/hud/info/info_accion2/texto.settext "$accion"
/game/scene/hud/info/info_accion2/texto.loadfont "fonts:toshi1$::ResSel.png"
}

proc SetEspecial1Info { especial } {
/game/scene/hud/info/info_especial1/texto.settext "$especial"
/game/scene/hud/info/info_especial1/texto.loadfont "fonts:toshi1$::ResSel.png"
}

proc SetEspecial2Info { especial } {
/game/scene/hud/info/info_especial2/texto.settext "$especial"
/game/scene/hud/info/info_especial2/texto.loadfont "fonts:toshi1$::ResSel.png"
}

proc SetFotoInfo { foto } {
/game/scene/hud/info/foto/tex.settexture 0 "$foto" "none"
}

proc MostrarInformacion { } {
/game/scene/hud/info.setactive true
}

proc OcultarLineaInformacion { } {
/game/scene/hud/lineaInfo.setactive false
}


proc MostrarLineaInformacion { texto } {
/game/scene/hud/lineaInfo.setactive true
/game/scene/hud/lineaInfo/texto.settext "$texto"
}

proc OcultarInformacionContextual { } {
/game/scene/hud/infoContextual.setactive false
}


proc MostrarInformacionContextual { texto posx posy } {
/game/scene/hud/infoContextual.setactive true
/game/scene/hud/infoContextual/texto.settext "$texto"
/game/scene/hud/infoContextual/texto.setpositionf $posx [expr $posy+0.04]
}