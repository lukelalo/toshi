# ---------------------------------------------------------------------------
#   console.tcl
#   ===========
#   Código relacionado con la consola TCL, que solo estará disponible en
# modo debug.
# ---------------------------------------------------------------------------
global debug

if { $debug == "true" } {
    new nconserver    /sys/servers/console
    #/sys/servers/console.watch gfx_triangles
    #/sys/servers/console.watch mem_alloc
    #/sys/servers/console.watch mem_used
    #/sys/servers/console.watch *
}

# ---------------------------------------------------------------------------
# Procedimiento llamado al asignar teclas. Si se esta en modo debug,
# se asigna la tecla ESC a la consola.
# ---------------------------------------------------------------------------
proc mapconsolekey { } {
    global debug

    if { $debug == "true" } {
	/sys/servers/input.beginmap    
        /sys/servers/input.map keyb0:esc.down         	"script:/sys/servers/console.toggle"
        /sys/servers/input.endmap
    }
}