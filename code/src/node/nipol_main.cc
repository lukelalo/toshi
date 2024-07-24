#define N_IMPLEMENTS nIpol
//-------------------------------------------------------------------
//  nipol_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ncmdprotonative.h"
#include "kernel/nclass.h"
#include "node/n3dnode.h"
#include "node/nipol.h"
#include "mathlib/quaternion.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"

nNebulaScriptClass(nIpol, "nanimnode");

//------------------------------------------------------------------------------
/**
*/
nIpol::nIpol() :
    scase(N_SCASE_NONE),
    cmd_proto(NULL),
    keyArray(4, 4),
    ipol_type(N_LINEAR),
    key_size(0),
    key_index(0)
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
nIpol::~nIpol()
{
    // empty
}

//-------------------------------------------------------------------
//  Initialize()
//  If parent is a nVisNode object, link to it as depend node.
//  13-Nov-00   floh    created
//-------------------------------------------------------------------
void nIpol::Initialize(void)
{
    nVisNode *p = (nVisNode *) this->GetParent();
    if (p && p->IsA(kernelServer->FindClass("nvisnode")))
    {
        p->DependsOn(this);
    }
    nVisNode::Initialize();
}

//-------------------------------------------------------------------
//  Connect()
//  05-Dec-98   floh    created
//  19-Dec-98   floh    + FindCmdByName() muss jetzt nicht mehr
//                        fuer jede Stufe in der Klassen-
//                        Hierarchie einzeln angewendet werden
//  24-Jun-99   floh    + Special-Case-Handling fuer ein paar
//                        oft verwendete Commands
//  02-Jul-99   floh    + Connect ist nicht mehr zwingend notwendig,
//                        deshalb passiert DependsOn() auf das Parent
//                        jetzt in Attach()
//  08-Aug-99   floh    + hebt sich nicht mehr lokale Kopie eines
//                        Cmd auf, sondern den Pointer auf den
//                        "Factory-VProto"
//  04-Nov-99   floh    + keine Assertion mehr bei "Reconnect"
//  24-Nov-99   floh    + brachte auch bei Erfolg 'false' zurueck
//-------------------------------------------------------------------
bool nIpol::Connect(const char *cmd_name)
{
    n_assert(cmd_name);

    nClass *n3dnode_class = kernelServer->FindClass("n3dnode");
    this->scase = N_SCASE_NONE;

    // testen, ob Parent-Objekt Kommando akzeptiert...
    bool retval = false;
    nVisNode *p = (nVisNode *) this->GetParent();
    n_assert(p && p->IsA(kernelServer->FindClass("nvisnode")));

    nClass *cl = p->GetClass();
    nCmdProtoNative *cproto = cl->FindNativeCmdByName(cmd_name);
    if (cproto) 
    {
        retval = true;
        this->cmd_proto = (nCmdProto *) cproto;

        // special case command?
        if (n3dnode_class && p->IsA(n3dnode_class))
        {
            switch (cproto->GetID()) {
                case 'TXYZ': this->scase = N_SCASE_TXYZ; break;
                case 'TX__': this->scase = N_SCASE_TX; break;
                case 'TY__': this->scase = N_SCASE_TY; break;
                case 'TZ__': this->scase = N_SCASE_TZ; break;
                case 'RXYZ': this->scase = N_SCASE_RXYZ; break;
                case 'RX__': this->scase = N_SCASE_RX; break;
                case 'RY__': this->scase = N_SCASE_RY; break;
                case 'RZ__': this->scase = N_SCASE_RZ; break;
            }
        }
    } 
    else 
    {
        n_printf("nIpol::Connect(): Parent object '%s' does not accept command '%s'!\n",
                 p->GetName(), cmd_name);
    }
    return retval;
}

