#ifndef N_NLABEL_H
#define N_NLABEL_H
//------------------------------------------------------------------------------
/**
    @class nLabel

    @brief 
  
    @author Felicidad Ramos Manjavacas, Marco Antonio G�mez Mart�n
*/

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif


#ifndef N_N3DNODE_H
#include "node/n3dnode.h"
#endif

#include "gfx/nscenegraph2.h" 
#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"

#include "text/nFont.h"

#include "file/nnpkfileserver.h"
//------------------------------------------------------------------------------

#define FONTSERVER_PATH "/sys/servers/fontserver"

//------------------------------------------------------------------------------

class N_PUBLIC nLabel : public nVisNode {

	nAutoRef<nFileServer2> refFileServer;
public:

	enum Align {A_LEFT, A_CENTER, A_RIGHT};

	nLabel();

	~nLabel();

	/**
	 * Establece el orden en el que aparecer�n los distintos sprites. Cuanto m�s alto
	 * m�s "cerca" de la pantalla aparecer�, es decir, sprites con n�mero altos
	 * aparecer�n por encima de sprites con n�meros m�s bajos. El resultado a
	 * igualdad de valor, es indeterminado.
	 */
	void setOrder(short order) {_order = order;}

	/**
	 * Devuelve el orden en el que aparecer� el sprite
	 */
	short getOrder() {return _order;}

	/**	
	 * Hace visible o no visible el label.
	 * @param visible Ser� cierto si el label ser� visible y falso en caso contrario.
	 */
	void setVisible(bool visible);

	/**
	 * Establece la alineaci�n deseada.
	 * @param type Tipo de alineaci�n.
	 */
	void setAlign(enum Align type);

	/**
	 * Establece las coordenadas de pantalla donde se dibujar� la etiqueta.
	 * @param posX
	 * @param posY
	 */
	void setScreenPosition(int posX, int posY);

	/**
	 * Establece las coordenadas de pantalla donde se dibujar� la etiqueta,
	 * expresadas de forma relativa al tama�o de la ventana.
	 * @param posX
	 * @param posY
	*/
	void setScreenPositionf(float posX, float posY);

	/**
	Devuelve la posici�n del label en pixeles.
	*/
	void getScreenPosition(int &posX, int &posY);

	/**
	Devuelve la posici�n del label en coordenadas relativas
	al tama�o de la ventana.
	*/
	void getScreenPositionf(float &posX, float &posY);
	
	/**	
	 * Establece el texto que contendr� la etiqueta asign�ndolo a la variable privada _text.
	 * @param texto Texto que contendr� la etiqueta. 
	 */
	void setText(const char* text);
	
	/**
	 * Establece el tama�o en la pantalla de lo que se va a dibujar.
	 */
	void setLabelSize(int width, int height);

	/**
	 * Devuelve el texto que contiene la etiqueta.
	 * @return Devuelve el texto del label.
	 */
	char* getText();

	/**
	* Devuelve el ancho y alto del archivo leido de la etiqueta dibujada.
	* @param longX
	* @param longY
	*/ 
	void getLabelSize(int& longX,int& longY);

	/**
	Devuelve si se est� dibujando o no.
	*/
	bool getVisible() {return _visible;}

	/**
	 * M�todo para establecer la textura de la etiquetaa partir del nombre de un fichero 
	 * en el que se encuentra dicha textura. 
	 * Este m�todo ser� el invocado para cargar texturas desde un script
	 * en tcl.Recibe como par�metro el archivo del que va a leer la textura y 
	 * por tanto, el par�metro "visible" se pondr� a cierto para que el nodo se a�ada 
	 * a la escena y se visualice posteriormente. Para esto,se llama aqu� a las funciones 
	 * que rellenan las estructuras correspondientes para la correcta renderizaci�n de este nodo.
	 * @param fichero Fichero del que se cargar� la textura.
	 */
	bool loadFont(const char * file);

	bool SaveCmds(nPersistServer* ps);
	
	/**
	 * Puntero al kernelServer
	 */
	static nKernelServer* kernelServer;	
	
	/**
	 * M�todo que a�ade este nodo a la escena.
	 */
	bool Attach(nSceneGraph2 *sceneGraph);
	
	/**
	 * M�todo al que llama cada nodo de la estructura de Nebula con todos los objetos visuales 
	 * que se le han ido a�adiendo para actualizar sus valores.
	 */
	void Compute(nSceneGraph2 *sceneGraph);

	/**
	Devuelve la fuente que est� utilizando la etiqueta.
	@return Fuente actual.
	@note La fuente no debe eliminarse.
	*/
	const nFont *getFuente() const { return font; }

	/**
	Establece el color de las letras.
	@param r Nivel de rojo
	@param g Nivel de verde
	@param b Nivel de azul.
	*/
	void setColor(float r, float g, float b);

	/**
	Devuelve en los par�metros de salida el color de las letras.
	@param r Nivel de rojo
	@param g Nivel de verde
	@param b Nivel de azul.
	*/
	void getColor(float &r, float &g, float &b) const;

	/**
	 * Devuelve el tama�o que ocupar�a un determinado texto usando la
	 * fuente configurada actualmente en la etiqueta.
	 * @param text Texto cuyo tama�o se desea conocer.
	 * @param tamX Anchura del texto (en p�xeles). Par�metro de salida.
	 * @param tamY Altura del texto (en p�xeles). Par�metro de salida.
	 */
	void getTextSize(const char *text, int &tamX, int &tamY) const {
		assert(font && "Fuente no establecida en nFont");
		font->getTextSize((const unsigned char*)text, tamX, tamY);
	}

protected:

	/**
	* Devuelve el shader de pixeles est�tico compartido por todos
	* los nCSprite (de nombre 'nLabelPS'). Si a�n no existe,
	* lo crea.
	*/	      
	nPixelShader* getStaticPixelShader();

	/**
	 * Rellena el buffer de �ndices cuando sea necesario.
	 */
	void loadIndexBuffer();

	/**
	* Rellena el buffer de v�rtices cuando sea necesario.
	*/
	void loadVertexBuffer();

	/**
	* Ajusta los buffers al nuevo tama�o
	*/
	void adjustBuffers();

	/**
	 * Buffer de v�rtices.
	 */
	nRef<nVertexBuffer> ref_vb;
	/**
	 * Buffer de �ndices.
	 */
	nIndexBuffer *indexBuffer;

	/**
	 * Tama�o de la ventana entera. 
	 */
	float screenWidth,screenHeight;

	/**
	 * Tama�o de la etiqueta.
	 */
	int labelWidth,labelHeight;

	// Tama�os de la textura
	int textWidth, textHeight;

	/**
	 * Coordenadas del v�rtice superior izquierdo a partir del que se dibuja la etiqueta.
	 */
	int screenX,screenY;
	
	/**
	 * Posici�n de la etiqueta respecto a las coordenadas anteriores: centrada a izda, centrada
	 * a la dcha o justificada.
	 */
	char* labelPosition;

	/**
	 * Variable que indica si el label estar� o no visible.
	 */
	bool _visible;
	
	/**
	 * Texto del label.
  	 */
	char* _text;

	/**
	* Fuente a utilizar
	*/
	nFont* font;

	/**
	* Tipo de alineaci�n.
	*/
	enum Align alineacion;

	/**
	Color de las letras
	*/
	float r, g, b;

	short _order;

private:
	/**
	Pixel shader global para todas las etiquetas (nLabelPS)
	*/
	nPixelShader *pixelShader;
	nTextureArray * taArchivoTextura;

	
};
//------------------------------------------------------------------------------
#endif