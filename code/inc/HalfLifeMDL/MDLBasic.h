//---------------------------------------------------------------------------
// MDLBasic.h
//---------------------------------------------------------------------------

/**
 * @file MDLBasic.h
 * Contiene las estructuras básicas que se guardan en un fichero
 * MDL. Por ejemplo, contiene una estructura que encaja perfectamente
 * con los primeros bytes del fichero, que componen la cabecera.
 * @author Pedro Pablo Gómez Martín
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
 * @todo Descripción más detallada (está en MDLBasic.h).
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
namespace HalfLifeMDL {

/**
 * Enumerado que define constantes para los 6 posibles grados de liberad
 * (Degree Of Freedom).
 * El fichero guarda algunos vectores de 6 elementos indicando, por ejemplo,
 * la configuración por defecto de un hueso en sus seis grados de libertad.
 * Para acceder a esos arrays con nombres ilustrativos se utiliza este
 * enumerado.
 * <p>
 * Los valores asociados a cada valor no pueden ser modificados, por lo que
 * los usuarios de este tipo pueden hacer uso de sus valores numéricos en
 * lugar de los valores constantes para hacer recorridos sobre los arrays
 * que lo requieran por ejemplo.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef enum {
	/** Desplazamiento X. */
	DoF_X = 0,
	/** Desplazamiento Y. */
	DoF_Y,
	/** Desplazamiento Z. */
	DoF_Z,
	/** Rotación sobre el eje X. */
	DoF_RX,
	/** Rotación sobre el eje Y. */
	DoF_RY,
	/** Rotación sobre el eje Z. */
	DoF_RZ
} DoF_t;

/**
 * Enumerado que define constantes para el campo flags de las texturas
 * (texture_t::flags). Sus valores se pueden condensar usando OR lógica
 * a nivel de bits.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef enum {
	/**
	 * Textura sin ninguna característica especial (las caracteristicas
	 * por defecto).
	 */
	TF_none = 0,
	/**
	 * Sombreado flat. No estoy seguro de esto.
	 * @todo ¿Buscar algún modelo que lo use? El lector que he usado
	 * para averiguar toda la información sobre el formato no comprueba nunca
	 * este valor.
	 */
	TF_FlatShade = 1,
	/**
	 * Material cromado.
	 * @todo Averiguar más sobre esto mirando el código del lector de
	 * referencia.
	 */
	TF_Chrome = 2,
	/**
	 * Material que debe iluminarse por completo. No sé qué significa :-D
	 * @todo ¿Buscar algún modelo que lo use? El lector que he usado
	 * para averiguar toda la información sobre el formato no comprueba nunca
	 * este valor.
	 */
	TF_Fullbright = 4

} textureFlags_t;

