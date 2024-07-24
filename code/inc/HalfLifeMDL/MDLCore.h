//---------------------------------------------------------------------------
// MDLCore.h
//---------------------------------------------------------------------------

/**
 * @file MDLCore.h
 * Contiene la declaración de la clase básica que almacena la información
 * de un modelo de Half Life (HalfLifeMDL::MDLCore). Esta clase tiene
 * métodos para consultar la información, como el número de texturas,
 * animaciones, etc.<p>
 * Desde el punto de vista del usuario final del namespace HalfLifeMDL,
 * seguramente lo más práctico es utilizar la clase
 * HalfLifeMDL::MDLInstance. Esta clase sólo se utilizará para inicializar
 * una nueva instancia.<p>
 * Hace uso contínuo de las estructuras definidas en MDLBasic.h.
 * @see HalfLifeMDL::MDLCore
 * @see HalfLifeMDL::MDLInstance
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 */

#ifndef __HalfLifeMDL_MDLCore
#define __HalfLifeMDL_MDLCore

#include <assert.h>
#include <vector> 

#include "MDLBasic.h"
#include "MDLBone.h"
#include "MDLTexture.h"
#include "MDLSequence.h"
#include "MDLBodyPart.h"
#include "MDLAttachment.h"
#include "MDLSkins.h"
#include "MDLDefinesDLLClass.h"

namespace HalfLifeMDL {

/**
 * Clase básica que almacena la información (estática) de un modelo de Half
 * Life. Guarda únicamente la información estática, por ejemplo el número de
 * texturas y de secuencias, pero no información dinámica como el
 * fotograma actual, o la piel seleccionada.<p>
 * Aunque el formato MDL permite la carga de animaciones bajo demanda
 * (si estas se almacenan en ficheros diferentes al archivo con la
 * geometría), esta clase no soporta esa posibilidad y toda la información
 * debe recopilarse al principio (vea @ref secuenciasYGrupos para más
 * información sobre esto).<p>
 * La inicialización de los objetos de esta clase se realiza mediante
 * los métodos MDLCore::setMainFile, MDLCore::setTextureFile y
 * MDLCore::setSeqGroupFile. El primero se utiliza para proporcionar el
 * contenido completo del fichero MDL cuyo modelo se quiere controlar
 * con el objeto. Los otros dos sólo serán necesarios si el fichero
 * proporcionado no tiene las texturas, y estas se almacenan en un
 * fichero separado (habrá que llamar a setTextureFile), o si el
 * archivo de la geometría no tiene todas las secuencias de animación
 * soportadas por el modelo (habrá que llamar a setSeqGroupFile).
 * Ambos métodos también reciben el fichero leído completamente en
 * el buffer pasado.
 * <p>
 * Half Life utiliza un convenio de nombres de archivo para buscar las
 * texturas y animaciones externas al fichero de geometría en caso de
 * ser necesarias. Si el archivo de la geometría se denomina
 * <tt>model.mdl</tt> y se detecta que no posee texturas, éstas se
 * leerán del fichero <tt>modelT.mdl</tt>. Del mismo modo, si 
 * <tt>model.mdl</tt> indica que necesita otros dos ficheros con
 * secuencias de animación adicionales, se supondrá que sus nombres son
 * <tt>model01.mdl</tt> y <tt>model02.mdl</tt>.
 * <p>
 * Una vez llamado a los métodos de inicialización, es necesario llamar
 * a MDLCore::pack para que el objeto procese toda la información y
 * la organice para almacenarla de un modo más cómodo para el usuario.
 * Hasta que no se llame al método anterior, el objeto no podrá ser
 * utilizado.
 * <p>
 * Según lo anterior, el esquema del código que requiere esta clase para
 * ser inicializada utilizando el <EM>convenio de nombres</EM> usado por
 * Half Life sería:
 * <p>
 * @include HalfLifeMDL/InicializacionMDLCore.cpp
 * <p>
 * En realidad el usuario de esta clase no necesita implementar esto, pues
 * ya se proporciona una implementación en HalfLifeMDL::MDLReader. Se detalla
 * aquí como ejemplo, por si se quisiera cargar un modelo a partir de un
 * archivo encapsulado y/o encriptado.
 * <p>
 * Tras la llamada a MDLCore::pack, es posible acceder a toda la
 * información estática del modelo con los métodos proporcionados para
 * ello. Esa información incluye las texturas. Típicamente éstas serán
 * enviadas a la librería gráfica, y no tiene sentido que siguan
 * almacenándose consumiendo memoria. Es posible liberar el espacio
 * ocupado llamando al método MDLCore::freeTextures(). Tras esta llamada,
 * la información sobre la imagen de las texturas dejará de estar disponible.
 * <p>
 * Esta clase juega un papel primordial dentro del paquete HalfLifeMDL. En
 * concreto, hace de puente entre las estructuras de datos almacenadas en
 * los ficheros MDL (definidas en MDLBasic.h), y las clases que se hacen
 * públicas al exterior. La conversión de unas a otras se realiza en el
 * código de MDLCore::pack.
 * <p>
 * Como ya se ha dicho, esta clase almacena la información estática de
 * un modelo, lo común es que los programas requieran almacenar información
 * dinámica, como el fotograma actual, o la secuencia que se está
 * reproduciendo en un determinado momento. Esa labor la realiza la clase
 * <b>HalfLifeMDL::MDLInstance</b>, que guarda una <EM>instancia</EM>. Dicha
 * clase es más práctica desde el punto de vista del usuario, evitandole el
 * trabajo, por ejemplo, de interpolar posiciones entre fotogramas.
 * @remarks Desde el punto de vista del usuario final, es más práctico
 * utilizar la clase MDLInstance.
 * @see HalfLifeMDL::MDLInstance, HalfLifeMDL::MDLReader,
 * @ref secuenciasYGrupos
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 * @todo Si se hace una página con la "arquitectura", hacer un enlace aquí y
 * en uno de los párrafos anteriores de la documentación.
 */
class MDL_PUBLIC MDLCore {
/**
 * @example HalfLifeMDL/InicializacionMDLCore.cpp
 * Ejemplo de inicialización de un objeto de la clase HalfLifeMDL::MDLCore.<p>
 * En realidad el usuario de esta clase no necesita implementar esto, pues ya
 * se proporciona una implementación en HalfLifeMDL::MDLReader. Se detalla
 * aquí como ejemplo, por si se quisiera cargar un modelo a partir de un
 * archivo encapsulado y/o encriptado.<p>
 * @see HalfLifeMDL::MDLCore
 * @author Pedro Pablo Gómez Martín
 * @date Octubre, 2003.
 * <p>
 */

public:

