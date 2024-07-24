#ifndef N_NETSERVER_H
#define N_NETSERVER_H
//------------------------------------------------------------------------------
/**
    @class nNetServer

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
#include "toshi/nnetPackets.h"


#undef N_DEFINES
#define N_DEFINES nNetSAServer
#include "kernel/ndefdllclass.h"


//#include "toshi/ntimequeue.h"
#include "toshi/njugador.h"



//------------------------------------------------------------------------------

class N_PUBLIC nNetServer : public nNet
{
public:

	nNetServer();

	~nNetServer();

	void Run();
	void Trigger();

	bool SaveCmds(nPersistServer* ps);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

	void SetHost(const char *net_path);
	const char *GetHost() const;

	void setPacketLoss(int probability);
	void enableLatency(int base, int jitter);
	void disableLatency();

private:
	nAutoRef<nScriptServer> scriptServer;
	PlayerStateNet lastPacket[8];
    peerID jugadorRed;
	nString _netHost;
	
	int packetLoss;

	bool latency_enabled;
	bool conexion;
	int latency_base;
	int latency_jitter;
	//nTimeQueue<EntityStateNet> latency_queue;

};

#endif
