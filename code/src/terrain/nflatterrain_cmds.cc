#define N_IMPLEMENTS nFlatTerrainNode
//-------------------------------------------------------------------
//  nflatterrain_cmds.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "terrain/nflatterrainnode.h"

// New dynamic texturing!
/* Undefined at the moment:
static void n_begintexturearray(void *, nCmd *);
static void n_settexture(void *, nCmd *);
static void n_endtexturearray(void *, nCmd *);
static void n_gettexturearraysize(void *, nCmd *);
static void n_gettexture(void *, nCmd *);
*/

static void n_getheight(void *, nCmd *);
static void n_setdetail(void *, nCmd *);
static void n_getdetail(void *, nCmd *);
static void n_setstaticerror(void *, nCmd *);
static void n_getstaticerror(void *, nCmd *);
static void n_setradius(void *, nCmd *);
static void n_getradius(void *, nCmd *);
static void n_setheightmap(void *, nCmd *);
static void n_getheightmap(void *, nCmd *);
static void n_setuvscale(void *, nCmd *);
static void n_getuvscale(void *, nCmd *);
static void n_savetree(void *, nCmd *);
static void n_settreefile(void *, nCmd *);
static void n_gettreefile(void *, nCmd *);
static void n_getnumnodes(void *, nCmd *);
static void n_intersect(void *, nCmd *);
static void n_savetowavefront(void *, nCmd *);
static void n_weld(void *, nCmd *);

//-------------------------------------------------------------------
/**
    @scriptclass
    nflatterrainnode

    @superclass
    nvisnode

    @classinfo
    Wraps around Thatcher Ulrich's adaptive lod terrain renderer
    adopted for Nebula.
*/
//-------------------------------------------------------------------
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setdetail_f",         'SDTL', n_setdetail);
    cl->AddCmd("f_getdetail_v",         'GDTL', n_getdetail);
    cl->AddCmd("v_setstaticerror_f",    'SSTE', n_setstaticerror);
    cl->AddCmd("f_getstaticerror_v",    'GSTE', n_getstaticerror);
    cl->AddCmd("v_setradius_f",         'SRAD', n_setradius);
    cl->AddCmd("f_getradius_v",         'GRAD', n_getradius);
    cl->AddCmd("v_setheightmap_s",      'SHMP', n_setheightmap);
    cl->AddCmd("s_getheightmap_v",      'GHMP', n_getheightmap);
    cl->AddCmd("v_settreefile_s",       'STRF', n_settreefile);
    cl->AddCmd("s_gettreefile_v",       'GTRF', n_gettreefile);
    cl->AddCmd("v_setuvscale_f",        'SUVS', n_setuvscale);
    cl->AddCmd("f_getuvscale_v",        'GUVS', n_getuvscale);
    cl->AddCmd("b_savetree_s",          'STRE', n_savetree);
    cl->AddCmd("ii_getnumnodes_v",      'GNND', n_getnumnodes);
    cl->AddCmd("bf_intersect_ffffff",   'INSC', n_intersect);
    cl->AddCmd("b_savetowavefront_s",   'STWF', n_savetowavefront);
    cl->AddCmd("b_weld_ss",				'WELD', n_weld);
    cl->AddCmd("f_getheight_ff",		'GHGT', n_getheight);
    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    setdetail

    @input
    f (Detail)

    @output
    v

    @info
    Set the detail threshold value.
