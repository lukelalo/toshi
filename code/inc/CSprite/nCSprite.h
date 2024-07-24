#ifndef N_NCSPRITE_H
#define N_NCSPRITE_H
//------------------------------------------------------------------------------

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif


#ifndef N_N3DNODE_H
#include "node/n3dnode.h"
#endif

#include "gfx/nscenegraph2.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"

//------------------------------------------------------------------------------
/**
Clase para dibujar sprites en la ventana, superponiendose a la escena 3D.<p>
El sprite viene a partir de una textura. Se puede definir tanto el rectángulo
a dibujar dentro de esa textura, como el rectángulo que ocupará en la pantalla.
Para ambas cosas, las coordenadas se pueden dar en píxeles (de textura o de pantalla),
o en tamaños relativos, es decir, utilizando valores entre 0 y 1, relativos al tamaño
de la textura o de la ventana o área de renderizado. La diferencia entre ambas
en las llamadas es por el nombre de la función; la función para establecer
la posición del sprite en la pantalla será setScreenPosition y setScreenPositionf,
indicando la última f del nombre, que los parámetros son reales (float).
Las funciones son: setScreenPosition{f}, setScreenSize{f}, setSourcePosition{f}, y
setSourceSize{f}.<p>
Si la posición del rectángulo en la ventana lo hace no visible, no se manda dibujar.
Por otro lado, los tamaños relativos (entre 0 y 1), tendrán un comportamiento indeterminado
si están fuera del rango (bueno... en modo debug, saltará un assert). Por último,
si el tamaño del rectángulo en la imagen origen (textura) hace que se salga fuera de la
textura, ésta se repetirá en la imagen final.<p>
La propiedad autoScale (setAutoScale(), getAutoScale()), sirve para no tener que indicar
el tamaño que se desea que ocupe el sprite en la ventana; en caso de estar activado,
ocupará el mismo número de píxeles que el rectángulo en la imagen origen (textura).<p>
La imagen origen puede establecerse de dos formas distintas: si el objeto tiene como
hermano una textura en el árbol de la escena, se utilizará ésta textura; en caso contrario,
se utilizará la textura indicada con createSprite(const char*). Ahora bien, en caso de estar
utilizando la textura hermana, externa a esta clase, en vez de la cargada con el método
anterior, la llamada a getImageSize devolverá valores erróneos.<p>
También existen dos posibilidades en cuanto al shader utilizado; si no hay un shader
hermano al objeto nCSprite, se utilizará uno por defecto.

@author Marco Antonio Gómez Martín, Felicidad Ramos Manjavacas
*/
class N_PUBLIC nCSprite : public nVisNode {

public:
   
   /**
	* Constructora sin parámetros. Como no existe textura aún, el parámetro "visible"
	* será puesto a false para que el nodo no se añada a la escena y por tanto no 
	* se visualice de momento. 
	*/
    nCSprite();
  
   /**
	* Destructora.
	*/
    ~nCSprite();

   /**
	* Método para crear un sprite a partir de un fichero en el que se encuentra
	* la textura. Este método será el invocado para cargar texturas desde un script
	* en tcl.Recibe como parámetro el archivo del que va a leer la textura y 
	* por tanto, el parámetro "visible" se pondrá a cierto para que el nodo se añada 
	* a la escena y se visualice posteriormente. Para esto,se llama aquí a las funciones 
	* que rellenan las estructuras correspondientes para la correcta renderización de este nodo.
	* @param fichero Fichero del que se cargará la textura.
	*/
	void createSprite(const char* fich);
	
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
	 * Hace visible el sprite.
	 * @param visible Será cierto si el sprite será visible y falso en caso contrario.
	 */
	void setVisible(bool visible);

	bool getVisible() {return _visible;}
	
	/**
	 * Establece las coordenadas de pantalla donde se dibujará el sprite.
	 */
	void setScreenPosition(int posX, int posY);

	/**
	 Establece las coordenadas relativas donde se dibujará el sprite en la
	 pantalla (entre 0 y 1).
	*/
	void setScreenPositionf(float posX, float posY);

	/**
	Devuelve la posición del sprite en la pantalla.
	*/
	void getPosition(int &posX, int &posY);

	/**
	Devuelve la posición del sprite en la pantalla.
	*/
	void getPositionf(float &posX, float &posY);

	/**
	Establece el tamaño en la pantalla de lo que se va a dibujar.
	*/
	void setScreenSize(int width, int height);

