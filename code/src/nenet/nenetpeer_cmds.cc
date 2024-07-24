#define N_IMPLEMENTS nENetPeer
//------------------------------------------------------------------------------
/* Copyright (c) 2003 Bruce Mitchener, Jr.
 *
 * See the file "nenet_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
#include "nenet/nenetpeer.h"
#include "kernel/npersistserver.h"

static void n_ping(void* slf, nCmd* cmd);
static void n_reset(void* slf, nCmd* cmd);
static void n_disconnect(void* slf, nCmd* cmd);
static void n_send(void* slf, nCmd* cmd);
static void n_configurethrottle(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nenetpeer

    @superclass
    nroot

    @classinfo
    ...
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_ping_v",                   'PING', n_ping);
    cl->AddCmd("v_reset_v",                  'RSET', n_reset);
    cl->AddCmd("v_disconnect_v",             'DSCT', n_disconnect);
    cl->AddCmd("i_send_isii",                'SEND', n_send);
    cl->AddCmd("v_configurethrottle_iii",    'CFGT', n_configurethrottle);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    ping
    @input
    v
    @output
    v
    @info
    ...
*/
static void
n_ping(void* slf, nCmd* /* cmd */)
{
    nENetPeer* self = (nENetPeer*) slf;
    self->Ping();
}

//------------------------------------------------------------------------------
/**
    @cmd
    reset
    @input
    v
    @output
    v
    @info
    ...
*/
static void
n_reset(void* slf, nCmd* /* cmd */)
{
    nENetPeer* self = (nENetPeer*) slf;
    self->Reset();
}

//------------------------------------------------------------------------------
/**
    @cmd
    disconnect
    @input
    v
    @output
    v
    @info
    ...
*/
static void
n_disconnect(void* slf, nCmd* /* cmd */)
{
    nENetPeer* self = (nENetPeer*) slf;
    self->Disconnect();
}

//------------------------------------------------------------------------------
/**
    @cmd
    send
    @input
    i(channel), s(data), i(length), i(flags)
    @output
    i
    @info
    Send the specified data over the given channel to the remote host.

    The flags may be set to either 0 or 1.  If the flag is 0, then the
    data will be transmitted unreliably.  For reliable data transmission,
    use a flag of value 1.

    A reliable packet is guarenteed to be delivered, and a number of
    retry attempts will be made until an acknowledgement is received
    from the foreign host the packet is sent to. If a certain number
    of retry attempts is reached without any acknowledgement, ENet
    will assume the peer has disconnected and forcefully reset the
    connection. If this flag is not specified, the packet is assumed
    an unreliable packet, and no retry attempts will be made nor
    acknowledgements generated.
*/
static void
n_send(void* slf, nCmd* cmd)
{
    nENetPeer* self = (nENetPeer*) slf;
    int channel = cmd->In()->GetI();
    const char * data = cmd->In()->GetS();
    int dataLength = cmd->In()->GetI();
    int flags = cmd->In()->GetI();
    cmd->Out()->SetI(self->Send(channel, (unsigned char *)data,
                                dataLength, flags));
}

//------------------------------------------------------------------------------
/**
    @cmd
    configurethrottle
    @input
    i(interval), i(acceleration), i(deceleration)
    @output
    v
    @info
    ...
*/
static void
n_configurethrottle(void* slf, nCmd* cmd)
{
    nENetPeer* self = (nENetPeer*) slf;
    int interval = cmd->In()->GetI();
    int acceleration = cmd->In()->GetI();
    int deceleration = cmd->In()->GetI();
    self->ConfigureThrottle(interval, acceleration, deceleration);
}

//------------------------------------------------------------------------------
/**
*/
bool
nENetPeer::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

