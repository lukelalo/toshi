#define N_IMPLEMENTS nENetHost
//------------------------------------------------------------------------------
/* Copyright (c) 2003 Bruce Mitchener, Jr.
 *
 * See the file "nenet_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
#include "nenet/nenethost.h"
#include "kernel/npersistserver.h"

static void n_destroy(void* slf, nCmd* cmd);
static void n_listen(void* slf, nCmd* cmd);
static void n_connect(void* slf, nCmd* cmd);
static void n_flush(void* slf, nCmd* cmd);
static void n_service(void* slf, nCmd* cmd);
static void n_broadcast(void* slf, nCmd* cmd);
static void n_limitbandwidth(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nenethost

    @superclass
    nroot

    @classinfo
    ...
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_destroy_v",           'DSTR', n_destroy);
    cl->AddCmd("v_listen_siiii",        'LSTN', n_listen);
    cl->AddCmd("o_connect_siiii",       'CNCT', n_connect);
    cl->AddCmd("v_flush_v",             'FLSH', n_flush);
    cl->AddCmd("i_service_v",           'SRVC', n_service);
    cl->AddCmd("v_broadcast_isii",      'BCST', n_broadcast);
    cl->AddCmd("v_limitbandwidth_ii",   'LBNW', n_limitbandwidth);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    destroy
    @input
    v
    @output
    v
    @info
    Destroy this host.
*/
static void
n_destroy(void* slf, nCmd* /* cmd */)
{
    nENetHost* self = (nENetHost*) slf;
    self->Destroy();
}

//------------------------------------------------------------------------------
/**
    @cmd
    listen
    @input
    s (Address), i (Port), i (PeerCount), i (incomingBandwidth), i (outgoingBandwidth)
    @output
    v
    @info
    Listen on the specified address and port for incoming connections.  Up
    to PeerCount connections will be permitted.  The bandwidth will be limited
    according to the incomingBandwidth and outgoingBandwidth parameters.

    See the documentation for the limitbandwidth method on host for further
    information about bandwidth limiting in ENet.
*/
static void
n_listen(void* slf, nCmd* cmd)
{
    nENetHost* self = (nENetHost*) slf;
    const char * addr = cmd->In()->GetS();
    int port = cmd->In()->GetI();
    int peerCount = cmd->In()->GetI();
    int incomingBandwidth = cmd->In()->GetI();
    int outgoingBandwidth = cmd->In()->GetI();
    self->Listen(addr, port, peerCount, incomingBandwidth, outgoingBandwidth);
}

//------------------------------------------------------------------------------
/**
    @cmd
    connect
    @input
    s (Address), i (Port), i (ChannelCount), i (IncomingBandwidth), i (OutgoingBandwidth)
    @output
    o (Peer)
    @info
    Connect to the specified address/port combination.  The remote end must
    be running ENet, as this isn't a raw connection.  When the connection is
    open, there will be ChannelCount channels available for use.  Bandwidth
    will be limited according to the IncomingBandwidth and OutgoingBandwidth
    parameters.

    See the documentation for the limitbandwidth method on host for further
    information about bandwidth limiting in ENet.
*/
static void
n_connect(void* slf, nCmd* cmd)
{
    nENetHost* self = (nENetHost*) slf;
    const char * addr = cmd->In()->GetS();
    int port = cmd->In()->GetI();
    int peerCount = cmd->In()->GetI();
    int incomingBandwidth = cmd->In()->GetI();
    int outgoingBandwidth = cmd->In()->GetI();
    cmd->Out()->SetO(self->Connect(addr, port, peerCount, incomingBandwidth,
                                   outgoingBandwidth));
}

//------------------------------------------------------------------------------
/**
    @cmd
    flush
    @input
    v
    @output
    v
    @info
    Flush all network events without requiring (or doing) a call to the
    service method.  This can be used to force some data to be written
    out to the network.
*/
static void
n_flush(void* slf, nCmd* /* cmd */)
{
    nENetHost* self = (nENetHost*) slf;
    self->Flush();
}

//------------------------------------------------------------------------------
/**
    @cmd
    service
    @input
    v
    @output
    i (eventCount) 
    @info
    ...
*/
static void
n_service(void* slf, nCmd* cmd)
{
    nENetHost* self = (nENetHost*) slf;
    nArray<nENetEvent *> eventList;
    cmd->Out()->SetI(self->Service(eventList, 0));
}

//------------------------------------------------------------------------------
/**
    @cmd
    broadcast
    @input
    i(channel), s(data), i(length), i(flags)
    @output
    v
    @info
    Broadcast the specified data on the given channel to all peers connected
    to this host.
    
    The flags may be set to either 0 or 1.  If the flag is 0, then the
    data will be transmitted unreliably.  For reliable data transmission,
    use a flag of value 1.  See the documentation of the send method on
    nENetPeer for further discussion of these flags.
*/
static void
n_broadcast(void* slf, nCmd* cmd)
{
    nENetHost* self = (nENetHost*) slf;
    int channel = cmd->In()->GetI();
    const char * data = cmd->In()->GetS();
    int dataLength = cmd->In()->GetI();
    int flags = cmd->In()->GetI();
    self->Broadcast(channel, (unsigned char *)data, dataLength, flags);
}

//------------------------------------------------------------------------------
/**
    @cmd
    limitbandwidth
    @input
    i(incoming), i(outgoing)
    @output
    v
    @info
    Set the new incoming and outgoing bandwidth rates.

    ENet will strategically drop packets on specific sides of a connection
    between hosts to ensure the host's bandwidth is not overwhelmed. The
    bandwidth parameters also determine the window size of a connection
    which limits the amount of reliable packets that may be in transit
    at any given time.
*/
static void
n_limitbandwidth(void* slf, nCmd* cmd)
{
    nENetHost* self = (nENetHost*) slf;
    int incoming = cmd->In()->GetI();
    int outgoing = cmd->In()->GetI();
    self->LimitBandwidth(incoming, outgoing);
}

//------------------------------------------------------------------------------
/**
*/
bool
nENetHost::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