	/**
	 * Método que busca cosas en el fichero. Es un método de depuración
	 * usado para analizar modelos e intentar deducir qué significan algunos
	 * campos.
	 * @todo Eliminar el método :-) También habría que quitar el #include del
	 * cpp para poder escribir por pantalla en este método.
	 */
	void analizar();

	/**
	 * Constructor sin parámetros. Inicializa todos los punteros a
	 * NULL.
	 */
	MDLCore();

	/**
	 * Destructor. Libera toda la memoria utilizada.
	 */
	~MDLCore() { freeModel(); }

	//---------------------------------------------------------------
	// Métodos de consulta de información general del modelo.
	//---------------------------------------------------------------

	/**
	 * Devuelve el nombre del modelo. Típicamente es el nombre del fichero
	 * del que proviene el modelo. Suele incluir información de ruta, a veces
	 * con '\' y otras con '/'. En cualquier caso, la ruta no es muy fiable.
	 * <p>
	 * En realidad el nombre se extrae del contenido del fichero MDL
	 * original, no del nomre del fichero real a partir del que se leyó.
	 * El nombre almacenado contiene, por lo tanto, una ruta relativa
	 * según la estructura de directorios cuando se creó el modelo, de
	 * ahí que sea poco ilustrativo.
	 * Este método sólo puede ser llamado después de invocar a pack.
	 * @return Nombre del modelo. Suele tener el aspecto de un nombre de
	 * fichero, incluyendo, quizá, una ruta que suele ser inválida.
	 */
	const std::string& getName() const { return _name; }

	/**
	 * Devuelve el Bounding Box alineado con los ejes del modelo en su
	 * posición por defecto. En realidad este método no funciona, porque
	 * los campos que contienen esta información en el fichero no se
	 * inicializan (vease, por ejemplo, HalfLifeMDL::header_t::bbmin).<p>
	 * Este método sólo puede ser llamado después de invocar a pack.
	 * @param bbmin Parámetro de salida con la esquina inferior del bounding
	 * box alineado con los ejes.
	 * @param bbmax Parámetro de salida con la esquina superior del bounding
	 * box alineado con los ejes.
	 * @todo Tal vez podría mejorarse pack para que calcualra los valores,
	 * ya que no lo hace StudioMDL. Si se hace, modificar la documentación
	 * de _bbmin y _bbmax en esta clase.
	 */
	void getBoundingBox(MDLPoint& bbmin, MDLPoint& bbmax) const {
					assert(_packInvoked);
					bbmin = _bbmin; bbmax = _bbmax; }