/*
//-------------------------------------------------------------------
//  Disconnect()
//  14-Nov-00   floh    created
//-------------------------------------------------------------------
void nIpol::Disconnect(void)
{
    this->cmd_proto = NULL;
    this->scase     = N_SCASE_NONE;
}
*/

//-------------------------------------------------------------------
//  GetConnect()
//  14-Nov-00   floh    created
//-------------------------------------------------------------------
const char *nIpol::GetConnect(void)
{
    if (this->cmd_proto) 
    {
        return this->cmd_proto->GetName();
    }
    return 0;
}

//-------------------------------------------------------------------
//  AddKey1f()
//  05-Dec-98   floh    created
//-------------------------------------------------------------------
void nIpol::AddKey1f(float time, float x)
{
    nIpolKey key(time, x);
    this->keyArray.PushBack(key);
    this->key_size = 1;
}

//-------------------------------------------------------------------
//  AddKey2f()
//  05-Dec-98   floh    created
//-------------------------------------------------------------------
void nIpol::AddKey2f(float time, float x, float y)
{
    nIpolKey key(time, x, y);
    this->keyArray.PushBack(key);
    this->key_size = 2;
}

//-------------------------------------------------------------------
//  AddKey3f()
//  05-Dec-98   floh    created
//-------------------------------------------------------------------
void nIpol::AddKey3f(float time, float x, float y, float z)
{
    nIpolKey key(time, x, y, z);
    this->keyArray.PushBack(key);
    this->key_size = 3;
}

//-------------------------------------------------------------------
//  AddKey4f()
//  05-Dec-98   floh    created
//-------------------------------------------------------------------
void nIpol::AddKey4f(float time, float x, float y, float z, float w)
{
    nIpolKey key(time, x, y, z, w);
    this->keyArray.PushBack(key);
    this->key_size = 4;
}

