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
El sprite viene a partir de una textura. Se puede definir tanto el rect�ngulo
a dibujar dentro de esa textura, como el rect�ngulo que ocupar� en la pantalla.
Para ambas cosas, las coordenadas se pueden dar en p�xeles (de textura o de pantalla),
o en tama�os relativos, es decir, utilizando valores entre 0 y 1, relativos al tama�o
de la textura o de la ventana o �rea de renderizado. La diferencia entre ambas
en las llamadas es por el nombre de la funci�n; la funci�n para establecer
la posici�n del sprite en la pantalla ser� setScreenPosition y setScreenPositionf,
indicando la �ltima f del nombre, que los par�metros son reales (float).
Las funciones son: setScreenPosition{f}, setScreenSize{f}, setSourcePosition{f}, y
setSourceSize{f}.<p>
Si la posici�n del rect�ngulo en la ventana lo hace no visible, no se manda dibujar.
Por otro lado, los tama�os relativos (entre 0 y 1), tendr�n un comportamiento indeterminado
si est�n fuera del rango (bueno... en modo debug, saltar� un assert). Por �ltimo,
si el tama�o del rect�ngulo en la imagen origen (textura) hace que se salga fuera de la
textura, �sta se repetir� en la imagen final.<p>
La propiedad autoScale (setAutoScale(), getAutoScale()), sirve para no tener que indicar
el tama�o que se desea que ocupe el sprite en la ventana; en caso de estar activado,
ocupar� el mismo n�mero de p�xeles que el rect�ngulo en la imagen origen (textura).<p>
La imagen origen puede establecerse de dos formas distintas: si el objeto tiene como
hermano una textura en el �rbol de la escena, se utilizar� �sta textura; en caso contrario,
se utilizar� la textura indicada con createSprite(const char*). Ahora bien, en caso de estar
utilizando la textura hermana, externa a esta clase, en vez de la cargada con el m�todo
anterior, la llamada a getImageSize devolver� valores err�neos.<p>
Tambi�n existen dos posibilidades en cuanto al shader utilizado; si no hay un shader
hermano al objeto nCSprite, se utilizar� uno por defecto.

@author Marco Antonio G�mez Mart�n, Felicidad Ramos Manjavacas
*/
class N_PUBLIC nCSprite : public nVisNode {

public:
   
   /**
	* Constructora sin par�metros. Como no existe textura a�n, el par�metro "visible"
	* ser� puesto a false para que el nodo no se a�ada a la escena y por tanto no 
	* se visualice de momento. 
	*/
    nCSprite();
  
   /**
	* Destructora.
	*/
    ~nCSprite();

   /**
	* M�todo para crear un sprite a partir de un fichero en el que se encuentra
	* la textura. Este m�todo ser� el invocado para cargar texturas desde un script
	* en tcl.Recibe como par�metro el archivo del que va a leer la textura y 
	* por tanto, el par�metro "visible" se pondr� a cierto para que el nodo se a�ada 
	* a la escena y se visualice posteriormente. Para esto,se llama aqu� a las funciones 
	* que rellenan las estructuras correspondientes para la correcta renderizaci�n de este nodo.
	* @param fichero Fichero del que se cargar� la textura.
	*/
	void createSprite(const char* fich);
	
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
	 * Hace visible el sprite.
	 * @param visible Ser� cierto si el sprite ser� visible y falso en caso contrario.
	 */
	void setVisible(bool visible);

	bool getVisible() {return _visible;}
	
	/**
	 * Establece las coordenadas de pantalla donde se dibujar� el sprite.
	 */
	void setScreenPosition(int posX, int posY);

	/**
	 Establece las coordenadas relativas donde se dibujar� el sprite en la
	 pantalla (entre 0 y 1).
	*/
	void setScreenPositionf(float posX, float posY);

	/**
	Devuelve la posici�n del sprite en la pantalla.
	*/
	void getPosition(int &posX, int &posY);

	/**
	Devuelve la posici�n del sprite en la pantalla.
	*/
	void getPositionf(float &posX, float &posY);

