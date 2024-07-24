//---------------------------------------------------------------------------
// MDLBasic.h
//---------------------------------------------------------------------------

/**
 * @file MDLBasic.h
 * Contiene las estructuras b�sicas que se guardan en un fichero
 * MDL. Por ejemplo, contiene una estructura que encaja perfectamente
 * con los primeros bytes del fichero, que componen la cabecera.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLBasic
#define __HalfLifeMDL_MDLBasic

#include "MDLActionTags.h"

/**
 * Espacio de nombres con todo lo relativo al cargador de ficheros MDL y
 * las clases usadas para mantenerlos en memoria.<p>
 * <b>IMPORTANTE: </b> para que las clases funcionen correctamente, el
 * sistema sobre el que se compilan debe ser little-endian.
 * @warning Este es el aviso.
 * @attention Que te peta!
 * @note Nota.
 * @remarks Remarks
 * @todo Descripci�n m�s detallada (est� en MDLBasic.h).
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
namespace HalfLifeMDL {

/**
 * Enumerado que define constantes para los 6 posibles grados de liberad
 * (Degree Of Freedom).
 * El fichero guarda algunos vectores de 6 elementos indicando, por ejemplo,
 * la configuraci�n por defecto de un hueso en sus seis grados de libertad.
 * Para acceder a esos arrays con nombres ilustrativos se utiliza este
 * enumerado.
 * <p>
 * Los valores asociados a cada valor no pueden ser modificados, por lo que
 * los usuarios de este tipo pueden hacer uso de sus valores num�ricos en
 * lugar de los valores constantes para hacer recorridos sobre los arrays
 * que lo requieran por ejemplo.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef enum {
	/** Desplazamiento X. */
	DoF_X = 0,
	/** Desplazamiento Y. */
	DoF_Y,
	/** Desplazamiento Z. */
	DoF_Z,
	/** Rotaci�n sobre el eje X. */
	DoF_RX,
	/** Rotaci�n sobre el eje Y. */
	DoF_RY,
	/** Rotaci�n sobre el eje Z. */
	DoF_RZ
} DoF_t;

/**
 * Enumerado que define constantes para el campo flags de las texturas
 * (texture_t::flags). Sus valores se pueden condensar usando OR l�gica
 * a nivel de bits.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef enum {
	/**
	 * Textura sin ninguna caracter�stica especial (las caracteristicas
	 * por defecto).
	 */
	TF_none = 0,
	/**
	 * Sombreado flat. No estoy seguro de esto.
	 * @todo �Buscar alg�n modelo que lo use? El lector que he usado
	 * para averiguar toda la informaci�n sobre el formato no comprueba nunca
	 * este valor.
	 */
	TF_FlatShade = 1,
	/**
	 * Material cromado.
	 * @todo Averiguar m�s sobre esto mirando el c�digo del lector de
	 * referencia.
	 */
	TF_Chrome = 2,
	/**
	 * Material que debe iluminarse por completo. No s� qu� significa :-D
	 * @todo �Buscar alg�n modelo que lo use? El lector que he usado
	 * para averiguar toda la informaci�n sobre el formato no comprueba nunca
	 * este valor.
	 */
	TF_Fullbright = 4

} textureFlags_t;

/**
 * Enumerado que define las constantes para el campo motiontype de las
 * secuencias (seqdesc_t::motiontype). Sus valores se pueden condensar
 * usando OR l�gica a nivel de bits, aunque no todas las combinaciones
 * tienen sentido. <p>
 * La idea es que una secuencia de un modelo a parte de la animaci�n de los
 * huesos puede originar un desplazamiento sobre el origen de coordenadas
 * del hueso principal. Por ejemplo, cuando el personaje anda, no s�lo
 * se deben mover los pies, sino que el motor debe desplazar por el mundo
 * al personaje para hacerle cambiar de posici�n. Los tipos de desplazamiento
 * pueden clasificarse en tres grupos:
 *	- Desplazamiento lineal: el ejemplo m�s claro aparece en los ciclos de
 * andar o correr. Especifica el avance del personaje que debe ser constante
 * en el tiempo.
 *	- Desplazamiento absoluto: tradicionalmente para secuencias no c�clicas
 * que originan un desplazamiento del personaje. Para simplificar las cosas,
 * el motor podr�a no desplazar al personaje durante la reproducci�n de la
 * secuencia, y ajustar la posici�n solamente al final. Por ejemplo, en una
 * animaci�n en la que se muestra como el personaje muere, �ste podr�a caer
 * hacia atr�s, o de lado y terminar tendido en el suelo. Durante toda la
 * animaci�n el eje de referencia no cambia, pero cuando la secuencia acaba
 * lo ideal ser�a establecer una "secuencia" donde el �nico fotograma es
 * el personaje tendido en la misma posici�n, y con el origen de coordenadas
 * colocado de forma razonable respecto al cuerpo. Ese desplazamiento absoluto
 * realizado al final de la secuencia de muerte entrar�a en esta categor�a.
 *	- Constante pero irregular: si se desea complicar la vida al motor, el
 * ejemplo anterior podr�a realizarse cambiando cont�nuamente la posici�n
 * del personaje en el mundo seg�n va cayendo hacia atr�s y terminando en
 * el suelo. Ese desplazamiento no es lineal como en el caso del ciclo de
 * andar, por lo que, a parte de las animaciones sobre los huesos, se
 * necesitar�a informaci�n sobre el desplazamiento absoluto del personaje
 * en cada fotograma respecto al anterior.
 *
 * �n realidad todos son un mismo modo de hacer lo mismo. Por ejemplo, el
 * ciclo de andar podr�a realizarse con desplazamiento absoluto en lugar
 * de lineal. Lo �nico que cambiar�a ser�a la posici�n de los huesos
 * respecto al eje de referencia del hueso ra�z.
 * <p>
 * Por otro lado, aunque hemos estado hablando de desplazamientos, tambi�n
 * habr�a que incluir rotaciones, por ejemplo para la animaci�n en la que
 * el personaje gira sobre s� mismo sin andar para mirar a otro lado.
 * <p>
 * Se supone que el formato de fichero de Half Life est� preparado para
 * guardar todos esos tipos de informaci�n, salvo la rotaci�n lineal, y
 * el campo seqdesc_t::motiontype contendr� el tipo de desplazamiento
 * particular de una secuencia concreta. En realidad, la granularidad
 * de este campo es mayor que las tres posibilidades comentadas antes,
 * y llega al nivel de coordenadas. Por ejemplo, en lugar de especificar
 * que la secuencia tiene animaci�n lineal y dar las tres coordenadas,
 * indica que tiene animaci�n lineal sobre el eje X, por lo que los
 * campos de animaci�n lineal sobre los ejes Y o Z de la secuencia no
 * deber�n ser atendidos. Esto ocasiona que el valor del campo sea en
 * realidad una mezcla (con OR l�gicas) de varios valores. Los posibles
 * valores a mezclar son los que especifica esta estructura. En realidad
 * no todas las mezclas tienen sentido (por ejemplo, que el personaje
 * tenga desplazamiento lineal en el eje X, y a la vez desplazamiento
 * irregular en el mismo eje).
 * <p>
 * Por desgracia, la realidad es bastante m�s triste. Aunque parece que
 * el formato del fichero de Half Life permite los tipos de desplazamiento
 * descritos, la realidad es que el empaquetador de los modelos (el
 * programa StudioMDL) no permite especificar todos los posibles valores
 * (o, m�s bien, los ignora), por lo que la �nica posibilidad admitida
 * es el desplazamiento lineal sobre alg�n eje. El an�lisis realizado
 * sobre todos los modelos del juego original muestran que s�lo esos
 * tipos de movimiento se usan en alg�n momento. M�s concretamente, los
 * personajes se desplazan cuando avanzan hacia el lado positivo del
 * eje X. Espor�dicamente alguno tiene desplazamiento lineal en el
 * eje Z en los saltos, aunque no parece tener mucho sentido. <p>
 * Es posible especificar todos los tipos de desplazamiento en el .QC
 * con el que crear el modelo, y StudioMDL marcar� los flags correctamente.
 * Sin embargo, ser�a l�gico que en el caso de desplazamiento irregular
 * luego especificara en alg�n sitio el desplazamiento por cada
 * fotograma, pero no es as�. Se supone que eso se ha dejado para que
 * sea el programa el que lo conozca, en lugar de estar como "informaci�n
 * declarativa" en el fichero..<p>
 * Otra cosa curiosa es que hay alguna animaci�n de giro sobre s� mismo
 * (por ejemplo en <code>female.mdl</code>), sin embargo, como no hay
 * posibilidad de especificar rotaci�n lineal, esas secuencias no tienen
 * ning�n flag activo.
 * <p>
 * Resumiendo, la informaci�n sobre el movimiento en el mundo de
 * los modelos para cada secuencia est� en los MDL no es completa. S�lo
 * se soporta completamente el desplazamiento lineal, pero no la rotaci�n.
 * El resto quiz� se pens� en soportar, pero finalmente parece que no
 * se hace. Eso lleva, por otro lado, a que todo lo contado aqu� sean
 * h�p�tesis, porque no hay nada probable. Sospecho que muchas de las
 * cosas se quedaron como "Buenas ideas para con un poco de suerte
 * utilizar en alguna versi�n futura". 
 * @see HalfLifeMDL::seqdesc_t
 * @see @ref secuenciasYGrupos
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef enum {

	/**
	 * Desplazamiento absoluto sobre el eje X. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener ese desplazamiento del fichero.
	 */
	MT_X = 0x0001,
	/**
	 * Desplazamiento absoluto sobre el eje Y. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener ese desplazamiento del fichero.
	 */
	MT_Y = 0x0002,
	/**
	 * Desplazamiento absoluto sobre el eje Z. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma obtener ese desplazamiento del fichero.
	 */
	MT_Z = 0x0004,
	/**
	 * Rotaci�n absoluta sobre el eje X. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener esa rotaci�n del fichero.
	 */
	MT_XR = 0x0008,
	/**
	 * Rotaci�n absoluta sobre el eje Y. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener esa rotaci�n del fichero.
	 */
	MT_YR = 0x0010,
	/**
	 * Rotaci�n absoluta sobre el eje Z. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma obtener esa rotaci�n del fichero.
	 */
	MT_ZR = 0x0020,
	/**
	 * Desplazamiento lineal en el eje X. El modelo debe ser desplazado
	 * en un movimiento lineal uniforme. El desplazamiento a aplicar se
	 * obtiene de HalfLifeMDL::seqdesc_t::linearmovement.
	 */
	MT_LX = 0x0040,
	/**
	 * Desplazamiento lineal en el eje Y. El modelo debe ser desplazado
	 * en un movimiento lineal uniforme. El desplazamiento a aplicar se
	 * obtiene de HalfLifeMDL::seqdesc_t::linearmovement.
	 */
	MT_LY = 0x0080,
	/**
	 * Desplazamiento lineal en el eje Z. El modelo debe ser desplazado
	 * en un movimiento lineal uniforme. El desplazamiento a aplicar se
	 * obtiene de HalfLifeMDL::seqdesc_t::linearmovement.
	 */
	MT_LZ = 0x0100,
	/**
	 * Desplazamiento irregular sobre el eje X. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener los desplazamientos de cada
	 * fotograma, aunque podr�a parecer que se deber�an obtener de
	 * HalfLifeMDL::seqdesc_t::automoveposindex.
	 */
	MT_AX = 0x0200,
	/**
	 * Desplazamiento irregular sobre el eje Y. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener los desplazamientos de cada
	 * fotograma, aunque podr�a parecer que se deber�an obtener de
	 * HalfLifeMDL::seqdesc_t::automoveposindex.
	 */
	MT_AY = 0x0400,
	/**
	 * Desplazamiento irregular sobre el eje Z. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener los desplazamientos de cada
	 * fotograma, aunque podr�a parecer que se deber�an obtener de
	 * HalfLifeMDL::seqdesc_t::automoveposindex.
	 */
	MT_AZ = 0x0800,
	/**
	 * Rotaci�n irregular sobre el eje X. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener la rotaci�n de cada
	 * fotograma, aunque podr�a parecer que se deber�an obtener de
	 * HalfLifeMDL::seqdesc_t::automoveangleindex.
	 */
	MT_AXR = 0x1000,
	/**
	 * Rotaci�n irregular sobre el eje Y. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener la rotaci�n de cada
	 * fotograma, aunque podr�a parecer que se deber�an obtener de
	 * HalfLifeMDL::seqdesc_t::automoveangleindex.
	 */
	MT_AYR = 0x2000,
	/**
	 * Rotaci�n irregular sobre el eje Z. En realidad es una hip�tesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener la rotaci�n de cada
	 * fotograma, aunque podr�a parecer que se deber�an obtener de
	 * HalfLifeMDL::seqdesc_t::automoveangleindex.
	 */
	MT_AZR = 0x4000,
	/** */
	MT_TYPES = 0x7FFF,
	/** Controller that wraps shortest distance */
	MT_RLOOP = 0x8000,
} motionType_t;