/**
 * Enumerado que define las constantes para el campo motiontype de las
 * secuencias (seqdesc_t::motiontype). Sus valores se pueden condensar
 * usando OR lógica a nivel de bits, aunque no todas las combinaciones
 * tienen sentido. <p>
 * La idea es que una secuencia de un modelo a parte de la animación de los
 * huesos puede originar un desplazamiento sobre el origen de coordenadas
 * del hueso principal. Por ejemplo, cuando el personaje anda, no sólo
 * se deben mover los pies, sino que el motor debe desplazar por el mundo
 * al personaje para hacerle cambiar de posición. Los tipos de desplazamiento
 * pueden clasificarse en tres grupos:
 *	- Desplazamiento lineal: el ejemplo más claro aparece en los ciclos de
 * andar o correr. Especifica el avance del personaje que debe ser constante
 * en el tiempo.
 *	- Desplazamiento absoluto: tradicionalmente para secuencias no cíclicas
 * que originan un desplazamiento del personaje. Para simplificar las cosas,
 * el motor podría no desplazar al personaje durante la reproducción de la
 * secuencia, y ajustar la posición solamente al final. Por ejemplo, en una
 * animación en la que se muestra como el personaje muere, éste podría caer
 * hacia atrás, o de lado y terminar tendido en el suelo. Durante toda la
 * animación el eje de referencia no cambia, pero cuando la secuencia acaba
 * lo ideal sería establecer una "secuencia" donde el único fotograma es
 * el personaje tendido en la misma posición, y con el origen de coordenadas
 * colocado de forma razonable respecto al cuerpo. Ese desplazamiento absoluto
 * realizado al final de la secuencia de muerte entraría en esta categoría.
 *	- Constante pero irregular: si se desea complicar la vida al motor, el
 * ejemplo anterior podría realizarse cambiando contínuamente la posición
 * del personaje en el mundo según va cayendo hacia atrás y terminando en
 * el suelo. Ese desplazamiento no es lineal como en el caso del ciclo de
 * andar, por lo que, a parte de las animaciones sobre los huesos, se
 * necesitaría información sobre el desplazamiento absoluto del personaje
 * en cada fotograma respecto al anterior.
 *
 * Én realidad todos son un mismo modo de hacer lo mismo. Por ejemplo, el
 * ciclo de andar podría realizarse con desplazamiento absoluto en lugar
 * de lineal. Lo único que cambiaría sería la posición de los huesos
 * respecto al eje de referencia del hueso raíz.
 * <p>
 * Por otro lado, aunque hemos estado hablando de desplazamientos, también
 * habría que incluir rotaciones, por ejemplo para la animación en la que
 * el personaje gira sobre sí mismo sin andar para mirar a otro lado.
 * <p>
 * Se supone que el formato de fichero de Half Life está preparado para
 * guardar todos esos tipos de información, salvo la rotación lineal, y
 * el campo seqdesc_t::motiontype contendrá el tipo de desplazamiento
 * particular de una secuencia concreta. En realidad, la granularidad
 * de este campo es mayor que las tres posibilidades comentadas antes,
 * y llega al nivel de coordenadas. Por ejemplo, en lugar de especificar
 * que la secuencia tiene animación lineal y dar las tres coordenadas,
 * indica que tiene animación lineal sobre el eje X, por lo que los
 * campos de animación lineal sobre los ejes Y o Z de la secuencia no
 * deberán ser atendidos. Esto ocasiona que el valor del campo sea en
 * realidad una mezcla (con OR lógicas) de varios valores. Los posibles
 * valores a mezclar son los que especifica esta estructura. En realidad
 * no todas las mezclas tienen sentido (por ejemplo, que el personaje
 * tenga desplazamiento lineal en el eje X, y a la vez desplazamiento
 * irregular en el mismo eje).
 * <p>
 * Por desgracia, la realidad es bastante más triste. Aunque parece que
 * el formato del fichero de Half Life permite los tipos de desplazamiento
 * descritos, la realidad es que el empaquetador de los modelos (el
 * programa StudioMDL) no permite especificar todos los posibles valores
 * (o, más bien, los ignora), por lo que la única posibilidad admitida
 * es el desplazamiento lineal sobre algún eje. El análisis realizado
 * sobre todos los modelos del juego original muestran que sólo esos
 * tipos de movimiento se usan en algún momento. Más concretamente, los
 * personajes se desplazan cuando avanzan hacia el lado positivo del
 * eje X. Esporádicamente alguno tiene desplazamiento lineal en el
 * eje Z en los saltos, aunque no parece tener mucho sentido. <p>
 * Es posible especificar todos los tipos de desplazamiento en el .QC
 * con el que crear el modelo, y StudioMDL marcará los flags correctamente.
 * Sin embargo, sería lógico que en el caso de desplazamiento irregular
 * luego especificara en algún sitio el desplazamiento por cada
 * fotograma, pero no es así. Se supone que eso se ha dejado para que
 * sea el programa el que lo conozca, en lugar de estar como "información
 * declarativa" en el fichero..<p>
 * Otra cosa curiosa es que hay alguna animación de giro sobre sí mismo
 * (por ejemplo en <code>female.mdl</code>), sin embargo, como no hay
 * posibilidad de especificar rotación lineal, esas secuencias no tienen
 * ningún flag activo.
 * <p>
 * Resumiendo, la información sobre el movimiento en el mundo de
 * los modelos para cada secuencia está en los MDL no es completa. Sólo
 * se soporta completamente el desplazamiento lineal, pero no la rotación.
 * El resto quizá se pensó en soportar, pero finalmente parece que no
 * se hace. Eso lleva, por otro lado, a que todo lo contado aquí sean
 * hípótesis, porque no hay nada probable. Sospecho que muchas de las
 * cosas se quedaron como "Buenas ideas para con un poco de suerte
 * utilizar en alguna versión futura". 
 * @see HalfLifeMDL::seqdesc_t
 * @see @ref secuenciasYGrupos
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef enum {

	/**
	 * Desplazamiento absoluto sobre el eje X. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener ese desplazamiento del fichero.
	 */
	MT_X = 0x0001,
	/**
	 * Desplazamiento absoluto sobre el eje Y. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener ese desplazamiento del fichero.
	 */
	MT_Y = 0x0002,
	/**
	 * Desplazamiento absoluto sobre el eje Z. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma obtener ese desplazamiento del fichero.
	 */
	MT_Z = 0x0004,
	/**
	 * Rotación absoluta sobre el eje X. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener esa rotación del fichero.
	 */
	MT_XR = 0x0008,
	/**
	 * Rotación absoluta sobre el eje Y. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener esa rotación del fichero.
	 */
	MT_YR = 0x0010,
	/**
	 * Rotación absoluta sobre el eje Z. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma obtener esa rotación del fichero.
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
	 * Desplazamiento irregular sobre el eje X. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener los desplazamientos de cada
	 * fotograma, aunque podría parecer que se deberían obtener de
	 * HalfLifeMDL::seqdesc_t::automoveposindex.
	 */
	MT_AX = 0x0200,
	/**
	 * Desplazamiento irregular sobre el eje Y. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener los desplazamientos de cada
	 * fotograma, aunque podría parecer que se deberían obtener de
	 * HalfLifeMDL::seqdesc_t::automoveposindex.
	 */
	MT_AY = 0x0400,
	/**
	 * Desplazamiento irregular sobre el eje Z. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener los desplazamientos de cada
	 * fotograma, aunque podría parecer que se deberían obtener de
	 * HalfLifeMDL::seqdesc_t::automoveposindex.
	 */
	MT_AZ = 0x0800,
	/**
	 * Rotación irregular sobre el eje X. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener la rotación de cada
	 * fotograma, aunque podría parecer que se deberían obtener de
	 * HalfLifeMDL::seqdesc_t::automoveangleindex.
	 */
	MT_AXR = 0x1000,
	/**
	 * Rotación irregular sobre el eje Y. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener la rotación de cada
	 * fotograma, aunque podría parecer que se deberían obtener de
	 * HalfLifeMDL::seqdesc_t::automoveangleindex.
	 */
	MT_AYR = 0x2000,
	/**
	 * Rotación irregular sobre el eje Z. En realidad es una hipótesis.
	 * Este valor no lo toma ninguna secuencia de los modelos del juego
	 * original. Tampoco hay forma de obtener la rotación de cada
	 * fotograma, aunque podría parecer que se deberían obtener de
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
 * Se utiliza para evitar posibles problemas en el cambio del tamaño de
 * la palabra de la arquitectura. No obstante, no se vigila el orden de
 * los bytes, por lo que el sistema debe ser little-endian.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef float MDLfloat;

/**
 * Tipo entero de cuatro bytes, tal y como se almacena en el fichero.
 * Se utiliza para evitar posibles problemas en el cambio del tamaño de
 * la palabra de la arquitectura. No obstante, no se vigila el orden de
 * los bytes, por lo que el sistema debe ser little-endian.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef int MDLint;

/**
 * Tipo entero sin signo de cuatro bytes, tal y como se almacena en el 
 * fichero. Se utiliza para evitar posibles problemas en el cambio del 
 * tamaño de la palabra de la arquitectura. No obstante, no se vigila 
 * el orden de los bytes, por lo que el sistema debe ser little-endian.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef unsigned int MDLuint;

/**
 * Tipo entero corto de dos bytes, tal y como se almacena en el fichero.
 * Se utiliza para evitar posibles problemas en el cambio del tamaño de
 * la palabra de la arquitectura. No obstante, no se vigila el orden de
 * los bytes, por lo que el sistema debe ser little-endian.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef short MDLshort;

/**
 * Tipo entero corto sin signo de dos bytes, tal y como se almacena en
 * el fichero. Se utiliza para evitar posibles problemas en el cambio
 * del tamaño de la palabra de la arquitectura. No obstante, no se 
 * vigila el orden de los bytes, por lo que el sistema debe ser
 * little-endian.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef unsigned short MDLushort;

/**
 * Tipo caracter (byte), tal y como se almacena en el fichero. Se
 * utiliza para evitar posibles problemas en el cambio del tamaño de
 * la palabra.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef char MDLchar;

/**
 * Tipo caracter (byte) sin signo, tal y como se almacena en el fichero. Se
 * utiliza para evitar posibles problemas en el cambio del tamaño de
 * la palabra.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef unsigned char MDLuchar;

/**
 * Tipo de un vector de tres flotantes. Se utiliza en el fichero para guardar
 * coordenadas tridimensionales.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef MDLfloat vec3_t[3];

/**
 * Tipo que representa un puntero a una posición del fichero. Es un
 * entero de cuatro bytes. Aunque hace bastante más incómodo el uso de
 * las estructuras que utilizan este tipo, los valores leidos se
 * mantienen en los campos de este tipo, en lugar de modificarlos (una
 * vez leído el fichero) para que guarde directamente el puntero a
 * la posición de memoria. Se ha decidido esto para evitar problemas
 * mayores si las clases se portan a una arquitectura de 64 bits.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef MDLint ptr_t;

/**
 * Información de la cabecera del fichero.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct {

	/**
	 * Identificador del fichero. Debe ser IDST o IDSQ. Las pruebas
	 * realizadas indican que es IDST en los ficheros con geometría o
	 * texturas, e IDSQ en los que sólo contienen animaciones, aunque
	 * no se puede inducir que eso siempre sea así.
	 * @see @ref secuenciasYGrupos
	 */
	MDLint id;

	/**
	 * Versión del formato. Debería ser 10 (supongo que o superior).
	 */
	MDLint version;

	/**
	 * Nombre del fichero. Puede incluir la ruta, a veces con '/' y otras
	 * con '\'. En cualquier caso, la ruta no es muy fiable.
	 */
	MDLchar name[64];

	/**
	 * Tamaño del fichero en bytes, incluyendo la cabecera.
	 */
	MDLuint length;

	/**
	 * Posición ideal del ojo del personaje. En teoría lo usa el módulo de IA
	 * para calcular lo que el carácter puede ver en cada momento.
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
	 * Bounding Box. Teóricamente, este campo debería contener la posición
	 * del vértice inferior del bounding box alineado con los ejes del modelo
	 * en su posición por defecto. La realidad es que, en los modelos
	 * probados, su valor es cero.<br>
	 * Teóricamente la esquina superior del bounding box es header_t::bbmax.
	 */
	vec3_t bbmin;

	/**
	 * Bounding Box. Teóricamente, este campo debería contener la posición
	 * del vértice superior del bounding box alineado con los ejes del modelo
	 * en su posición por defecto. La realidad es que, en los modelos
	 * probados, su valor es cero.<br>
	 * Teóricamente la esquina superior del bounding box es header_t::bbmin.
	 */
	vec3_t bbmax;		

	/**
	 * Información adicional de las características del modelo. Ni idea de
	 * qué valores puede contener, ni lo que indica.
	 */
	MDLint flags;

	/**
	 * Número de huesos (articulaciones) del modelo. No puede ser mayor que
	 * 255, pues el hueso al que está enlazado cada vértice se almacena como
	 * un byte (vea HalfLifeMDL::model_t::vertinfoindex).
	 */
	MDLuint numbones;

	/**
	 * Posición del fichero donde empieza la información de los huesos
	 * (array de bone_t).
	 */
	ptr_t boneindex;

	/**
	 * Número de controladores de huesos.
	 */
	MDLuint numbonecontrollers;

	/**
	 * Posición del fichero donde empieza la información de los controladores
	 * (array de bonecontroller_t).
	 */
	ptr_t bonecontrollerindex;

	/**
	 * Número de "hit boxes". Son "complex bounding boxes" que no sé para qué son.
	 */
	MDLuint numhitboxes;

	/**
	 * Posición del fichero donde empieza la información de los hitboxes
	 * (array de bbox_t).
	 */
	ptr_t hitboxindex;
	
	/**
	 * Número de secuencias del modelo.
	 * @attention Si el número de grupos de secuencias
	 * (header_t::numseqgroups) es 0, este valor no será válido y debe
	 * considerarse cero. Si se utiliza el valor de este campo como el
	 * valor real del número de secuencias, el resultado puede ser
	 * impredecible.
	 * @see @ref secuenciasYGrupos
	 */
	MDLuint numseq;

	/**
	 * Posición de la información de las secuencias (array de seqdesc_t).
	 * @see @ref secuenciasYGrupos
	 */
	ptr_t seqindex;

	/**
	 * Número de grupos de secuencias. En realidad es un nombre confuso;
	 * debería ser el número de grupos de animaciones. Si su valor es 0,
	 * el campo header_t::numseq es inválido. Si hay más de uno, las
	 * animaciones del grupo <tt>n</tt> estarán en el fichero
	 * <tt>modelXX.mdl</tt>, suponiendo que el modelo original es
	 * <tt>model.mdl</tt>, y que <tt>XX</tt> es la representación con dos 
	 * dígitos decimales del número <tt>n</tt>.
	 * @see @ref secuenciasYGrupos
	 */
	MDLuint numseqgroups;

	/**
	 * Posición de la información de los grupos de secuencias. También hay una
	 * entrada para el grupo 0 (que representa al fichero actual). Es un array
	 * de seqgroup_t.
	 * @see @ref secuenciasYGrupos
	 */
	ptr_t seqgroupindex;

	/**
	 * Número de texturas en el fichero.
	 */
	MDLuint numtextures;

	/**
	 * Posición de la información de las texturas (array de texture_t).
	 */
	ptr_t textureindex;

	/**
	 * Parece ser la posición de los píxeles de la primera textura. Su valor
	 * no sirve para nada, pues es redundante (está en la información de la
	 * primera textura.
	 */
	ptr_t texturedataindex;

	/**
	 * Número de texturas que hay en cada piel.
	 */
	MDLuint numskinref;

	/**
	 * Número de pieles.
	 */
	MDLuint numskinfamilies;

	/**
	 * Posición de la información de cada piel de texturas. Esa información es
	 * un array de enteros cortos de dos bytes de numskinref*numskinfamilies
	 * elementos.<p>
	 * Cada valor referencia a una textura del modelo. Las mallas no referencian
	 * texturas directamente, sino el índice de la piel donde se almacena el
	 * índice a la textura. Si las texturas están en otro fichero (modelT.mdl),
	 * la información sobre las pieles también se saca del modelo de las texturas
	 * en lugar de del MDL principal.
	 */
	ptr_t skinindex;

	/**
	 * Número de partes del modelo.
	 */
	MDLuint numbodyparts;

	/**
	 * Posición de la información de las partes del cuerpo (array de bodyparts_t).
	 */
	ptr_t bodypartindex;

	/**
	 * Número de "adjuntos" del modelo ("queryable attachable points").
	 */
	MDLuint numattachments;
	
	/**
	 * Posición de la información de los "adjuntos" (array de attachment_t).
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
 * Información sobre un hueso (articulación) del modelo.<p>
 * La información básica del esqueleto (huesos o articulaciones, y sus
 * relaciones), se almacena en los campos header_t::numbones y
 * header_t::boneindex de la cabecera. El primero de ellos indica el número
 * de huesos que contiene el modelo del fichero, y el segundo el puntero
 * donde empieza la información, compuesta por un vector de numbones
 * elementos de tipo bone_t.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Nombre del hueso (dado por el diseñador al crear el modelo).
	 */
	MDLchar name[32];

	/**
	 * Hueso padre del que depende el actual. Es el índice dentro del "array" de
	 * huesos. Un -1 indica que el hueso no tiene padre, es decir es el "raíz" del
	 * modelo.
	 */
	MDLint parent;

	/**
	 * Uso desconocido.
	 */
	MDLint flags;

	/**
	 * Índice del controlador para el grado de libertad iésimo del hueso.
	 * Los grados de libertad son:
	 *
	 *	- 0: Desplazamiento X.
	 *	- 1: Desplazamiento Y.
	 *	- 2: Desplazamiento Z.
	 *	- 3: Rotación sobre el eje X.
	 *	- 4: Rotación sobre el eje Y.
	 *	- 5: Rotación sobre el eje Z.
	 *
	 * El enumerado HalfLifeMDL::DoF_t (Degree Of Freedom) contiene las
	 * constantes pertinentes para acceder con nombres simbólicos a los
	 * elementos de este vector. Hay más información sobre los controladores
	 * en la documentación de la estructura bonecontroller.
	 */ 
	MDLint bonecontroller[6];

	/**
	 * Valores por defecto de los 6 grados de libertad del hueso.
	 * Los grados de libertad son:
	 *
	 *	- 0: Desplazamiento X.
	 *	- 1: Desplazamiento Y.
	 *	- 2: Desplazamiento Z.
	 *	- 3: Rotación sobre el eje X.
	 *	- 4: Rotación sobre el eje Y.
	 *	- 5: Rotación sobre el eje Z.
	 *
	 * El enumerado HalfLifeMDL::DoF_t (Degree Of Freedom) contiene las
	 * constantes pertinentes para acceder con nombres simbólicos a los
	 * elementos de este vector.<p>
	 * Si un hueso no tiene animación en una secuencia, se usan los valores de
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
	 *	- 3: Rotación sobre el eje X.
	 *	- 4: Rotación sobre el eje Y.
	 *	- 5: Rotación sobre el eje Z.
	 *
	 * El enumerado HalfLifeMDL::DoF_t (Degree Of Freedom) contiene las
	 * constantes pertinentes para acceder con nombres simbólicos a los
	 * elementos de este vector.<p>
	 * Este escalado sólo se usa para los valores obtenidos con las animaciones,
	 * pero no para los valores por defecto (que se supone que están ya escalados).
	 */
	MDLfloat scale[6];

} bone_t;

