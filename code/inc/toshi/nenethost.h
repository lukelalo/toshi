#ifndef N_ENETHOST_H
#define N_ENETHOST_H
//------------------------------------------------------------------------------
/* Copyright (c) 2003 Bruce Mitchener, Jr.
 *
 * See the file "nenet_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
/**
    @class nENetHost
    @ingroup NENetContribModule
    @brief Wrap an ENetHost object.

    (C) 2003 Bruce Mitchener, Jr.
*/
//------------------------------------------------------------------------------
#include <enet/enet.h>
#include "nenet/nenetevent.h"

class nENetPeer;

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#undef N_DEFINES
#define N_DEFINES nENetHost
#include "kernel/ndefdllclass.h"
//------------------------------------------------------------------------------
class N_PUBLIC nENetHost : public nRoot
{
public:
    nENetHost();
    ~nENetHost();


    /** Flush net data */
    void Flush();
    
	/** Listen to the address and port for connections
	    peerCont: maximum connections
		incomingBandwidth: maximum incoming bandwidth
		outgoingBandwidth: maximum outgoing bandwidth
	*/
    void Listen(const char * addr, int port, int peerCount, int incomingBandwidth, int outgoingBandwidth);
    
	/** Connects to a host defined by the host(IP address) and port.
	    channelCount: number of channels
		incomingBandwidth: maximum incoming bandwidth
		outgoingBandwidth: maximum outgoing bandwidth
	*/
    nENetPeer * Connect(const char * host, int port, int channelCount, int incomingBandwidth, int outgoingBandwidth);
    
	/** Send and recive net data. 
	    This method stores incoming data in the event list
	*/
    int Service(nArray<nENetEvent *> & eventList, unsigned int timeout);
    
	/** Broadcast data to all the connected peers.
	    Channel: channel to use.
		flags: 0 not reliable or ENET_PACKET_FLAG_RELIABLE reliable packet (must arrive)
	*/
    void Broadcast(int channel, unsigned char * data, unsigned int length, int flags);

	/** Limit incoming and outgoing bandwidth */
    void LimitBandwidth(int incomingBandwidth, int outgoingBandwidth);

    bool SaveCmds(nPersistServer *);

	/** Gets the Nebula peer name using the address and port */
    nString getPeerName(int address, short port);

    /** Sets the internal eNet Host */
	void SetRawHost(ENetHost *);

    /** Gets the internal eNet Host */
    ENetHost * GetRawHost(void);

    /// nKernelServer dependency.
    static nKernelServer* kernelServer;

    /** Destroy the enet connection without waiting for timeouts */
    void Destroy();

    /** Destroy the enet connection waiting for timeouts */
	bool Release();

private:
    ENetHost * host;

    nENetPeer * wrapPeer(ENetPeer *);
};
#endif /* N_ENETHOST_H */