	/**
	 * Devuelve el número de huesos del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Número de huesos del modelo.
	 */
	unsigned int getNumBones() const {
						assert(_packInvoked);
						return _numBones; }

	/**
	 * Devuelve un puntero al array con la información de los huesos.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Vector con la información de los huesos del modelo.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLBone** getBones() const {
						assert(_packInvoked);
						assert(_bones);
						return (const MDLBone**) _bones; }

	/**
	 * Devuelve el puntero al hueso solicitado.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @param bone Índice del hueso que se solicita.
	 * @return Hueso solicitado.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLBone* getBone(unsigned int bone) const {
						assert(_packInvoked);
						assert(_bones);
						assert(bone < _numBones);
						return (const MDLBone*) _bones[bone]; }

	/**
	 * Devuelve el número de controladores de huesos del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Número de controladores de hueso del modelo.
	 */
/*	unsigned int getNumBoneControllers() {
						assert(_packInvoked);
						return _header->numbonecontrollers; }
*/
	/**
	 * Devuelve un puntero al array con la información de los 
	 * controladores de huesos.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Vector con la información de los controladores de
	 * huesos del modelo.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const bonecontroller_t* getBoneControllers() {
						assert(_packInvoked);
						assert(_header->bonecontrollerindex);
						return (bonecontroller_t*)
								((MDLchar*)_header +
										_header->bonecontrollerindex); }

	// hitboxes
	// secuencias
	// grupos de secuencias

	/**
	 * Devuelve el número de secuencias del modelo. <p>
	 * El modelo debe haberse inicializado correctamente antes de usar este
	 * método, y debe haberse llamado a pack().
	 * @return Número de secuencias del modelo.
	 * @see @ref secuenciasYGrupos
	 */
	unsigned int getNumSequences() const {
						assert(_packInvoked);
						return _numSequences; }

	/**
	 * Devuelve la información de una secuencia del modelo. <p>
	 * El modelo debe haberse inicializado correctamente antes de usar este
	 * método, y debe haberse llamado a pack().
	 * @param sequence Índice de la secuencia. No se comprueba que el valor
	 * esté dentro del rango (salvo en modo depuración con assert). Un valor
	 * inválido ocasionará un resultado indeterminado. La primera secuencia
	 * tiene índice 0.
	 * @return Secuencia solicitada. El puntero no debe liberarse. El objeto
	 * se encarga de la gestión de la memoria.
	 */
	const MDLSequence* getSequence(unsigned int sequence) const {
						assert(_packInvoked);
						assert(sequence < _numSequences);
						assert(_sequences);
						return _sequences[sequence]; }

	/**
	 * Devuelve el número de grupos de secuencias del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de usar
	 * este método (al menos haber llamado a MDLCore::setMainFile), y
	 * no debe haberse llamado a pack().
	 * @return Número de grupos de secuencias del modelo.
	 * @see @ref secuenciasYGrupos
	 */
	unsigned int getNumSeqGroups() const {
						assert(!_packInvoked);
						assert(_header);
						return _header->numseqgroups; }

	/**
	 * Devuelve el número de texturas del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método. Funciona antes y después de llamar a
	 * MDLCore::pack.
	 * @return Número de texturas del modelo.
	 */
	unsigned int getNumTextures() const {
						if (!_packInvoked) {
							assert(_header);
							return _textureHeader->numtextures;
						}
						else
							return _numTextures; }

	/**
	 * Devuelve un puntero a la textura solicitada. Es válido para
	 * objetos constantes, y la textura recibida también será
	 * constante (no se puede liberar ni cambiar el tag).<p>
	 * El modelo debe haberse inicializado correctamente (y llamado a
	 * pack()) antes de usar este método.
	 * @param texture Índice de la textura solicitada.
	 * @return Puntero a la textura pedida.
	 */
	const MDLTexture* getTexture(unsigned int texture) const {
						assert(_packInvoked);
						assert(texture < _numTextures);
						return _textures[texture]; }

	/**
	 * Devuelve un puntero a la textura solicitada. Este método es para
	 * objetos no constantes, y devuelve una textura no constante a la que
	 * se le puede cambiar el tag o que puede liberarse. <p>
	 * El modelo debe haberse inicializado correctamente (y llamado a
	 * pack()) antes de usar este método.
	 * @param texture Índice de la textura solicitada.
	 * @return Puntero a la textura pedida.
	 */
	MDLTexture* getTexture(unsigned int texture) {
						assert(_packInvoked);
						assert(texture < _numTextures);
						return _textures[texture]; }