/**
 * Tipo flotante de cuatro bytes, tal y como se almacena en el fichero.
 * Se utiliza para evitar posibles problemas en el cambio del tama�o de
 * la palabra de la arquitectura. No obstante, no se vigila el orden de
 * los bytes, por lo que el sistema debe ser little-endian.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef float MDLfloat;

/**
 * Tipo entero de cuatro bytes, tal y como se almacena en el fichero.
 * Se utiliza para evitar posibles problemas en el cambio del tama�o de
 * la palabra de la arquitectura. No obstante, no se vigila el orden de
 * los bytes, por lo que el sistema debe ser little-endian.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef int MDLint;

/**
 * Tipo entero sin signo de cuatro bytes, tal y como se almacena en el 
 * fichero. Se utiliza para evitar posibles problemas en el cambio del 
 * tama�o de la palabra de la arquitectura. No obstante, no se vigila 
 * el orden de los bytes, por lo que el sistema debe ser little-endian.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef unsigned int MDLuint;

/**
 * Tipo entero corto de dos bytes, tal y como se almacena en el fichero.
 * Se utiliza para evitar posibles problemas en el cambio del tama�o de
 * la palabra de la arquitectura. No obstante, no se vigila el orden de
 * los bytes, por lo que el sistema debe ser little-endian.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef short MDLshort;

/**
 * Tipo entero corto sin signo de dos bytes, tal y como se almacena en
 * el fichero. Se utiliza para evitar posibles problemas en el cambio
 * del tama�o de la palabra de la arquitectura. No obstante, no se 
 * vigila el orden de los bytes, por lo que el sistema debe ser
 * little-endian.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef unsigned short MDLushort;

/**
 * Tipo caracter (byte), tal y como se almacena en el fichero. Se
 * utiliza para evitar posibles problemas en el cambio del tama�o de
 * la palabra.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef char MDLchar;

/**
 * Tipo caracter (byte) sin signo, tal y como se almacena en el fichero. Se
 * utiliza para evitar posibles problemas en el cambio del tama�o de
 * la palabra.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef unsigned char MDLuchar;

/**
 * Tipo de un vector de tres flotantes. Se utiliza en el fichero para guardar
 * coordenadas tridimensionales.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef MDLfloat vec3_t[3];

/**
 * Tipo que representa un puntero a una posici�n del fichero. Es un
 * entero de cuatro bytes. Aunque hace bastante m�s inc�modo el uso de
 * las estructuras que utilizan este tipo, los valores leidos se
 * mantienen en los campos de este tipo, en lugar de modificarlos (una
 * vez le�do el fichero) para que guarde directamente el puntero a
 * la posici�n de memoria. Se ha decidido esto para evitar problemas
 * mayores si las clases se portan a una arquitectura de 64 bits.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef MDLint ptr_t;

/**
 * Informaci�n de la cabecera del fichero.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {

	/**
	 * Identificador del fichero. Debe ser IDST o IDSQ. Las pruebas
	 * realizadas indican que es IDST en los ficheros con geometr�a o
	 * texturas, e IDSQ en los que s�lo contienen animaciones, aunque
	 * no se puede inducir que eso siempre sea as�.
	 * @see @ref secuenciasYGrupos
	 */
	MDLint id;

	/**
	 * Versi�n del formato. Deber�a ser 10 (supongo que o superior).
	 */
	MDLint version;

	/**
	 * Nombre del fichero. Puede incluir la ruta, a veces con '/' y otras
	 * con '\'. En cualquier caso, la ruta no es muy fiable.
	 */
	MDLchar name[64];

	/**
	 * Tama�o del fichero en bytes, incluyendo la cabecera.
	 */
	MDLuint length;

	/**
	 * Posici�n ideal del ojo del personaje. En teor�a lo usa el m�dulo de IA
	 * para calcular lo que el car�cter puede ver en cada momento.
	 */
	vec3_t eyeposition;

	/**
	 * "Ideal movement hull size"
	 */
	vec3_t min;

	/**
	 * "Ideal movement hull size"
	 */
	vec3_t max;

	/**
	 * Bounding Box. Te�ricamente, este campo deber�a contener la posici�n
	 * del v�rtice inferior del bounding box alineado con los ejes del modelo
	 * en su posici�n por defecto. La realidad es que, en los modelos
	 * probados, su valor es cero.<br>
	 * Te�ricamente la esquina superior del bounding box es header_t::bbmax.
	 */
	vec3_t bbmin;

	/**
	 * Bounding Box. Te�ricamente, este campo deber�a contener la posici�n
	 * del v�rtice superior del bounding box alineado con los ejes del modelo
	 * en su posici�n por defecto. La realidad es que, en los modelos
	 * probados, su valor es cero.<br>
	 * Te�ricamente la esquina superior del bounding box es header_t::bbmin.
	 */
	vec3_t bbmax;		

	/**
	 * Informaci�n adicional de las caracter�sticas del modelo. Ni idea de
	 * qu� valores puede contener, ni lo que indica.
	 */
	MDLint flags;

	/**
	 * N�mero de huesos (articulaciones) del modelo. No puede ser mayor que
	 * 255, pues el hueso al que est� enlazado cada v�rtice se almacena como
	 * un byte (vea HalfLifeMDL::model_t::vertinfoindex).
	 */
	MDLuint numbones;

	/**
	 * Posici�n del fichero donde empieza la informaci�n de los huesos
	 * (array de bone_t).
	 */
	ptr_t boneindex;

	/**
	 * N�mero de controladores de huesos.
	 */
	MDLuint numbonecontrollers;

	/**
	 * Posici�n del fichero donde empieza la informaci�n de los controladores
	 * (array de bonecontroller_t).
	 */
	ptr_t bonecontrollerindex;

	/**
	 * N�mero de "hit boxes". Son "complex bounding boxes" que no s� para qu� son.
	 */
	MDLuint numhitboxes;

	/**
	 * Posici�n del fichero donde empieza la informaci�n de los hitboxes
	 * (array de bbox_t).
	 */
	ptr_t hitboxindex;
	
	/**
	 * N�mero de secuencias del modelo.
	 * @attention Si el n�mero de grupos de secuencias
	 * (header_t::numseqgroups) es 0, este valor no ser� v�lido y debe
	 * considerarse cero. Si se utiliza el valor de este campo como el
	 * valor real del n�mero de secuencias, el resultado puede ser
	 * impredecible.
	 * @see @ref secuenciasYGrupos
	 */
	MDLuint numseq;

	/**
	 * Posici�n de la informaci�n de las secuencias (array de seqdesc_t).
	 * @see @ref secuenciasYGrupos
	 */
	ptr_t seqindex;

	/**
	 * N�mero de grupos de secuencias. En realidad es un nombre confuso;
	 * deber�a ser el n�mero de grupos de animaciones. Si su valor es 0,
	 * el campo header_t::numseq es inv�lido. Si hay m�s de uno, las
	 * animaciones del grupo <tt>n</tt> estar�n en el fichero
	 * <tt>modelXX.mdl</tt>, suponiendo que el modelo original es
	 * <tt>model.mdl</tt>, y que <tt>XX</tt> es la representaci�n con dos 
	 * d�gitos decimales del n�mero <tt>n</tt>.
	 * @see @ref secuenciasYGrupos
	 */
	MDLuint numseqgroups;

	/**
	 * Posici�n de la informaci�n de los grupos de secuencias. Tambi�n hay una
	 * entrada para el grupo 0 (que representa al fichero actual). Es un array
	 * de seqgroup_t.
	 * @see @ref secuenciasYGrupos
	 */
	ptr_t seqgroupindex;

	/**
	 * N�mero de texturas en el fichero.
	 */
	MDLuint numtextures;

	/**
	 * Posici�n de la informaci�n de las texturas (array de texture_t).
	 */
	ptr_t textureindex;

	/**
	 * Parece ser la posici�n de los p�xeles de la primera textura. Su valor
	 * no sirve para nada, pues es redundante (est� en la informaci�n de la
	 * primera textura.
	 */
	ptr_t texturedataindex;

	/**
	 * N�mero de texturas que hay en cada piel.
	 */
	MDLuint numskinref;

	/**
	 * N�mero de pieles.
	 */
	MDLuint numskinfamilies;

	/**
	 * Posici�n de la informaci�n de cada piel de texturas. Esa informaci�n es
	 * un array de enteros cortos de dos bytes de numskinref*numskinfamilies
	 * elementos.<p>
	 * Cada valor referencia a una textura del modelo. Las mallas no referencian
	 * texturas directamente, sino el �ndice de la piel donde se almacena el
	 * �ndice a la textura. Si las texturas est�n en otro fichero (modelT.mdl),
	 * la informaci�n sobre las pieles tambi�n se saca del modelo de las texturas
	 * en lugar de del MDL principal.
	 */
	ptr_t skinindex;

	/**
	 * N�mero de partes del modelo.
	 */
	MDLuint numbodyparts;

	/**
	 * Posici�n de la informaci�n de las partes del cuerpo (array de bodyparts_t).
	 */
	ptr_t bodypartindex;

	/**
	 * N�mero de "adjuntos" del modelo ("queryable attachable points").
	 */
	MDLuint numattachments;
	
	/**
	 * Posici�n de la informaci�n de los "adjuntos" (array de attachment_t).
	 */
	ptr_t attachmentindex;

	/**
	 * Uso desconocido.
	 */
	MDLuint soundtable;

	/**
	 * Uso desconocido.
	 */
	ptr_t soundindex;

	/**
	 * Uso desconocido.
	 */
	MDLuint soundgroups;

	/**
	 * Uso desconocido.
	 */
	ptr_t soundgroupindex;


	/**
	 * Uso desconocido. "Animation node to animation node transition graph".
	 */
	MDLuint numtransitions;

	/**
	 * Uso desconocido.
	 */
	ptr_t transitionindex;

} header_t;

