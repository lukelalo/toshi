#define N_IMPLEMENTS nLabel

#include "gfx/ntexturearray.h"
#include "gfx/npixelshader.h"


#include "text/nLabel.h"
#include "text/nFontServer.h"

nNebulaScriptClass(nLabel, "nvisnode");

#define NLABEL_VERTEXBUFFER "nLabelVB" 
#define NLABEL_INDEXBUFFER "nLabelIB"
#define NLABEL_PIXELSHADER "nLabelPS"
#define NLABEL_MAX_LENGTH 80
#define N_BUFFERSIZE 512

//------------------------------------------------------------------------------
/**
Constructora
*/
nLabel::nLabel() : nVisNode(), pixelShader(NULL), taArchivoTextura(NULL),
					indexBuffer(NULL), refFileServer(kernelServer, this),
					alineacion(A_LEFT), font(NULL) {
	_text = 0;
	screenX = 200;
	screenY = 200;
	labelWidth  = 256;
	labelHeight = 20;
	r = g = b = 1.0;
	this->_visible=true;
	this->refFileServer = "/sys/servers/file2";
}
//------------------------------------------------------------------------------
/**
Destructora
*/
nLabel::~nLabel(){
	if (pixelShader)
		pixelShader->Release();
	if (indexBuffer)
		indexBuffer->Release();
}


/**
 * Método para establecer la textura de la etiqueta a partir del nombre de 
 * un fichero en el que se encuentra dicha textura. 
 * Este método será el invocado para cargar texturas desde un script
 * en tcl.Recibe como parámetro el archivo del que va a leer la textura y 
 * por tanto, el parámetro "visible" se pondrá a cierto para que el nodo se añada 
 * a la escena y se visualice posteriormente. Para esto,se llama aquí a las funciones 
 * que rellenan las estructuras correspondientes para la correcta renderización de este nodo.
 * @param fichero Fichero del que se cargará la textura.
 */
bool nLabel::loadFont(const char * file)
{
	textWidth=0;
	textHeight=0;
	_visible = false;


	if((!file) || (strlen(file) == 0))
		return false;
	char buffer[N_BUFFERSIZE];
	nAutoRef<nFontServer> fontserver(kernelServer,this);
	refFileServer->ManglePath(file,buffer,N_BUFFERSIZE);
	fontserver = FONTSERVER_PATH;
	font = fontserver->getFont(buffer);

	if(!font)
		return false;

	setVisible(true);


	// Actualizamos el tamaño de la textura
	taArchivoTextura = font->getTexture();
	nTexture* tex=taArchivoTextura->GetTexture(0);
	if (tex) {
		textWidth=tex->GetWidth();
		textHeight=tex->GetHeight();
	}

	getStaticPixelShader();
	loadIndexBuffer();
	loadVertexBuffer();

	return true;
}

//------------------------------------------------------------------------------
/**	
 * Establece el texto que contendrá la etiqueta asignándolo a la variable privada _text.
 * @param texto Texto que contendrá la etiqueta. 
 */
