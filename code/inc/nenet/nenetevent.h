#ifndef N_ENETEVENT_H
#define N_ENETEVENT_H
//------------------------------------------------------------------------------
// Juan Ant.Recio 
//------------------------------------------------------------------------------

/**
    @class nENetEvent
    @ingroup NENetContribModule
    @brief Wrap an ENetEvent object.

    Juan Ant. Recio
*/
//------------------------------------------------------------------------------
#include <enet/enet.h>
#include <kernel/ntypes.h>
#include <nenet/nenetpeer.h>

//------------------------------------------------------------------------------
typedef enum
{
   /** no event occurred within the specified time limit */
   NENET_EVENT_TYPE_NONE       = ENET_EVENT_TYPE_NONE,  

   /** a connection request initiated by enet_host_connect has completed.  
     */
   NENET_EVENT_TYPE_CONNECT    = ENET_EVENT_TYPE_CONNECT,  

   /** a peer has disconnected.  This event is generated on a successful 
     * completion of a disconnect.
     */
   NENET_EVENT_TYPE_DISCONNECT = ENET_EVENT_TYPE_DISCONNECT,  

   /** a packet has been received from a peer. 
     */
   NENET_EVENT_TYPE_RECEIVE    = ENET_EVENT_TYPE_RECEIVE
} nENetEventType;



class nENetEvent
{
public:

	nENetEvent();

	nENetEvent(nENetEventType type, unsigned char* data, int length, int address, short port, short channel, double rtt)
	{
		_type = type;
		_length = length;
		_address = address;
		_port = port;
		_channel = channel;
		_rtt = rtt;

		if( (data != 0) && (_length>0) )
		{
			_data = new unsigned char[_length];
			memcpy(_data, data, _length);
		}
		else
		{
			_data = 0;
			_length = 0;
		}

			
	};

	~nENetEvent()
	{
		if(_data != 0)
			delete[] _data;
	};


	nENetEventType getType()
	{
		return _type;
	};

	unsigned char * getData()
	{
		return _data;
	};

	int  getDataLength()
	{
		return _length;
	};

	int  getAddress()
	{
		return _address;
	};

	short getPort()
	{
		return _port;
	};

	short getChannel()
	{
		return _channel;
	}

	double getRTT()
	{
		return _rtt;
	}

private:
    nENetEventType _type;
	unsigned char * _data;
	int  _length;
	
	int _address;
	short _port;
	short _channel;
	double _rtt;
    
};
#endif /* N_ENETEVENT_H */