/**
 * Informaci�n sobre un hueso (articulaci�n) del modelo.<p>
 * La informaci�n b�sica del esqueleto (huesos o articulaciones, y sus
 * relaciones), se almacena en los campos header_t::numbones y
 * header_t::boneindex de la cabecera. El primero de ellos indica el n�mero
 * de huesos que contiene el modelo del fichero, y el segundo el puntero
 * donde empieza la informaci�n, compuesta por un vector de numbones
 * elementos de tipo bone_t.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Nombre del hueso (dado por el dise�ador al crear el modelo).
	 */
	MDLchar name[32];

	/**
	 * Hueso padre del que depende el actual. Es el �ndice dentro del "array" de
	 * huesos. Un -1 indica que el hueso no tiene padre, es decir es el "ra�z" del
	 * modelo.
	 */
	MDLint parent;

	/**
	 * Uso desconocido.
	 */
	MDLint flags;

	/**
	 * �ndice del controlador para el grado de libertad i�simo del hueso.
	 * Los grados de libertad son:
	 *
	 *	- 0: Desplazamiento X.
	 *	- 1: Desplazamiento Y.
	 *	- 2: Desplazamiento Z.
	 *	- 3: Rotaci�n sobre el eje X.
	 *	- 4: Rotaci�n sobre el eje Y.
	 *	- 5: Rotaci�n sobre el eje Z.
	 *
	 * El enumerado HalfLifeMDL::DoF_t (Degree Of Freedom) contiene las
	 * constantes pertinentes para acceder con nombres simb�licos a los
	 * elementos de este vector. Hay m�s informaci�n sobre los controladores
	 * en la documentaci�n de la estructura bonecontroller.
	 */ 
	MDLint bonecontroller[6];

	/**
	 * Valores por defecto de los 6 grados de libertad del hueso.
	 * Los grados de libertad son:
	 *
	 *	- 0: Desplazamiento X.
	 *	- 1: Desplazamiento Y.
	 *	- 2: Desplazamiento Z.
	 *	- 3: Rotaci�n sobre el eje X.
	 *	- 4: Rotaci�n sobre el eje Y.
	 *	- 5: Rotaci�n sobre el eje Z.
	 *
	 * El enumerado HalfLifeMDL::DoF_t (Degree Of Freedom) contiene las
	 * constantes pertinentes para acceder con nombres simb�licos a los
	 * elementos de este vector.<p>
	 * Si un hueso no tiene animaci�n en una secuencia, se usan los valores de
	 * este vector para colocarlo con respecto al hueso padre.
	 */
	MDLfloat value[6];

	/**
	 * Escalado de cada grado de libertad.
	 * Los grados de libertad son:
	 *
	 *	- 0: Desplazamiento X.
	 *	- 1: Desplazamiento Y.
	 *	- 2: Desplazamiento Z.
	 *	- 3: Rotaci�n sobre el eje X.
	 *	- 4: Rotaci�n sobre el eje Y.
	 *	- 5: Rotaci�n sobre el eje Z.
	 *
	 * El enumerado HalfLifeMDL::DoF_t (Degree Of Freedom) contiene las
	 * constantes pertinentes para acceder con nombres simb�licos a los
	 * elementos de este vector.<p>
	 * Este escalado s�lo se usa para los valores obtenidos con las animaciones,
	 * pero no para los valores por defecto (que se supone que est�n ya escalados).
	 */
	MDLfloat scale[6];

} bone_t;

/**
 * Informaci�n de una parte del modelo.<p>
 * El modelo tiene los huesos, y las animaciones sobre esos huesos. Luego
 * tiene las mallas sobre los huesos. En realidad se pueden asociar
 * varias mayas a los mismos huesos, para disponer un modo "barato" de
 * tener diferentes personajes, o varias versiones del mismo (por ejemplo,
 * tener un personaje que puede tener o no casco).<p>
 * Para dar m�s versatilidad a esta posibilidad, en realidad no necesariamente
 * todo el "esqueleto" debe ser cambiado. Es decir, si se quiere tener en
 * un .mdl varios personajes, se pueden reutilizar algunas mallas de un
 * personaje en otro (en el caso del casco, se aprovechar�a todo menos la
 * parte de la cabeza). Para lograrlo, las mallas se agrupan en "partes del
 * cuerpo", y son las diferentes partes del cuerpo las que pueden proporcionar
 * diferentes alternativas (varias posibles mallas). Hay que remarcar que
 * lo de tener varias partes del cuerpo aqu� es completamente diferente a
 * las diferentes partes del cuerpo usadas en Quake III (donde se usaba por
 * motivos de animaci�n). Aqu� las animaciones no cambian (est�n especificadas
 * en los huesos); lo que cambia son las mallas que se obtienen a partir de
 * esos huesos. En condiciones normales s�lo habr� una parte del cuerpo, con
 * un �nico modelo (el del personaje).<p>
 * Un ejemplo donde se usa esto es en el modelo barney.mdl (hay una s�la parte
 * del cuerpo, con dos modelos). Tambi�n en el del jugador, donde hay dos partes
 * la cabeza (con dos modelos, uno normal y otro con casco), y el resto del
 * cuerpo con uno solo.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Nombre de la parte del cuerpo. Ejemplos son "studio" o "head".
	 */
	MDLchar name[64];
	
	/**
	 * N�mero de modelos (recubrimientos) para esta parte del cuerpo.
	 */
	MDLuint nummodels;

	/**
	 * "Base num�rica" usada para condensar en un s�lo entero el n�mero del
	 * modelo seleccionado en cada parte del cuerpo del modelo.<p>
	 * C�mo se calcula exactamente no lo s�... el caso es que utilizando
	 * la base se puede obtener del entero qu� modelo est� seleccionado en la
	 * parte del cuerpo. Para eso se divide el entero por esta base, y con el
	 * resultado se hace  el m�dulo con el n�mero de modelos del grupo.
	 */
	MDLint	base;

	/**
	 * Posici�n en el fichero donde est� la informaci�n de los modelos
	 * (recubrimientos) de esta parte del modelo. Es un array de model_t.
	 */
	ptr_t modelindex;

} bodyparts_t;

