#define N_IMPLEMENTS nCurveArrayNode
//------------------------------------------------------------------------------
//  ncurvearraynode_cmds.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/ncurvearraynode.h"
#include "kernel/npersistserver.h"

static void n_setfilename(void* slf, nCmd* cmd);
static void n_getfilename(void* slf, nCmd* cmd);
static void n_begincurvebundles(void* slf, nCmd* cmd);
static void n_beginbundle(void* slf, nCmd* cmd);
static void n_setbundlecurve(void* slf, nCmd* cmd);
static void n_endbundle(void* slf, nCmd* cmd);
static void n_endcurvebundles(void* slf, nCmd* cmd);
static void n_beginconnects(void* slf, nCmd* cmd);
static void n_setcurveconnect(void* slf, nCmd* cmd);
static void n_setcurveconnect2(void* slf, nCmd* cmd);
static void n_setbundleconnect(void* slf, nCmd* cmd);
static void n_setbundleconnect2(void* slf, nCmd* cmd);
static void n_endconnects(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ncurvearraynode
    
    @superclass
    nvisnode

    @classinfo
    Container for animation curves, connects animation curves to object
    commands. Curves can be "bundled" into curve bundles, a curve bundle
    mixes many curve samples into one resulting sample, controlled
    by per-curve-weights.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setfilename_s",           'SFLN', n_setfilename);
    cl->AddCmd("s_getfilename_v",           'GFLN', n_getfilename);
    cl->AddCmd("v_begincurvebundles_i",     'BGCB', n_begincurvebundles);
    cl->AddCmd("v_beginbundle_isi",         'BGBD', n_beginbundle);
    cl->AddCmd("v_setbundlecurve_iss",      'SCBD', n_setbundlecurve);
    cl->AddCmd("v_endbundle_v",             'EDBD', n_endbundle);
    cl->AddCmd("v_endcurvebundles_v",       'EBCB', n_endcurvebundles);
    cl->AddCmd("v_beginconnects_i",         'BGCN', n_beginconnects);
    cl->AddCmd("v_setcurveconnect_isss",    'SCCN', n_setcurveconnect);
    cl->AddCmd("v_setcurveconnect2_issss",  'SCC2', n_setcurveconnect2);
    cl->AddCmd("v_setbundleconnect_isss",   'SBCN', n_setbundleconnect);
    cl->AddCmd("v_setbundleconnect2_issss", 'SBC2', n_setbundleconnect2);
    cl->AddCmd("v_endconnects_v",           'EDCN', n_endconnects);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfilename
    @input
    s(AnimFileName)
    @output
    v
    @info
    Sets the filename of the animation resource file which contains
    curve definitions. The file is loaded immediately.
*/
static
void
n_setfilename(void* slf, nCmd* cmd)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    self->SetFilename(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfilename
    @input
    v
    @output
    s(AnimFileName)
    @info
    Get the filename of the animation resource file.
*/
static
void
n_getfilename(void* slf, nCmd* cmd)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    cmd->Out()->SetS(self->GetFilename());
}

//------------------------------------------------------------------------------
/**
    @cmd
    begincurvebundles
    @input
    i(NumCurveBundles)
    @output
    v
    @info
    Begin defining curve bundles. Takes the number of curve bundles as
    argument.
*/
static
void
n_begincurvebundles(void* slf, nCmd* cmd)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    self->BeginCurveBundles(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginbundle
    @input
    i(BundleIndex), s(BundleName), i(NumCurves)
    @output
    v
    @info
    Begin defining a single curve bundle, defined by BundleIndex. Takes
    a BundleName and the number of curves in the bundle as args.
*/
static
void
n_beginbundle(void* slf, nCmd* cmd)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    int i0         = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    int i1         = cmd->In()->GetI();
    self->BeginBundle(i0, s0, i1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setbundlecurve
    @input
    i(CurveIndex), s(CurveName), s(ChannelName)
    @output
    v
    @info
    Adds a curve to the curve bundle. CurveName is the name of the curve
    (must exist in the already loaded anim file), ChannelName is the
    name of an animation channel which provides the weight for this
    curve for mixing.
*/
static
void
n_setbundlecurve(void* slf, nCmd* cmd)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    int i0         = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetBundleCurve(i0, s0, s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endbundle
    @input
    v
    @output
    v
    @info
    Finish defining the current bundle.
*/
static
void
n_endbundle(void* slf, nCmd* /*cmd*/)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    self->EndBundle();
}

//------------------------------------------------------------------------------
/**
    @cmd
    endcurvebundles
    @input
    v
    @output
    v
    @info
    Finish defining all curve bundles.
*/
static
void
n_endcurvebundles(void* slf, nCmd* /*cmd*/)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    self->EndCurveBundles();
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginconnects
    @input
    i(NumConnects)
    @output
    v
    @info
    Begin defining connections, takes the number of connections as argument.
    A connection connects an animation curve or bundle to an object's
    script command. The sampled value from the curve or bundle will be wrapped
    into a script command and invoked on the object.
*/
static
void
n_beginconnects(void* slf, nCmd* cmd)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    self->BeginConnects(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcurveconnect
    @input
    i(ConnectionIndex), s(CurveName), s(ObjectName), s(CmdName)
    @output
    v
    @info
    Define a curve connection. This connects the curve named 'CurveName' (must
    exist in the loaded animation resource file) to command 'CmdName' of
    object 'ObjectName'. Every frame, curve will be sampled at the current
    frame time, the resulting value be wrapped into a script command (which
    may take 1..4 float arguments), and the command will be invoked on
    the object defined by 'ObjectName'.
*/
static
void
n_setcurveconnect(void* slf, nCmd* cmd)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    int i0 = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    const char* s2 = cmd->In()->GetS();
    self->SetCurveConnect(i0, s0, s1, s2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcurveconnect2
    
    @input
    i(ConnectionIndex), s(CurveName), s(ObjectName), s(CmdName), s(ChnName)
    
    @output
    v
    
    @info
    Like setcurveconnect, but allows an explicit channel name for the
    sample time to be defined.
*/
static
void
n_setcurveconnect2(void* slf, nCmd* cmd)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    int i0 = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    const char* s2 = cmd->In()->GetS();
    const char* s3 = cmd->In()->GetS();
    self->SetCurveConnect2(i0, s0, s1, s2, s3);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setbundleconnect
    @input
    i(ConnectionIndex), s(BundleName), s(ObjectName), s(CmdName)
    @output
    v
    @info
    Works exactly like 'setcurveconnect', except that a complete curve bundle
    will be connected to an object's script command.
*/
static
void
n_setbundleconnect(void* slf, nCmd* cmd)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    int i0 = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    const char* s2 = cmd->In()->GetS();
    self->SetBundleConnect(i0, s0, s1, s2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setbundleconnect2
    
    @input
    i(ConnectionIndex), s(BundleName), s(ObjectName), s(CmdName)
    
    @output
    v
    
    @info
    Like setbundleconnect, but allows an explicit channel name for
    the sample time to be defined.
*/
static
void
n_setbundleconnect2(void* slf, nCmd* cmd)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    int i0 = cmd->In()->GetI();
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    const char* s2 = cmd->In()->GetS();
    const char* s3 = cmd->In()->GetS();
    self->SetBundleConnect2(i0, s0, s1, s2, s3);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endconnects
    @input
    v
    @output
    v
    @info
    Finish defining connections.
*/
static
void
n_endconnects(void* slf, nCmd* /*cmd*/)
{
    nCurveArrayNode* self = (nCurveArrayNode*) slf;
    self->EndConnects();
}


//------------------------------------------------------------------------------
/**
*/
bool
nCurveArrayNode::SaveCmds(nPersistServer* fs)
{
    if (nAnimNode::SaveCmds(fs))
    {
        nCmd* cmd;

        //--- setfilename ---
        cmd = fs->GetCmd(this, 'SFLN');
        cmd->In()->SetS(this->GetFilename());
        fs->PutCmd(cmd);

        if (this->numCurveBundles > 0)
        {
            //--- begincurvebundles ---
            cmd = fs->GetCmd(this, 'BGCB');
            cmd->In()->SetI(this->numCurveBundles);
            fs->PutCmd(cmd);

            // for each curve bundle...
            int i;
            for (i = 0; i < this->numCurveBundles; i++)
            {
                nAnimCurveBundle& bundle = this->curveBundles[i];
                int numCurves = bundle.GetNumCurves();

                //--- beginbundle ---
                cmd = fs->GetCmd(this, 'BGBD');
                cmd->In()->SetI(i);
                cmd->In()->SetS(bundle.GetName());
                cmd->In()->SetI(numCurves);
                fs->PutCmd(cmd);

                // for each curve in bundle
                int j;
                for (j = 0; j < numCurves; j++)
                {
                    //--- setbundlecurve ---
                    cmd = fs->GetCmd(this, 'SCBD');
                    cmd->In()->SetI(j);
                    cmd->In()->SetS(bundle.GetCurve(j)->GetName());
                    cmd->In()->SetS(bundle.GetChannelName(j));
                    fs->PutCmd(cmd);
                }

                //--- endbundle ---
                cmd = fs->GetCmd(this, 'EDBD');
                fs->PutCmd(cmd);
            }

            //--- endcurvebundles ---
            cmd = fs->GetCmd(this, 'EBCB');
            fs->PutCmd(cmd);
        }

        if (this->numConnects > 0)
        {
            //--- beginconnects ---
            cmd = fs->GetCmd(this, 'BGCN');
            cmd->In()->SetI(this->numConnects);
            fs->PutCmd(cmd);

            // for each connection...
            int i;
            for (i = 0; i < this->numConnects; i++)
            {
                nConnection& curConnect = this->connects[i];

                nConnection::nConnectionType type = curConnect.GetConnectionType();
                if (nConnection::CURVE == type)
                {
                    //--- setcurveconnect2 ---
                    cmd = fs->GetCmd(this, 'SCC2');
                    cmd->In()->SetI(i);
                    cmd->In()->SetS(curConnect.GetCurve()->GetName());
                    cmd->In()->SetS(curConnect.GetObjectName());
                    cmd->In()->SetS(curConnect.GetCmdName());
                    cmd->In()->SetS(curConnect.GetChannelName());
                    fs->PutCmd(cmd);
                }
                else
                {
                    //--- setbundleconnect2 ---
                    cmd = fs->GetCmd(this, 'SBC2');
                    cmd->In()->SetI(i);
                    cmd->In()->SetS(curConnect.GetBundle()->GetName());
                    cmd->In()->SetS(curConnect.GetObjectName());
                    cmd->In()->SetS(curConnect.GetCmdName());
                    cmd->In()->SetS(curConnect.GetChannelName());
                    fs->PutCmd(cmd);
                }
            }

            //--- endconnects ---
            cmd = fs->GetCmd(this, 'EDCN');
            fs->PutCmd(cmd);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------