	/**
	 * Devuelve el número de texturas que hay en cada piel del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Número de texturas de cada piel del modelo.
	 */
	unsigned int getNumTexturesInSkin() const {
						assert(_packInvoked);
						assert(_skins);
						return _skins->getNumTexturesPerSkin(); }

	/**
	 * Devuelve el número de pieles que hay en el modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Número de pieles (de textura) del modelo.
	 */
	unsigned int getNumSkins() const {
						assert(_packInvoked);
						assert(_skins);
						return _skins->getNumSkins(); }


	/**
	 * Devuelve la información de las pieles del modelo. <p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Información de las pieles (de textura) del modelo.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLSkins* getSkins() const {
						assert(_packInvoked);
						assert(_skins);
						return _skins; }

	/**
	 * Devuelve el número de partes de las que se compone el modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Número de partes del modelo.
	 */
	unsigned int getNumBodyParts() const {
						assert(_packInvoked);
						return _numBodyParts; }

	/**
	 * Devuelve un puntero al array con la información de las
	 * partes del cuerpo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Vector con la información de las partes del modelo.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLBodyPart** getBodyParts() const {
						assert(_packInvoked);
						assert(_bodyParts);
						return (const MDLBodyPart**) _bodyParts; }

	/**
	 * Devuelve un puntero a la parte del cuerpo solicitada. <p>
	 * El modelo debe haberse inicializado correctamente antes de usar
	 * este método.
	 * @param bodyPart Parte del cuerpo solicitada.
	 * @return Parte del cuerpo solicitada.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLBodyPart* getBodyPart(unsigned int bodyPart) const {
						assert(_packInvoked);
						assert(_bodyParts);
						assert(bodyPart < _numBodyParts);
						return (const MDLBodyPart*) _bodyParts[bodyPart]; }

	/**
	 * Devuelve el número de puntos de unión del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Número de puntos de unión.
	 */
	unsigned int getNumAttachments() const {
						assert(_packInvoked);
						return _numAttachments; }

	/**
	 * Devuelve un puntero al array con la información de los
	 * puntos de unión.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este método.
	 * @return Vector con la información de los puntos de unión del modelo.
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLAttachment** getAttachments() const {
						assert(_packInvoked);
						assert(_attachments);
						return (const MDLAttachment**) _attachments; }

	/**
	 * Devuelve un puntero al punto de unión solicitado. <p>
	 * El modelo debe haberse inicializado correctamente antes de usar
	 * este método.
	 * @param attachment Punto de unión solicitada.
	 * @return Punto de unión solicitado. <br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLAttachment* getAttachment(unsigned int attachment) const {
						assert(_packInvoked);
						assert(_attachments);
						assert(attachment < _numAttachments);
						return (const MDLAttachment*) _attachments[attachment]; }

	//---------------------------------------------------------------
	// Métodos de inicialización. Deben ser llamados antes de
	// consultar la información del modelo.
	//---------------------------------------------------------------

	/**
	 * Indica la posición de memoria donde se ha leído el fichero
	 * principal del modelo.<p>
	 * La memoria debe haberse solicitado con new MDLchar[xx], y será
	 * liberada por este objeto en su destructor o en la llamada a
	 * MDLCore::pack (con delete[] buffer).
	 * Si este método se llama tras haber inicializado el objeto
	 * previamente, el modelo anterior se liberará y se considerará
	 * que se va a realizar una nueva inicialización.
	 * @param buffer Puntero donde se ha leído todo el fichero MDL.
	 * Si se pasa NULL, se libera todo el modelo.
	 * @return Cierto si la información de la cabecera del fichero
	 * es correcta.
	 */
	bool setMainFile(MDLchar* buffer);

	/**
	 * Indica la posición de memoria donde se ha leído el fichero
	 * con las texturas. Esta llamada sólo es necesaria si el fichero
	 * con la geometría no contenía texturas. Este método no debe
	 * realizarse antes de llamar a setMainFile.<p>
	 * La memoria debe haberse solicitado con new MDLchar[xx], y será
	 * liberada por este objeto en su destructor o en la llamada a
	 * MDLCore::pack (con delete[] buffer).
	 * @param buffer Puntero donde se ha leído todo el fichero MDL.
	 * @return Cierto si la información de la cabecera del fichero
	 * es correcta.
	 */
	bool setTextureFile(MDLchar* buffer);