void nLabel::setText(const char* text){
	if(_text) 
		delete[] _text;
	if (text && strlen(text)) {
		int len = strlen(text);
		if(len > NLABEL_MAX_LENGTH)
		   len = NLABEL_MAX_LENGTH;
		_text= new char[len+1]; // +1 pal 0 final

		strcpy(_text, text);
	} else
		_text = NULL;
	adjustBuffers();
}
//------------------------------------------------------------------------------
/**	
* Hace visible o no visible el label.
* @param visible Será cierto si el label será visible y falso en caso contrario.
*/
void nLabel::setVisible(bool visible){
	_visible=visible;
}
//------------------------------------------------------------------------------
/**
* Establece las coordenadas de pantalla donde se dibujará la etiqueta.
* @param posX
* @param posY
*/
void nLabel::setScreenPosition(int posX, int posY){
	screenX = posX;
	screenY = posY;
	adjustBuffers();
}
//------------------------------------------------------------------------------
/**
 * Establece las coordenadas de pantalla donde se dibujará la etiqueta,
 * expresadas de forma relativa al tamaño de la ventana.
 * @param posX
 * @param posY
*/
void nLabel::setScreenPositionf(float posX, float posY) {
	int dx0, dy0, dx1, dy1;
	refGfx->GetDisplayDesc( dx0,dy0, dx1,dy1);
	float displayW = (float)(dx1-dx0);
	float displayH = (float)(dy1-dy0);

	setScreenPosition((int)(posX*displayW), (int)(posY*displayH));
}
//------------------------------------------------------------------------------
/**
Devuelve la posición del label en pixeles.
*/
void nLabel::getScreenPosition(int &posX, int &posY) {
	posX = screenX;
	posY = screenY;
}
//------------------------------------------------------------------------------
void nLabel::setAlign(enum Align type) {

	assert((type == A_LEFT) || (type == A_CENTER) || (type == A_RIGHT));

	alineacion = type;
	adjustBuffers();
}
//------------------------------------------------------------------------------
/**
Devuelve la posición del label en coordenadas relativas
al tamaño de la ventana.
*/
void nLabel::getScreenPositionf(float &posX, float &posY) {
	int dx0, dy0, dx1, dy1;
	refGfx->GetDisplayDesc( dx0,dy0, dx1,dy1);
	float displayW = (float)(dx1-dx0);
	float displayH = (float)(dy1-dy0);

	posX = screenX / displayW;
	posY = screenY / displayH;
}
//------------------------------------------------------------------------------
/**
 * Establece el tamaño en la pantalla de lo que se va a dibujar.
 * @param width
 * @param height
 */
void nLabel::setLabelSize(int width, int height){
	labelWidth=width;
	labelHeight=height;
}


//------------------------------------------------------------------------------
/**
 * Devuelve el texto que contiene la etiqueta.
 * @return Devuelve el texto del label.
 */
char* nLabel::getText(){
	return _text;
}
//------------------------------------------------------------------------------
/**
 * Devuelve el ancho y alto del archivo leido de la etiqueta dibujada.
 * @param longX
 * @param longY
 */ 
void nLabel::getLabelSize(int& longX,int& longY){
	longX = labelWidth;
	longY = labelHeight;
}

//------------------------------------------------------------------------------

/**
 * Rellena el buffer de índices cuando sea necesario.
 */
void nLabel::loadIndexBuffer(){
	//El identificador del buffer de índices será nLabelIB.
	//Primero se busca por si ya existe.
	indexBuffer = refGfx.get()->FindIndexBuffer(NLABEL_INDEXBUFFER);
	if (indexBuffer) {
		indexBuffer->AddRef();
		return;
	}

	//Si no se encuentra se crea con el identificador indicado.
	indexBuffer = refGfx->NewIndexBuffer(NLABEL_INDEXBUFFER);
	if (!indexBuffer) {
		n_message("ERROR: The index buffer can't be shared");
		return;
	}
	nIBufType ibType =  N_IBTYPE_WRITEONLY;  //N_IBTYPE_STATIC; 
	nPrimType ibPrimType = N_PTYPE_TRIANGLE_LIST; 
	//cada letra va a suponer añadir seis índives al buffer de índices.
	indexBuffer->Begin(ibType, ibPrimType, 6*NLABEL_MAX_LENGTH); 
	
	//Rellenamos el buffer con los índices de cada una de las letras del texto que se va a 
	//mostrar.
	int num_index=0;
	int num_vert=0;
	while (num_index < NLABEL_MAX_LENGTH*6){
		//El triángulo 1 está formado por los vértices 0, 1 y 2
		indexBuffer->Index(num_index,num_vert); //vértice0
		indexBuffer->Index(num_index+1,num_vert+2); //vértice2
		indexBuffer->Index(num_index+2,num_vert+1); //vértice1
		//El triángulo 2 está formado por los vértices 0, 2 y 3
		indexBuffer->Index(num_index+3,num_vert); //vértice0
		indexBuffer->Index(num_index+4,num_vert+3); //vértice3
		indexBuffer->Index(num_index+5,num_vert+2); //vértice2
		num_index+=6;
		num_vert+=4;
	}
	indexBuffer->End();
}
//------------------------------------------------------------------------------
/**
 * Rellena el buffer de vértices cuando sea necesario.
 */