/**
 * Información de una parte del modelo.<p>
 * El modelo tiene los huesos, y las animaciones sobre esos huesos. Luego
 * tiene las mallas sobre los huesos. En realidad se pueden asociar
 * varias mayas a los mismos huesos, para disponer un modo "barato" de
 * tener diferentes personajes, o varias versiones del mismo (por ejemplo,
 * tener un personaje que puede tener o no casco).<p>
 * Para dar más versatilidad a esta posibilidad, en realidad no necesariamente
 * todo el "esqueleto" debe ser cambiado. Es decir, si se quiere tener en
 * un .mdl varios personajes, se pueden reutilizar algunas mallas de un
 * personaje en otro (en el caso del casco, se aprovecharía todo menos la
 * parte de la cabeza). Para lograrlo, las mallas se agrupan en "partes del
 * cuerpo", y son las diferentes partes del cuerpo las que pueden proporcionar
 * diferentes alternativas (varias posibles mallas). Hay que remarcar que
 * lo de tener varias partes del cuerpo aquí es completamente diferente a
 * las diferentes partes del cuerpo usadas en Quake III (donde se usaba por
 * motivos de animación). Aquí las animaciones no cambian (están especificadas
 * en los huesos); lo que cambia son las mallas que se obtienen a partir de
 * esos huesos. En condiciones normales sólo habrá una parte del cuerpo, con
 * un único modelo (el del personaje).<p>
 * Un ejemplo donde se usa esto es en el modelo barney.mdl (hay una sóla parte
 * del cuerpo, con dos modelos). También en el del jugador, donde hay dos partes
 * la cabeza (con dos modelos, uno normal y otro con casco), y el resto del
 * cuerpo con uno solo.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Nombre de la parte del cuerpo. Ejemplos son "studio" o "head".
	 */
	MDLchar name[64];
	
	/**
	 * Número de modelos (recubrimientos) para esta parte del cuerpo.
	 */
	MDLuint nummodels;

	/**
	 * "Base numérica" usada para condensar en un sólo entero el número del
	 * modelo seleccionado en cada parte del cuerpo del modelo.<p>
	 * Cómo se calcula exactamente no lo sé... el caso es que utilizando
	 * la base se puede obtener del entero qué modelo está seleccionado en la
	 * parte del cuerpo. Para eso se divide el entero por esta base, y con el
	 * resultado se hace  el módulo con el número de modelos del grupo.
	 */
	MDLint	base;

	/**
	 * Posición en el fichero donde está la información de los modelos
	 * (recubrimientos) de esta parte del modelo. Es un array de model_t.
	 */
	ptr_t modelindex;

} bodyparts_t;