*/
//-------------------------------------------------------------------
void n_setdetail(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    self->SetDetail(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getdetail

    @input
    v

    @output
    f (Detail)

    @info
    Get the detail threshold value.
*/
//-------------------------------------------------------------------
void n_getdetail(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    cmd->Out()->SetF(self->GetDetail());
}

//-------------------------------------------------------------------
/**
    @cmd
    setstaticerror

    @input
    f (StaticError)

    @output
    v

    @info
    Error threshold used to remove triangles in flat areas
    of the height map. This happens once after loading a heightmap.
    The default value is 25. Changing this value causes a heightmap
    reload.
*/
//-------------------------------------------------------------------
void n_setstaticerror(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    self->SetStaticError(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getstaticerror

    @input
    v

    @output
    f (StaticError)

    @info
    Return the static error value set by 'setstaticerror'.
*/
//-------------------------------------------------------------------
void n_getstaticerror(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    cmd->Out()->SetF(self->GetStaticError());
}

//-------------------------------------------------------------------
/**
    @cmd
    setradius

    @input
    f (Radius)

    @output
    v

    @info
    Define the maximum radius of the terrain in meters. Default
    radius is 10 meters. 
*/
//-------------------------------------------------------------------
void n_setradius(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    self->SetRadius(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getradius

    @input
    v

    @output
    f (Radius)

    @info
    Return the radius defined by 'setradius'.
*/
//-------------------------------------------------------------------
void n_getradius(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    cmd->Out()->SetF(self->GetRadius());
}

//-------------------------------------------------------------------
/**
    @cmd
    setheightmap

    @input
    s (HeightMapPath)

    @output
    v

    @info
    Define the name of a 24 bpp BMP file which is loaded as height map.
    Only the red channel is used.
*/
//-------------------------------------------------------------------
void n_setheightmap(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    self->SetHeightMap(cmd->In()->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    getheightmap

    @input
    v

    @output
    s (HeightMapPath)

    @info
    Return the current height map filename.
*/
//-------------------------------------------------------------------
void n_getheightmap(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    cmd->Out()->SetS(self->GetHeightMap());
}

//-------------------------------------------------------------------
/**
    @cmd
    settreefile

    @input
    s (TreeFilePath)

    @output
    v

    @info
    Define name of a tree file (saved with 'savetree' command),
    which is loaded instead of a height map.
*/
//-------------------------------------------------------------------
void n_settreefile(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    self->SetTreeFile(cmd->In()->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    gettreefile

    @input
    v

    @output
    s (TreeFilePath)

    @info
    Return name of tree file, or 'null' if no tree file name
    is defined.
*/
//-------------------------------------------------------------------
void n_gettreefile(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    cmd->Out()->SetS(self->GetTreeFile());
}

//-------------------------------------------------------------------
/**
    @cmd
    setuvscale

    @input
    f (UvScale)

    @output
    v

    @info
    Set scaling values for texture coord generation from 3d 
    coordinates:

      u = x * scale
      v = z * scale

    Default value is 1.0f (texture repeats every meter).
*/
//-------------------------------------------------------------------
static void n_setuvscale(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    self->SetUvScale(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getuvscale

    @input
    v

    @output
    f (UvScale)

    @info
    Get the uv scale values.
*/
//-------------------------------------------------------------------
static void n_getuvscale(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    cmd->Out()->SetF(self->GetUvScale());
}

//-------------------------------------------------------------------
/**
    @cmd
    savetree

    @input
    s (Filename)

    @output
    b (Success)

    @info
    Save the internal quadtree structure into a private fileformat.
    Loading from the fileformat avoids excessive preprocessing
    and memory usage which happens when loading from a bitmap
    heightfield.
*/
//-------------------------------------------------------------------
static void n_savetree(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    cmd->Out()->SetB(self->SaveTree(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    getnumnodes

    @input
    v

    @output
    i (PreCullNodes), i (PostCullNodes)

    @info
    Get some statistics for static culling (this culling occurs
    when loading heightmap data from a bitmap via 'setheightmap'.
    The lower the value set by 'setstaticerror', the more detail
    will be removed.
*/
//-------------------------------------------------------------------
static void n_getnumnodes(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    int precull,postcull;
    self->GetNumNodes(precull,postcull);
    cmd->Out()->SetI(precull);
    cmd->Out()->SetI(postcull);
}

//-------------------------------------------------------------------
/**
    @cmd
    savetowavefront

    @input
    s (Filename)

    @output
    b (Success)

    @info
    Save the terrain geometry to a Wavefront file. Can be
    used to generate collision geometry and stuff...
*/
//-------------------------------------------------------------------
static void n_savetowavefront(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    cmd->Out()->SetB(self->SaveToWavefront(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    weld

    @input
    s (The WESTern nFlatTerrainNode's name "/usr/scene/grid1/terrain1" to weld to (or "none"))
    s (The NORTHern nFlatTerrainNode's name "/usr/scene/grid1/terrain3" to weld to (or "none"))

    @output
    b (Success)

    @info
    Weld this flatterrain to other terrains - causing their edges to 
    align seamlessly.  The specified nFlatTerrainNode's edges will 
    alter to the median difference between it's eastern edge vertices
    and this one's.  The normals are then aligned by copying this 
    terrain's normals onto the argument's edges.  This fixes a slight
    oddity in the terrain's generation that causes the south and east
    edges to look like slopes when lit.
*/
//-------------------------------------------------------------------
static void n_weld(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    {
        const char *arg0 = cmd->In()->GetS();
        const char *arg1 = cmd->In()->GetS();
        nRoot *south = self->ks->Lookup(arg0);
		nRoot *east  = self->ks->Lookup(arg1);

		// Set failure if neither can be found - or else continue
        if ( south || east ) cmd->Out()->SetB(self->Weld( south, east ));
        else    cmd->Out()->SetB(FALSE);
    }
}

//-------------------------------------------------------------------
/**
    @cmd
    getheight

    @input
    f (x coordinate), f (z coordinate)

    @output
    f height at this point

    @info
    Get the height (y) of the terrain at this given x/z coordinate
*/
//-------------------------------------------------------------------
static void n_getheight(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    {
        const float arg0 = cmd->In()->GetF();
        const float arg1 = cmd->In()->GetF();

		cmd->Out()->SetF( self->GetHeight( arg0, arg1 ) );
    }
}

//-------------------------------------------------------------------
/**
    @cmd
    intersect

    @input
    fff (Start), fff (End)

    @output
    b (Intersection), f (Intersectionpos)

    @info
    DEBUG ONLY!!!
*/
//-------------------------------------------------------------------
static void n_intersect(void *o, nCmd *cmd)
{
    nFlatTerrainNode *self = (nFlatTerrainNode *) o;
    vector3 v0,v1;
    v0.x = cmd->In()->GetF();
    v0.y = cmd->In()->GetF();
    v0.z = cmd->In()->GetF();
    v1.x = cmd->In()->GetF();
    v1.y = cmd->In()->GetF();
    v1.z = cmd->In()->GetF();
    line3 l(v0,v1);
    float ipos;
    triangle itri;
    bool b = self->Intersect(l,ipos,itri);
    cmd->Out()->SetB(b);
    cmd->Out()->SetF(ipos);
}

//-------------------------------------------------------------------
//  SaveCmds()
//  27-Apr-00   floh    created
//-------------------------------------------------------------------
bool nFlatTerrainNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;
        const char *p;

        //--- setdetail ---
        if ((cmd = fs->GetCmd(this,'SDTL'))) {
            cmd->In()->SetF(this->GetDetail());
            fs->PutCmd(cmd);
        }

        //--- setstaticerror ---
        if ((cmd = fs->GetCmd(this,'SSTE'))) {
            cmd->In()->SetF(this->GetStaticError());
            fs->PutCmd(cmd);
        }

        //--- setradius ---
        if ((cmd = fs->GetCmd(this,'SRAD'))) {
            cmd->In()->SetF(this->GetRadius());
            fs->PutCmd(cmd);
        }

        //--- setuvscale ---
        if ((cmd = fs->GetCmd(this,'SUVS'))) {
            cmd->In()->SetF(this->GetUvScale());
            fs->PutCmd(cmd);
        }

        //--- setheightmap/settreefile ---
        p = this->image_path.GetPath();
        if (p) {
            //--- setheightmap ---
            if ((cmd = fs->GetCmd(this,'SHMP'))) {
                cmd->In()->SetS(p);
                fs->PutCmd(cmd);
            }
        } else {
            p = this->tree_path.GetPath();
            n_assert(p);
            //--- settreefile ---
            if ((cmd = fs->GetCmd(this,'STRF'))) {
                cmd->In()->SetS(p);
                fs->PutCmd(cmd);
            }
        }
        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