void nLabel::loadVertexBuffer(){
	//Buscamos si existe el buffer de vértices por su id. Si no está, lo creamos.
	if (!ref_vb.isvalid()) {
		char nombreRecurso[N_MAXNAMELEN];
		sprintf(nombreRecurso, "%s%d",NLABEL_VERTEXBUFFER, (int)this);
		int vbComponents = (N_VT_COORD | N_VT_UV0 | N_VT_RGBA); // N_VT_NORM?
		nVBufType vbType =  N_VBTYPE_STATIC; // N_VBTYPE_WRITEONLY;
		int numVertex = 4*NLABEL_MAX_LENGTH;
		ref_vb = refGfx.get()->NewVertexBuffer(nombreRecurso, vbType, vbComponents, numVertex);
	}

}

void nLabel::adjustBuffers()
{

	float u0,v0,u1,v1;
	float x0,y0,x1,y1;
	int vertice=0;
	

	int dx0, dy0, dx1, dy1;
	refGfx->GetDisplayDesc( dx0,dy0, dx1,dy1);
	float displayW = (float)(dx1-dx0);
	float displayH = (float)(dy1-dy0);

	float sx = screenX/displayW*2 -1;
	float sy = 1-screenY/displayH*2;

	// Ajuste de la posición inicial del texto, dependiendo de
	// su tamaño.
	switch (alineacion) {
	case A_LEFT:
		break;
	case A_CENTER: {
		int anchoPixeles, altoPixeles;
		font->getTextSize((const unsigned char*)_text, anchoPixeles, altoPixeles);
		sx = sx - (anchoPixeles / displayW);
				   }
		break;
	case A_RIGHT: {
		int anchoPixeles, altoPixeles;
		font->getTextSize((const unsigned char*) _text, anchoPixeles, altoPixeles);
		sx = sx - 2*anchoPixeles / displayW;
				   }
		break;
	}

	//Rellenamos las coordenadas de textura y las coordenadas de vértices.
	ref_vb->LockVertices(); 
	for(unsigned int i=0;i<(_text?strlen(_text):0);i++){

		//La fuente (CTextureFont) nos proporciona las coordenadas de textura,y de los vértices.
		const CTextureFont::tGlyphRel*	glyphRel= font->getGlyphRel(_text[i]);
		const CTextureFont::tGlyph*		glyph	= font->getGlyph(_text[i]);

		//coordenadas de los vértices de la letra i-ésima del texto.
		x0=(float)glyph->x;
		x1=(float)glyph->x + glyph->width;		
		y0=(float)glyph->y + glyph->height;
		y1=(float)glyph->y;
		//coordenadas de textura de la letra i-ésima del texto.
		u0=glyphRel->x1;
		u1=glyphRel->x2;
		v0=glyphRel->y1;
		v1=glyphRel->y2;
#define toScreen(pixeles)	((float)2*pixeles/displayW)
		x0 = sx + toScreen(glyph->back);
		x1 = x0 + toScreen(glyph->width);//(float)2*glyph->width/displayW;//glyph->width/textWidth;
		y0 = sy;
		y1 = sy - (float)2*glyph->height/displayH;

		//Vértice arriba a la izquierda.
		ref_vb->Coord(vertice,vector3(x0,y0,0));
		ref_vb->Uv(vertice,0,vector2(u0,v0));
		ref_vb->Color(vertice, n_f2rgba(r, g, b, 1.0));
			
		//Vértice arriba a la derecha.
		ref_vb->Coord(vertice+1,vector3(x1, y0,0));
		ref_vb->Uv(vertice+1,0,vector2(u1,v0));	
		ref_vb->Color(vertice+1, n_f2rgba(r, g, b, 1.0));

		//Vértice abajo a la derecha.
		ref_vb->Coord(vertice+2,vector3(x1, y1 ,0));
		ref_vb->Uv(vertice+2,0,vector2(u1,v1));	
		ref_vb->Color(vertice+2, n_f2rgba(r, g, b, 1.0));

		//Vértice abajo a la izquierda.
		ref_vb->Coord(vertice+3,vector3(x0, y1, 0));
		ref_vb->Uv(vertice+3,0,vector2(u0,v1));	
		ref_vb->Color(vertice+3, n_f2rgba(r, g, b, 1.0));

		sx = sx + toScreen(glyph->back) + toScreen(glyph->width) + toScreen(glyph->forward);//x1;
		vertice += 4;
	}
	ref_vb->UnlockVertices();

}