	/**
	Establece el tamaño relativo en la pantalla de lo que se va a dibujar,
	entre 0 y 1 (relativo a la pantalla).
	*/
	void setScreenSizef(float width, float height);

	/**
	Devuelve el tamaño del sprite en la pantalla.
	*/
	void getScreenSize(int &width, int &height);

	/**
	Devuelve el tamaño del sprite en la pantalla.
	*/
	void getScreenSizef(float &width, float &height);

	/**
	Establece la posición de la esquina superior izquierda en la
	textura origen.
	*/
	void setSourcePosition(int posX, int posY);

	/**
	Establece la posición de la esquina superior izquierda en la
	textura origen, relativo a la textura, es decir, entre 0 y 1,
	independientemente del tamaño de la textura.
	*/
	void setSourcePositionf(float posX, float posY);

	/**
	Devuelve la posición de la esquina superior izquierda del sprite
	dentro de la imagen origen (textura).
	*/
	void getSourcePosition(int &posX, int &posY);

	/**
	Devuelve la posición de la esquina superior izquierda del sprite
	dentro de la imagen origen (textura).
	*/
	void getSourcePositionf(float &posX, float &posY);

	/**
	Establece el tamaño del rectángulo en la imagen origen.
	*/
	void setSourceSize(int width, int height);

	/**
	Establece el tamaño del rectángulo en la imagen origen, relativo
	al tamaño de la textura, es decir, entre 0 y 1.
	*/
	void setSourceSizef(float width, float height);

	/**
	Devuelve el tamaño del rectángulo en la imagen origen.
	*/
	void getSourceSize(int &width, int &height);

	/**
	Devuelve el tamaño del rectángulo en la imagen origen.
	*/
	void getSourceSizef(float &width, float &height);

	/**
	Establece si se quiere que el rectángulo elegido en la textura
	ocupe el mismo número de píxeles en la ventana. Si es así,
	setScreenSize no tendrá efecto.
	*/
	void setAutoScale(bool autoScale);

	/**
	Devuelve el valor del auto escalado.
	*/
	bool getAutoScale() {return autoScale;}

	/**
	 * Devuelve el ancho y alto del sprite dibujado actualmente.
	 */   
	//void getSpriteSize(int& longW,int& longH);

	/**
	 * Devuelve el ancho y alto del archivo leido.
	 */
	void getImageSize(int& longX,int& longY);

	/**
	 * Carga en pantalla la textura entera
	 */
	void allRectangle();

	bool SaveCmds(nPersistServer* ps);
	
	static nKernelServer* kernelServer;	//Puntero al kernelServer

	/**
     * Método que rellena los buffers de vértices e índices.
     */
	void loadBuffers();

	/**
     * Método que rellena los buffers de vértices cuando se ha realizado un cambio
	 * en el sprite o parte de él que quiere mostrarse.
     */
	void nCSprite::reloadBuffers();

	/**
	 * Método que añade este nodo a la escena.
	 */
	bool Attach(nSceneGraph2 *sceneGraph);
	
	/**
	 * Método al que llama cada nodo de la estructura de Nebula con todos los objetos visuales 
	 * que se le han ido añadiendo para actualizar sus valores.
	 */
	void Compute(nSceneGraph2 *sceneGraph);

protected:

	/**
	Buffer de vértices. Es específico de cada objeto, es decir, no se
	comparte entre varios. Su nombre de recurso es nCSpriteVB[this]
	*/
	nRef<nVertexBuffer> ref_vb;

	/**
	Flag que indica si el buffer de vértices hay que recalcularlo,
	por algún cambio de posición.
	*/
	bool vbDirty;

	/**
	Indica el orden o prioridad de renderizado.
	*/
	short _order;

	/**
	Rellena el buffer de vértices, teniendo en cuenta las posiciones origen
	y destino.
	@param ta Array de texturas que se va a utilizar para dibujar. De él, mirará
	el tamaño de la textura 0 que se dibuja, y considerará que es de ahí de donde
	se coge el sprite. Si no se indica parámetro (valor por defecto NULL),
	se considera que la textura es la establecida con createSprite.
	*/
	void calculateVertexBuffer(nTextureArray *ta = NULL);

