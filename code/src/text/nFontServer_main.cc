#pragma warning( disable : 4786 )

#define N_IMPLEMENTS nFontServer

#include "text/nFontServer.h"

nNebulaScriptClass(nFontServer, "nroot");

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------


/**
 * Constructora.
 */
nFontServer::nFontServer()
{
}

nFontServer::~nFontServer()
{
	_fonts.clear();
}

nFont* nFontServer::getFont(const char* filename)
{
	nFont* font = _fonts[filename];
	//if(!font)
	{
		font = new nFont();
		if(!font->load(filename))
		{
			delete font;
			return 0;
		}
/*		char* id = new char[strlen(filename) + 1];
		strcpy(id,filename);
		_fonts[id] = font;*/
		_fonts[std::string(filename)] = font;
	}

	return font;
}