#define N_IMPLEMENTS nENetPeer
//------------------------------------------------------------------------------
/* Copyright (c) 2003 Bruce Mitchener, Jr.
 *
 * See the file "nenet_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
#include "nenet/nenetpeer.h"
#include "kernel/nkernelserver.h"

nNebulaScriptClass(nENetPeer, "nroot")

//------------------------------------------------------------------------------
/**
*/
nENetPeer::nENetPeer() :
    peer(NULL)
{
}

//------------------------------------------------------------------------------
/**
*/
nENetPeer::~nENetPeer()
{
/*
    if (NULL != this->peer)
    {
        enet_peer_reset(this->peer);
        this->peer = NULL;
    }
*/
}

//------------------------------------------------------------------------------
/**
*/
void
nENetPeer::Ping()
{
    n_assert(this->peer);

    enet_peer_ping(this->peer);
}

//------------------------------------------------------------------------------
/**
*/
void
nENetPeer::Reset()
{
    n_assert(this->peer);

    enet_peer_reset(this->peer);
}

//------------------------------------------------------------------------------
/**
*/
void
nENetPeer::Disconnect()
{
    n_assert(this->peer);
    enet_peer_disconnect(this->peer);
}

//------------------------------------------------------------------------------
/**
*/
int
nENetPeer::Send(int channel, unsigned char * data, unsigned int length, int flags)
{
    n_assert(this->peer);

    ENetPacket * packet = enet_packet_create(data, length, flags);
    if (NULL != packet)
    {
        return enet_peer_send(this->peer, channel, packet);
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nENetPeer::ConfigureThrottle(int interval, int acceleration, int deceleration)
{
    n_assert(this->peer);

    enet_peer_throttle_configure(this->peer, interval, acceleration, deceleration);
}

//------------------------------------------------------------------------------
/**
*/
void
nENetPeer::SetRawPeer(ENetPeer * peer)
{
    this->peer = peer;
    peer->data = (void *)this;
}

//------------------------------------------------------------------------------
/**
*/
ENetPeer *
nENetPeer::GetRawPeer(void)
{
    return this->peer;
}
