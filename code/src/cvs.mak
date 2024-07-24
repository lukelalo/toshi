#--------------------------------------------------------------------
#  cvs.mak
#  =======
#  Defines build targets for communication with CVS:
#    edit:     Make binaries editable
#    checkin:  Add new binaries and commit them
#    update:   Update source tree from cvs repository   
#  (C) 1999 A.Weissflog
#--------------------------------------------------------------------

ifeq ($(N_PLATFORM),__WIN32__)
  N_BINDIR   = $(NOMADS_HOME)/bin/win32
  N_BINFILES = n*.dll *.exe *.mll tclnebula.dll opcode11.dll
  N_ASCFILES = *.toc
else
  N_BINDIR   = $(NOMADS_HOME)/bin/linux
  N_BINFILES = libn*.so nsh gfxserv nomads wf*  libnopcode11.so
  N_ASCFILES = *.toc
endif

#--------------------------------------------------------------------
edit:
	-cd $(N_BINDIR); cvs edit $(N_BINFILES) $(N_ASCFILES)

#--------------------------------------------------------------------
checkin:
	-cd $(N_BINDIR); cvs add -kb $(N_BINFILES)
	-cd $(N_BINDIR); cvs add $(N_ASCFILES)
	-cd $(N_BINDIR); cvs commit -m "Binary_Update..." $(N_BINFILES) $(N_ASCFILES)
	-cd $(N_BINDIR); cvs edit $(N_BINFILES) $(N_ASCFILES)
#--------------------------------------------------------------------
update:
	-cd $(NOMADS_HOME)/code; cvs update

#--------------------------------------------------------------------
#  EOF
#--------------------------------------------------------------------
