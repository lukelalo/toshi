# 
#   loadtk.tcl
#
#   L�d - abh�ngig vom Betriebssystem und der
#   aktuellen TCL-Version die entsprechende TK_lib.
#   Mu� dazu zuerst tcl_library auf die mitgelieferten Libs setzen.
#   ACHTUNG ! DIESES FILE MUSS GE�NDERT WERDEN, FALLS MAL EINE Andere TK_LIB
#   MITGELIEFERT WIRD.
#   ALLE SCRIPTS DIE TK BRAUCHEN, M�SSEN DIESES SCRIPT ZUM LADEN VERWENDEN !
#   DIESES SCRIPT VERL�SST SICH DARAUF, da� das aufrufende Script die
#   Variable home mit NOMADS_HOME gef�llt hat.
#
#   Dieses Script SOURCED au�erdem diverse Standard-Routinen aus den Standard
#   Libs
#
#
#	created:	09-Jun-99
#	upd.:		07-Jul-99
#               05-Feb-00   floh    + auf Tk8.2 umgestellt
#               15-Dec-00   floh    + changed to Tk8.4
# 
#
#	(c) 1999 Bernd Beyreuther
#

set     tcl_library $home/bin/tcl/tcl8.4
set     tk_library  $home/bin/tcl/tk8.4

if {$tcl_platform(platform) == "windows"} {
    load    $home/bin/win32/tk84.dll
    source  $tk_library/focus.tcl
    source  $tk_library/button.tcl
} 
if { $tcl_platform(platform) == "unix" } {
    load libtk8.4.so tk
}