/**
 * Información sobre el modelo (recubrimiento) de una parte del cuerpo. Cada
 * modelo almacena los vértices. Además, cada modelo puede tener asociada
 * más de una malla, todas ellas compartiendo los mismos vértices.<p>
 * Cada vértice estará asociado a un hueso (y sólo a uno). Un triángulo
 * de una malla puede estar compuesto por vértices asociados a diferentes
 * huesos (sino, quedarían espacios entre los triángulos asociados a cada
 * hueso).
 * @author Pedro Pablo Gómez Martín
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
	 * Número de mallas del modelo.
	 */
	MDLuint nummesh;

	/**
	 * Posición en el fichero donde está la información de las mallas
	 * del modelo. Es un array de mesh_t.
	 */
	ptr_t meshindex;

	/**
	 * Número de vértices del modelo.
	 */
	MDLuint numverts;

	/**
	 * Posición en el fichero donde está la información de a qué hueso
	 * pertenece cada vértice. Es un vector de bytes, cada uno con el índice
	 * del hueso del que depende el vértice iésimo del modelo.
	 */
	ptr_t vertinfoindex;

	/**
	 * Posición en el fichero donde están las coordenadas de cada vértice.
	 * Es un vector de vec3_t con la posición de cada vértice en función
	 * de su articulación de referencia. Como lo que se anima en el modelo
	 * son los huesos, los vértices no cambian nunca, lo que cambia es
	 * la orientación de las articulaciones.
	 */
	ptr_t vertindex;

	/**
	 * Número de normales (únicas) del modelo. <p>
	 * Las mallas pue forman el modelo referencian a las normales del
	 * modelo para formar sus vértices. Este campo especifica la
	 * longitud del array apuntado por normindex.
	 */
	MDLuint numnorms;

	/**
	 * Posición en el fichero donde está la información de respecto a qué
	 * hueso están dadas las coordenadas de cada normal. Es un vector
	 * de bytes, cada uno con el índice del hueso del que depende la
	 * normal iésima del modelo.
	 */
	ptr_t norminfoindex;

	/**
	 * Posición en el fichero donde están las coordenadas de cada normal.
	 * Es un vector de vec3_t con la orientación de cada normal en función
	 * de su articulación de referencia. Como lo que se anima en el modelo
	 * son los huesos, las normales no cambian nunca, lo que cambia es la
	 * orientación de las articulaciones.
	 */
	ptr_t normindex;

	/**
	 * Número de grupos de deformación. No sé qué es exactamente.
	 */
	MDLuint numgroups;

	/**
	 * Información de los grupos de deformación.
	 */
	ptr_t groupindex;

} model_t;

/**
 * Información de una malla. Cada malla está asociada a un modelo o 
 * recubrimiento (model_t), cada uno de ellos perteneciendo a una
 * parte del modelo (bodyparts_t).
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct {

	/**
	 * Supongo que será el número de triángulos de la malla. Su valor no se
	 * utiliza para dibujar.
	 */
	MDLuint numtris;

	/**
	 * Posición del fichero donde empieza el vector con la información de los
	 * triángulos de la malla. El vector es de enteros cortos (de 16 bits).
	 * Lo que contiene es la información de los triángulos de forma 
	 * encapsulada, por grupos. Cada grupo tiene un primer valor que indica 
	 * el número de vértices que posee. Si el valor es 0, es el último
	 * grupo del vector, y no hay que seguir leyendo en el array.<p>
	 * Si el valor es positivo, los vértices que aparecen a continuación
	 * construyen un grupo de triángulos. Salvo para los dos primeros
	 * vértices, cada uno de los demás crea un nuevo triángulo. Para valores
	 * de n impares, el triángulo n-ésimo está formado por los vértices n, n+1
	 * y n+2. Para n pares, el triángulo n-ésimo está formado por los
	 * vértices n+1, n y n+2. Se corresponde con el formato esperado por
	 * glBegin(GL_TRIANGLE_STRIP) de OpenGL.<p>
	 * Si el valor del primer elemento del grupo es negativo, el número de
	 * vértices es el valor absoluto, y el modo de construir el grupo de
	 * triángulos es diferente. Igual que antes, salvo para los dos primeros
	 * vértices, cada uno de los demás crea un nuevo triángulo. El triángulo
	 * n-ésimo es definido con los vértices 1, n+1 y n+2. Se corresponde
	 * con el formato esperado por glBegin(GL_TRIANGLE_FAN) de OpenGL.<p>
	 * Después del valor inicial, vienen los vértices. Cada uno utiliza
	 * cuatro posiciones del vector. La primera es el índice al vértice en el
	 * vector de vértices del modelo al que pertenece la malla (model_t).
	 * El segundo es el índice a la normal en el vector de normales del
	 * modelo. El tercero es la coordenada u de textura, y el cuarto la
	 * coordenada v. Como en realidad los campos son enteros cortos, los
	 * valores reales de las coordenadas de textura se calculan dividiendo
	 * los valores leídos por el ancho y el alto de la textura que se
	 * aplica sobre la malla.<br>
	 * La división de las coordenadas u y v anterior supone que todas las
	 * texturas de mismo índice dentro de una piel deben tener las mismas
	 * dimensiones (ver MDLSkins).
	 * <p>
	 * Algunas texturas son "cromadas". En ese caso las corodenadas de
	 * textura deberían calcularse "al vuelo" en función de la normal del
	 * vértice y el punto de vista, y los elementos con las coordenadas
	 * no contendrán información útil.
	 * <p>
	 * La cara frontal de los triángulos formados por los vértices definidos
	 * aquí es aquella que deja a los vértices ordenados en el sentido de la
	 * agujas del reloj.
	 * @warning El valor de este campo puede repetirse en más de una malla.
	 * Eso ocurre por ejemplo en barney.mdl. Tiene dos partes: el cuerpo y la
	 * pistola. Ésta última tiene tres versiones diferentes, una usada
	 * para que la pistola aparezca en la cartuchera, otra para que aparezca
	 * en la mano de Barney, y la última para que no aparezca. En cada caso
	 * los vértices de los triángulos y de qué hueso dependen serán diferentes
	 * pero la conexión de esos vértices siempre será igual. Como es
	 * precisamente esa información la que se apunta desde este campo, las
	 * tres versiones de la pistola apuntan a la misma información de
	 * conexión.
	 */
	ptr_t triindex;

	/**
	 * Índice a la textura usada. No es directamente el índice a la textura,
	 * sino a la textura de la piel. El índice de la textura se obtiene
	 * consultando la posición indicada por este campo en el array de índices
	 * a texturas de la piel actual.
	 */
	MDLuint skinref;

	/**
	 * Número de normales de la malla. El modelo (model_t) al que pertence
	 * esta malla tiene su propio número de normales (model_t::numnorms) y
	 * una lista de normales (model_t::norminfoindex). Supuestamente, este
	 * campo indica cuantas de las normales del modelo son usadas por esta
	 * malla. <p>
	 * En realidad esto no está comprobado. Lo que está claro es que si un
	 * modelo tiene, pongamos, 10 mallas, la suma de este campo
	 * (mesh_t::numnorms) de las 10 mallas da el número de normales de
	 * del modelo (model_t::numnorms). Por lo tanto, se deduce que la
	 * primera malla sólo usará las mesh_t::numnorms primeras normales,
	 * la segunda las siguientes, y así sucesivamente. Esto, sin embargo,
	 * no lo hemos comprobado.

	 * Número de normales de la malla. El contenido de este campo es pura
	 * especulación. Un mesh_t está contenido en un model_t, que puede
	 * contener simultáneamente varias mallas (mesh_t). Un model_t tiene
	 * una lista de normales, y, por lo tanto, un campo con el número de
	 * normales que contiene, al igual que ocurre con los vértices. Las
	 * mallas (mesh_t) referencian a esa lista de vértices y de normales
	 * para definir los triángulos que la forman (ver mesh_t::triindex).
	 * <br>
	 * El caso de los vértices es el normal. Las mallas referencian
	 * directamente un índice de la lista de vértices. Sin embargo, con las
	 * normales, se han complicado un poco más. Da la impresión de que las
	 * normales de la lista de model_t <b>no</b> se comparten entre mallas
	 * como ocurre con los vértices. Es decir, si un model_t almacena,
	 * pongamos, 100 normales y tiene 4 mallas, las 20 primeras normales
	 * son referenciadas únicamente por la primera malla, las, digamos
	 * 30 siguientes por la segunda, las 25 siguientes por la tercera y
	 * las restantes por la cuarta. <br>
	 * Esta división es bastante artificial. Parece que la hicieron para
	 * facilitar el trabajo al módulo que pinta un modelo, con el trato
	 * de las texturas cromadas. <p>
	 * Como se ha dicho, todo lo anterior no está comprobado, pero se deduce a
	 * partir del código usado como base para construir esta librería, y del
	 * análisis realizado sobre todos los modelos de juego original, en los
	 * que se cumple que la suma del campo numnorms de todas las mallas
	 * de un submodelo da el mismo valor que el campo model_t::numnorms
	 */
	MDLuint numnorms;

	/**
	 * Posición en el fichero del array de vec3_t con las normales. En
	 * realidad este campo siempre está a cero en todos los modelos del
	 * juego original. Las normales de una malla se guardan en la información
	 * del modelo (model_t) al que pertenecen, por lo que este campo
	 * no parece tener sentido.
	 */
	ptr_t normindex;

} mesh_t;

