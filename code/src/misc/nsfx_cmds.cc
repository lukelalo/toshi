#define N_IMPLEMENTS nSpecialFxServer
//-------------------------------------------------------------------
//  nsfx_cmds.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    @scriptclass
    nspecialfxserver

    @superclass
    nroot

    @classinfo
    Central manager for frame-level visual special fx. The special fx
    server lives under the name '/sys/servers/specialfx'.
    Current offers are:
         - an alpha overlay plane
    Most special effects are configured by object hierarchies
    that live under the special fx server object. These are
    mostly conventional visual hierarchies like the ones
    that live under /usr/scene. Most of the objects must have
    a special name, because the special fx server needs to
    access them:
    
    /sys/servers/specialfx/root/    
        a 3dnode defining visual hierarchy root
    /sys/servers/specialfx/root/overlay
        a 3dnode defining the alpha overlay plane effect,
        usually has a noverlayplane, (optional) ntexarraynode and
        nshadernode subobject. The nshadernode object should be
        configured like this:
           .setalphaenable true
           .setalphablend srcalpha invsrcalpha
           .setzwriteenable false
           .setlightenable false
           .setfogenable false
           .setrenderpri <very high>
     /sys/servers/specialfx/root/overlay/plane
         an object of class 'noverlayplane'
*/

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