/**
 * Informaci�n sobre el modelo (recubrimiento) de una parte del cuerpo. Cada
 * modelo almacena los v�rtices. Adem�s, cada modelo puede tener asociada
 * m�s de una malla, todas ellas compartiendo los mismos v�rtices.<p>
 * Cada v�rtice estar� asociado a un hueso (y s�lo a uno). Un tri�ngulo
 * de una malla puede estar compuesto por v�rtices asociados a diferentes
 * huesos (sino, quedar�an espacios entre los tri�ngulos asociados a cada
 * hueso).
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Nombre del modelo.
	 */
	MDLchar name[64];

	/**
	 * Tipo del modelo. Sus posibles valores los desconocemos.
	 */
	MDLint type;

	/**
	 * Uso desconocido.
	 */
	MDLfloat boundingradius;

	/**
	 * N�mero de mallas del modelo.
	 */
	MDLuint nummesh;

	/**
	 * Posici�n en el fichero donde est� la informaci�n de las mallas
	 * del modelo. Es un array de mesh_t.
	 */
	ptr_t meshindex;

	/**
	 * N�mero de v�rtices del modelo.
	 */
	MDLuint numverts;

	/**
	 * Posici�n en el fichero donde est� la informaci�n de a qu� hueso
	 * pertenece cada v�rtice. Es un vector de bytes, cada uno con el �ndice
	 * del hueso del que depende el v�rtice i�simo del modelo.
	 */
	ptr_t vertinfoindex;

	/**
	 * Posici�n en el fichero donde est�n las coordenadas de cada v�rtice.
	 * Es un vector de vec3_t con la posici�n de cada v�rtice en funci�n
	 * de su articulaci�n de referencia. Como lo que se anima en el modelo
	 * son los huesos, los v�rtices no cambian nunca, lo que cambia es
	 * la orientaci�n de las articulaciones.
	 */
	ptr_t vertindex;

	/**
	 * N�mero de normales (�nicas) del modelo. <p>
	 * Las mallas pue forman el modelo referencian a las normales del
	 * modelo para formar sus v�rtices. Este campo especifica la
	 * longitud del array apuntado por normindex.
	 */
	MDLuint numnorms;

	/**
	 * Posici�n en el fichero donde est� la informaci�n de respecto a qu�
	 * hueso est�n dadas las coordenadas de cada normal. Es un vector
	 * de bytes, cada uno con el �ndice del hueso del que depende la
	 * normal i�sima del modelo.
	 */
	ptr_t norminfoindex;

	/**
	 * Posici�n en el fichero donde est�n las coordenadas de cada normal.
	 * Es un vector de vec3_t con la orientaci�n de cada normal en funci�n
	 * de su articulaci�n de referencia. Como lo que se anima en el modelo
	 * son los huesos, las normales no cambian nunca, lo que cambia es la
	 * orientaci�n de las articulaciones.
	 */
	ptr_t normindex;

	/**
	 * N�mero de grupos de deformaci�n. No s� qu� es exactamente.
	 */
	MDLuint numgroups;

	/**
	 * Informaci�n de los grupos de deformaci�n.
	 */
	ptr_t groupindex;

} model_t;

/**
 * Informaci�n de una malla. Cada malla est� asociada a un modelo o 
 * recubrimiento (model_t), cada uno de ellos perteneciendo a una
 * parte del modelo (bodyparts_t).
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {

	/**
	 * Supongo que ser� el n�mero de tri�ngulos de la malla. Su valor no se
	 * utiliza para dibujar.
	 */
	MDLuint numtris;

	/**
	 * Posici�n del fichero donde empieza el vector con la informaci�n de los
	 * tri�ngulos de la malla. El vector es de enteros cortos (de 16 bits).
	 * Lo que contiene es la informaci�n de los tri�ngulos de forma 
	 * encapsulada, por grupos. Cada grupo tiene un primer valor que indica 
	 * el n�mero de v�rtices que posee. Si el valor es 0, es el �ltimo
	 * grupo del vector, y no hay que seguir leyendo en el array.<p>
	 * Si el valor es positivo, los v�rtices que aparecen a continuaci�n
	 * construyen un grupo de tri�ngulos. Salvo para los dos primeros
	 * v�rtices, cada uno de los dem�s crea un nuevo tri�ngulo. Para valores
	 * de n impares, el tri�ngulo n-�simo est� formado por los v�rtices n, n+1
	 * y n+2. Para n pares, el tri�ngulo n-�simo est� formado por los
	 * v�rtices n+1, n y n+2. Se corresponde con el formato esperado por
	 * glBegin(GL_TRIANGLE_STRIP) de OpenGL.<p>
	 * Si el valor del primer elemento del grupo es negativo, el n�mero de
	 * v�rtices es el valor absoluto, y el modo de construir el grupo de
	 * tri�ngulos es diferente. Igual que antes, salvo para los dos primeros
	 * v�rtices, cada uno de los dem�s crea un nuevo tri�ngulo. El tri�ngulo
	 * n-�simo es definido con los v�rtices 1, n+1 y n+2. Se corresponde
	 * con el formato esperado por glBegin(GL_TRIANGLE_FAN) de OpenGL.<p>
	 * Despu�s del valor inicial, vienen los v�rtices. Cada uno utiliza
	 * cuatro posiciones del vector. La primera es el �ndice al v�rtice en el
	 * vector de v�rtices del modelo al que pertenece la malla (model_t).
	 * El segundo es el �ndice a la normal en el vector de normales del
	 * modelo. El tercero es la coordenada u de textura, y el cuarto la
	 * coordenada v. Como en realidad los campos son enteros cortos, los
	 * valores reales de las coordenadas de textura se calculan dividiendo
	 * los valores le�dos por el ancho y el alto de la textura que se
	 * aplica sobre la malla.<br>
	 * La divisi�n de las coordenadas u y v anterior supone que todas las
	 * texturas de mismo �ndice dentro de una piel deben tener las mismas
	 * dimensiones (ver MDLSkins).
	 * <p>
	 * Algunas texturas son "cromadas". En ese caso las corodenadas de
	 * textura deber�an calcularse "al vuelo" en funci�n de la normal del
	 * v�rtice y el punto de vista, y los elementos con las coordenadas
	 * no contendr�n informaci�n �til.
	 * <p>
	 * La cara frontal de los tri�ngulos formados por los v�rtices definidos
	 * aqu� es aquella que deja a los v�rtices ordenados en el sentido de la
	 * agujas del reloj.
	 * @warning El valor de este campo puede repetirse en m�s de una malla.
	 * Eso ocurre por ejemplo en barney.mdl. Tiene dos partes: el cuerpo y la
	 * pistola. �sta �ltima tiene tres versiones diferentes, una usada
	 * para que la pistola aparezca en la cartuchera, otra para que aparezca
	 * en la mano de Barney, y la �ltima para que no aparezca. En cada caso
	 * los v�rtices de los tri�ngulos y de qu� hueso dependen ser�n diferentes
	 * pero la conexi�n de esos v�rtices siempre ser� igual. Como es
	 * precisamente esa informaci�n la que se apunta desde este campo, las
	 * tres versiones de la pistola apuntan a la misma informaci�n de
	 * conexi�n.
	 */
	ptr_t triindex;

	/**
	 * �ndice a la textura usada. No es directamente el �ndice a la textura,
	 * sino a la textura de la piel. El �ndice de la textura se obtiene
	 * consultando la posici�n indicada por este campo en el array de �ndices
	 * a texturas de la piel actual.
	 */
	MDLuint skinref;

	/**
	 * N�mero de normales de la malla. El modelo (model_t) al que pertence
	 * esta malla tiene su propio n�mero de normales (model_t::numnorms) y
	 * una lista de normales (model_t::norminfoindex). Supuestamente, este
	 * campo indica cuantas de las normales del modelo son usadas por esta
	 * malla. <p>
	 * En realidad esto no est� comprobado. Lo que est� claro es que si un
	 * modelo tiene, pongamos, 10 mallas, la suma de este campo
	 * (mesh_t::numnorms) de las 10 mallas da el n�mero de normales de
	 * del modelo (model_t::numnorms). Por lo tanto, se deduce que la
	 * primera malla s�lo usar� las mesh_t::numnorms primeras normales,
	 * la segunda las siguientes, y as� sucesivamente. Esto, sin embargo,
	 * no lo hemos comprobado.

	 * N�mero de normales de la malla. El contenido de este campo es pura
	 * especulaci�n. Un mesh_t est� contenido en un model_t, que puede
	 * contener simult�neamente varias mallas (mesh_t). Un model_t tiene
	 * una lista de normales, y, por lo tanto, un campo con el n�mero de
	 * normales que contiene, al igual que ocurre con los v�rtices. Las
	 * mallas (mesh_t) referencian a esa lista de v�rtices y de normales
	 * para definir los tri�ngulos que la forman (ver mesh_t::triindex).
	 * <br>
	 * El caso de los v�rtices es el normal. Las mallas referencian
	 * directamente un �ndice de la lista de v�rtices. Sin embargo, con las
	 * normales, se han complicado un poco m�s. Da la impresi�n de que las
	 * normales de la lista de model_t <b>no</b> se comparten entre mallas
	 * como ocurre con los v�rtices. Es decir, si un model_t almacena,
	 * pongamos, 100 normales y tiene 4 mallas, las 20 primeras normales
	 * son referenciadas �nicamente por la primera malla, las, digamos
	 * 30 siguientes por la segunda, las 25 siguientes por la tercera y
	 * las restantes por la cuarta. <br>
	 * Esta divisi�n es bastante artificial. Parece que la hicieron para
	 * facilitar el trabajo al m�dulo que pinta un modelo, con el trato
	 * de las texturas cromadas. <p>
	 * Como se ha dicho, todo lo anterior no est� comprobado, pero se deduce a
	 * partir del c�digo usado como base para construir esta librer�a, y del
	 * an�lisis realizado sobre todos los modelos de juego original, en los
	 * que se cumple que la suma del campo numnorms de todas las mallas
	 * de un submodelo da el mismo valor que el campo model_t::numnorms
	 */
	MDLuint numnorms;

	/**
	 * Posici�n en el fichero del array de vec3_t con las normales. En
	 * realidad este campo siempre est� a cero en todos los modelos del
	 * juego original. Las normales de una malla se guardan en la informaci�n
	 * del modelo (model_t) al que pertenecen, por lo que este campo
	 * no parece tener sentido.
	 */
	ptr_t normindex;

} mesh_t;

