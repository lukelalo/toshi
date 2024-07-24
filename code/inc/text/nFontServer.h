#pragma warning( disable : 4786 )

#ifndef N_FONT_SERVER_H
#define N_FONT_SERVER_H
//------------------------------------------------------------------------------
/**
    @class nFontServer

    @brief Nebula Font Server
  
    @author Juan Antonio Recio García, Marco Antonio Gómez Martín
*/

#include <map>

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif


#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif
#include <string>

#include "text/nFont.h"

//------------------------------------------------------------------------------
class N_PUBLIC nFontServer : public nRoot {

public:

	nFontServer();

	~nFontServer();

	nFont* getFont(const char* filename);

	bool SaveCmds(nPersistServer* ps);

	static nKernelServer* kernelServer;	//Puntero al kernelServer
	
private:
	std::map<std::string, nFont*>	_fonts;

};
//------------------------------------------------------------------------------
#endif