#define N_IMPLEMENTS nENetHost
//------------------------------------------------------------------------------
/* Copyright (c) 2003 Bruce Mitchener, Jr.
 *
 * See the file "nenet_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
#include "nenet/nenethost.h"
#include "nenet/nenetpeer.h"
#include "nenet/nenetevent.h"
#include "kernel/nkernelserver.h"
#include <wchar.h>

nNebulaScriptClass(nENetHost, "nroot");

//------------------------------------------------------------------------------
/**
*/
nENetHost::nENetHost() :
    host(NULL)
{
}

//------------------------------------------------------------------------------
/**
*/
nENetHost::~nENetHost()
{
    if (NULL != this->host)
    {
        enet_host_destroy(this->host);
        this->host = NULL;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nENetHost::Destroy()
{
    n_assert(this->host);

    enet_host_destroy(this->host);
}


bool nENetHost::Release()
{
    nENetPeer* peer = NULL;
	for (peer = (nENetPeer*)GetHead(); peer != NULL; 
		 peer = (nENetPeer*)peer->GetSucc())
	{
		peer->Disconnect();
		this->Flush();
		ENetEvent event;
		bool normalDisconnectOK = false;
        while ((enet_host_service(this->host, &event, 3) > 0)
			   && (!normalDisconnectOK) )
		{
			if(event.type == ENET_EVENT_TYPE_RECEIVE)
				enet_packet_destroy (event.packet);
			else if(event.type == ENET_EVENT_TYPE_DISCONNECT)
				normalDisconnectOK = true;;
		}
		if(!normalDisconnectOK)
			peer->Reset();
	}
	return nRoot::Release();
}

//------------------------------------------------------------------------------
/**
*/
void
nENetHost::Listen(const char * addr, int port, int peerCount, int incomingBandwidth, int outgoingBandwidth)
{
    ENetAddress address;

    if ((NULL != addr) && (strlen(addr) > 0))
    {
		if (strcmp(addr,"ANY")==0)
           address.host = ENET_HOST_ANY;			
        enet_address_set_host(&address, addr);
    }
    else 
    {
        address.host = ENET_HOST_ANY;
    }
    address.port = port;

    this->host = enet_host_create(&address, peerCount, incomingBandwidth,
                                    outgoingBandwidth);

    // XXX: Handle this better
    n_assert(this->host);
}

//------------------------------------------------------------------------------
/**
*/
nENetPeer *
nENetHost::Connect(const char * addr, int port, int channelCount, int incomingBandwidth, int outgoingBandwidth)
{
    n_assert(addr);
    n_assert(strlen(addr) > 0);

    ENetAddress address;
    ENetPeer * peer;

    if (NULL != this->host)
    {
        enet_host_destroy(this->host);
    }
    this->host = enet_host_create(NULL, 1, incomingBandwidth, outgoingBandwidth);

    enet_address_set_host(&address, addr);
    address.port = port;

    peer = enet_host_connect(this->host, &address, channelCount);

	n_printf("Connecting to: %s, %d\n", addr, port);

    ENetEvent event;
    if (enet_host_service (this->host, & event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
    	n_printf("Connection to: %s:%d, successful\n", addr, port);
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset (peer);
    	n_printf("Connection to: %s:%d, failed\n", addr, port);
    }

    return this->wrapPeer(peer);
}

nString nENetHost::getPeerName(int address, short port)
{
    char clientObjName[N_MAXPATH], peerName[N_MAXPATH];

    this->GetFullName(clientObjName, N_MAXPATH);

    int r;
    r = _snprintf(peerName, N_MAXPATH, "%s/%u_%u", clientObjName,
                 address, port);

	return nString(peerName);
}

nENetPeer *
nENetHost::wrapPeer(ENetPeer * peer)
{
    nENetPeer * npeer;
    char clientObjName[N_MAXPATH], peerName[N_MAXPATH];

    this->GetFullName(clientObjName, N_MAXPATH);

    int r;
    r = _snprintf(peerName, N_MAXPATH, "%s/%u_%u", clientObjName,
                 peer->address.host, peer->address.port);

    if (r == N_MAXPATH)
    {
        enet_peer_reset(peer);
        n_warn("Peer object name too long.");
        return NULL;
    }

    npeer = (nENetPeer *)kernelServer->New("nenetpeer", peerName);

    npeer->SetRawPeer(peer);

    return npeer;
}

//------------------------------------------------------------------------------
/**
*/
void
nENetHost::Flush()
{
    n_assert(this->host);

    enet_host_flush(this->host);
}

//------------------------------------------------------------------------------
/**
*/
int
nENetHost::Service(nArray<nENetEvent *> & eventList, unsigned int timeout)
{
    n_assert(this->host);

    int eventsProcessed = 0;
    ENetEvent event;
    while (enet_host_service(this->host, &event, timeout) > 0)
    {
        eventsProcessed++;
		//eventList.add(new ENetEvent(event));

        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                {
                    n_printf("Enet Connect packet arrived\n");
                    nENetPeer * npeer = this->wrapPeer(event.peer);

					nENetEventType type = NENET_EVENT_TYPE_CONNECT;
					nENetEvent* nevent = new nENetEvent(type, 0, 0, event.peer->address.host, event.peer->address.port, event.channelID, event.peer->roundTripTime);
					eventList.PushBack(nevent);
                   
                    break;
                }

            case ENET_EVENT_TYPE_RECEIVE:
                {
                    n_printf("A packet of length %u has arrived on %u\n", event.packet->dataLength, event.channelID);
					
					nENetEventType type = NENET_EVENT_TYPE_RECEIVE;
					nENetEvent* nevent = new nENetEvent(type, event.packet->data, event.packet->dataLength, event.peer->address.host, event.peer->address.port, event.channelID, event.peer->roundTripTime);
					eventList.PushBack(nevent);

                    // Clean up the data
                    enet_packet_destroy(event.packet);
                    break;
                }

            case ENET_EVENT_TYPE_DISCONNECT:
                {
					nENetEventType type = NENET_EVENT_TYPE_DISCONNECT;
					nENetEvent* nevent = new nENetEvent(type, 0, 0, event.peer->address.host, event.peer->address.port, event.channelID, event.peer->roundTripTime);
					eventList.PushBack(nevent);

					nString peerName = getPeerName(nevent->getAddress(), nevent->getPort());
					nENetPeer* peer = (nENetPeer*)kernelServer->Lookup(peerName.Get());
					//peer->Release();
                    break;
                }

            case ENET_EVENT_TYPE_NONE:
                {
                    // Ignore this ..
                    break;
                }

            default:
                {
                    n_error("Unexpected ENET_EVENT_TYPE: %d", event.type);
                    break;
                }
        }
    }
    return eventsProcessed;
}

//------------------------------------------------------------------------------
/**
*/
void
nENetHost::Broadcast(int channel, unsigned char * data, unsigned int length, int flags)
{
    n_assert(this->host);

    ENetPacket * packet = enet_packet_create(data, length, flags);
    if (NULL != packet)
    {
        enet_host_broadcast(this->host, channel, packet);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nENetHost::LimitBandwidth(int incomingBandwidth, int outgoingBandwidth)
{
    n_assert(this->host);

    enet_host_bandwidth_limit(this->host, incomingBandwidth, outgoingBandwidth);
}

//------------------------------------------------------------------------------
/**
*/
void
nENetHost::SetRawHost(ENetHost * host)
{
    this->host = host;
}

//------------------------------------------------------------------------------
/**
*/
ENetHost *
nENetHost::GetRawHost(void)
{
    return this->host;
}