/**
 * Informaci�n generica de una textura.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Nombre de la textura. Normalmente es el nombre del fichero del que
	 * procede.
	 */
	MDLchar name[64];

	/**
	 * Caracter�sticas de la textura. No estoy seguro de qu� esto exactamente,
	 * aunque sospecho que lo que indica es el modo en el que debe dibujarse
	 * la malla que utilice esta textura. Los posibles valores son los
	 * especificados en el enumerado HalfLifeMDL::textureFlags_t, que pueden
	 * usarse simult�neamente uniendolos con ORs a nivel de bits.
	 */
	MDLint flags;

	/**
	 * N�mero de p�xeles de ancho.
	 */
	MDLuint width;

	/**
	 * N�mero de p�xeles de alto.
	 */
	MDLuint height;

	/**
	 * Posici�n en el fichero donde est�n los p�xeles de la imagen. Primero
	 * van width*height bytes, cada uno para un pixel. Despu�s va la paleta
	 * en RGB. Los p�xeles van de izquierda a derecha, y de abajo a arriba.
	 */
	ptr_t index;

} texture_t;

/**
 * Informaci�n sobre un controlador de hueso. Aunque las secuencias del
 * modelo especifican los desplazamientos y rotaciones que los huesos deben
 * sufrir para ejecutar una animaci�n, es posible retocar ligeramente
 * alguno de los grados de libertad (HalfLifeMDL::DoF_t) de alguno de los
 * huesos para para adaptarlo a los requerimientos del programa. Esto se 
 * realiza mediante controladores de huesos, que se asocian a un hueso y a un
 * grado de libertad, y modifican ese grado de libertad para ese hueso,
 * sumando o restando una cantidad al valor especificado para ese
 * grado de libertad por la animaci�n que se est� ejecutando.<br>
 * Quiz� el ejemplo m�s sencillo sea la asociaci�n de un controlador
 * a un hueso cuya rotaci�n crea el efecto de abrir y cerrar la boca.
 * En todas las animaciones ese hueso estar� en la posici�n que supone
 * la boca cerrada, pero tendr� asociado un controlador. Durante la
 * ejecuci�n de cualquier secuencia, el programa puede modificar el
 * controlador para crear el efecto de que el personaje est� hablando.
 * Un segundo ejemplo es asociar un controlador a la rotaci�n del hueso
 * del cuello, para que el programa pueda hacer al personaje girar la
 * cabeza mientras corre, sin que el animador tenga que crear una
 * animaci�n de correr por cada posible direcci�n de la mirada.<p>
 * Naturalmente, para que esto tenga sentido, debe existir coherencia
 * entre el modelo y el c�digo que lo controla. Los modelos de Half Life
 * no obligan a la existencia de controladores determinados, por lo que
 * un c�digo que suponga que existe un controlador para la boca tendr�
 * que manejar un modelo que lo tenga.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {

	/**
	 * Hueso al que est� asociado el controlador. El hueso 0 es el
	 * primer hueso del array de huesos del modelo. Un valor de
	 * -1 indica que el controlador no tiene asociado hueso.
	 * @todo Asegurarse de esto.
	 */
	MDLint bone;	// -1 == 0

	/**
	 * Grado de libertad al que est� asociado el hueso. Toma los valores
	 * constantes definidos por el enumerado HalfLifeMDL::DoF_t. El campo 
	 * es de tipo MDLint y no HalfLifeMDL::DoF_t para asegurar que ocupa 4
	 * bytes (igual que en el fichero).
	 */
	MDLint type;	// X, Y, Z, XR, YR, ZR, M
								// STUDIO_XR, STUDIO_YR, STUDIO_ZR
	/** @todo Rellenar
	 */
	MDLfloat start;

	/** @todo Rellenar
	 */
	MDLfloat end;

	/** @todo Rellenar
	 */
	MDLint rest;	// byte index value at rest

	/** @todo Rellenar
	 */
	MDLint index;	// 0-3 user set controller, 4 mouth

} bonecontroller_t;

// intersection boxes
/**
 * Bounding box que almacena una caja de intersecci�n.
 * @todo no s� para qu� se usa esto. �Averiguarlo y comentarlo m�s?
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {
	/** @todo */
	MDLint bone;
	/** @todo */
	MDLint group;			// intersection group
	/** @todo */
	vec3_t bbmin;		// bounding box
	/** @todo */
	vec3_t bbmax;		
} bbox_t;

/**
 * Tipo de datos que hace las veces de <EM>tag</EM>.
 * @see HalfLifeMDL::seqgroup_t::cache.
 */
typedef void *cache_user_t;

/**
 * Informaci�n sobre un grupo de secuencias.
 * @see @ref secuenciasYGrupos
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Nombre del grupo de secuencias. Generalmente, para el grupo 0
	 * (representando las animaciones del fichero actual) se obtiene el
	 * nombre <tt>default</tt>. Para los grupos de los ficheros externos
	 * este campo suele estar vac�o.
	 */
	MDLchar label[32];

	/**
	 * Nombre del fichero donde est�n las animaciones. Generalmente para
	 * el grupo 0 (representando las animaciones del gurpo actual) se
	 * obtiene la cadena vac�a. Para los grupos de los ficheros externos
	 * este campo suele contener el nombre <tt>modelXX.mdl</tt>, si el
	 * fichero de la geometr�a es <tt>model.mdl</tt> y XX es la
	 * representaci�n con dos d�gitos decimales del n�mero del grupo.<br>
	 * Sin embargo, normalmente no se utilizar� directamente este nombre
	 * para buscar el fichero, pues suele contener informaci�n sobre la
	 * ruta que no siempre es v�lida.
	 */
	MDLchar name[64];

	/**
	 * Campo que hace las veces de <EM>tag</EM> para que la aplicaci�n
	 * guarde el valor que desee. En realidad el formato est� muy ligado al
	 * cargador, de modo que el cargador original manten�a directamente todo
	 * el fichero en memoria sin convertirlo a otra estructura de datos. Como
	 * se supone que los grupos de secuencia est�n pensados para la carga
	 * bajo demanda, este campo permite al cargador almacenar cualquier
	 * valor con informaci�n personalizada sobre el estado de la carga de
	 * este grupo. Es un campo de 4 bytes.<p>
	 * Esta librer�a no lo utiliza.
	 */
	cache_user_t cache;

	/**
	 * Truco para facilitar la obtenci�n de la posici�n de las animaciones
	 * del grupo de secuencia 0 dentro del fichero actual. Este campo no
	 * se utiliza si el grupo no es el 0. Si lo es, la posici�n de los
	 * datos de las animaciones de todas las secuencias que lo referencien
	 * ser� la suma de este campo data, y el campo seqdesc_t::animindex de
	 * las secuencias.<p>
	 * De todas formas, el an�lisis de todos los ficheros de HalfLife
	 * muestra que este campo est� en realidad siempre a 0.
	 * @see HalfLifeMDL::seqdesc_t::animindex.
	 */
	MDLint data;

} seqgroup_t;