	/**
	 * Indica la posición de memoria donde se ha leído el fichero
	 * con la información de un grupo de secuencias. Esta llamada
	 * sólo es necesaria si el fichero con la geometría no contenía
	 * la información de todos los grupos de secuencias. Este
	 * método no debe realizarse antes de llamar a setMainFile.<p>
	 * La memoria debe haberse solicitado con new MDLchar[xx], y será
	 * liberada por este objeto en su destructor o en la llamada a
	 * MDLCore::pack (con delete[] buffer).
	 * @param buffer Puntero donde se ha leído todo el fichero MDL.
	 * @param index Índice del grupo de secuencias cuya información
	 * se establece. El valor 1 se usa para el primero de los grupos
	 * de secuencia externos.
	 * @return Cierto si la información de la cabecera del fichero
	 * es correcta.
	 */
	bool setSeqGroupFile(MDLchar* buffer, unsigned int index);

	/**
	 * Organiza toda la información sobre el modelo recibida en las
	 * llamadas a MDLCore::setMainFile, MDLCore::setTextureFile y
	 * MDLCore::setSeqGroupFile para hacerla más cómoda de consultar.
	 * Es obligatoria la llamada a esta función para poder hacer uso
	 * del modelo.<p>
	 * La memoria de los buffers proporcionados en las llamadas
	 * a setMainFile, setTextureFile y setSeqGroupFile serán liberadas
	 * si el resultado de la operación es correcto.
	 * <p>
	 * Por norma general, no se pueden empaquetar un modelo para el que no se
	 * haya proporcionado información sobre texturas o pieles. Esa información
	 * puede estar en el mismo fichero que la geometría o en uno separado,
	 * pero debe proporcionarse. Esta necesidad se debe a que las coordenadas
	 * de textura se calculan usando el tamaño de la textura que se aplicará
	 * sobre la malla, y no puede calcularse sin ese tamaño. En cierto modo,
	 * eso limita un poco las capacidades de la librería, pues no puede usarse
	 * para mostrar modelos en los que se quiere usar un color plano sin
	 * textura. <br>
	 * Si se quisiera soportar esto, es suficiente cambiar el valor de la
	 * "constante" HalfLifeMDL::AllowModelsWithoutTextures definida en
	 * MDLBasicType.h y asignarla el valor 1. Si eso se hace, es
	 * responsabilidad del programador controlar la posibilidad de que un
	 * modelo no tenga texturas, en cuyo caso la información suministrada
	 * por el modelo será parcial. Que no tenga texturas no significa que
	 * las mallas no tengan asociada un índice de textura o coordenadas de
	 * textura, por lo que posiblemente las mallas proporcionarán información
	 * sobre texturas a utilizar y coordenadas de textura, y será el usuario
	 * del objeto el que tenga que dar sentido a esa información. Si no hay
	 * texturas, las coordenadas de textura no se modifican, y se proporciona
	 * el mismo valor que se guarda en el fichero. En condiciones normales ese
	 * valor se habría dividido por el tamaño de la textura.<br>
	 * Naturalmente, si se establece el valor de esa constante a 1, pero
	 * se carga un modelo que sí tiene información sobre pieles o texturas,
	 * el funcionamiento será el habitual.
	 *
	 * @param connectedTriangles Si se desea que las mallas del modelo
	 * mantengan los triángulos conectados, ya sea en modo FAN o en modo
	 * STRIP, se debe pasar cierto en este parámetro. Si el motor no es
	 * capaz de dibujar triángulos compactos, se pasa falso, de modo que
	 * los triángulos se almacenan de forma separada. Vea
	 * HalfLifeMDL::MDLMesh para más información. Por defecto tiene valor
	 * falso. <br>
	 * En realidad, aunque el motor pueda dibujar triángulos en modo fan o
	 * strip, podría no resultar interesante hacer uso de ello. Cuando las
	 * mallas se crean de ese modo, su número puede ser bastante abundante
	 * y el número medio de triángulos bastante bajo. Lo mejor es probar :-)
	 * @param clockwise Especifica el orden de los vértices que crean la
	 * cara delantera en las mallas del modelo que se creará. Si se desea
	 * que los triángulos devueltos tengan su cara frontal con los vértices
	 * en el sentido de las agujas del reloj hay que especificar cierto en
	 * este campo, y falso en caso contrario. <br>
	 * Si se solicitan las caras con los vértices en el sentido contrario a
	 * las agujas del reloj, y manteniendo los triángulos conectados, las
	 * mallas en modo STRIP tendrán un primer triángulo "corrupto", en el
	 * que el primer vértice y el tercero serán el mismo.
	 * @return 
	 *	- Cierto : la información proporcionada previamente es
	 * correcta y el modelo puede empezar a utilizarse. 
	 *	- Falso : se ha producido algún error.
	 *
	 * @attention En ocasiones StudioMDL genera ficheros "incorrectos".
	 * Cuando se le pide que almacene información sobre animaciones en
	 * ficheros externos al de la geometría (vea @ref secuenciasYGrupos),
	 * no se preocupa de inicializar muchos de los campos "vitales" del
	 * fichero, como puede ser el número de huesos llegando a indicar
	 * valores de varios millones. Si el cargador supone,
	 * como es lógico, que ese campo es correcto, intentará pedir mucha
	 * memoria para almacenar la información de todos los huesos,
	 * intentará acceder a posiciones de memoria inválidos, y entrará
	 * en bucles larguísimos de finalización incierta. El código de este
	 * método se protege de esos problemas haciendo comprobaciones obvias
	 * de integridad (por ejemplo, que el número de huesos anunciado
	 * es menor al espacio disponible en el fichero leído), por lo que
	 * si se detectan inconsistencias se devuelve falso. El problema es
	 * que como los valores leídos son aleatorios, podría darse el caso
	 * de que un fichero que no tiene información de huesos dé la impresión
	 * de sí tenerlos. En ese caso, este método es incapaz de diferenciar
	 * entre un fichero válido y otro inválido, por lo que el resultado
	 * podría ser inválido. En realidad la probabilidad de que esto ocurra
	 * es muy escasa, y, en principio, sólo podría ocurrir si se intenta
	 * leer un fichero de animaciones como uno de geometría. Los ficheros
	 * con sólo texturas sí contienen los campos inicializados correctamente.
	 * <br>
	 * En los análisis realizados, se ha comprobado que el campo id de la
	 * cabecera (header_t::id) contiene la cadena "IDST" en los ficheros
	 * de geometría y texturas, e "IDSQ" en los de animaciones, por lo que
	 * podría usarse esa información para anular la carga de uno de animación
	 * si se intenta leer como uno de geometría. No obstante no hay una
	 * seguridad total de que esto se cumpla siempre, por lo que no se
	 * utilizará.
	 */
	bool pack(bool connectedTriangles = false, bool clockwise = false);

