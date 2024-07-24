#pragma warning( disable : 4786 )

#define N_IMPLEMENTS nFont


#include "text/nFont.h"
#include "kernel/nautoref.h"

nNebulaScriptClass(nFont, "nroot");

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------


/**
 * Constructora.
 */
nFont::nFont()
{
	texture = 0;
	fontData = 0;
}

nFont::~nFont()
{
	release();
}

bool nFont::load(const char* filename)
{
	release();
	
	fontData = new CTextureFont();
	if(!fontData->setFont(filename)) {
		delete fontData;
		fontData = NULL;
		return false;
	}

	texture = new nTextureArray(this);
	nAutoRef<nGfxServer> gfx(kernelServer,this);
	gfx = "/sys/servers/gfx";

	texture->SetTexture(gfx.get(),0, filename, 0, true, true);
	return true;
}

void nFont::release()
{
	if(texture)
		delete texture;
	if(fontData)
		delete fontData;
	texture = 0;
	fontData = 0;
}


/**
 * Devuelve el tamaño de un texto dado en pixeles.
 *
 * @param text Texto del que se desea conocer el tamaño.
 * @param tamX Parametro de salida que contendra el ancho del texto
 * completo.
 * @param tamY Parámetro de salida que contendrá el alto del texto
 * completo.
 */
void nFont::getTextSize(const unsigned char *text, int &tamX, int &tamY) const {

	const unsigned char *c = text;
	tamX = 0; tamY = 0;
	if (!c)
		return;

	while (*c) {

		const CTextureFont::tGlyph*	glyph = getGlyph(*c);

		tamX += glyph->back + glyph->width;
		//if (*(c+1))
			tamX += glyph->forward;
		tamY = (tamY < (int)glyph->height) ? (int)glyph->height : tamY;
		c++;
	}

}
