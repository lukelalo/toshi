#define N_IMPLEMENTS nNetServer

#include "toshi/ngame.h"
#include "toshi/npartida.h"
#include "toshi/nNetServer.h"
#include "nenet/nenethost.h"
#include "nenet/nenetpeer.h"
#include "kernel/ntimeserver.h"
#include "kernel/nscriptserver.h"
#include <time.h>


nNebulaScriptClass(nNetServer, "nroot");

//------------------------------------------------------------------------------


nNetServer::nNetServer():
    scriptServer(kernelServer, this)
{
	scriptServer		=	"/sys/servers/script";
	packetLoss = 0;
	latency_enabled = false;
	latency_base = 0;
	latency_jitter = 0;
	srand( 100 );
	jugadorRed.address=0;
	jugadorRed.port=0;
	conexion=false;
	for (int i=0;i<8;i++)
	{
		lastPacket[i].accion.numAccion=-1;
	}
}

nNetServer::~nNetServer() {
}



void nNetServer::SetHost(const char *host_path)
{
	_netHost = host_path;
}

const char * nNetServer::GetHost() const
{
	return _netHost.Get();
}


void nNetServer::setPacketLoss(int probability)
{
	// n_printf("PacketLoss probability = %d%\n", probability);
	packetLoss = probability;
}

void nNetServer::Trigger() {
	//Network Server variables
	nENetHost* server = (nENetHost*) kernelServer->Lookup(_netHost.Get());
	if(!server)
		return;

	nArray<nENetEvent *> eventList;
	server->Service(eventList,0);

	nGame *juego=(nGame*) kernelServer->Lookup("/game");
	nJugador* jugador;
	
	for(int i=0; i<eventList.Size(); i++)
	{
		nENetEvent * event = eventList.At(i);
		if(event->getType() == NENET_EVENT_TYPE_RECEIVE)
		{
			if(event->getDataLength()== sizeof(PlayerStateNet))
			{
				const unsigned char * data = event->getData();
				PlayerStateNet packet;
				memcpy(&packet, data, sizeof(packet));
				jugador = juego->GetPartida()->GetJugadores()->At(packet.num_jugador);
				jugador->setPlayerStateNet(packet);
				//n_printf("Servidor recibe: Accion %d, Destino %d\n",packet.accion.accion,packet.accion.clase_destino);
				//n_printf("Jugador %d, posicion(%f,%f,%f)\n",packet.accion.jugador,packet.accion.posicion_x,packet.accion.posicion_y,packet.accion.posicion_z);
				//n_printf("Personaje %d, Tiempo %f\n",packet.accion.personaje,packet.accion.tiempo);			
			}
		}
		if(event->getType() == NENET_EVENT_TYPE_CONNECT)
		{
			    //n_printf("El servidor recibe un paquete de conexion\n");
				nString peerName = server->getPeerName(event->getAddress(),event->getPort());
				nENetPeer *peer=(nENetPeer *) kernelServer->Lookup(peerName.Get());
				jugadorRed.address=event->getAddress();
				jugadorRed.port=event->getPort();
				ControlData data;
				data.connectionNumber=0;
				data.disconnectionNumber=0;
				data.type=CONNECT;
				unsigned char * rawdata = new unsigned char[sizeof(data)];
				memcpy(rawdata, &data, sizeof(data));
				// Send data to client
				peer->Send(0,rawdata,sizeof(data),0);
				//n_printf("El servidor envía un paquete de conexión aceptada\n");
				this->conexion=true;
				delete rawdata;
				const char* result;
				scriptServer->Run("ConectadoCliente", result);
		}
		delete event;
	}
	eventList.Clear();

	if ( jugadorRed.address!=0 && conexion && juego->GetPartida()!=NULL )
	{
		for (int i=0; i < juego->GetPartida()->GetJugadores()->Size(); i++) 
		{
			jugador =  jugador = juego->GetPartida()->GetJugadores()->At(i);

			// Tomamos el estado del jugador
			PlayerStateNet packet = jugador->getPlayerStateNet();

			// Mandamos un paquete si ha cambiado en el servidor
			if ( packet.accion.numAccion != lastPacket[i].accion.numAccion && 
				 packet.num_jugador == i )
			{
				//n_printf("Servidor envia: Accion %d, Destino %d\n",packet.accion.accion,packet.accion.clase_destino);
				//n_printf("Jugador %d, posicion(%f,%f,%f)\n",packet.accion.jugador,packet.accion.posicion_x,packet.accion.posicion_y,packet.accion.posicion_z);
				//n_printf("Personaje %d, Tiempo %f\n",packet.accion.personaje,packet.accion.tiempo);
				unsigned char * rawdata = new unsigned char[sizeof(packet)];
				memcpy(rawdata, &packet, sizeof(packet));

				// Creamos una conexión para el jugador que se encuentra en la red
				nString peerName2 = server->getPeerName(jugadorRed.address,jugadorRed.port);
				nENetPeer *peer=(nENetPeer *) kernelServer->Lookup(peerName2.Get());

				// Enviamos los datos
				peer->Send(0,rawdata,sizeof(packet),0);
				//n_printf("El servidor envia un paquete de datos a %d %d\n",jugadorRed.address,jugadorRed.port);
				delete rawdata;
				lastPacket[i] = packet;
			}
		}
	}

	server->Flush();
}

void nNetServer::enableLatency(int base, int jitter)
{
	/*
	n_printf("Latency enabled: Base %d, jitter %d\n", base, jitter);
	latency_queue.clear();
	latency_enabled = true;
	latency_base = base;
	latency_jitter = jitter;
	*/
}

void nNetServer::disableLatency()
{
	/*
	n_printf("Latency disabled\n");
	latency_enabled = false;
	latency_queue.clear();
	*/
}