	/**
	 * Libera la memoria de las imagenes contenidas en las texturas del
	 * modelo. Esto es práctico si se utiliza una librería gráfica a la que
	 * se le proporcionan las imágenes y que las mantiene. En ese caso puede
	 * no tener sentido que el programa mantenga también las imágenes
	 * desaprovechando memoria. Tras la llamada a este método la información
	 * básica de las texturas (cuantas son, su altura, anchura, etc.) se
	 * sigue almacenando; pero los datos del color de los píxeles se libera.
	 * En realidad llama al método MDLTexture::freeImage() con todas las
	 * texturas del modelo.
	 * <p>
	 * Este método sólo debe ser llamado si se ha realizado con éxito la
	 * inicialización del modelo.
	 */
	void freeTextures();

#ifndef NDEBUG
	/**
	 * Devuelve la diferencia entre el número de transformaciones
	 * necesarias por cada fotograma con la unificación de las matrices
	 * de adyacencia y sin ella. En concreto, es la resta el doble del
	 * número de vértices "unificados" (el doble porque hay el mismo
	 * número de vértices que de normales) menos el número de vértices
	 * y de normales que había en el submodelo según se guardaba en el MDL.<p>
	 * Este método sólo tiene utilidad para estadísticas por curiosidad.
	 * De hecho sólo existe en la compilación en modo depuración. <p>
	 * En realidad, se devuelve la suma de los incrementos en todas los
	 * posibles recubrimientos de todas las partes del cuerpo. En la práctica,
	 * sólo se utilizará un recubrimiento para cada parte del cuerpo, por lo
	 * que no se incrementa en ese número por fotograma si hay más de un
	 * recubrimiento en alguna parte del cuerpo.
	 * <p>
	 * Este método sólo debe ser llamado si se ha realizado con éxito la
	 * inicialización del modelo.
	 */
	int getIncrTransformations();
#endif

protected:

	/**
	 * Libera la memoria de las estructuras usadas por el modelo.
	 */
	void freeModel();

	/**
	 * Libera la memoria de las estructuras de bajo nivel usadas por el
	 * modelo. Por bajo nivel nos referimos a los bloques de memoria donde
	 * se almacena el contenido de los ficheros desde los que se ha obtenido
	 * la información del modelo. Esta liberación sólo se realiza si
	 * realmente existe memoria solicitada para ello.<p>
	 * Esta función es auxiliar. Es llamada por MDLCore::freeModel y por
	 * MDLCore::pack.
	 */
	void freeLowLevel();

