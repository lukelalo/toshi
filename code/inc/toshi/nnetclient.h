
#ifndef N_NETCLIENT_H
#define N_NETCLIENT_H
//------------------------------------------------------------------------------
/**
    @class nNetClient

    @brief Clase que representa un controlador de red

	Es llamado por el bucle principal (nGame), a intervalos constantes.

*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#include "toshi/nnet.h"
#include "toshi/njugador.h"
#include "nenet/nenetpeer.h"
#include "toshi/nnetPackets.h"

#undef N_DEFINES
#define N_DEFINES nNetClient
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------

class N_PUBLIC nNetClient: public  nNet
{
public:

	nNetClient();
	
	~nNetClient();

	virtual void Trigger();

	bool SaveCmds(nPersistServer* ps);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

	void SetPeer(const char *peer_path);
	const char * GetPeer() const;

	void SetChannel(int channel);
	int GetChannel();

	void Finish();

private:
	nAutoRef<nScriptServer> scriptServer;
	PlayerStateNet lastPacket[8];
	short   _entityNumber;
	nJugador* player;

	nENetPeer* peer;
	nString _netPeer;
	int _channel;
	int connected;
	int time;
};

#endif