	/**
	Establece el tama�o en la pantalla de lo que se va a dibujar.
	*/
	void setScreenSize(int width, int height);

	/**
	Establece el tama�o relativo en la pantalla de lo que se va a dibujar,
	entre 0 y 1 (relativo a la pantalla).
	*/
	void setScreenSizef(float width, float height);

	/**
	Devuelve el tama�o del sprite en la pantalla.
	*/
	void getScreenSize(int &width, int &height);

	/**
	Devuelve el tama�o del sprite en la pantalla.
	*/
	void getScreenSizef(float &width, float &height);

	/**
	Establece la posici�n de la esquina superior izquierda en la
	textura origen.
	*/
	void setSourcePosition(int posX, int posY);

	/**
	Establece la posici�n de la esquina superior izquierda en la
	textura origen, relativo a la textura, es decir, entre 0 y 1,
	independientemente del tama�o de la textura.
	*/
	void setSourcePositionf(float posX, float posY);

	/**
	Devuelve la posici�n de la esquina superior izquierda del sprite
	dentro de la imagen origen (textura).
	*/
	void getSourcePosition(int &posX, int &posY);

	/**
	Devuelve la posici�n de la esquina superior izquierda del sprite
	dentro de la imagen origen (textura).
	*/
	void getSourcePositionf(float &posX, float &posY);

	/**
	Establece el tama�o del rect�ngulo en la imagen origen.
	*/
	void setSourceSize(int width, int height);

	/**
	Establece el tama�o del rect�ngulo en la imagen origen, relativo
	al tama�o de la textura, es decir, entre 0 y 1.
	*/
	void setSourceSizef(float width, float height);

	/**
	Devuelve el tama�o del rect�ngulo en la imagen origen.
	*/
	void getSourceSize(int &width, int &height);

	/**
	Devuelve el tama�o del rect�ngulo en la imagen origen.
	*/
	void getSourceSizef(float &width, float &height);

	/**
	Establece si se quiere que el rect�ngulo elegido en la textura
	ocupe el mismo n�mero de p�xeles en la ventana. Si es as�,
	setScreenSize no tendr� efecto.
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
     * M�todo que rellena los buffers de v�rtices e �ndices.
     */
	void loadBuffers();

	/**
     * M�todo que rellena los buffers de v�rtices cuando se ha realizado un cambio
	 * en el sprite o parte de �l que quiere mostrarse.
     */
	void nCSprite::reloadBuffers();

	/**
	 * M�todo que a�ade este nodo a la escena.
	 */
	bool Attach(nSceneGraph2 *sceneGraph);
	
	/**
	 * M�todo al que llama cada nodo de la estructura de Nebula con todos los objetos visuales 
	 * que se le han ido a�adiendo para actualizar sus valores.
	 */
	void Compute(nSceneGraph2 *sceneGraph);

protected:

	/**
	Buffer de v�rtices. Es espec�fico de cada objeto, es decir, no se
	comparte entre varios. Su nombre de recurso es nCSpriteVB[this]
	*/
	nRef<nVertexBuffer> ref_vb;

	/**
	Flag que indica si el buffer de v�rtices hay que recalcularlo,
	por alg�n cambio de posici�n.
	*/
	bool vbDirty;

	/**
	Indica el orden o prioridad de renderizado.
	*/
	short _order;

	/**
	Rellena el buffer de v�rtices, teniendo en cuenta las posiciones origen
	y destino.
	@param ta Array de texturas que se va a utilizar para dibujar. De �l, mirar�
	el tama�o de la textura 0 que se dibuja, y considerar� que es de ah� de donde
	se coge el sprite. Si no se indica par�metro (valor por defecto NULL),
	se considera que la textura es la establecida con createSprite.
	*/
	void calculateVertexBuffer(nTextureArray *ta = NULL);