/**
 * Información generica de una textura.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Nombre de la textura. Normalmente es el nombre del fichero del que
	 * procede.
	 */
	MDLchar name[64];

	/**
	 * Características de la textura. No estoy seguro de qué esto exactamente,
	 * aunque sospecho que lo que indica es el modo en el que debe dibujarse
	 * la malla que utilice esta textura. Los posibles valores son los
	 * especificados en el enumerado HalfLifeMDL::textureFlags_t, que pueden
	 * usarse simultáneamente uniendolos con ORs a nivel de bits.
	 */
	MDLint flags;

	/**
	 * Número de píxeles de ancho.
	 */
	MDLuint width;

	/**
	 * Número de píxeles de alto.
	 */
	MDLuint height;

	/**
	 * Posición en el fichero donde están los píxeles de la imagen. Primero
	 * van width*height bytes, cada uno para un pixel. Después va la paleta
	 * en RGB. Los píxeles van de izquierda a derecha, y de abajo a arriba.
	 */
	ptr_t index;

} texture_t;

/**
 * Información sobre un controlador de hueso. Aunque las secuencias del
 * modelo especifican los desplazamientos y rotaciones que los huesos deben
 * sufrir para ejecutar una animación, es posible retocar ligeramente
 * alguno de los grados de libertad (HalfLifeMDL::DoF_t) de alguno de los
 * huesos para para adaptarlo a los requerimientos del programa. Esto se 
 * realiza mediante controladores de huesos, que se asocian a un hueso y a un
 * grado de libertad, y modifican ese grado de libertad para ese hueso,
 * sumando o restando una cantidad al valor especificado para ese
 * grado de libertad por la animación que se está ejecutando.<br>
 * Quizá el ejemplo más sencillo sea la asociación de un controlador
 * a un hueso cuya rotación crea el efecto de abrir y cerrar la boca.
 * En todas las animaciones ese hueso estará en la posición que supone
 * la boca cerrada, pero tendrá asociado un controlador. Durante la
 * ejecución de cualquier secuencia, el programa puede modificar el
 * controlador para crear el efecto de que el personaje está hablando.
 * Un segundo ejemplo es asociar un controlador a la rotación del hueso
 * del cuello, para que el programa pueda hacer al personaje girar la
 * cabeza mientras corre, sin que el animador tenga que crear una
 * animación de correr por cada posible dirección de la mirada.<p>
 * Naturalmente, para que esto tenga sentido, debe existir coherencia
 * entre el modelo y el código que lo controla. Los modelos de Half Life
 * no obligan a la existencia de controladores determinados, por lo que
 * un código que suponga que existe un controlador para la boca tendrá
 * que manejar un modelo que lo tenga.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct {

	/**
	 * Hueso al que está asociado el controlador. El hueso 0 es el
	 * primer hueso del array de huesos del modelo. Un valor de
	 * -1 indica que el controlador no tiene asociado hueso.
	 * @todo Asegurarse de esto.
	 */
	MDLint bone;	// -1 == 0

	/**
	 * Grado de libertad al que está asociado el hueso. Toma los valores
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
 * Bounding box que almacena una caja de intersección.
 * @todo no sé para qué se usa esto. ¿Averiguarlo y comentarlo más?
 * @author Pedro Pablo Gómez Martín
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
 * Información sobre un grupo de secuencias.
 * @see @ref secuenciasYGrupos
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Nombre del grupo de secuencias. Generalmente, para el grupo 0
	 * (representando las animaciones del fichero actual) se obtiene el
	 * nombre <tt>default</tt>. Para los grupos de los ficheros externos
	 * este campo suele estar vacío.
	 */
	MDLchar label[32];

	/**
	 * Nombre del fichero donde están las animaciones. Generalmente para
	 * el grupo 0 (representando las animaciones del gurpo actual) se
	 * obtiene la cadena vacía. Para los grupos de los ficheros externos
	 * este campo suele contener el nombre <tt>modelXX.mdl</tt>, si el
	 * fichero de la geometría es <tt>model.mdl</tt> y XX es la
	 * representación con dos dígitos decimales del número del grupo.<br>
	 * Sin embargo, normalmente no se utilizará directamente este nombre
	 * para buscar el fichero, pues suele contener información sobre la
	 * ruta que no siempre es válida.
	 */
	MDLchar name[64];

	/**
	 * Campo que hace las veces de <EM>tag</EM> para que la aplicación
	 * guarde el valor que desee. En realidad el formato está muy ligado al
	 * cargador, de modo que el cargador original mantenía directamente todo
	 * el fichero en memoria sin convertirlo a otra estructura de datos. Como
	 * se supone que los grupos de secuencia están pensados para la carga
	 * bajo demanda, este campo permite al cargador almacenar cualquier
	 * valor con información personalizada sobre el estado de la carga de
	 * este grupo. Es un campo de 4 bytes.<p>
	 * Esta librería no lo utiliza.
	 */
	cache_user_t cache;

	/**
	 * Truco para facilitar la obtención de la posición de las animaciones
	 * del grupo de secuencia 0 dentro del fichero actual. Este campo no
	 * se utiliza si el grupo no es el 0. Si lo es, la posición de los
	 * datos de las animaciones de todas las secuencias que lo referencien
	 * será la suma de este campo data, y el campo seqdesc_t::animindex de
	 * las secuencias.<p>
	 * De todas formas, el análisis de todos los ficheros de HalfLife
	 * muestra que este campo está en realidad siempre a 0.
	 * @see HalfLifeMDL::seqdesc_t::animindex.
	 */
	MDLint data;

} seqgroup_t;