/**
 * Contenido de la informaci�n sobre una secuencia del modelo.
 * @see @ref secuenciasYGrupos
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {

	/**
	 * Nombre de la secuencia. Se establece en el fichero QC usado para
	 * empaquetar el MDL a partir de los SMD.
	 */
	MDLchar label[32];

	/**
	 * Fotogramas por segundo de la reproducci�n.
	 */
	MDLfloat fps;

	/**
	 * Informaci�n adicional de la secuencia. Si es 0 no hay nada nuevo. Si es
	 * 1, la secuencia es un ciclo (por ejemplo el de andar).
	 * @todo Quiz� se podr�a crear una constante aqu�... pero en realidad
	 * puede usarse como booleano... :-D (STUDIO_LOOPING)
	 */
	MDLint flags;

	/**
	 * Identificador de la secuencia. Es el entero asociado a cada 
	 * identificador de las animaciones est�ndar que se puede
	 * establecer en el fichero QC (ACT_ARM, ACT_RUN...). El enumerado
	 * HalfLifeMDL::actionTags_t especifica los posibles valores.
	 */
	MDLint activity;

	/**
	 * "Peso" de esta secuencia para la actividad seqdesc_t::activity.
	 * Un mismo modelo puede tener varias secuencias para la misma actividad,
	 * y la parte del motor gr�fico que se encarga de controlar al modelo
	 * puede establecer aleatoriamente una cualquiera cuando desde fuera
	 * se le pide que establezca una secuencia proprcionando el identificador
	 * de secuencia est�ndar. Esa elecci�n aleatoria se puede ajustar dando
	 * un peso a cada versi�n de la secuencia est�ndar, para dar m�s
	 * probabilidad a unas que a otras. Ese peso es almacenado en este campo,
	 * que no tendr� sentido si la secuencia no es est�ndar. Si s�lo hay
	 * una secuencia para esta actividad, su valor tampoco tendr� mucho
	 * sentido, pero ser� diferente de 0.
	 */
	MDLint actweight;

	/**
	 * N�mero de eventos de la secuencia.
	 */
	MDLint numevents;

	/**
	 * Posici�n en el fichero donde empieza la informaci�n de los eventos
	 * (array de event_t).
	 */
	ptr_t eventindex;

	/**
	 * N�mero de fotogramas de la secuencia. Si la secuencia es un ciclo,
	 * el �ltimo fotograma de su (o sus) animacion(es) debe ser exactamente
	 * igual que el primero. Este contador incluye tambi�n ese fotograma
	 * repetido.
	 */
	MDLint numframes;

	/**
	 * "Number of foot pivots". No tengo ni idea de qu� es esto. En cualquier
	 * caso ning�n modelo contenido en el juego original parece hacer uso de
	 * esto (todos los MDL lo tienen a 0). Tampoco he encontrado informaci�n
	 * sobre c�mo establecer este campo desde el fichero QC. Por tanto,
	 * sospecho que este campo es un claro ejemplo de "Buenas ideas para
	 * con un poco de suerte utilizar en alguna versi�n futura".
	 */
	MDLint numpivots;

	/**
	 * Posici�n en el fichero donde empieza la informaci�n de los pivotes.
	 * En realidad el contenido de este valor no es seguro. De todas formas
	 * si depende de numpivots, su valor seguramente no se use (pues en
	 * todos los modelos consultados, numpivots est� a 0).
	 */
	MDLint pivotindex;

	/**
	 * Tipo de movimiento del personaje respecto al mundo en la secuencia
	 * actual. Los posibles valores se especifican usando las constantes
	 * definidas por el tipo HalfLifeMDL::motionType_t.
	 */
	MDLint motiontype;

	/**
	 * Hueso principal que hay que desplazar si hay movimiento. En realidad
	 * el contenido de este valor no es seguro... de hecho todos los modelos
	 * del juego original parecen tener aqu� un 0, referenciando al hueso 0
	 * que siempre es el hueso ra�z o padre del que cuelgan todos los dem�s.
	 */
	MDLint motionbone;

	/**
	 * Desplazamiento del hueso padre (�� seqdesc_t::motionbone ??) entre el
	 * primer fotograma y el �ltimo de la secuencia. Cada coordenada del
	 * vector s�lo es v�lida si seqdesc_t::motiontype tiene activo el bit
	 * especificado por HalfLifeMDL::MT_LX, HalfLifeMDL::MT_LY, o
	 * HalfLifeMDL::MT_LZ. Este desplazamiento debe ser interpolado a lo largo
	 * de toda la secuencia. Un t�pico ejemplo es el desplazamiento causado
	 * por un ciclo de andar. Para calcular el avance en el eje E en un lapso
	 * de tiempo de epsilon segundos, habr� que utilizar la formula:
	 * \f[
	 * avance\ en\ eje\ E = \frac{linearmovement[E] * fps * epsilon}
	 *		                      {numero\ de\ fotogramas\ de\ la\ secuencia}
	 * \f]
	 */
	vec3_t linearmovement;

	/**
	 * No utilizada. En todos los modelos del juego original est� a 0. Por el
	 * nombre, se puede deducir que es la posici�n del fichero donde est� la
	 * informaci�n del desplazamiento del modelo en el mundo cuando se utiliza
	 * el tipo de movimiento autom�tico (HalfLifeMDL::MT_AX, 
	 * HalfLifeMDL::MT_AY, o HalfLifeMDL::MT_AZ), aunque esto no es m�s que
	 * pura especulaci�n. En realidad, esos tipos de movimiento no se usan en
	 * ning�n modelo, y si se usan, el empaquetador de MDL de las SDK de Half
	 * Life (StudioMDL) no parece inicializar este campo tampoco.
	 */
	MDLint automoveposindex;

	/**
	 * No utilizada. En todos los modelos del juego original est� a 0. Por el
	 * nombre, se puede deducir que es la posici�n del fichero donde est� la
	 * informaci�n de rotaci�n del modelo en el mundo cuando se utiliza
	 * el tipo de movimiento autom�tico (HalfLifeMDL::MT_AXR,
	 * HalfLifeMDL::MT_AYR, o HalfLifeMDL::MT_AZR), aunque esto no es m�s que
	 * pura especulaci�n. En realidad, esos tipos de movimiento no se usan en
	 * ning�n modelo, y si se usan, el empaquetador de MDL de las SDK de Half
	 * Life (StudioMDL) no parece inicializar este campo tampoco.
	 */
	MDLint automoveangleindex;

	/**
	 * Coordenada m�nima del bounding box de la secuencia completa.Es un
	 * bounding box alineado con los ejes, donde el sistema de coordenadas
	 * utilizado es el utilizado para colocar la articulaci�n ra�z del
	 * modelo. Es decir, las coordenadas se dan en el mismo sistemas de
	 * coordenadas que el hueso padre. Ese sistema ser� establecido por el
	 * motor seg�n d�nde quiera colocar al personaje.<br>
	 * Si durante la secuencia de la animaci�n el personaje es desplazado
	 * autom�ticamente por el motor (por ejemplo, el movimiento lineal en
	 * los ciclos de andar), las coordenadas del bounding box tambi�n
	 * deben "desplazarse" de la misma forma (pues estas coordenadas est�n
	 * en el sistema del hueso ra�z que es el que se modifica para desplazar
	 * al personaje).<p>
	 * Si la animaci�n no obliga a un desplazamiento del hueso ra�z por parte
	 * del motor, pero �ste se desplaza (por ejemplo en una animaci�n en la
	 * que el personaje muere y cae a cierta distancia de la posici�n inicial)
	 * el bounding box es incluye a todas esas posiciones. Es decir, todos los
	 * estados por los que pasa el modelo durante la secuencia completa entran
	 * dentro del bounding box.
	 * @image html
	 * GafotasSubiendo.gif "Secuencia con desplazamiento de hueso principal"
	 * @warning Por alguna raz�n, el empaquetador de MDL no siempre construye
	 * un bounding box completamente correcto. En ocasiones, ciertas partes 
	 * del modelo salen fuera del bounding box de forma espor�dica, como la
	 * punta de los dedos. Esto no sucede a menudo y, en cualquier caso, la
	 * desviaci�n producida es muy leve.
	 * @image html ErrorBoundingBoxes.gif "Error de los bounding boxes"
	 */
	vec3_t bbmin;

	/**
	 * Coordenada m�xima del bounding box de la secuencia completa. Para una
	 * mayor explicaci�n, vease seqdesc_t::bbmin.
	 * @see seqdesc_t::bbmin
	 */
	vec3_t bbmax;

	/**
	 * N�mero de animaciones a partir de las que se crea la secuencia actual.
	 * Una secuencia puede formarse por varias animaciones sobre los huesos,
	 * y el motor mezclar� esas animaciones dando un peso a cada una. El campo
	 * indica cuantas animaciones se usan para esta secuencia.
	 * @see @ref secuenciasYGrupos
	 */
	MDLint numblends;

	/**
	 * Indica la posici�n donde est� la informaci�n de la animaci�n o
	 * animaciones de la secuencia actual (puntero a un array de
	 * HalfLifeMDL::anim_t). Las animaciones de una secuencia
	 * pueden estar en el mismo fichero que la geometr�a, o en otro. Si
	 * el campo seqdesc_t::seqgroup de esta misma estructura guarda un 0,
	 * la animaci�n est� en el mismo fichero que la geometr�a, y la posici�n
	 * real de las animaciones de la secuencia dentro del fichero se obtiene
	 * sumando este campo animindex al campo seqgroup_t::data de la 
	 * informaci�n de la secuencia.<p>
	 * Si esta secuencia no est� en el grupo 0, la animaci�n o animaciones
	 * estar�n en un fichero externo. La posici�n donde est� la animaci�n en
	 * ese fichero se indica en este campo (y el campo data de la informaci�n
	 * del grupo de secuencia no se utiliza).
	 * <p>
	 * En cualquier caso, una vez conseguida la posici�n real del comienzo del
	 * array de anim_t a partir de este campo, ese array es bidimensional:
	 * <p>
	 * <center><code>anim[blend][hueso]</code></center>
	 * <p>
	 * Cada elemento contiene la animaci�n
	 * del hueso del segundo �ndice para la animaci�n "blend" de la secuencia.
	 * Esto �ltimo se debe a que cada secuencia puede formarse por la mezcla
	 * de varias animaciones sobre los huesos, dando un peso a cada una. En
	 * este array se almacenan todas esas animaciones, de ah� la existencia
	 * del �ndice "blend". El tama�o de la dimensi�n <code>blend</code> viene
	 * dado por el campo seqdesc_t::numblends de esta misma estructura. El
	 * tama�o de la dimensi�n <code>hueso</code> viene dado por el n�mero
	 * de huesos del modelo al que pertenece la animaci�n.
	 * 
	 * @see @ref secuenciasYGrupos
	 * @see HalfLifeMDL::seqgroup_t::data
	 * @see HalfLifeMDL::anim_t
	 */
	MDLint animindex;

	/** @todo */
	MDLint blendtype[2];	// X, Y, Z, XR, YR, ZR
	/** @todo */
	MDLfloat blendstart[2];	// starting value
	/** @todo */
	MDLfloat blendend[2];	// ending value

	/**
	 * Uso desconocido. Todos los modelos del juego Half Life tienen este
	 * campo a 0.
	 */
	MDLint blendparent;

	/**
	 * N�mero de grupo donde est� la animaci�n (o animaciones) de la
	 * secuencia. Si el valor es 0, la animaci�n est� en el fichero actual.
	 * Si no, deber�a estar en el fichero cuyo nombre est� en la informaci�n
	 * del grupo de secuencias seqgroup del fichero. En la pr�ctica se
	 * obtiene del fichero <code>modelXX.mdl</code>, si <code>model.mdl</code>
	 * es el fichero actual y <code>XX</code> es el n�mero de grupo (a partir
	 * de 01).
	 * @see @ref secuenciasYGrupos
	 */
	MDLint seqgroup;

	/**
	 * Uso desconocido. En todos los modelos de Half Life este campo
	 * est� a 0. Quiz� tenga relaci�n con seqdesc_t::nextseq para hacer
	 * transiciones de secuencias con mezclado de forma autom�tica.
	 * <p>
	 * La "documentaci�n" original del campo era "transition node at entry".
	 */
	MDLint entrynode;

	/**
	 * Uso desconocido. En todos los modelos de Half Life este campo
	 * est� a 0. Quiz� tenga relaci�n con seqdesc_t::nextseq para hacer
	 * transiciones de secuencias con mezclado de forma autom�tica.
	 * <p>
	 * La "documentaci�n" original del campo era "transition node at exit".
	 */
	MDLint exitnode;

	/**
	 * Uso desconocido. En todos los modelos de Half Life este campo
	 * est� a 0. Quiz� tenga relaci�n con seqdesc_t::nextseq para hacer
	 * transiciones de secuencias con mezclado de forma autom�tica.
	 * <p>
	 * La "documentaci�n" original del campo era "transition rules".
	 */
	MDLint nodeflags;

	/**
	 * Uso desconocido. Por el nombre del campo, se puede intuir que almacena
	 * el n�mero de la secuencia que debe reproducirse cuando la actual
	 * termine. Por ejemplo, para la animaci�n de "morir", la secuencia
	 * siguiente podr�a ser "tendido en el suelo", de modo que el propio
	 * motor podria actualizar la secuencia actual del modelo para ahorrar
	 * trabajo a su controlador.<p>
	 * En el an�lisis realizado a todos los modelos originales del juego Half
	 * Life, este campo siempre est� a cero. Tampoco he encontrado informaci�n
	 * sobre c�mo establecer este campo desde el fichero QC. Por tanto,
	 * sospecho que este campo es un claro ejemplo de "Buenas ideas para
	 * con un poco de suerte utilizar en alguna versi�n futura".
	 * <p>
	 * La "documentaci�n" original del campo era "auto advancing sequences".
	 * @todo Si se hace una p�gina sobre c�mo se empaqueta un MDL, 
	 * referenciarla aqu�.
	 */
	MDLint nextseq;

} seqdesc_t;