	// --
	// VARIABLES QUE DEFINEN �REAS. PUEDEN SER ABSOLUTAS (EN P�XELES)
	// O RELATIVAS (ENTRE 0 Y 1), DE AH� EL CONJUNTO TAN GRANDE DE ELLAS,
	// ENTRE VALORES Y FLAGS
	// --

	/**
	Indica que con los valores actuales de las coordenadas, el sprite no es
	visible, por lo tanto, no se dibuja. Este flag es interno a la clase, y
	calculado, dependiendo de los valores; es una optimizaci�n, para no mandar
	a dibujar el sprite, cuando se sabe que no se dibuja, porque no se ve.
	*/
	bool notVisible;

	// ----- COORDENADAS DE LA VENTANA ----
	// Posiciones x e y en la pantalla.
	int screenX, screenY;
	float fScreenX, fScreenY;
	bool screenIntUsed;

	// Tama�os del rect�ngulo en la pantalla
	int rectangleScreenAncho, rectangleScreenAlto;
	float fRectangleScreenAncho, fRectangleScreenAlto;
	bool rectangleScreenIntUsed;

	// Tama�o de la ventana completa (para hacer interpolaci�n...)
	int screenAncho, screenAlto;
	/**
	Comprueba el tama�o de la ventana, y si cambia, devuelve cierto,
	lo que significar� que hay que recalcular los v�rtices.
	*/
	bool UpdateScreenSize();

	// ----- COORDENADAS EN LA TEXTURA ORIGEN -----
	// Posiciones de la esquina superior izquierda
	int sourceX, sourceY;
	float fSourceX, fSourceY;
	bool sourceIntUsed;

	// Tama�os del rect�nculo en la textura origen
	int rectangleSourceAncho, rectangleSourceAlto;
	float fRectangleSourceAncho, fRectangleSourceAlto;
	bool rectangleSourceIntUsed;

	// Tama�os de la textura
	int imageAncho, imageAlto;

	// --
	// OTRAS VARIABLES
	// --

	/**
	Indica si se desea dibujar el sprite o no. Se cambia con el m�todo
	setVisible().
	*/
	bool _visible;

	/**
	Cuando est� a cierto, el tama�o en p�xeles del rect�ngulo elegido en la
	textura coincide con el tama�o que ocupar� en pixeles en la ventana,
	independientemente del tama�o que se eliga como rect�ngulo en la pantalla.
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
	Array de texturas, que contiene en la primera posici�n la textura del
	sprite que se est� dibujando. S�lo tiene validez cuando se ha
	indicado el nombre de la textura en esta clase, en vez de utilizar
	el nTextureArrayNode que tiene esta clase como hermano.
	*/
	nTextureArray *taArchivoTextura;

	/**
	Flag que indica si en el �ltimo frame renderizado se utiliz� una
	textura externa, o la cargada.
	*/
	bool lastFrameUsingExternalTexture;

	int sizeXExternalTexture;
	int sizeYExternalTexture;

	/**
	Devuelve cierto si el tama�o de la textura del par�metro es distinta
	de (sizeXExternalTexture, sizeYExternalTexture).
	@note Si es distinta, iguala el tama�o de la del par�metro a las variables,
	es decir, la siguiente llamada a esta funci�n, devolver�a false.
	*/
	bool UpdateSiblingTextureSize(nTextureArray *ta);

	/**
	Devuelve el shader de pixeles est�tico compartido por todos
	los nCSprite (de nombre 'nCSpriteShader'). Si a�n no existe,
	lo crea.
	*/
	nPixelShader *getStaticPixelShader();

	/**
	Buffer de �ndices, que define los dos tri�ngulos. Dado que
	todos los sprites tendr�n los mismos cuadrados, se comparte
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
	Inicializa el buffer de �ndices est�tico compartido por
	todos los nCSprite (de nombre 'nCSpriteIndexBuffer'). Es estatico
	a todos, porque todos son cuadrados con los dos tri�ngulos igual.
	Si no existe a�n, lo crea.
	*/
	void initStaticIndexBuffer();

};
//------------------------------------------------------------------------------
#endif