/**
 * Contenido de la información sobre una secuencia del modelo.
 * @see @ref secuenciasYGrupos
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct {

	/**
	 * Nombre de la secuencia. Se establece en el fichero QC usado para
	 * empaquetar el MDL a partir de los SMD.
	 */
	MDLchar label[32];

	/**
	 * Fotogramas por segundo de la reproducción.
	 */
	MDLfloat fps;

	/**
	 * Información adicional de la secuencia. Si es 0 no hay nada nuevo. Si es
	 * 1, la secuencia es un ciclo (por ejemplo el de andar).
	 * @todo Quizá se podría crear una constante aquí... pero en realidad
	 * puede usarse como booleano... :-D (STUDIO_LOOPING)
	 */
	MDLint flags;

	/**
	 * Identificador de la secuencia. Es el entero asociado a cada 
	 * identificador de las animaciones estándar que se puede
	 * establecer en el fichero QC (ACT_ARM, ACT_RUN...). El enumerado
	 * HalfLifeMDL::actionTags_t especifica los posibles valores.
	 */
	MDLint activity;

	/**
	 * "Peso" de esta secuencia para la actividad seqdesc_t::activity.
	 * Un mismo modelo puede tener varias secuencias para la misma actividad,
	 * y la parte del motor gráfico que se encarga de controlar al modelo
	 * puede establecer aleatoriamente una cualquiera cuando desde fuera
	 * se le pide que establezca una secuencia proprcionando el identificador
	 * de secuencia estándar. Esa elección aleatoria se puede ajustar dando
	 * un peso a cada versión de la secuencia estándar, para dar más
	 * probabilidad a unas que a otras. Ese peso es almacenado en este campo,
	 * que no tendrá sentido si la secuencia no es estándar. Si sólo hay
	 * una secuencia para esta actividad, su valor tampoco tendrá mucho
	 * sentido, pero será diferente de 0.
	 */
	MDLint actweight;

	/**
	 * Número de eventos de la secuencia.
	 */
	MDLint numevents;

	/**
	 * Posición en el fichero donde empieza la información de los eventos
	 * (array de event_t).
	 */
	ptr_t eventindex;

	/**
	 * Número de fotogramas de la secuencia. Si la secuencia es un ciclo,
	 * el último fotograma de su (o sus) animacion(es) debe ser exactamente
	 * igual que el primero. Este contador incluye también ese fotograma
	 * repetido.
	 */
	MDLint numframes;

	/**
	 * "Number of foot pivots". No tengo ni idea de qué es esto. En cualquier
	 * caso ningún modelo contenido en el juego original parece hacer uso de
	 * esto (todos los MDL lo tienen a 0). Tampoco he encontrado información
	 * sobre cómo establecer este campo desde el fichero QC. Por tanto,
	 * sospecho que este campo es un claro ejemplo de "Buenas ideas para
	 * con un poco de suerte utilizar en alguna versión futura".
	 */
	MDLint numpivots;

	/**
	 * Posición en el fichero donde empieza la información de los pivotes.
	 * En realidad el contenido de este valor no es seguro. De todas formas
	 * si depende de numpivots, su valor seguramente no se use (pues en
	 * todos los modelos consultados, numpivots está a 0).
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
	 * del juego original parecen tener aquí un 0, referenciando al hueso 0
	 * que siempre es el hueso raíz o padre del que cuelgan todos los demás.
	 */
	MDLint motionbone;

	/**
	 * Desplazamiento del hueso padre (¿¿ seqdesc_t::motionbone ??) entre el
	 * primer fotograma y el último de la secuencia. Cada coordenada del
	 * vector sólo es válida si seqdesc_t::motiontype tiene activo el bit
	 * especificado por HalfLifeMDL::MT_LX, HalfLifeMDL::MT_LY, o
	 * HalfLifeMDL::MT_LZ. Este desplazamiento debe ser interpolado a lo largo
	 * de toda la secuencia. Un típico ejemplo es el desplazamiento causado
	 * por un ciclo de andar. Para calcular el avance en el eje E en un lapso
	 * de tiempo de epsilon segundos, habrá que utilizar la formula:
	 * \f[
	 * avance\ en\ eje\ E = \frac{linearmovement[E] * fps * epsilon}
	 *		                      {numero\ de\ fotogramas\ de\ la\ secuencia}
	 * \f]
	 */
	vec3_t linearmovement;

	/**
	 * No utilizada. En todos los modelos del juego original está a 0. Por el
	 * nombre, se puede deducir que es la posición del fichero donde está la
	 * información del desplazamiento del modelo en el mundo cuando se utiliza
	 * el tipo de movimiento automático (HalfLifeMDL::MT_AX, 
	 * HalfLifeMDL::MT_AY, o HalfLifeMDL::MT_AZ), aunque esto no es más que
	 * pura especulación. En realidad, esos tipos de movimiento no se usan en
	 * ningún modelo, y si se usan, el empaquetador de MDL de las SDK de Half
	 * Life (StudioMDL) no parece inicializar este campo tampoco.
	 */
	MDLint automoveposindex;

	/**
	 * No utilizada. En todos los modelos del juego original está a 0. Por el
	 * nombre, se puede deducir que es la posición del fichero donde está la
	 * información de rotación del modelo en el mundo cuando se utiliza
	 * el tipo de movimiento automático (HalfLifeMDL::MT_AXR,
	 * HalfLifeMDL::MT_AYR, o HalfLifeMDL::MT_AZR), aunque esto no es más que
	 * pura especulación. En realidad, esos tipos de movimiento no se usan en
	 * ningún modelo, y si se usan, el empaquetador de MDL de las SDK de Half
	 * Life (StudioMDL) no parece inicializar este campo tampoco.
	 */
	MDLint automoveangleindex;

	/**
	 * Coordenada mínima del bounding box de la secuencia completa.Es un
	 * bounding box alineado con los ejes, donde el sistema de coordenadas
	 * utilizado es el utilizado para colocar la articulación raíz del
	 * modelo. Es decir, las coordenadas se dan en el mismo sistemas de
	 * coordenadas que el hueso padre. Ese sistema será establecido por el
	 * motor según dónde quiera colocar al personaje.<br>
	 * Si durante la secuencia de la animación el personaje es desplazado
	 * automáticamente por el motor (por ejemplo, el movimiento lineal en
	 * los ciclos de andar), las coordenadas del bounding box también
	 * deben "desplazarse" de la misma forma (pues estas coordenadas están
	 * en el sistema del hueso raíz que es el que se modifica para desplazar
	 * al personaje).<p>
	 * Si la animación no obliga a un desplazamiento del hueso raíz por parte
	 * del motor, pero éste se desplaza (por ejemplo en una animación en la
	 * que el personaje muere y cae a cierta distancia de la posición inicial)
	 * el bounding box es incluye a todas esas posiciones. Es decir, todos los
	 * estados por los que pasa el modelo durante la secuencia completa entran
	 * dentro del bounding box.
	 * @image html
	 * GafotasSubiendo.gif "Secuencia con desplazamiento de hueso principal"
	 * @warning Por alguna razón, el empaquetador de MDL no siempre construye
	 * un bounding box completamente correcto. En ocasiones, ciertas partes 
	 * del modelo salen fuera del bounding box de forma esporádica, como la
	 * punta de los dedos. Esto no sucede a menudo y, en cualquier caso, la
	 * desviación producida es muy leve.
	 * @image html ErrorBoundingBoxes.gif "Error de los bounding boxes"
	 */
	vec3_t bbmin;

	/**
	 * Coordenada máxima del bounding box de la secuencia completa. Para una
	 * mayor explicación, vease seqdesc_t::bbmin.
	 * @see seqdesc_t::bbmin
	 */
	vec3_t bbmax;

	/**
	 * Número de animaciones a partir de las que se crea la secuencia actual.
	 * Una secuencia puede formarse por varias animaciones sobre los huesos,
	 * y el motor mezclará esas animaciones dando un peso a cada una. El campo
	 * indica cuantas animaciones se usan para esta secuencia.
	 * @see @ref secuenciasYGrupos
	 */
	MDLint numblends;

	/**
	 * Indica la posición donde está la información de la animación o
	 * animaciones de la secuencia actual (puntero a un array de
	 * HalfLifeMDL::anim_t). Las animaciones de una secuencia
	 * pueden estar en el mismo fichero que la geometría, o en otro. Si
	 * el campo seqdesc_t::seqgroup de esta misma estructura guarda un 0,
	 * la animación está en el mismo fichero que la geometría, y la posición
	 * real de las animaciones de la secuencia dentro del fichero se obtiene
	 * sumando este campo animindex al campo seqgroup_t::data de la 
	 * información de la secuencia.<p>
	 * Si esta secuencia no está en el grupo 0, la animación o animaciones
	 * estarán en un fichero externo. La posición donde está la animación en
	 * ese fichero se indica en este campo (y el campo data de la información
	 * del grupo de secuencia no se utiliza).
	 * <p>
	 * En cualquier caso, una vez conseguida la posición real del comienzo del
	 * array de anim_t a partir de este campo, ese array es bidimensional:
	 * <p>
	 * <center><code>anim[blend][hueso]</code></center>
	 * <p>
	 * Cada elemento contiene la animación
	 * del hueso del segundo índice para la animación "blend" de la secuencia.
	 * Esto último se debe a que cada secuencia puede formarse por la mezcla
	 * de varias animaciones sobre los huesos, dando un peso a cada una. En
	 * este array se almacenan todas esas animaciones, de ahí la existencia
	 * del índice "blend". El tamaño de la dimensión <code>blend</code> viene
	 * dado por el campo seqdesc_t::numblends de esta misma estructura. El
	 * tamaño de la dimensión <code>hueso</code> viene dado por el número
	 * de huesos del modelo al que pertenece la animación.
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
	 * Número de grupo donde está la animación (o animaciones) de la
	 * secuencia. Si el valor es 0, la animación está en el fichero actual.
	 * Si no, debería estar en el fichero cuyo nombre está en la información
	 * del grupo de secuencias seqgroup del fichero. En la práctica se
	 * obtiene del fichero <code>modelXX.mdl</code>, si <code>model.mdl</code>
	 * es el fichero actual y <code>XX</code> es el número de grupo (a partir
	 * de 01).
	 * @see @ref secuenciasYGrupos
	 */
	MDLint seqgroup;

	/**
	 * Uso desconocido. En todos los modelos de Half Life este campo
	 * está a 0. Quizá tenga relación con seqdesc_t::nextseq para hacer
	 * transiciones de secuencias con mezclado de forma automática.
	 * <p>
	 * La "documentación" original del campo era "transition node at entry".
	 */
	MDLint entrynode;

	/**
	 * Uso desconocido. En todos los modelos de Half Life este campo
	 * está a 0. Quizá tenga relación con seqdesc_t::nextseq para hacer
	 * transiciones de secuencias con mezclado de forma automática.
	 * <p>
	 * La "documentación" original del campo era "transition node at exit".
	 */
	MDLint exitnode;

	/**
	 * Uso desconocido. En todos los modelos de Half Life este campo
	 * está a 0. Quizá tenga relación con seqdesc_t::nextseq para hacer
	 * transiciones de secuencias con mezclado de forma automática.
	 * <p>
	 * La "documentación" original del campo era "transition rules".
	 */
	MDLint nodeflags;

	/**
	 * Uso desconocido. Por el nombre del campo, se puede intuir que almacena
	 * el número de la secuencia que debe reproducirse cuando la actual
	 * termine. Por ejemplo, para la animación de "morir", la secuencia
	 * siguiente podría ser "tendido en el suelo", de modo que el propio
	 * motor podria actualizar la secuencia actual del modelo para ahorrar
	 * trabajo a su controlador.<p>
	 * En el análisis realizado a todos los modelos originales del juego Half
	 * Life, este campo siempre está a cero. Tampoco he encontrado información
	 * sobre cómo establecer este campo desde el fichero QC. Por tanto,
	 * sospecho que este campo es un claro ejemplo de "Buenas ideas para
	 * con un poco de suerte utilizar en alguna versión futura".
	 * <p>
	 * La "documentación" original del campo era "auto advancing sequences".
	 * @todo Si se hace una página sobre cómo se empaqueta un MDL, 
	 * referenciarla aquí.
	 */
	MDLint nextseq;

} seqdesc_t;

