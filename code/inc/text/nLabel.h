#ifndef N_NLABEL_H
#define N_NLABEL_H
//------------------------------------------------------------------------------
/**
    @class nLabel

    @brief 
  
    @author Felicidad Ramos Manjavacas, Marco Antonio Gómez Martín
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
	 * Establece el orden en el que aparecerán los distintos sprites. Cuanto más alto
	 * más "cerca" de la pantalla aparecerá, es decir, sprites con número altos
	 * aparecerán por encima de sprites con números más bajos. El resultado a
	 * igualdad de valor, es indeterminado.
	 */
	void setOrder(short order) {_order = order;}

	/**
	 * Devuelve el orden en el que aparecerá el sprite
	 */
	short getOrder() {return _order;}

	/**	
	 * Hace visible o no visible el label.
	 * @param visible Será cierto si el label será visible y falso en caso contrario.
	 */
	void setVisible(bool visible);

	/**
	 * Establece la alineación deseada.
	 * @param type Tipo de alineación.
	 */
	void setAlign(enum Align type);

	/**
	 * Establece las coordenadas de pantalla donde se dibujará la etiqueta.
	 * @param posX
	 * @param posY
	 */
	void setScreenPosition(int posX, int posY);

	/**
	 * Establece las coordenadas de pantalla donde se dibujará la etiqueta,
	 * expresadas de forma relativa al tamaño de la ventana.
	 * @param posX
	 * @param posY
	*/
	void setScreenPositionf(float posX, float posY);

	/**
	Devuelve la posición del label en pixeles.
	*/
	void getScreenPosition(int &posX, int &posY);

	/**
	Devuelve la posición del label en coordenadas relativas
	al tamaño de la ventana.
	*/
	void getScreenPositionf(float &posX, float &posY);
	
	/**	
	 * Establece el texto que contendrá la etiqueta asignándolo a la variable privada _text.
	 * @param texto Texto que contendrá la etiqueta. 
	 */
	void setText(const char* text);
	
	/**
	 * Establece el tamaño en la pantalla de lo que se va a dibujar.
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
	Devuelve si se está dibujando o no.
	*/
	bool getVisible() {return _visible;}

	/**
	 * Método para establecer la textura de la etiquetaa partir del nombre de un fichero 
	 * en el que se encuentra dicha textura. 
	 * Este método será el invocado para cargar texturas desde un script
	 * en tcl.Recibe como parámetro el archivo del que va a leer la textura y 
	 * por tanto, el parámetro "visible" se pondrá a cierto para que el nodo se añada 
	 * a la escena y se visualice posteriormente. Para esto,se llama aquí a las funciones 
	 * que rellenan las estructuras correspondientes para la correcta renderización de este nodo.
	 * @param fichero Fichero del que se cargará la textura.
	 */
	bool loadFont(const char * file);

	bool SaveCmds(nPersistServer* ps);
	
	/**
	 * Puntero al kernelServer
	 */
	static nKernelServer* kernelServer;	
	
	/**
	 * Método que añade este nodo a la escena.
	 */
	bool Attach(nSceneGraph2 *sceneGraph);
	
	/**
	 * Método al que llama cada nodo de la estructura de Nebula con todos los objetos visuales 
	 * que se le han ido añadiendo para actualizar sus valores.
	 */
	void Compute(nSceneGraph2 *sceneGraph);

	/**
	Devuelve la fuente que está utilizando la etiqueta.
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
	Devuelve en los parámetros de salida el color de las letras.
	@param r Nivel de rojo
	@param g Nivel de verde
	@param b Nivel de azul.
	*/
	void getColor(float &r, float &g, float &b) const;

	/**
	 * Devuelve el tamaño que ocuparía un determinado texto usando la
	 * fuente configurada actualmente en la etiqueta.
	 * @param text Texto cuyo tamaño se desea conocer.
	 * @param tamX Anchura del texto (en píxeles). Parámetro de salida.
	 * @param tamY Altura del texto (en píxeles). Parámetro de salida.
	 */
	void getTextSize(const char *text, int &tamX, int &tamY) const {
		assert(font && "Fuente no establecida en nFont");
		font->getTextSize((const unsigned char*)text, tamX, tamY);
	}

protected:

	/**
	* Devuelve el shader de pixeles estático compartido por todos
	* los nCSprite (de nombre 'nLabelPS'). Si aún no existe,
	* lo crea.
	*/	      
	nPixelShader* getStaticPixelShader();

	/**
	 * Rellena el buffer de índices cuando sea necesario.
	 */
	void loadIndexBuffer();

	/**
	* Rellena el buffer de vértices cuando sea necesario.
	*/
	void loadVertexBuffer();

	/**
	* Ajusta los buffers al nuevo tamaño
	*/
	void adjustBuffers();

	/**
	 * Buffer de vértices.
	 */
	nRef<nVertexBuffer> ref_vb;
	/**
	 * Buffer de índices.
	 */
	nIndexBuffer *indexBuffer;

	/**
	 * Tamaño de la ventana entera. 
	 */
	float screenWidth,screenHeight;

	/**
	 * Tamaño de la etiqueta.
	 */
	int labelWidth,labelHeight;

	// Tamaños de la textura
	int textWidth, textHeight;

	/**
	 * Coordenadas del vértice superior izquierdo a partir del que se dibuja la etiqueta.
	 */
	int screenX,screenY;
	
	/**
	 * Posición de la etiqueta respecto a las coordenadas anteriores: centrada a izda, centrada
	 * a la dcha o justificada.
	 */
	char* labelPosition;

	/**
	 * Variable que indica si el label estará o no visible.
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
	* Tipo de alineación.
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