	// --
	// VARIABLES QUE DEFINEN ÁREAS. PUEDEN SER ABSOLUTAS (EN PÍXELES)
	// O RELATIVAS (ENTRE 0 Y 1), DE AHÍ EL CONJUNTO TAN GRANDE DE ELLAS,
	// ENTRE VALORES Y FLAGS
	// --

	/**
	Indica que con los valores actuales de las coordenadas, el sprite no es
	visible, por lo tanto, no se dibuja. Este flag es interno a la clase, y
	calculado, dependiendo de los valores; es una optimización, para no mandar
	a dibujar el sprite, cuando se sabe que no se dibuja, porque no se ve.
	*/
	bool notVisible;

	// ----- COORDENADAS DE LA VENTANA ----
	// Posiciones x e y en la pantalla.
	int screenX, screenY;
	float fScreenX, fScreenY;
	bool screenIntUsed;

	// Tamaños del rectángulo en la pantalla
	int rectangleScreenAncho, rectangleScreenAlto;
	float fRectangleScreenAncho, fRectangleScreenAlto;
	bool rectangleScreenIntUsed;

	// Tamaño de la ventana completa (para hacer interpolación...)
	int screenAncho, screenAlto;
	/**
	Comprueba el tamaño de la ventana, y si cambia, devuelve cierto,
	lo que significará que hay que recalcular los vértices.
	*/
	bool UpdateScreenSize();

	// ----- COORDENADAS EN LA TEXTURA ORIGEN -----
	// Posiciones de la esquina superior izquierda
	int sourceX, sourceY;
	float fSourceX, fSourceY;
	bool sourceIntUsed;

	// Tamaños del rectánculo en la textura origen
	int rectangleSourceAncho, rectangleSourceAlto;
	float fRectangleSourceAncho, fRectangleSourceAlto;
	bool rectangleSourceIntUsed;

	// Tamaños de la textura
	int imageAncho, imageAlto;

	// --
	// OTRAS VARIABLES
	// --

	/**
	Indica si se desea dibujar el sprite o no. Se cambia con el método
	setVisible().
	*/
	bool _visible;

	/**
	Cuando está a cierto, el tamaño en píxeles del rectángulo elegido en la
	textura coincide con el tamaño que ocupará en pixeles en la ventana,
	independientemente del tamaño que se eliga como rectángulo en la pantalla.
	*/
	bool autoScale;

	/**
	Nombre del fichero de la textura.
	*/
	char archivoTextura[N_MAXNAMELEN];

	/**
	Flag que indica si el nombre de la textura del sprite ha cambiado, y hay
	que recargar la textura.
	*/
	bool textrDirty;

	/**
	Array de texturas, que contiene en la primera posición la textura del
	sprite que se está dibujando. Sólo tiene validez cuando se ha
	indicado el nombre de la textura en esta clase, en vez de utilizar
	el nTextureArrayNode que tiene esta clase como hermano.
	*/
	nTextureArray *taArchivoTextura;

	/**
	Flag que indica si en el último frame renderizado se utilizó una
	textura externa, o la cargada.
	*/
	bool lastFrameUsingExternalTexture;

	int sizeXExternalTexture;
	int sizeYExternalTexture;

	/**
	Devuelve cierto si el tamaño de la textura del parámetro es distinta
	de (sizeXExternalTexture, sizeYExternalTexture).
	@note Si es distinta, iguala el tamaño de la del parámetro a las variables,
	es decir, la siguiente llamada a esta función, devolvería false.
	*/
	bool UpdateSiblingTextureSize(nTextureArray *ta);

	/**
	Devuelve el shader de pixeles estático compartido por todos
	los nCSprite (de nombre 'nCSpriteShader'). Si aún no existe,
	lo crea.
	*/
	nPixelShader *getStaticPixelShader();

	/**
	Buffer de índices, que define los dos triángulos. Dado que
	todos los sprites tendrán los mismos cuadrados, se comparte
	entre todos. Este puntero se inicializa en el constructor,
	y se libera en el destructor.
	*/
	nIndexBuffer *indexBuffer;

private:

	/**
	Pixel shader global para todos los sprites (nCSpriteShader)
	*/
	nPixelShader *pixelShader;

	/**
	Inicializa el buffer de índices estático compartido por
	todos los nCSprite (de nombre 'nCSpriteIndexBuffer'). Es estatico
	a todos, porque todos son cuadrados con los dos triángulos igual.
	Si no existe aún, lo crea.
	*/
	void initStaticIndexBuffer();

};
//------------------------------------------------------------------------------
#endif