	/**
	 * Libera la memoria de las estructuras de alto nivel usadas por el
	 * modelo. Por alto nivel nos referimos a los bloques de memoria con
	 * la información del modelo organizada en clases de un modo cómodo
	 * para proporcionarsela al usuario. Esta liberación sólo se realiza
	 * si realmente existe memoria solicitada para ello.<p>
	 * Esta función es auxiliar. Es llamada por MDLCore::freeModel y por
	 * MDLCore::pack. Establece MDL:Core::_packInvoked a falso.
	 */
	void freeHighLevel();

	/**
	 * Método auxiliar para la implementación del método MDLCore::pack.
	 * En realidad contiene toda la implementación salvo la del control
	 * de la memoria. La implementación de MDLCore::pack llama a este
	 * método, y se ocupa de liberar la memoria de los atributos del objeto
	 * en función del resultado. Eso facilita la implementación sin usar
	 * goto's :-)
	 * @param connectedTriangles Si se desea que las mallas del modelo
	 * mantengan los triángulos conectados, ya sea en modo FAN o en modo
	 * STRIP, se debe pasar cierto en este parámetro. Si el motor no es
	 * capaz de dibujar triángulos compactos, se pasa falso, de modo que
	 * los triángulos se almacenan de forma separada. Vea
	 * HalfLifeMDL::MDLMesh para más información.
	 * @param clockwise Especifica el orden de los vértices que crean la
	 * cara delantera en las mallas del modelo que se creará. Si se desea
	 * que los triángulos devueltos tengan su cara frontal con los vértices
	 * en el sentido de las agujas del reloj hay que especificar cierto en
	 * este campo, y falso en caso contrario. <br>
	 * Si se solicitan las caras con los vértices en el sentido contrario a
	 * las agujas del reloj, y manteniendo los triángulos conectados, las
	 * mallas en modo STRIP tendrán un primer triángulo "corrupto", en el
	 * que el primer vértice y el tercero serán el mismo.
	 * 
	 * @return 
	 *	- Cierto : la información proporcionada sobre el modelo es
	 * correcta y el modelo puede empezar a utilizarse. 
	 *	- Falso : se ha producido algún error.
	 * @see MDLCore::pack
	 */
	bool auxPack(bool connectedTriangles, bool clockwise);

	/**
	 * Enumerado que se utiliza para definir constantes (en lugar de
	 * utilizar defines).
	 */
	enum MDLCoreConstants {
		/** Máximo número de ficheros externos con animaciones adicionales. */
		MaxExternalSequenceFiles = 32
	};

	/**
	 * Puntero a la cabecera. Realmente no sólo guarda la cabecera, sino
	 * que a partir de esta posición va todo el fichero. El puntero se
	 * utilizará por lo tanto para acceder a los campos de la cabecera y
	 * para usar los desplazamientos que contiene para buscar la posición
	 * en memoria del resto de bloques de información.
	 * <p>
	 * Este puntero sólo es válido antes de la llamada a MDLCore::pack;
	 */
	header_t* _header;

	/**
	 * Puntero a la cabecera del fichero con las texturas y la información
	 * de las pieles del modelo. Si las texturas están en el mismo fichero
	 * que la geometría, este puntero será el mismo que _header. En caso
	 * contrario, apuntará a la información del fichero modelT.mdl (suponiendo
	 * que el nombre del fichero con la geometría es model.mdl). Al igual que
	 * en _header, la zona de memoria apuntada no sólo contiene la cabecera,
	 * sino que almacena todo el fichero. El puntero se usará por lo tanto
	 * para acceder a los campos de la cabecera y para usar los
	 * desplazamientos que contiene para buscar la posición en memoria del
	 * resto de bloques de información.
	 * <p>
	 * Este puntero sólo es válido antes de la llamada a MDLCore::pack;
	 */
	header_t* _textureHeader;

	/**
	 * Vector de punteros a las cabeceras de los ficheros con animaciones
	 * adicionales. Un fichero puede almacenar la geometría y dejar algunas
	 * animaciones sin definir, y leerlas de otros ficheros. En ese caso
	 * los punteros a las cabeceras de esos ficheros se almacenan aquí.
	 * En realidad las zonas de memoria apuntadas no sólo contienen las
	 * cabeceras, sino que almacenan los ficheros completos. Los punteros
	 * se usan por lo tanto para acceder a los campos de las cabeceras y
	 * para usar los desplazamientos que contiene para buscar la posición
	 * en memoria del resto de bloques de información.<p>
	 * El primer elemento apunta al primer fichero externo. Las secuencias
	 * almacenadas por el fichero de la geometría deben ser accedidas 
	 * a través de _header.
	 * <p>
	 * Este puntero sólo es válido antes de la llamada a MDLCore::pack;
	 */
	header_t* _animHeader[MaxExternalSequenceFiles];