/**
 * Información de eventos en las secuencias. Cada secuencia puede tener
 * eventos asociados, que suceden en fotogramas concretos (establecidos
 * en el fichero .qc). El evento que salta se especifica con un entero.
 * Se supone que el código asociado al modelo sabrá qué es ese evento.
 * Algunos son estandar (por ejemplo, momento en el que el personaje
 * muere en la animación de morir), y otros dependen de la entidad.
 * Algunos pueden tener parámetros (por ejemplo, la ruta del .wav a
 * reproducir).
 * Las estructuras de tipo seqdesc_t contienen un "puntero" a
 * un array de esta estructura.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct {

	/**
	 * Fotograma en el que sucede el evento en la secuencia
	 * (seqdesc_t) a la que esté asociada el evento.
	 */
	MDLuint frame;

	/**
	 * Código del evento. Algunos son "estándar". Por ejemplo el valor
	 * 1000 indica el fotograma en el que el personaje muere. La lista
	 * está en el docuento "Modeling for Half-Life.doc" proporcionado
	 * junto con las SDK de Half Life.<br>
	 * A parte, se pueden asignar cualquier valor, con tal de que el código
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
	 * reproducir. El contenido depende del evento, y es el código
	 * asociado al modelo el que le da el sentido a la cadena.
	 */
	MDLchar options[64];

} event_t;


/**
 * Pivotes ("pivots" ¿¿"foot pivots"??). No sé qué son. Todos los modelos
 * consultados indican que el número de pivotes es 0, y tampoco hay
 * información sobre cómo establecerlos desde el QC.
 * <p>
 * Sospecho que este campo es un claro ejemplo de "Buenas ideas para
 * con un poco de suerte utilizar en alguna versión futura".
 * @author Pedro Pablo Gómez Martín
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
 * "Puntos de unión" del modelo (por ejemplo pistolas).
 * El modelador establece la posición de esos puntos (que no se utilizan para dibujar),
 * y el código que controla a ese personaje cuenta con ellos para saber dónde colocar
 * el arma, o cualquier otra cosa. Puede haber hasta 4 (creo) puntos de estos, para,
 * por ejemplo, distintos tipos de armas. La información sobre cada uno de estos
 * puntos de unión se guarda en esta estructura.
 * @todo Analizar esto más para quitar el "creo". Comentar los elementos.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct 
{
	/**
	 * Nombre del punto de unión. En todos los ficheros del juego este
	 * campo está vacío. En realidad no hay forma de especificar nombre
	 * desde el fichero <code>QC</code>, al menos según la documentación
	 * oficial de las SDK.
	 */
	MDLchar name[32];

	/**
	 * Tipo del punto de unión. En todos los ficheros del juego este
	 * campo vale 0. En realidad no hay forma de especificar nombre
	 * desde el fichero <code>QC</code>, al menos según la documentación
	 * oficial de las SDK.
	 */
	MDLint type;

	/**
	 * Hueso del modelo al que está asociado este punto de unión.
	 */
	MDLint bone;

	/**
	 * Desplazamiento del punto de control respecto al hueso del que
	 * depende. Las coordenadas están, como no podía ser de otra forma
	 * en el sistema de coordenadas del hueso.
	 */
	vec3_t org;

	/**
	 * Uso desconocido. En todos los modelos del juego original estos
	 * vectores tienen sus coordenadas a 0. Además no parece haber forma
	 * de establecer valores desde el fichero <code>QC</code>. Tal vez
	 * están metidos para una ampliación futura. El único sentido que veo
	 * a esa ampliación es que sea la matriz de rotación. A juzgar por
	 * el código del visor usado de referencia para entender el formato del
	 * fichero, esto tal vez sea eso pero un poco cambiado. Ese visor
	 * dibuja líneas que van desde org hasta estos tres nuevos vértices,
	 * considerando todos en el sistema de coordenadas del hueso. Si
	 * consideramos que esas líneas dibujarían el nuevo sistema de referencia
	 * (cosa que no puede comprobarse, en cualquier caso), estos
	 * vértices tendrían que ser modificados (restando org) para obtener
	 * la matriz (o algo por el estilo).
	 */
	vec3_t vectors[3];

} attachment_t;