//------------------------------------------------------------------------------

/**
* Método que añade este nodo a la escena.
*/
bool nLabel::Attach(nSceneGraph2 *sceneGraph){
	nVisNode::Attach(sceneGraph);	
	if (_visible) {
		sceneGraph->AttachMeshNode(this);
		sceneGraph->AttachRenderPri(_order);
	}
	return _visible;
}

//------------------------------------------------------------------------------

/**
 * Devuelve el shader de pixeles estático compartido por todos
 * los nCSprite (de nombre 'nLabelPS'). Si aún no existe,
 * lo crea.
 */	      
nPixelShader* nLabel::getStaticPixelShader() {

	if (pixelShader)
		return pixelShader;

	//Vemos si ya está creado, para no volverlo a hacer.
	pixelShader = refGfx.get()->FindPixelShader(NLABEL_PIXELSHADER);

	if (pixelShader) {
		pixelShader->AddRef();
		return pixelShader;
	}

	/*
	Explicación del shader: suponemos que las texturas tienen
	transparencia, y las letras están en blanco (255, 255, 255).
	De esta forma, la etiqueta puede ponerse de cualquier color,
	gracias a este shader. Lo que hace es mezclar el color
	de los vértices y textura de la siguiente forma:
	- Color (RGB): multiplica el color del vértice por el
		color de la textura (nPSI::nArg::MUL <-> GL_MODULATE).
		De esta forma, al ser la textura blanca (1.0, 1.0, 1.0),
		el color resultante es el los vértices.
	- Alfa (A): se queda con el valor que tenga la textura,
		independientemente de qué valor haya en los vértices
		(nPSI::nArg::REPLACE <-> GL_REPLACE)

	Para que todo esto funcione, el color del vértice _no_ debe
	ser afectador por las luces, por lo que se deshabilitan
	las luces (también, creo, valdría con usar N_CMAT_EMISSIVE
	como color del material, pero de esto no estoy muy seguro,
	porque posiblemente esté equivocado, al haber hecho
	pruebas solo en escenas sin luces O:-) ).
	*/
	pixelShader = refGfx->NewPixelShader(NLABEL_PIXELSHADER);

	nPixelShaderDesc *psDesc=new nPixelShaderDesc();
	psDesc->SetNumStages(1);

	psDesc->SetColorOp(0, nPSI::nOp::REPLACE,
					nPSI::nArg::TEX,
					nPSI::nArg::PREV,
					nPSI::nArg::NOARG,
					nPSI::nScale::ONE);
	//psDesc->SetConst(0,vector4(0,0,0,0));
	psDesc->SetAddressU(0, N_TADDR_CLAMP);
	psDesc->SetAddressV(0, N_TADDR_CLAMP);
	psDesc->SetMinFilter(0, N_TFILTER_LINEAR_MIPMAP_NEAREST);//N_TFILTER_LINEAR_MIPMAP_NEAREST);
	psDesc->SetMagFilter(0, N_TFILTER_LINEAR_MIPMAP_NEAREST); // LINEAR? Yo creo que es mejor nearest...
	psDesc->SetTexCoordSrc(0,N_TCOORDSRC_UV0);
	psDesc->SetEnableTransform(0,false);
	/*psDesc->Txyz(0,vector3(0,0,0));
	psDesc->Rxyz(0,vector3(0,0,0));
	psDesc->Sxyz(0,vector3(1,1,1));*/
	// Para el alpha...
	psDesc->SetAlphaEnable(true);
	psDesc->SetAlphaTestEnable(true);
	//nRStateParam::
	psDesc->SetAlphaSrcBlend(N_ABLEND_SRCALPHA);
	psDesc->SetAlphaDestBlend(N_ABLEND_INVSRCALPHA);
	psDesc->SetAlphaFunc(N_CMP_GREATEREQUAL);
	//Debe ser para OPENGL 0.0625 y para DIRECTX 0.9375
	if (strcmp(refGfx->GetClass()->GetName(),"nglserver")==0) 
		psDesc->SetAlphaRef(0.9375);
	else
		psDesc->SetAlphaRef(0.0625);
	psDesc->SetAlphaOp(0, nPSI::nOp::REPLACE,
					nPSI::nArg::TEX,
					nPSI::nArg::PREV,
					nPSI::nArg::NOARG,
					nPSI::nScale::ONE);
	psDesc->SetColorMaterial(N_CMAT_DIFFUSE);
	psDesc->SetZWriteEnable(true);
	psDesc->SetZFunc(N_CULL_NONE);

	/*psDesc->SetColorOp(0, nPSI::nOp::REPLACE,
					nPSI::nArg::TEX,
					nPSI::nArg::PREV,
					nPSI::nArg::NOARG,
					nPSI::nScale::ONE);*/
	/*psDesc->SetColorOp(0, nPSI::nOp::REPLACE,
					nPSI::nArg::TEX,
					nPSI::nArg::PREV,
					nPSI::nArg::NOARG,
					nPSI::nScale::ONE);

	
	// Para el alpha...
	psDesc->SetAlphaEnable(true);
	psDesc->SetAlphaTestEnable(true);
	psDesc->SetAlphaOp(0, nPSI::nOp::REPLACE,
					nPSI::nArg::TEX,
					nPSI::nArg::PREV,
					nPSI::nArg::NOARG,
					nPSI::nScale::ONE);
	
	psDesc->SetAddressU(0, N_TADDR_WRAP);
	psDesc->SetAddressV(0, N_TADDR_WRAP);
	psDesc->SetMinFilter(0, N_TFILTER_LINEAR);//N_TFILTER_LINEAR_MIPMAP_NEAREST);
	psDesc->SetMagFilter(0, N_TFILTER_LINEAR); // LINEAR? Yo creo que es mejor nearest...
	psDesc->SetTexCoordSrc(0,N_TCOORDSRC_UV0);
	psDesc->SetEnableTransform(0,false);
	// Para el alpha...
	psDesc->SetAlphaEnable(true);
	psDesc->SetAlphaTestEnable(true);
	// nRStateParam
	psDesc->SetAlphaSrcBlend(N_ABLEND_SRCALPHA);
	psDesc->SetAlphaDestBlend(N_ABLEND_INVSRCALPHA);
	psDesc->SetAlphaFunc(N_CMP_GREATEREQUAL);
	//Debe ser para OPENGL 0.0625 y para DIRECTX 0.9375
	if (strcmp(refGfx->GetClass()->GetName(),"nglserver")==0) 
		psDesc->SetAlphaRef(0.5);
	else
		psDesc->SetAlphaRef(0.9);
	psDesc->SetAlphaOp(0, nPSI::nOp::REPLACE,
					nPSI::nArg::TEX,
					nPSI::nArg::PREV,
					nPSI::nArg::NOARG,
					nPSI::nScale::ONE);
	//psDesc->SetColorMaterial(N_CMAT_DIFFUSE);
	psDesc->SetZWriteEnable(true);
	psDesc->SetZFunc(N_CULL_NONE);*/

	/*psDesc->SetNumStages(1);

	psDesc->SetLightEnable(false);
	psDesc->SetColorMaterial(N_CMAT_EMISSIVE);
	psDesc->SetAddressU(0, N_TADDR_WRAP);
	psDesc->SetAddressV(0, N_TADDR_WRAP);
	psDesc->SetMinFilter(0, N_TFILTER_NEAREST);//N_TFILTER_LINEAR_MIPMAP_NEAREST);
	psDesc->SetMagFilter(0, N_TFILTER_NEAREST); // LINEAR? Yo creo que es mejor nearest...
	psDesc->SetTexCoordSrc(0,N_TCOORDSRC_UV0);
	psDesc->SetEnableTransform(0,false);
	psDesc->Txyz(0,vector3(0,0,0));
	psDesc->Rxyz(0,vector3(0,0,0));
	psDesc->Sxyz(0,vector3(1,1,1));

	if (strcmp(refGfx->GetClass()->GetName(),"nglserver")==0) 
		psDesc->SetAlphaRef(0.0625);
	else
		psDesc->SetAlphaRef(0.9375);

	// Para el color...
	psDesc->SetColorOp(0, nPSI::nOp::REPLACE,
					(nPSI::nArg)(nPSI::nArg::PRIMARY | nPSI::nArg::COLOR),
					nPSI::nArg::NOARG,
					nPSI::nArg::NOARG,
					nPSI::nScale::ONE);

	
	// Para el alpha...
	psDesc->SetAlphaEnable(true);
	psDesc->SetAlphaTestEnable(true);
	psDesc->SetAlphaOp(0, nPSI::nOp::REPLACE,
					nPSI::nArg::TEX,
					nPSI::nArg::NOARG,
					nPSI::nArg::NOARG,
					nPSI::nScale::ONE);
	*/
	//psDesc->SetZWriteEnable(true);

	pixelShader->SetShaderDesc(psDesc);

	return pixelShader;
}
//------------------------------------------------------------------------------	
/**
* Método al que llama cada nodo de la estructura de Nebula con todos los objetos visuales 
* que se le han ido añadiendo para actualizar sus valores.
*/
void nLabel::Compute(nSceneGraph2 *sceneGraph){


	/* Miramos si hay que actualizar el buffer de vértices. Eso ocurre cuando ocurre
	una de las siguientes cosas:
	- El tamaño de la ventana a cambiado
	- La referencia al buffer de vértices no es válida (aún no se ha inicializado).
	- El contenido está 'sucio', debido al cambio de algún rectángulo.
	- Existe una textura 'hermana', y o bien en el último frame no la utilizamos,
	o bien ha cambiado su tamaño.
	- No existe una textura hermana, y en el último frame utilizamos una.
	* /
	if (UpdateScreenSize() || vbDirty || (!ref_vb.isvalid()) ||
		((ta) && (ta->GetTexture(0)) && 
			( (!lastFrameUsingExternalTexture) || (UpdateSiblingTextureSize(ta) ))) ||
		(lastFrameUsingExternalTexture && (!ta || !ta->GetTexture(0))))
//		 (lastFrameUsingExternalTexture) && (UpdateSiblingTextureSize(ta))) ||
//		((ta) && (lastFrameUsingExternalTexture != (bool)ta->GetTexture(0))))
		calculateVertexBuffer(ta);

  */

	if (! _visible)
		return;

	nVisNode::Compute(sceneGraph);

	if (!_text)
		return;
	matrix44 identity;
        
    refGfx.get()->PushMatrix(N_MXM_PROJECTION);
    refGfx.get()->PushMatrix(N_MXM_MODELVIEW);	
	refGfx.get()->SetMatrix(N_MXM_MODELVIEW, identity);
    refGfx.get()->SetMatrix(N_MXM_PROJECTION,identity);

	ref_vb->Truncate(strlen(_text)*4);
	indexBuffer->Truncate(strlen(_text)*6);

	ref_vb.get()->Render(indexBuffer,pixelShader,taArchivoTextura);

	ref_vb->Untruncate();
	indexBuffer->Untruncate();


    refGfx.get()->PopMatrix(N_MXM_PROJECTION);
    refGfx.get()->PopMatrix(N_MXM_MODELVIEW);

}

/**
Establece el color de las letras.
@param r Nivel de rojo
@param g Nivel de verde
@param b Nivel de azul.
*/
void nLabel::setColor(float r, float g, float b) {
	this->r = r;
	this->g = g;
	this->b = b;
	adjustBuffers();
}

/**
Devuelve en los parámetros de salida el color de las letras.
@param r Nivel de rojo
@param g Nivel de verde
@param b Nivel de azul.
*/
void nLabel::getColor(float &r, float &g, float &b) const {
	r = this->r;
	g = this->g;
	b = this->b;
}
