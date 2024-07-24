#define N_IMPLEMENTS nNetClient

#include "toshi/nnetclient.h"

#include "toshi/ngame.h"
#include "toshi/npartida.h"
#include "nenet/nenethost.h"
#include "kernel/nfileserver2.h"
#include "input/ninputserver.h"
#include "toshi/ncamera.h"
#include "kernel/nscriptserver.h"


#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif



nNebulaScriptClass(nNetClient, "nroot");


//------------------------------------------------------------------------------


nNetClient::nNetClient():
    scriptServer(kernelServer, this)
{
	scriptServer		=	"/sys/servers/script";
	time = 0;
	connected = 0;
	peer = 0;
	player = 0;
	for (int i=0;i<8;i++)
	{
		lastPacket[i].accion.numAccion=-1;
	}	
}

nNetClient::~nNetClient() {
}

void nNetClient::Finish()
{
	ControlData data;
	data.type = DISCONNECT;
	data.connectionNumber = -1;
	peer->Send(_channel, (unsigned char*)&data, sizeof(ControlData), 0);

	nENetHost* host = (nENetHost*) peer->GetParent();
	host->Flush();
	nArray<nENetEvent *> eventList;
	host->Service(eventList,0);
}

void nNetClient::SetPeer(const char *peer_path)
{
	_netPeer = peer_path;
}

const char * nNetClient::GetPeer() const
{
	return _netPeer.Get();
} 

void nNetClient::SetChannel(int channel)
{
	_channel = channel;
}

int nNetClient::GetChannel()
{
	return _channel;
}

void nNetClient::Trigger() 
{
	if (_netPeer.IsEmpty())
		return;
	nENetPeer* peer = (nENetPeer*) kernelServer->Lookup(_netPeer.Get());

	nGame *juego=(nGame*) kernelServer->Lookup("/game");
	nJugador* jugador;
	if ( connected && juego->GetPartida() != NULL )
	{
		for (int i=0; i < juego->GetPartida()->GetJugadores()->Size(); i++)
		{
			jugador = juego->GetPartida()->GetJugadores()->At(i);

			// Sólo enviamos datos en caso de que sean de nuestro propio jugador
			if ( jugador->GetJugadorPrincipal() )
			{
				// Obtenemos el estado de nuestro jugador
				PlayerStateNet packet = jugador->getPlayerStateNet();
				if ( packet.accion.numAccion!=lastPacket[i].accion.numAccion && 
					packet.num_jugador == i )
				{
					//n_printf("Cliente envia: Accion %d, Destino %s\n",packet.accion.accion,packet.accion.clase_destino);
					//n_printf("Jugador %d, posicion(%f,%f,%f)\n",packet.accion.jugador,packet.accion.posicion_x,packet.accion.posicion_y,packet.accion.posicion_z);
					//n_printf("Personaje %d, Tiempo %f\n",packet.accion.personaje,packet.accion.tiempo);
					unsigned char * rawdata = new unsigned char[sizeof(packet)];
					memcpy(rawdata, &packet, sizeof(packet));
					// Enviamos el estado
					peer->Send(_channel, rawdata, sizeof(packet), 0);
					//n_printf("El cliente envia un paquete de datos\n");
					delete rawdata;
					lastPacket[i] = packet;
				}
			}
		}
	}

	nENetHost* host = (nENetHost*) peer->GetParent();
	// Enviar y recibir los datos
	host->Flush();
	nArray<nENetEvent *> eventList;
	host->Service(eventList,0);

	/***********************
	// Ahora se procesan
	// los datos recibidos
	***********************/

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
				jugador = (nJugador*) juego->GetPartida()->GetJugadores()->At(packet.num_jugador);
				jugador->setPlayerStateNet(packet);
				//n_printf("Cliente recibe: Accion %d, Destino %d\n",packet.accion.accion,packet.accion.clase_destino);
				//n_printf("Jugador %d, posicion(%f,%f,%f)\n",packet.accion.jugador,packet.accion.posicion_x,packet.accion.posicion_y,packet.accion.posicion_z);
				//n_printf("Personaje %d, Tiempo %f\n",packet.accion.personaje,packet.accion.tiempo);	
			}
			if(event->getDataLength()== sizeof(ControlData))
			{
				const unsigned char * data = event->getData();
				ControlData control;
				memcpy(&control, data, sizeof(control));
				if (control.type==CONNECT)
				{
					connected=1;
				}
				//n_printf("El cliente recibe un paquete de conexión\n");
				const char* result;
				scriptServer->Run("ConectadoAlServidor", result);
			}
		}
		delete event;
	}
	eventList.Clear();
}