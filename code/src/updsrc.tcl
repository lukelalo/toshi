#--------------------------------------------------------------------
#   updsrc.tcl
#   ==========
#   Global Nebula package definition file. Run through tcl shell
#   to update Makefiles and VS workspace files
#   AFTER ADDING NEW FILES OR PACKAGES!
#
#   Automatic Package Building
#   ------------------------------------
#   This script loads *.pak files and adds all targets within to
#   a workspace. The default workspace is nebula, but this can be changed
#   by putting in the command 'workspace <name>' in the .pak file. The
#   wftools.pak file is an example of this.
#   note: For makefiles a single makefile is created, containing all
#   targets.
#
#   (C) 2000 A.Weissflog
#
#   04-Mar-00   floh    created
#   15-May-00   floh    ndsound package added
#   26-May-00   floh    nfmod package added
#   07-Jun-00   floh    ncollide package added
#   11-Jul-00   floh    + wfnormalize
#   22-Jul-00   floh    + cleaned up a lot
#   22-Aug-00   floh    + wfradius
#   01-Nov-00   floh    + nparticle and nobsolete packages
#   07-Nov-00   floh    + plugins package
#   27-Nov-00   floh    + added tclnebula package to public Nebula targets
#   08-Jan-01   floh    + nterrain.pak
#   21-Jan-01   floh    + runcpptests target
#   24-Jan-01   floh    + added ndirect3d8 package
#   04-Mar-01   leaf    relative path fallback if NOMADS_HOME is not defined
#   07-Mar-01   leaf    automatic extension package building. see above.
#   03-Apr-01   floh    + added wfplanar tool
#   24-May-01   floh    + added nebulatests.pak
#   14-Feb-01   leaf    added code to generate VS.NET build files,
#                       contributed by Stefan Kirsch.
#   18-Feb-02   leaf    now loads .pak files automatically, there is
#                       no need to hardcode .pak files within this script.
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#   Set home directory
#--------------------------------------------------------------------
if [info exists env(NOMADS_HOME)] {
    set nomads_home [string map {\\ /} $env(NOMADS_HOME)]
} else {
    set nomads_home [file join [file dir [info script]] .. .. ]
}
    
set home $nomads_home
set includePath "$nomads_home/code/inc"

source $nomads_home/bin/makemake.tcl

#--------------------------------------------------------------------
#   Are we at home?
#--------------------------------------------------------------------
if {[catch {set dummy $env(N_BUILDNOMADS)} err]==0} {
    set athome 1
} else {
    set athome 0
}

#--------------------------------------------------------------------
#   if at home, 'cvs edit' all affected files
#--------------------------------------------------------------------
if {$athome} {
    puts ":: PERFORMING CVS EDIT"
    make_cvs_edit $nomads_home/code
}

#--------------------------------------------------------------------
# read package definition files
#--------------------------------------------------------------------
set global_gendeps 1
foreach ext [glob -nocomplain $home/code/src/*.{pak,epk} $home/code/src/*/*.{pak,epk}] {
    workspace nebula
    source $ext
}

#====================================================================

#--------------------------------------------------------------------
#   Generate the workspaces
#--------------------------------------------------------------------
set all_targets {}
foreach {ws wsl} [array get workspaces] {
    
    # remove any duplicate targets
    set wsl [lsort -unique $wsl]

    puts ""
    puts ":: GENERATING WORKSPACE $ws..."
    puts "==========================="
    
    # Build VS6 workspace files.
    gen_workspace $ws $wsl
    # Build VS7 solution files.
    gen_solution $ws $wsl
    foreach {m} $wsl {
    	lappend all_targets $m
    }
}

#--------------------------------------------------------------------
#   Generate Makefile for everything
#--------------------------------------------------------------------
puts ""
puts ":: GENERATING nebula.mak..."
puts "==============================="
set all_targets [lsort -unique $all_targets]
gen_makefile nebula $all_targets

#--------------------------------------------------------------------
#   if at home, 'cvs add' all affected files
#--------------------------------------------------------------------
if {$athome} {
    puts ":: PERFORMING CVS ADD..."
    make_cvs_add $nomads_home/code
}

puts "done."

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