//-------------------------------------------------------------------
//  Compute()
//  20-Apr-99   floh    created
//  06-May-99   floh    funktioniert jetzt auch, wenn nicht
//                      connected. In dem Fall wird kein Script-
//                      Kommando auf das Parent angewendet,
//                      sondern nur this->curval[] ausgefuellt,
//                      auf dass jemand anders die Werte abfragt
//  25-May-99   floh    ist jetzt nicht mehr auf die Globalzeit
//                      hardgecodet, sondern benutzt den eingestellten
//                      Channel-Index
//  24-Jun-99   floh    spezielle Direct-Call-Optimierung fuer ein
//                      paar Special-Case-Commands
//  02-Jul-99   floh    + ReadChannels(),WriteChannels()
//  30-Sep-99   floh    + clampt t im OneShot Modus auf den gueltigen
//                        Wertebereich
//  04-Nov-99   floh    + geaendertes Auffuellen des nCmd Objects
//  31-Oct-00   floh    + didn't do an 'active' check
//  07-Dec-00   floh    + 
//  05-Jun-01   floh    + converted to new scene graph and channel handling
//  24-Jul-01   floh    + some corrections to prevent illegal array accesses
//  14-Apr-02   cygon   + cubic and spline (catmull-rom) interpolation
//-------------------------------------------------------------------
void 
nIpol::Compute(nSceneGraph2 *sceneGraph)
{
    n_assert(sceneGraph);

    // let nVisNode class invoke Compute() on my depend nodes
    nVisNode::Compute(sceneGraph);

    // do the interpolation stuff...
    if (this->keyArray.Size() > 1)
    {
        float tscale = this->scale;
        float min_t = this->keyArray.Front().t * tscale;
        float max_t = this->keyArray.Back().t * tscale;
        if (max_t > 0.0)
        {
            // get current anim channel value
            nChannelContext* chnContext = sceneGraph->GetChannelContext();
            n_assert(chnContext);
            float t = chnContext->GetChannel1f(this->localChannelIndex);

            if (this->repType == N_REPTYPE_LOOP) 
            {
                // in loop mode, wrap time into loop time 
                t = (float) (t - (floor(t/max_t) * max_t));
            } 

            // clamp time to legal range
            if      (t < min_t)  t = min_t;
            else if (t >= max_t) t = max_t - 0.001f;

            // find the right keys
            int i = 0;
            if (this->keyArray.Front().t > 0.0f)
            {
                char buf[N_MAXPATH];
                n_error("Object '%s' 1st keyframe > 0.0f (%f)!\n", this->GetFullName(buf,sizeof(buf)), this->keyArray.Front().t);
            };

            while ((this->keyArray.At(i).t * tscale) <= t)
            {
                i++;
            }
            n_assert((i > 0) && (i < this->keyArray.Size()));
            nIpolKey& k0 = this->keyArray.At(i - 1);
            nIpolKey& k1 = this->keyArray.At(i);
            float t0 = k0.t * tscale;
            float t1 = k1.t * tscale;

            // compute the actual interpolated values
            float l;
            if (t1 > t0) l = (float) ((t-t0)/(t1-t0));
            else         l = 1.0f;

            switch (this->ipol_type) {
                case N_STEP:
                    this->curval = k0.f;
                    break;

                case N_LINEAR:
                    this->curval = k0.f + ((k1.f - k0.f) * l);
                    break;

                case N_QUATERNION:
                    {
                        quaternion q0(k0.f.x, k0.f.y, k0.f.z, k0.f.w);
                        quaternion q1(k1.f.x, k1.f.y, k1.f.z, k1.f.w);
                        quaternion q2;
                        q2.slerp(q0, q1, l);
                        this->curval.set(q2.x, q2.y, q2.z, q2.w);
                    }
                    break;

                case N_CUBIC:
                    {
                        float l2 = l*l;
                        float l3 = l2*l;

                        this->curval.set(k0.f.x * (2*l3 - 3*l2 + 1) + k1.f.x * (3*l2 - 2*l3),
                                         k0.f.y * (2*l3 - 3*l2 + 1) + k1.f.y * (3*l2 - 2*l3),
                                         k0.f.z * (2*l3 - 3*l2 + 1) + k1.f.z * (3*l2 - 2*l3),
                                         k0.f.w * (2*l3 - 3*l2 + 1) + k1.f.w * (3*l2 - 2*l3));
                    }
                    break;

                case N_SPLINE:
                    {
                        // Spline interpolation needs an additional key on each end
                        // If there's no key, we reuse the end keys and extrapolate time

                        float tA, tB; // times at kA, kB 

                        nIpolKey *kA; // key left to i-1
                        if (i == 1)
                        {
                            kA = &(this->keyArray.At(i-1));
                            tA = k0.t - (k1.t - k0.t);
                        }
                        else
                        {
                            kA = &(this->keyArray.At(i-2));
                            tA = kA->t;
                        }

                        nIpolKey *kB; // key right to i
                        if(i == this->keyArray.Size() - 1)
                        {
                            kB = &(this->keyArray.At(i));
                            tB = k1.t + (k1.t - k0.t);
                        }
                        else
                        {
                            kB = &(this->keyArray.At(i + 1));
                            tB = kB->t;
                        }

                        float l2 = l*l;
                        float l3 = l2*l;
                        float k0t_tA = k0.t - tA;
                        float k1t_k0t = k1.t - k0.t;
                        float tB_k1t = tB - k1.t;

                        float k_0, k_1; // tangents at k0, k1

                        k_0 = 0.5f * (k0.f.x - kA->f.x) / (k0t_tA) + 0.5f * (k1.f.x - k0.f.x) / (k0t_tA);
                        k_1 = 0.5f * (k1.f.x - k0.f.x) / (k1t_k0t) + 0.5f * (kB->f.x - k1.f.x) / (k1t_k0t);
                        this->curval.x = k0.f.x * (2*l3 - 3*l2 + 1) + k1.f.x * (3*l2 - 2*l3) +
                                         k_0 * (k1t_k0t) * (l3 - 2*l2 + l) +
                                         k_1 * (tB_k1t) * (l3 - l2);

                        k_0 = 0.5f * (k0.f.y - kA->f.y) / (k0t_tA) + 0.5f * (k1.f.y - k0.f.y) / (k0t_tA);
                        k_1 = 0.5f * (k1.f.y - k0.f.y) / (k1t_k0t) + 0.5f * (kB->f.y - k1.f.y) / (k1t_k0t);
                        this->curval.y = k0.f.y * (2*l3 - 3*l2 + 1) + k1.f.y * (3*l2 - 2*l3) +
                                         k_0 * (k1t_k0t) * (l3 - 2*l2 + l) +
                                         k_1 * (tB_k1t) * (l3 - l2);

                        k_0 = 0.5f * (k0.f.z - kA->f.z) / (k0t_tA) + 0.5f * (k1.f.z - k0.f.z) / (k0t_tA);
                        k_1 = 0.5f * (k1.f.z - k0.f.z) / (k1t_k0t) + 0.5f * (kB->f.z - k1.f.z) / (k1t_k0t);
                        this->curval.z = k0.f.z * (2*l3 - 3*l2 + 1) + k1.f.z * (3*l2 - 2*l3) +
                                         k_0 * (k1t_k0t) * (l3 - 2*l2 + l) +
                                         k_1 * (tB_k1t) * (l3 - l2);

                        k_0 = 0.5f * (k0.f.w - kA->f.w) / (k0t_tA) + 0.5f * (k1.f.w - k0.f.w) / (k0t_tA);
                        k_1 = 0.5f * (k1.f.w - k0.f.w) / (k1t_k0t) + 0.5f * (kB->f.w - k1.f.w) / (k1t_k0t);
                        this->curval.w = k0.f.w * (2*l3 - 3*l2 + 1) + k1.f.w * (3*l2 - 2*l3) +
                                         k_0 * (k1t_k0t) * (l3 - 2*l2 + l) +
                                         k_1 * (tB_k1t) * (l3 - l2);
                        
                    }
                    break;
            }

            // if connected, send command to parent
            if (this->scase != N_SCASE_NONE) 
            {
                // if special case, directly invoke c++ method on parent
                #define V0 (this->curval.x)
                #define V1 (this->curval.y)
                #define V2 (this->curval.z)
                switch (this->scase) 
                {
                    case N_SCASE_TXYZ: ((n3DNode *)this->parent)->Txyz(V0,V1,V2); break;
                    case N_SCASE_TX:   ((n3DNode *)this->parent)->Tx(V0); break;
                    case N_SCASE_TY:   ((n3DNode *)this->parent)->Ty(V0); break;
                    case N_SCASE_TZ:   ((n3DNode *)this->parent)->Tz(V0); break;
                    case N_SCASE_RXYZ: ((n3DNode *)this->parent)->Rxyz(V0,V1,V2); break;
                    case N_SCASE_RX:   ((n3DNode *)this->parent)->Rx(V0); break;
                    case N_SCASE_RY:   ((n3DNode *)this->parent)->Ry(V0); break;
                    case N_SCASE_RZ:   ((n3DNode *)this->parent)->Rz(V0); break;
                    default: break;
                }
            } 
            else if (this->cmd_proto) 
            {
                // general case: send a cmd object
                nCmd *cmd = this->cmd_proto->NewCmd();
                cmd->Rewind();
                int j = cmd->GetNumInArgs();
                if (j>0) cmd->In()->SetF(this->curval.x);
                if (j>1) cmd->In()->SetF(this->curval.y);
                if (j>2) cmd->In()->SetF(this->curval.z);
                if (j>3) cmd->In()->SetF(this->curval.w);
                this->parent->Dispatch(cmd);
                this->cmd_proto->RelCmd(cmd);
            }
        }
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
    