/**
 * Informaci�n de eventos en las secuencias. Cada secuencia puede tener
 * eventos asociados, que suceden en fotogramas concretos (establecidos
 * en el fichero .qc). El evento que salta se especifica con un entero.
 * Se supone que el c�digo asociado al modelo sabr� qu� es ese evento.
 * Algunos son estandar (por ejemplo, momento en el que el personaje
 * muere en la animaci�n de morir), y otros dependen de la entidad.
 * Algunos pueden tener par�metros (por ejemplo, la ruta del .wav a
 * reproducir).
 * Las estructuras de tipo seqdesc_t contienen un "puntero" a
 * un array de esta estructura.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {

	/**
	 * Fotograma en el que sucede el evento en la secuencia
	 * (seqdesc_t) a la que est� asociada el evento.
	 */
	MDLuint frame;

	/**
	 * C�digo del evento. Algunos son "est�ndar". Por ejemplo el valor
	 * 1000 indica el fotograma en el que el personaje muere. La lista
	 * est� en el docuento "Modeling for Half-Life.doc" proporcionado
	 * junto con las SDK de Half Life.<br>
	 * A parte, se pueden asignar cualquier valor, con tal de que el c�digo
	 * que luego controla el modelo y recibe el evento sea capaz de
	 * identificar el valor.
	 */
	MDLint event;

	/**
	 * Uso desconocido. Todos los modelos del juego Half Life tienen este
	 * campo a 0.
	 */
	MDLint type;

	/**
	 * Opciones del evento. Por ejemplo, el nombre del fichero wav a
	 * reproducir. El contenido depende del evento, y es el c�digo
	 * asociado al modelo el que le da el sentido a la cadena.
	 */
	MDLchar options[64];

} event_t;


/**
 * Pivotes ("pivots" ��"foot pivots"??). No s� qu� son. Todos los modelos
 * consultados indican que el n�mero de pivotes es 0, y tampoco hay
 * informaci�n sobre c�mo establecerlos desde el QC.
 * <p>
 * Sospecho que este campo es un claro ejemplo de "Buenas ideas para
 * con un poco de suerte utilizar en alguna versi�n futura".
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Uso desconocido.
	 * <p>
	 * El comentario original de este campo era "pivot point".
	 */
	vec3_t org;
	/** Uso desconocido. */
	MDLint start;
	/** Uso desconocido. */
	MDLint end;
} pivot_t;

/**
 * "Puntos de uni�n" del modelo (por ejemplo pistolas).
 * El modelador establece la posici�n de esos puntos (que no se utilizan para dibujar),
 * y el c�digo que controla a ese personaje cuenta con ellos para saber d�nde colocar
 * el arma, o cualquier otra cosa. Puede haber hasta 4 (creo) puntos de estos, para,
 * por ejemplo, distintos tipos de armas. La informaci�n sobre cada uno de estos
 * puntos de uni�n se guarda en esta estructura.
 * @todo Analizar esto m�s para quitar el "creo". Comentar los elementos.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct 
{
	/**
	 * Nombre del punto de uni�n. En todos los ficheros del juego este
	 * campo est� vac�o. En realidad no hay forma de especificar nombre
	 * desde el fichero <code>QC</code>, al menos seg�n la documentaci�n
	 * oficial de las SDK.
	 */
	MDLchar name[32];

	/**
	 * Tipo del punto de uni�n. En todos los ficheros del juego este
	 * campo vale 0. En realidad no hay forma de especificar nombre
	 * desde el fichero <code>QC</code>, al menos seg�n la documentaci�n
	 * oficial de las SDK.
	 */
	MDLint type;

	/**
	 * Hueso del modelo al que est� asociado este punto de uni�n.
	 */
	MDLint bone;

	/**
	 * Desplazamiento del punto de control respecto al hueso del que
	 * depende. Las coordenadas est�n, como no pod�a ser de otra forma
	 * en el sistema de coordenadas del hueso.
	 */
	vec3_t org;

	/**
	 * Uso desconocido. En todos los modelos del juego original estos
	 * vectores tienen sus coordenadas a 0. Adem�s no parece haber forma
	 * de establecer valores desde el fichero <code>QC</code>. Tal vez
	 * est�n metidos para una ampliaci�n futura. El �nico sentido que veo
	 * a esa ampliaci�n es que sea la matriz de rotaci�n. A juzgar por
	 * el c�digo del visor usado de referencia para entender el formato del
	 * fichero, esto tal vez sea eso pero un poco cambiado. Ese visor
	 * dibuja l�neas que van desde org hasta estos tres nuevos v�rtices,
	 * considerando todos en el sistema de coordenadas del hueso. Si
	 * consideramos que esas l�neas dibujar�an el nuevo sistema de referencia
	 * (cosa que no puede comprobarse, en cualquier caso), estos
	 * v�rtices tendr�an que ser modificados (restando org) para obtener
	 * la matriz (o algo por el estilo).
	 */
	vec3_t vectors[3];

} attachment_t;

/**
 * Almacena la informaci�n de una animaci�n sobre un hueso del modelo.
 * @see HalfLifeMDL::seqdesc_t::animindex
 * @see @ref secuenciasYGrupos
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Array con la informaci�n de la animaci�n de cada grado de libertad
	 * del hueso para el que la estructura almacena la animaci�n.
	 * Es un array con seis posiciones, una para cada grupo de libertad.
	 * En lugar de acceder directamente con �ndices n�mericos, se deber�a
	 * utilizar, para mayor claridad, los valores del enumerado
	 * HalfLifeMDL::DoF_t.
	 * <p>
	 * Si para un determinado grado de libertad el valor es 0, el hueso
	 * cuya animaci�n estamos guardando no cambiar� su valor por defecto
	 * para ese grado de libertad durante toda la animaci�n. Por tanto,
	 * durante toda la animaci�n el hueso tendr� en el grado de libertad
	 * el valor que puede consultarse en la informaci�n del hueso
	 * (HalfLifeMDL::bone_t::value).
	 * <p>
	 * Si el valor no es cero, se debe sumar a la posici�n donde comienza
	 * el array el valor le�do para llegar a los datos de la animaci�n
	 * en ese grado de libertad, que ser� un array de
	 * HalfLifeMDL::animvalue_t. Es decir, si tenemos en la variable
	 * <code>anim</code> un puntero a una de estas estructuras anim_t,
	 * estaremos en la siguiente situaci�n:
	 * <br>
	 * <table border="0" cellspacing="1" cellpadding="1" align="center">
	 * <tr>
	 * <td>
	 * <pre>
	 * anim_t* anim;
	 * // [...]
	 * if (anim.offset[dof] == 0) {
	 *	// No hay animaci�n sobre ese grado de libertad (dof).
	 *	// El valor de ese grado de libertad es constante en
	 *	// toda la animaci�n y se obtiene de la informaci�n del
	 *	// hueso cuya animaci�n almacena el puntero anim.
	 * }
	 * else {
	 *	// Si hay animaci�n. Tenemos que obtener el array de
	 *	// animvalue_t:
	 *	animvalue_t* animacion;
	 *	animacion = (animvalue_t*)((char*) anim) + anim.offset[dof];
	 * }
	 * </pre>
	 * </td>
	 * </tr>
	 * </table>
	 * El array obtenido en el segundo caso contiene la informaci�n necesaria
	 * para obtener el valor del grado de libertad para ese hueso en esta
	 * animaci�n, a partir del fotograma actual.
	 * <br>
	 * @see HalfLifeMDL::seqdesc_t::animindex
	 * @see @ref secuenciasYGrupos
	 * @see HalfLifeMDL::animvalue_t
	*/
	MDLushort offset[6];

} anim_t;