/**
 * Almacena la información de una animación sobre un hueso del modelo.
 * @see HalfLifeMDL::seqdesc_t::animindex
 * @see @ref secuenciasYGrupos
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef struct {
	/**
	 * Array con la información de la animación de cada grado de libertad
	 * del hueso para el que la estructura almacena la animación.
	 * Es un array con seis posiciones, una para cada grupo de libertad.
	 * En lugar de acceder directamente con índices númericos, se debería
	 * utilizar, para mayor claridad, los valores del enumerado
	 * HalfLifeMDL::DoF_t.
	 * <p>
	 * Si para un determinado grado de libertad el valor es 0, el hueso
	 * cuya animación estamos guardando no cambiará su valor por defecto
	 * para ese grado de libertad durante toda la animación. Por tanto,
	 * durante toda la animación el hueso tendrá en el grado de libertad
	 * el valor que puede consultarse en la información del hueso
	 * (HalfLifeMDL::bone_t::value).
	 * <p>
	 * Si el valor no es cero, se debe sumar a la posición donde comienza
	 * el array el valor leído para llegar a los datos de la animación
	 * en ese grado de libertad, que será un array de
	 * HalfLifeMDL::animvalue_t. Es decir, si tenemos en la variable
	 * <code>anim</code> un puntero a una de estas estructuras anim_t,
	 * estaremos en la siguiente situación:
	 * <br>
	 * <table border="0" cellspacing="1" cellpadding="1" align="center">
	 * <tr>
	 * <td>
	 * <pre>
	 * anim_t* anim;
	 * // [...]
	 * if (anim.offset[dof] == 0) {
	 *	// No hay animación sobre ese grado de libertad (dof).
	 *	// El valor de ese grado de libertad es constante en
	 *	// toda la animación y se obtiene de la información del
	 *	// hueso cuya animación almacena el puntero anim.
	 * }
	 * else {
	 *	// Si hay animación. Tenemos que obtener el array de
	 *	// animvalue_t:
	 *	animvalue_t* animacion;
	 *	animacion = (animvalue_t*)((char*) anim) + anim.offset[dof];
	 * }
	 * </pre>
	 * </td>
	 * </tr>
	 * </table>
	 * El array obtenido en el segundo caso contiene la información necesaria
	 * para obtener el valor del grado de libertad para ese hueso en esta
	 * animación, a partir del fotograma actual.
	 * <br>
	 * @see HalfLifeMDL::seqdesc_t::animindex
	 * @see @ref secuenciasYGrupos
	 * @see HalfLifeMDL::animvalue_t
	*/
	MDLushort offset[6];

} anim_t;

// animation frames
/**
 * Un array de estas estructuras almacena, de forma comprimida, una animación
 * de un grado de libertad de un hueso del modelo.
 * <p>
 * La forma cómoda de almacenar una animación para un grado de libertad de
 * un hueso sería con un array que contuviera una entrada por cada fotograma
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
 * @todo Paso de perder el tiempo. Hay que pasar la explicación
 * que hice en el cuaderno sobre todo esto.
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */
typedef union {

	/** 
	 * Información almacenada por una entrada del array cuando el elemento
	 * se utiliza para la información sobre compresión.
	 */
	struct {
		/** Pendiente */
		MDLuchar valid;
		/** Pendiente */
		MDLuchar total;
	} num;
	/** 
	 * Información almacenada por una entrada del array cuando el elemento
	 * se utiliza para dato de animación. Almacena el valor <b>a sumar</b>
	 * al valor por defecto del hueso para el grado de libertad cuya
	 * animación almacena el array al que pertenece este elemento.
	 */
	MDLshort value;	// Valor a SUMAR al valor por defecto.

} animvalue_t;

} // namespace HalfLifeMDL


/**
 * @page secuenciasYGrupos Secuencias, animaciones y grupos de secuencias.
 * El formato del fichero MDL mantiene secuencias, grupos de secuencias y
 * animaciones, algo confuso a menudo. La finalidad perseguida al mantener
 * esta separación es doble:
 *
 *	-# Ahorrar trabajo a los diseñadores gráficos, que pueden reutilizar
 * animaciones (serie de movimientos) más de una vez.
 *	-# Añadir la posibilidad de partir un modelo en varios ficheros,
 * manteniendo fuera del fichero de la geometría las animaciones (al igual
 * que puede hacerse con las texturas).
 * 
 * Se entiende por secuencia a toda la información de una serie de
 * movimientos del modelo. Ejemplos de secuencia son el ciclo de andar,
 * el movimiento de salto, etc. Esta información incluye el nombre de la
 * secuencia, el desplazamiento del personaje, la velocidad de reproducción
 * (fotogramas por segundo), etc. La información de una secuencia se almacena
 * en la estructura HalfLifeMDL::seqdesc_t en un fichero MDL.
 * <p>
 * Se denomina animación a la información concreta sobre el movimiento de los
 * huesos. Una secuencia tendrá asociada, al menos, una animación. Es posible
 * asociar más de una, y el motor gráfico mezclará ambas durante la ejecución,
 * dando un peso a cada una de ellas (para hacer una mezcla ponderada).
 * Para que esto sea posible, se obliga a que todas las animaciones de la
 * secuencia tengan el mismo número de fotogramas.<br>
 * Esta funcionalidad es útil para crear animaciones parciales. Por ejemplo el
 * diseñador puede crar una animación de disparar, que manipula la parte
 * superior del cuerpo, y una de correr y saltar que controlan la inferior.
 * Luego se crearían dos secuencias que mezclarían la animación de disparar
 * con cada una de las otras dos.<br>
 * Hay que tener cuidado con esto, porque esa animación mezcla no se crea
 * "al vuelo" durante la ejecución del programa (como ocurre con el formato
 * de personajes de Quake III, que están siempre divididos en tres partes), 
 * sino que se crea en el momento de empaquetar el fichero MDL.<br>
 * Además, con el formato MDL se pueden mezclar ambas animaciones
 * proporcionando un peso. Las animaciones de Quake III se referían a partes
 * del cuerpo disjuntas, y no había conflictos. Ahora las animaciones afectan
 * a todo el cuerpo, por lo que la mezcla requiere un peso para cada
 * una. Eso permite, hipotéticamente, mezclar una animación de andar y otra
 * de correr, y que el motor incremente el peso de una y decremente el de otra
 * con el tiempo para mostrar la transición de un estado a otro suavemente.
 * Como ya se ha dicho, para poder hacer esto, ambas animaciones deben tener
 * el mismo número de fotogramas, y debe existir una secuencia en el modelo
 * que las relacione.
 * <p>
 * La separación entre animaciones y secuencias permite una reutilización
 * adicional. Si se dispone de un ciclo de andar, éste puede utilizarse
 * en la secuencia de andar, y también en la de correr, con tal de especificar
 * en la secuencia de correr una velocidad de reproducción más rápida. En el
 * fichero MDL resultante, las dos secuencias apuntarán a la misma información
 * sobre animaciones. En esta librería, sin embargo, cuando lee y analiza
 * el fichero, creará dos objetos de animación diferente. Eso consume más
 * memoria, pero facilita la implementación.
 * <p>
 * Los grupos de secuencia permiten controlar las <EM>animaciones</EM>
 * almacenadas en ficheros diferentes. En realidad el nombre es confuso, pues
 * debería denominarse "grupos de animaciones". Un grupo de secuencias
 * agrupa la información de las animaciones contenidas en un fichero. La idea
 * es que el fichero de la geometría posee un array de grupos de secuencias,
 * cada una representando las animaciones de un fichero
 * (HalfLifeMDL::header_t::seqgroupindex). El elemento 0 se refiere a las
 * animaciones del fichero de la geometría. El resto guarda información de
 * los ficheros externos con animaciones del modelo actual. Lo que se
 * pretende es que el programa cargue las animaciones "bajo demanda", según
 * se necesiten, para ahorrar memoria. En la práctica, esta librería no hará
 * eso, y todos los ficheros externos con animaciones deberían ser cargados al
 * principio.<br>
 * Por norma general, si se lee el modelo de geometría del fichero
 * <tt>model.mdl</tt>, y se detecta que el número de grupos de secuencias es
 * mayor que 1, la información general de cada grupo de secuencia estará
 * siempre en el propio fichero <tt>model.mdl</tt>. Sin embargo, las
 * animaciones asociadas al grupo <tt>n</tt> estarán en el fichero
 * <tt>modelXX.mdl</tt>, siendo XX el número <tt>n</tt> representado con dos
 * dígitos decimales.<p>
 * La información guardada por cada grupo de secuencia es muy pequeña, y
 * relativamente poco importante. Vease HalfLifeMDL::seqgroup_t para más
 * información.<p>
 * Si un fichero indica que no tiene ningún grupo de secuencia, ni siquiera
 * existirá el grupo de secuencias del fichero de la geometría. En ese caso
 * el campo HalfLifeMDL::header_t::numseq será inválido y debe considerarse 0.
 * Esto ocurre únicamente con los ficheros <tt>modelXX.mdl</tt> anteriormente
 * descritos.
 * @todo Si se hace otra página de estas con la creación de un modelo,
 * meter un enlace.
 * @see HalfLifeMDL::seqdesc_t, HalfLifeMDL::seqgroup_t, HalfLifeMDL::header_t
 */

#endif