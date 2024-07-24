#--------------------------------------------------------------------
#  /code/src/Makefile
#  ==================
#  Makefile frontend for Nebula project directory.
#
#  12-Mar-2000  floh  complete rewrite for new build process...
#
#  (C) 1999..2000 A.Weissflog
#--------------------------------------------------------------------
include $(NOMADS_HOME)/code/defines.mak

#--------------------------------------------------------------------
# At home at RadonLabs, build everything including Nomads,
# in the world, only compile Nebula
#--------------------------------------------------------------------
ifeq ($(N_BUILDNOMADS),yes)
  all: everything
  global: update edit release all checkin
  include everything.mak
  include $(NOMADS_HOME)/code/src/cvs.mak
else
  all: nebula
  nebula.mak:
	@echo You must run tclsh8.4 updsrc.tcl!
	@echo Please read the compilation instructions in nebula/doc/source/compiling.txt.
	@exit 1
  include nebula.mak
endif

#--------------------------------------------------------------------
include $(NOMADS_HOME)/code/postmake.mak
#-----------------------------------------------------------------
