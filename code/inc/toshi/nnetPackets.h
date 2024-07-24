	typedef enum  
	{
		CONNECT = 0,
		DISCONNECT = 1
	} ControlType;
	typedef struct _ControlData
	{
		ControlType type;
		int connectionNumber;
		int disconnectionNumber;
	} ControlData;
	typedef struct _peerID
	{
		int address;
		short port;
	} peerID;