	/**
	 * Atributo que almacena si se ha realizado la llamada a MDLCore::pack
	 * o no. En función de su valor, los métodos de acceso a la información
	 * del modelo serán válidos o no.
	 * <p>
	 * También se utiliza en los métodos de inicialización para saber qué
	 * memoria hay que liberar.
	 */
	bool _packInvoked;

	/**
	 * Nombre del modelo. Sólo es correcto tras llamar a MDLCore::pack.
	 */
	std::string _name;

	/**
	 * Vector con los huesos del modelo. Este vector sólo es correcto tras
	 * llamar a MDLCore::pack. Se almacenan punteros a huesos. Su tamaño se
	 * almacena en MDLCore::_numBones.
	 */
	MDLBone** _bones;

	/**
	 * Número de huesos del modelo.
	 */
	unsigned int _numBones;

	/**
	 * Vector con las texturas del modelo. Este vector sólo es correcto tras
	 * llamar a MDLCore::pack. Se almacenan punteros a texturas. Su tamaño se
	 * almacena en MDLCore::_numTextures.
	 */
	MDLTexture** _textures;

	/**
	 * Número de texturas del modelo.
	 */
	unsigned int _numTextures;

	/**
	 * Vector con las secuencias del modelo. Este vector sólo es correcto tras
	 * llamar a MDLCore::pack. Se almacenan punteros a secuencias. Su tamaño se
	 * almacena en MDLCore::_numSequences.
	 */
	MDLSequence** _sequences;

	/**
	 * Número de secuencias del modelo. Este valor sólo es correcto tras
	 * llamar a MDLCore::pack.
	 */
	unsigned int _numSequences;

	/**
	 * Vector con las partes del modelo. Este vector sólo es correcto tras
	 * llamar a MDLCore::pack. Se almacenan punteros a partes del cuerpo.
	 * Su tamaño se almacena en MDLCore::_numBodyParts.
	 */
	MDLBodyPart** _bodyParts;

	/**
	 * Número de partes del modelo.
	 */
	unsigned int _numBodyParts;

	/**
	 * Vector con los puntos de unión del modelo. Este vector sólo es
	 * correcto tras llamar a MDLCore::pack. Se almacenan punteros a
	 * puntos de unión. Su tamaño se almacena en MDLCore::_numAttachments.
	 */
	MDLAttachment** _attachments;

	/**
	 * Información sobre las pieles del modelo. Este puntero sólo es
	 * correcto tras llamar a MDLCore::pack.
	 */
	MDLSkins* _skins;

	/**
	 * Número de puntos de unión del modelo. Este valor sólo es correcto
	 * tras llamar a MDLCore::pack
	 */
	unsigned int _numAttachments;

	/**
	 * Bounding Box. Teóricamente, este campo debería contener la posición
	 * del vértice inferior del bounding box alineado con los ejes del modelo
	 * en su posición por defecto. La realidad es que este valor se obtiene
	 * directamente del fichero, y en los modelos probados, su valor es 
	 * cero.<br>
	 * @see HalfLifeMDL::header_t::bbmin
	 */
	MDLPoint _bbmin;

	/**
	 * Bounding Box. Teóricamente, este campo debería contener la posición
	 * del vértice superior del bounding box alineado con los ejes del modelo
	 * en su posición por defecto. La realidad es que este valor se obtiene
	 * directamente del fichero, y en los modelos probados, su valor es 
	 * cero.<br>
	 * @see HalfLifeMDL::header_t::bbmax
	 */
	MDLPoint _bbmax;

private:

	/**
	 * Constructor copia. Se declara privado porque no debe utilizarse, pues
	 * no está implementado. Su implementación debería hacer copia de todos
	 * los punteros para no compartir memoria y que los destructures no
	 * fallen.
	 * @param source Objeto a partir del que construir el nuevo.
	 */
	MDLCore(const MDLCore& source) { assert(!"Error: uso de constructor privado"); }

	/**
	 * Operador de asignación. Se declara privado porque no debe utilizarse,
	 * pues no está implementado. Su implementación debería hacer copia de 
	 * todos los punteros para no compartir memoria y que los destructures no
	 * fallen.
	 * @param source Objeto a copiar.
	 * @return Objeto this.
	 */
	MDLCore& operator=(const MDLCore& source) {
						assert(!"Error: uso de constructor privado"); 
						return *this;}

}; // class MDLCore


} // namespace HalfLifeMDL

#endif
