#--------------------------------------------------------------------
#   gentocfile.tcl [target] [platform=win32|linux|macosx] [build=release|debug]
#   ==============================================
#   Generate class package .toc file from Nebula package definition.
#   TOC files will be written to $nomads_home/bin/win32, $nomads_home/bin/linux
#   or $nomads_home/bin/macosx. Should be called from the normal build
#   process.
#
#	04-Mar-01	leaf	Relative path fallback if NOMADS_HOME is not defined
#   07-Mar-01   leaf    Recognises extensions (*.epk)
#--------------------------------------------------------------------

if [info exists env(NOMADS_HOME)] {
	set nomads_home [string map {\\ /} $env(NOMADS_HOME)]
} else {
	set nomads_home [file join [file dir [info script]] .. .. ]
}
	
set home $nomads_home
source $nomads_home/bin/makemake.tcl

set target_name [lindex $argv 0]
set target_platform [lindex $argv 1]
if {$argc == 3} {
    set target_build [lindex $argv 2]
} else {
    set target_build release
}

#--------------------------------------------------------------------
#   Are we at home?
#--------------------------------------------------------------------
if {[catch {set dummy $env(N_BUILDNOMADS)} err]==0} {
    set athome 1
} else {
    set athome 0
}

#--------------------------------------------------------------------
#   read package definition files
#--------------------------------------------------------------------
set global_gendeps 0
foreach ext [glob -nocomplain $home/code/src/*.{pak,epk} $home/code/src/*/*.{pak,epk}] {
    source $ext
}

#--------------------------------------------------------------------
#   find the target in question and check that it is indeed a package
#--------------------------------------------------------------------
set t [findtargetbyname $target_name]
if {$tar($t,type) != "package"} {
    puts "Target $target_name not a class package!"
    exit
}

#--------------------------------------------------------------------
#   append 'd' to outdir if this is a debug build
#--------------------------------------------------------------------
if {$target_build == "debug"} {
	set outdir $target_platform\d
} else {
	set outdir $target_platform
}

#--------------------------------------------------------------------
#   generate the toc file
#--------------------------------------------------------------------
gentocfile $target_name $outdir

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