// animation frames
/**
 * Un array de estas estructuras almacena, de forma comprimida, una animaci�n
 * de un grado de libertad de un hueso del modelo.
 * <p>
 * La forma c�moda de almacenar una animaci�n para un grado de libertad de
 * un hueso ser�a con un array que contuviera una entrada por cada fotograma
 * con el valor para ese grado de libertad.
<table cellpadding="2" cellspacing="2" border="1" width="1%">
    <tr>
      <td valign="top">Fotograma<br>
      </td>
      <td valign="top">0<br>
      </td>
      <td valign="top">1<br>
      </td>
      <td valign="top">2<br>
      </td>
      <td valign="top">3<br>
      </td>
      <td valign="top">4<br>
      </td>
      <td valign="top">5<br>
      </td>
      <td valign="top">6<br>
      </td>
      <td valign="top">......<br>
      </td>
      <td valign="top">20<br>
      </td>
      <td valign="top">21<br>
      </td>
      <td valign="top">22<br>
      </td>
      <td valign="top">......<br>
      </td>
      <td valign="top">26<br>
      </td>
    </tr>
    <tr>
      <td valign="top">Desplazamiento<br>
      </td>
      <td valign="top">0<br>
      </td>
      <td valign="top">0<br>
      </td>
      <td valign="top">0<br>
      </td>
      <td valign="top">0<br>
      </td>
      <td valign="top">-1<br>
      </td>
      <td valign="top">-2<br>
      </td>
      <td valign="top">-3<br>
      </td>
      <td valign="top">......<br>
      </td>
      <td valign="top">-17<br>
      </td>
      <td valign="top">-17<br>
      </td>
      <td valign="top">-17<br>
      </td>
      <td valign="top">......<br>
      </td>
      <td valign="top">-16<br>
      </td>
    </tr>
</table>
 * @todo Paso de perder el tiempo. Hay que pasar la explicaci�n
 * que hice en el cuaderno sobre todo esto.
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 */
typedef union {

	/** 
	 * Informaci�n almacenada por una entrada del array cuando el elemento
	 * se utiliza para la informaci�n sobre compresi�n.
	 */
	struct {
		/** Pendiente */
		MDLuchar valid;
		/** Pendiente */
		MDLuchar total;
	} num;
	/** 
	 * Informaci�n almacenada por una entrada del array cuando el elemento
	 * se utiliza para dato de animaci�n. Almacena el valor <b>a sumar</b>
	 * al valor por defecto del hueso para el grado de libertad cuya
	 * animaci�n almacena el array al que pertenece este elemento.
	 */
	MDLshort value;	// Valor a SUMAR al valor por defecto.

} animvalue_t;

} // namespace HalfLifeMDL


/**
 * @page secuenciasYGrupos Secuencias, animaciones y grupos de secuencias.
 * El formato del fichero MDL mantiene secuencias, grupos de secuencias y
 * animaciones, algo confuso a menudo. La finalidad perseguida al mantener
 * esta separaci�n es doble:
 *
 *	-# Ahorrar trabajo a los dise�adores gr�ficos, que pueden reutilizar
 * animaciones (serie de movimientos) m�s de una vez.
 *	-# A�adir la posibilidad de partir un modelo en varios ficheros,
 * manteniendo fuera del fichero de la geometr�a las animaciones (al igual
 * que puede hacerse con las texturas).
 * 
 * Se entiende por secuencia a toda la informaci�n de una serie de
 * movimientos del modelo. Ejemplos de secuencia son el ciclo de andar,
 * el movimiento de salto, etc. Esta informaci�n incluye el nombre de la
 * secuencia, el desplazamiento del personaje, la velocidad de reproducci�n
 * (fotogramas por segundo), etc. La informaci�n de una secuencia se almacena
 * en la estructura HalfLifeMDL::seqdesc_t en un fichero MDL.
 * <p>
 * Se denomina animaci�n a la informaci�n concreta sobre el movimiento de los
 * huesos. Una secuencia tendr� asociada, al menos, una animaci�n. Es posible
 * asociar m�s de una, y el motor gr�fico mezclar� ambas durante la ejecuci�n,
 * dando un peso a cada una de ellas (para hacer una mezcla ponderada).
 * Para que esto sea posible, se obliga a que todas las animaciones de la
 * secuencia tengan el mismo n�mero de fotogramas.<br>
 * Esta funcionalidad es �til para crear animaciones parciales. Por ejemplo el
 * dise�ador puede crar una animaci�n de disparar, que manipula la parte
 * superior del cuerpo, y una de correr y saltar que controlan la inferior.
 * Luego se crear�an dos secuencias que mezclar�an la animaci�n de disparar
 * con cada una de las otras dos.<br>
 * Hay que tener cuidado con esto, porque esa animaci�n mezcla no se crea
 * "al vuelo" durante la ejecuci�n del programa (como ocurre con el formato
 * de personajes de Quake III, que est�n siempre divididos en tres partes), 
 * sino que se crea en el momento de empaquetar el fichero MDL.<br>
 * Adem�s, con el formato MDL se pueden mezclar ambas animaciones
 * proporcionando un peso. Las animaciones de Quake III se refer�an a partes
 * del cuerpo disjuntas, y no hab�a conflictos. Ahora las animaciones afectan
 * a todo el cuerpo, por lo que la mezcla requiere un peso para cada
 * una. Eso permite, hipot�ticamente, mezclar una animaci�n de andar y otra
 * de correr, y que el motor incremente el peso de una y decremente el de otra
 * con el tiempo para mostrar la transici�n de un estado a otro suavemente.
 * Como ya se ha dicho, para poder hacer esto, ambas animaciones deben tener
 * el mismo n�mero de fotogramas, y debe existir una secuencia en el modelo
 * que las relacione.
 * <p>
 * La separaci�n entre animaciones y secuencias permite una reutilizaci�n
 * adicional. Si se dispone de un ciclo de andar, �ste puede utilizarse
 * en la secuencia de andar, y tambi�n en la de correr, con tal de especificar
 * en la secuencia de correr una velocidad de reproducci�n m�s r�pida. En el
 * fichero MDL resultante, las dos secuencias apuntar�n a la misma informaci�n
 * sobre animaciones. En esta librer�a, sin embargo, cuando lee y analiza
 * el fichero, crear� dos objetos de animaci�n diferente. Eso consume m�s
 * memoria, pero facilita la implementaci�n.
 * <p>
 * Los grupos de secuencia permiten controlar las <EM>animaciones</EM>
 * almacenadas en ficheros diferentes. En realidad el nombre es confuso, pues
 * deber�a denominarse "grupos de animaciones". Un grupo de secuencias
 * agrupa la informaci�n de las animaciones contenidas en un fichero. La idea
 * es que el fichero de la geometr�a posee un array de grupos de secuencias,
 * cada una representando las animaciones de un fichero
 * (HalfLifeMDL::header_t::seqgroupindex). El elemento 0 se refiere a las
 * animaciones del fichero de la geometr�a. El resto guarda informaci�n de
 * los ficheros externos con animaciones del modelo actual. Lo que se
 * pretende es que el programa cargue las animaciones "bajo demanda", seg�n
 * se necesiten, para ahorrar memoria. En la pr�ctica, esta librer�a no har�
 * eso, y todos los ficheros externos con animaciones deber�an ser cargados al
 * principio.<br>
 * Por norma general, si se lee el modelo de geometr�a del fichero
 * <tt>model.mdl</tt>, y se detecta que el n�mero de grupos de secuencias es
 * mayor que 1, la informaci�n general de cada grupo de secuencia estar�
 * siempre en el propio fichero <tt>model.mdl</tt>. Sin embargo, las
 * animaciones asociadas al grupo <tt>n</tt> estar�n en el fichero
 * <tt>modelXX.mdl</tt>, siendo XX el n�mero <tt>n</tt> representado con dos
 * d�gitos decimales.<p>
 * La informaci�n guardada por cada grupo de secuencia es muy peque�a, y
 * relativamente poco importante. Vease HalfLifeMDL::seqgroup_t para m�s
 * informaci�n.<p>
 * Si un fichero indica que no tiene ning�n grupo de secuencia, ni siquiera
 * existir� el grupo de secuencias del fichero de la geometr�a. En ese caso
 * el campo HalfLifeMDL::header_t::numseq ser� inv�lido y debe considerarse 0.
 * Esto ocurre �nicamente con los ficheros <tt>modelXX.mdl</tt> anteriormente
 * descritos.
 * @todo Si se hace otra p�gina de estas con la creaci�n de un modelo,
 * meter un enlace.
 * @see HalfLifeMDL::seqdesc_t, HalfLifeMDL::seqgroup_t, HalfLifeMDL::header_t
 */

#endif