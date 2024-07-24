#define N_IMPLEMENTS nTerrainNode
//-------------------------------------------------------------------
//  nterrain_cmds.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "terrain/nterrainnode.h"

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

//------------------------------------------------------------------------------
/**
    @scriptclass
    nterrainnode

    @superclass
    nvisnode

    @classinfo
    Wraps around Thatcher Ulrich's adaptive lod terrain renderer 
    adopted for Nebula.
*/
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
    cl->EndCmds();
}

//------------------------------------------------------------------------------
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
void n_setdetail(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    self->SetDetail(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
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
void n_getdetail(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    cmd->Out()->SetF(self->GetDetail());
}

//------------------------------------------------------------------------------
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
void n_setstaticerror(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    self->SetStaticError(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
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
void n_getstaticerror(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    cmd->Out()->SetF(self->GetStaticError());
}

//------------------------------------------------------------------------------
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
void n_setradius(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    self->SetRadius(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
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
void n_getradius(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    cmd->Out()->SetF(self->GetRadius());
}

//------------------------------------------------------------------------------
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
void n_setheightmap(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    self->SetHeightMap(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
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
void n_getheightmap(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    cmd->Out()->SetS(self->GetHeightMap());
}

//------------------------------------------------------------------------------
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
void n_settreefile(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    self->SetTreeFile(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettreefile

    @input
    s (TreeFilePath)

    @output
    v

    @info
    Return name of tree file, or 'null' if no tree file name
    is defined.
*/
void n_gettreefile(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    cmd->Out()->SetS(self->GetTreeFile());
}

//------------------------------------------------------------------------------
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
static void n_setuvscale(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    self->SetUvScale(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuvscale

    @input
    f (UvScale)

    @output
    v

    @info
    Get the uv scale values.
*/
static void n_getuvscale(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    cmd->Out()->SetF(self->GetUvScale());
}

//------------------------------------------------------------------------------
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
static void n_savetree(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    cmd->Out()->SetB(self->SaveTree(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
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
static void n_getnumnodes(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    int precull,postcull;
    self->GetNumNodes(precull,postcull);
    cmd->Out()->SetI(precull);
    cmd->Out()->SetI(postcull);
}

//------------------------------------------------------------------------------
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
static void n_savetowavefront(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
    cmd->Out()->SetB(self->SaveToWavefront(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
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
static void n_intersect(void *o, nCmd *cmd)
{
    nTerrainNode *self = (nTerrainNode *) o;
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
//  09-Oct-00   floh    + texture tiles
//-------------------------------------------------------------------
bool nTerrainNode::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nCmd *cmd;
        const char *p;

        //--- setdetail ---
        cmd = fs->GetCmd(this,'SDTL');
        cmd->In()->SetF(this->GetDetail());
        fs->PutCmd(cmd);

        //--- setstaticerror ---
        cmd = fs->GetCmd(this,'SSTE');
        cmd->In()->SetF(this->GetStaticError());
        fs->PutCmd(cmd);

        //--- setradius ---
        cmd = fs->GetCmd(this,'SRAD');
        cmd->In()->SetF(this->GetRadius());
        fs->PutCmd(cmd);

        //--- setuvscale ---
        cmd = fs->GetCmd(this,'SUVS');
        cmd->In()->SetF(this->GetUvScale());
        fs->PutCmd(cmd);

        //--- setheightmap/settreefile ---
        p = this->image_path.GetPath();
        if (p) {
            //--- setheightmap ---
            cmd = fs->GetCmd(this,'SHMP');
            cmd->In()->SetS(p);
            fs->PutCmd(cmd);
        } else {
            p = this->tree_path.GetPath();
            n_assert(p);
            //--- settreefile ---
            cmd = fs->GetCmd(this,'STRF');
            cmd->In()->SetS(p);
            fs->PutCmd(cmd);
        }
        retval = true;
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
