//---------------------------------------------------------------------------
// MDLCore.h
//---------------------------------------------------------------------------

/**
 * @file MDLCore.h
 * Contiene la declaraci�n de la clase b�sica que almacena la informaci�n
 * de un modelo de Half Life (HalfLifeMDL::MDLCore). Esta clase tiene
 * m�todos para consultar la informaci�n, como el n�mero de texturas,
 * animaciones, etc.<p>
 * Desde el punto de vista del usuario final del namespace HalfLifeMDL,
 * seguramente lo m�s pr�ctico es utilizar la clase
 * HalfLifeMDL::MDLInstance. Esta clase s�lo se utilizar� para inicializar
 * una nueva instancia.<p>
 * Hace uso cont�nuo de las estructuras definidas en MDLBasic.h.
 * @see HalfLifeMDL::MDLCore
 * @see HalfLifeMDL::MDLInstance
 * @author Pedro Pablo G�mez Mart�n
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
 * Clase b�sica que almacena la informaci�n (est�tica) de un modelo de Half
 * Life. Guarda �nicamente la informaci�n est�tica, por ejemplo el n�mero de
 * texturas y de secuencias, pero no informaci�n din�mica como el
 * fotograma actual, o la piel seleccionada.<p>
 * Aunque el formato MDL permite la carga de animaciones bajo demanda
 * (si estas se almacenan en ficheros diferentes al archivo con la
 * geometr�a), esta clase no soporta esa posibilidad y toda la informaci�n
 * debe recopilarse al principio (vea @ref secuenciasYGrupos para m�s
 * informaci�n sobre esto).<p>
 * La inicializaci�n de los objetos de esta clase se realiza mediante
 * los m�todos MDLCore::setMainFile, MDLCore::setTextureFile y
 * MDLCore::setSeqGroupFile. El primero se utiliza para proporcionar el
 * contenido completo del fichero MDL cuyo modelo se quiere controlar
 * con el objeto. Los otros dos s�lo ser�n necesarios si el fichero
 * proporcionado no tiene las texturas, y estas se almacenan en un
 * fichero separado (habr� que llamar a setTextureFile), o si el
 * archivo de la geometr�a no tiene todas las secuencias de animaci�n
 * soportadas por el modelo (habr� que llamar a setSeqGroupFile).
 * Ambos m�todos tambi�n reciben el fichero le�do completamente en
 * el buffer pasado.
 * <p>
 * Half Life utiliza un convenio de nombres de archivo para buscar las
 * texturas y animaciones externas al fichero de geometr�a en caso de
 * ser necesarias. Si el archivo de la geometr�a se denomina
 * <tt>model.mdl</tt> y se detecta que no posee texturas, �stas se
 * leer�n del fichero <tt>modelT.mdl</tt>. Del mismo modo, si 
 * <tt>model.mdl</tt> indica que necesita otros dos ficheros con
 * secuencias de animaci�n adicionales, se supondr� que sus nombres son
 * <tt>model01.mdl</tt> y <tt>model02.mdl</tt>.
 * <p>
 * Una vez llamado a los m�todos de inicializaci�n, es necesario llamar
 * a MDLCore::pack para que el objeto procese toda la informaci�n y
 * la organice para almacenarla de un modo m�s c�modo para el usuario.
 * Hasta que no se llame al m�todo anterior, el objeto no podr� ser
 * utilizado.
 * <p>
 * Seg�n lo anterior, el esquema del c�digo que requiere esta clase para
 * ser inicializada utilizando el <EM>convenio de nombres</EM> usado por
 * Half Life ser�a:
 * <p>
 * @include HalfLifeMDL/InicializacionMDLCore.cpp
 * <p>
 * En realidad el usuario de esta clase no necesita implementar esto, pues
 * ya se proporciona una implementaci�n en HalfLifeMDL::MDLReader. Se detalla
 * aqu� como ejemplo, por si se quisiera cargar un modelo a partir de un
 * archivo encapsulado y/o encriptado.
 * <p>
 * Tras la llamada a MDLCore::pack, es posible acceder a toda la
 * informaci�n est�tica del modelo con los m�todos proporcionados para
 * ello. Esa informaci�n incluye las texturas. T�picamente �stas ser�n
 * enviadas a la librer�a gr�fica, y no tiene sentido que siguan
 * almacen�ndose consumiendo memoria. Es posible liberar el espacio
 * ocupado llamando al m�todo MDLCore::freeTextures(). Tras esta llamada,
 * la informaci�n sobre la imagen de las texturas dejar� de estar disponible.
 * <p>
 * Esta clase juega un papel primordial dentro del paquete HalfLifeMDL. En
 * concreto, hace de puente entre las estructuras de datos almacenadas en
 * los ficheros MDL (definidas en MDLBasic.h), y las clases que se hacen
 * p�blicas al exterior. La conversi�n de unas a otras se realiza en el
 * c�digo de MDLCore::pack.
 * <p>
 * Como ya se ha dicho, esta clase almacena la informaci�n est�tica de
 * un modelo, lo com�n es que los programas requieran almacenar informaci�n
 * din�mica, como el fotograma actual, o la secuencia que se est�
 * reproduciendo en un determinado momento. Esa labor la realiza la clase
 * <b>HalfLifeMDL::MDLInstance</b>, que guarda una <EM>instancia</EM>. Dicha
 * clase es m�s pr�ctica desde el punto de vista del usuario, evitandole el
 * trabajo, por ejemplo, de interpolar posiciones entre fotogramas.
 * @remarks Desde el punto de vista del usuario final, es m�s pr�ctico
 * utilizar la clase MDLInstance.
 * @see HalfLifeMDL::MDLInstance, HalfLifeMDL::MDLReader,
 * @ref secuenciasYGrupos
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 * @todo Si se hace una p�gina con la "arquitectura", hacer un enlace aqu� y
 * en uno de los p�rrafos anteriores de la documentaci�n.
 */
class MDL_PUBLIC MDLCore {
/**
 * @example HalfLifeMDL/InicializacionMDLCore.cpp
 * Ejemplo de inicializaci�n de un objeto de la clase HalfLifeMDL::MDLCore.<p>
 * En realidad el usuario de esta clase no necesita implementar esto, pues ya
 * se proporciona una implementaci�n en HalfLifeMDL::MDLReader. Se detalla
 * aqu� como ejemplo, por si se quisiera cargar un modelo a partir de un
 * archivo encapsulado y/o encriptado.<p>
 * @see HalfLifeMDL::MDLCore
 * @author Pedro Pablo G�mez Mart�n
 * @date Octubre, 2003.
 * <p>
 */

public:

	/**
	 * M�todo que busca cosas en el fichero. Es un m�todo de depuraci�n
	 * usado para analizar modelos e intentar deducir qu� significan algunos
	 * campos.
	 * @todo Eliminar el m�todo :-) Tambi�n habr�a que quitar el #include del
	 * cpp para poder escribir por pantalla en este m�todo.
	 */
	void analizar();

	/**
	 * Constructor sin par�metros. Inicializa todos los punteros a
	 * NULL.
	 */
	MDLCore();

	/**
	 * Destructor. Libera toda la memoria utilizada.
	 */
	~MDLCore() { freeModel(); }

	//---------------------------------------------------------------
	// M�todos de consulta de informaci�n general del modelo.
	//---------------------------------------------------------------

	/**
	 * Devuelve el nombre del modelo. T�picamente es el nombre del fichero
	 * del que proviene el modelo. Suele incluir informaci�n de ruta, a veces
	 * con '\' y otras con '/'. En cualquier caso, la ruta no es muy fiable.
	 * <p>
	 * En realidad el nombre se extrae del contenido del fichero MDL
	 * original, no del nomre del fichero real a partir del que se ley�.
	 * El nombre almacenado contiene, por lo tanto, una ruta relativa
	 * seg�n la estructura de directorios cuando se cre� el modelo, de
	 * ah� que sea poco ilustrativo.
	 * Este m�todo s�lo puede ser llamado despu�s de invocar a pack.
	 * @return Nombre del modelo. Suele tener el aspecto de un nombre de
	 * fichero, incluyendo, quiz�, una ruta que suele ser inv�lida.
	 */
	const std::string& getName() const { return _name; }

	/**
	 * Devuelve el Bounding Box alineado con los ejes del modelo en su
	 * posici�n por defecto. En realidad este m�todo no funciona, porque
	 * los campos que contienen esta informaci�n en el fichero no se
	 * inicializan (vease, por ejemplo, HalfLifeMDL::header_t::bbmin).<p>
	 * Este m�todo s�lo puede ser llamado despu�s de invocar a pack.
	 * @param bbmin Par�metro de salida con la esquina inferior del bounding
	 * box alineado con los ejes.
	 * @param bbmax Par�metro de salida con la esquina superior del bounding
	 * box alineado con los ejes.
	 * @todo Tal vez podr�a mejorarse pack para que calcualra los valores,
	 * ya que no lo hace StudioMDL. Si se hace, modificar la documentaci�n
	 * de _bbmin y _bbmax en esta clase.
	 */
	void getBoundingBox(MDLPoint& bbmin, MDLPoint& bbmax) const {
					assert(_packInvoked);
					bbmin = _bbmin; bbmax = _bbmax; }

	/**
	 * Devuelve el n�mero de huesos del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return N�mero de huesos del modelo.
	 */
	unsigned int getNumBones() const {
						assert(_packInvoked);
						return _numBones; }

	/**
	 * Devuelve un puntero al array con la informaci�n de los huesos.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return Vector con la informaci�n de los huesos del modelo.<br>
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
	 * usar este m�todo.
	 * @param bone �ndice del hueso que se solicita.
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
	 * Devuelve el n�mero de controladores de huesos del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return N�mero de controladores de hueso del modelo.
	 */
/*	unsigned int getNumBoneControllers() {
						assert(_packInvoked);
						return _header->numbonecontrollers; }
*/
	/**
	 * Devuelve un puntero al array con la informaci�n de los 
	 * controladores de huesos.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return Vector con la informaci�n de los controladores de
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
	 * Devuelve el n�mero de secuencias del modelo. <p>
	 * El modelo debe haberse inicializado correctamente antes de usar este
	 * m�todo, y debe haberse llamado a pack().
	 * @return N�mero de secuencias del modelo.
	 * @see @ref secuenciasYGrupos
	 */
	unsigned int getNumSequences() const {
						assert(_packInvoked);
						return _numSequences; }

	/**
	 * Devuelve la informaci�n de una secuencia del modelo. <p>
	 * El modelo debe haberse inicializado correctamente antes de usar este
	 * m�todo, y debe haberse llamado a pack().
	 * @param sequence �ndice de la secuencia. No se comprueba que el valor
	 * est� dentro del rango (salvo en modo depuraci�n con assert). Un valor
	 * inv�lido ocasionar� un resultado indeterminado. La primera secuencia
	 * tiene �ndice 0.
	 * @return Secuencia solicitada. El puntero no debe liberarse. El objeto
	 * se encarga de la gesti�n de la memoria.
	 */
	const MDLSequence* getSequence(unsigned int sequence) const {
						assert(_packInvoked);
						assert(sequence < _numSequences);
						assert(_sequences);
						return _sequences[sequence]; }

	/**
	 * Devuelve el n�mero de grupos de secuencias del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de usar
	 * este m�todo (al menos haber llamado a MDLCore::setMainFile), y
	 * no debe haberse llamado a pack().
	 * @return N�mero de grupos de secuencias del modelo.
	 * @see @ref secuenciasYGrupos
	 */
	unsigned int getNumSeqGroups() const {
						assert(!_packInvoked);
						assert(_header);
						return _header->numseqgroups; }

	/**
	 * Devuelve el n�mero de texturas del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo. Funciona antes y despu�s de llamar a
	 * MDLCore::pack.
	 * @return N�mero de texturas del modelo.
	 */
	unsigned int getNumTextures() const {
						if (!_packInvoked) {
							assert(_header);
							return _textureHeader->numtextures;
						}
						else
							return _numTextures; }

	/**
	 * Devuelve un puntero a la textura solicitada. Es v�lido para
	 * objetos constantes, y la textura recibida tambi�n ser�
	 * constante (no se puede liberar ni cambiar el tag).<p>
	 * El modelo debe haberse inicializado correctamente (y llamado a
	 * pack()) antes de usar este m�todo.
	 * @param texture �ndice de la textura solicitada.
	 * @return Puntero a la textura pedida.
	 */
	const MDLTexture* getTexture(unsigned int texture) const {
						assert(_packInvoked);
						assert(texture < _numTextures);
						return _textures[texture]; }

	/**
	 * Devuelve un puntero a la textura solicitada. Este m�todo es para
	 * objetos no constantes, y devuelve una textura no constante a la que
	 * se le puede cambiar el tag o que puede liberarse. <p>
	 * El modelo debe haberse inicializado correctamente (y llamado a
	 * pack()) antes de usar este m�todo.
	 * @param texture �ndice de la textura solicitada.
	 * @return Puntero a la textura pedida.
	 */
	MDLTexture* getTexture(unsigned int texture) {
						assert(_packInvoked);
						assert(texture < _numTextures);
						return _textures[texture]; }

	/**
	 * Devuelve el n�mero de texturas que hay en cada piel del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return N�mero de texturas de cada piel del modelo.
	 */
	unsigned int getNumTexturesInSkin() const {
						assert(_packInvoked);
						assert(_skins);
						return _skins->getNumTexturesPerSkin(); }

	/**
	 * Devuelve el n�mero de pieles que hay en el modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return N�mero de pieles (de textura) del modelo.
	 */
	unsigned int getNumSkins() const {
						assert(_packInvoked);
						assert(_skins);
						return _skins->getNumSkins(); }


	/**
	 * Devuelve la informaci�n de las pieles del modelo. <p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return Informaci�n de las pieles (de textura) del modelo.<br>
 	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLSkins* getSkins() const {
						assert(_packInvoked);
						assert(_skins);
						return _skins; }

	/**
	 * Devuelve el n�mero de partes de las que se compone el modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return N�mero de partes del modelo.
	 */
	unsigned int getNumBodyParts() const {
						assert(_packInvoked);
						return _numBodyParts; }

	/**
	 * Devuelve un puntero al array con la informaci�n de las
	 * partes del cuerpo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return Vector con la informaci�n de las partes del modelo.<br>
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
	 * este m�todo.
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
	 * Devuelve el n�mero de puntos de uni�n del modelo.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return N�mero de puntos de uni�n.
	 */
	unsigned int getNumAttachments() const {
						assert(_packInvoked);
						return _numAttachments; }

	/**
	 * Devuelve un puntero al array con la informaci�n de los
	 * puntos de uni�n.<p>
	 * El modelo debe haberse inicializado correctamente antes de
	 * usar este m�todo.
	 * @return Vector con la informaci�n de los puntos de uni�n del modelo.
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLAttachment** getAttachments() const {
						assert(_packInvoked);
						assert(_attachments);
						return (const MDLAttachment**) _attachments; }

	/**
	 * Devuelve un puntero al punto de uni�n solicitado. <p>
	 * El modelo debe haberse inicializado correctamente antes de usar
	 * este m�todo.
	 * @param attachment Punto de uni�n solicitada.
	 * @return Punto de uni�n solicitado. <br>
	 * El puntero devuelto no debe liberarse (la memoria a la que apunta
	 * es gestionada por el objeto).
	 */
	const MDLAttachment* getAttachment(unsigned int attachment) const {
						assert(_packInvoked);
						assert(_attachments);
						assert(attachment < _numAttachments);
						return (const MDLAttachment*) _attachments[attachment]; }

	//---------------------------------------------------------------
	// M�todos de inicializaci�n. Deben ser llamados antes de
	// consultar la informaci�n del modelo.
	//---------------------------------------------------------------

	/**
	 * Indica la posici�n de memoria donde se ha le�do el fichero
	 * principal del modelo.<p>
	 * La memoria debe haberse solicitado con new MDLchar[xx], y ser�
	 * liberada por este objeto en su destructor o en la llamada a
	 * MDLCore::pack (con delete[] buffer).
	 * Si este m�todo se llama tras haber inicializado el objeto
	 * previamente, el modelo anterior se liberar� y se considerar�
	 * que se va a realizar una nueva inicializaci�n.
	 * @param buffer Puntero donde se ha le�do todo el fichero MDL.
	 * Si se pasa NULL, se libera todo el modelo.
	 * @return Cierto si la informaci�n de la cabecera del fichero
	 * es correcta.
	 */
	bool setMainFile(MDLchar* buffer);

	/**
	 * Indica la posici�n de memoria donde se ha le�do el fichero
	 * con las texturas. Esta llamada s�lo es necesaria si el fichero
	 * con la geometr�a no conten�a texturas. Este m�todo no debe
	 * realizarse antes de llamar a setMainFile.<p>
	 * La memoria debe haberse solicitado con new MDLchar[xx], y ser�
	 * liberada por este objeto en su destructor o en la llamada a
	 * MDLCore::pack (con delete[] buffer).
	 * @param buffer Puntero donde se ha le�do todo el fichero MDL.
	 * @return Cierto si la informaci�n de la cabecera del fichero
	 * es correcta.
	 */
	bool setTextureFile(MDLchar* buffer);

	/**
	 * Indica la posici�n de memoria donde se ha le�do el fichero
	 * con la informaci�n de un grupo de secuencias. Esta llamada
	 * s�lo es necesaria si el fichero con la geometr�a no conten�a
	 * la informaci�n de todos los grupos de secuencias. Este
	 * m�todo no debe realizarse antes de llamar a setMainFile.<p>
	 * La memoria debe haberse solicitado con new MDLchar[xx], y ser�
	 * liberada por este objeto en su destructor o en la llamada a
	 * MDLCore::pack (con delete[] buffer).
	 * @param buffer Puntero donde se ha le�do todo el fichero MDL.
	 * @param index �ndice del grupo de secuencias cuya informaci�n
	 * se establece. El valor 1 se usa para el primero de los grupos
	 * de secuencia externos.
	 * @return Cierto si la informaci�n de la cabecera del fichero
	 * es correcta.
	 */
	bool setSeqGroupFile(MDLchar* buffer, unsigned int index);

	/**
	 * Organiza toda la informaci�n sobre el modelo recibida en las
	 * llamadas a MDLCore::setMainFile, MDLCore::setTextureFile y
	 * MDLCore::setSeqGroupFile para hacerla m�s c�moda de consultar.
	 * Es obligatoria la llamada a esta funci�n para poder hacer uso
	 * del modelo.<p>
	 * La memoria de los buffers proporcionados en las llamadas
	 * a setMainFile, setTextureFile y setSeqGroupFile ser�n liberadas
	 * si el resultado de la operaci�n es correcto.
	 * <p>
	 * Por norma general, no se pueden empaquetar un modelo para el que no se
	 * haya proporcionado informaci�n sobre texturas o pieles. Esa informaci�n
	 * puede estar en el mismo fichero que la geometr�a o en uno separado,
	 * pero debe proporcionarse. Esta necesidad se debe a que las coordenadas
	 * de textura se calculan usando el tama�o de la textura que se aplicar�
	 * sobre la malla, y no puede calcularse sin ese tama�o. En cierto modo,
	 * eso limita un poco las capacidades de la librer�a, pues no puede usarse
	 * para mostrar modelos en los que se quiere usar un color plano sin
	 * textura. <br>
	 * Si se quisiera soportar esto, es suficiente cambiar el valor de la
	 * "constante" HalfLifeMDL::AllowModelsWithoutTextures definida en
	 * MDLBasicType.h y asignarla el valor 1. Si eso se hace, es
	 * responsabilidad del programador controlar la posibilidad de que un
	 * modelo no tenga texturas, en cuyo caso la informaci�n suministrada
	 * por el modelo ser� parcial. Que no tenga texturas no significa que
	 * las mallas no tengan asociada un �ndice de textura o coordenadas de
	 * textura, por lo que posiblemente las mallas proporcionar�n informaci�n
	 * sobre texturas a utilizar y coordenadas de textura, y ser� el usuario
	 * del objeto el que tenga que dar sentido a esa informaci�n. Si no hay
	 * texturas, las coordenadas de textura no se modifican, y se proporciona
	 * el mismo valor que se guarda en el fichero. En condiciones normales ese
	 * valor se habr�a dividido por el tama�o de la textura.<br>
	 * Naturalmente, si se establece el valor de esa constante a 1, pero
	 * se carga un modelo que s� tiene informaci�n sobre pieles o texturas,
	 * el funcionamiento ser� el habitual.
	 *
	 * @param connectedTriangles Si se desea que las mallas del modelo
	 * mantengan los tri�ngulos conectados, ya sea en modo FAN o en modo
	 * STRIP, se debe pasar cierto en este par�metro. Si el motor no es
	 * capaz de dibujar tri�ngulos compactos, se pasa falso, de modo que
	 * los tri�ngulos se almacenan de forma separada. Vea
	 * HalfLifeMDL::MDLMesh para m�s informaci�n. Por defecto tiene valor
	 * falso. <br>
	 * En realidad, aunque el motor pueda dibujar tri�ngulos en modo fan o
	 * strip, podr�a no resultar interesante hacer uso de ello. Cuando las
	 * mallas se crean de ese modo, su n�mero puede ser bastante abundante
	 * y el n�mero medio de tri�ngulos bastante bajo. Lo mejor es probar :-)
	 * @param clockwise Especifica el orden de los v�rtices que crean la
	 * cara delantera en las mallas del modelo que se crear�. Si se desea
	 * que los tri�ngulos devueltos tengan su cara frontal con los v�rtices
	 * en el sentido de las agujas del reloj hay que especificar cierto en
	 * este campo, y falso en caso contrario. <br>
	 * Si se solicitan las caras con los v�rtices en el sentido contrario a
	 * las agujas del reloj, y manteniendo los tri�ngulos conectados, las
	 * mallas en modo STRIP tendr�n un primer tri�ngulo "corrupto", en el
	 * que el primer v�rtice y el tercero ser�n el mismo.
	 * @return 
	 *	- Cierto : la informaci�n proporcionada previamente es
	 * correcta y el modelo puede empezar a utilizarse. 
	 *	- Falso : se ha producido alg�n error.
	 *
	 * @attention En ocasiones StudioMDL genera ficheros "incorrectos".
	 * Cuando se le pide que almacene informaci�n sobre animaciones en
	 * ficheros externos al de la geometr�a (vea @ref secuenciasYGrupos),
	 * no se preocupa de inicializar muchos de los campos "vitales" del
	 * fichero, como puede ser el n�mero de huesos llegando a indicar
	 * valores de varios millones. Si el cargador supone,
	 * como es l�gico, que ese campo es correcto, intentar� pedir mucha
	 * memoria para almacenar la informaci�n de todos los huesos,
	 * intentar� acceder a posiciones de memoria inv�lidos, y entrar�
	 * en bucles largu�simos de finalizaci�n incierta. El c�digo de este
	 * m�todo se protege de esos problemas haciendo comprobaciones obvias
	 * de integridad (por ejemplo, que el n�mero de huesos anunciado
	 * es menor al espacio disponible en el fichero le�do), por lo que
	 * si se detectan inconsistencias se devuelve falso. El problema es
	 * que como los valores le�dos son aleatorios, podr�a darse el caso
	 * de que un fichero que no tiene informaci�n de huesos d� la impresi�n
	 * de s� tenerlos. En ese caso, este m�todo es incapaz de diferenciar
	 * entre un fichero v�lido y otro inv�lido, por lo que el resultado
	 * podr�a ser inv�lido. En realidad la probabilidad de que esto ocurra
	 * es muy escasa, y, en principio, s�lo podr�a ocurrir si se intenta
	 * leer un fichero de animaciones como uno de geometr�a. Los ficheros
	 * con s�lo texturas s� contienen los campos inicializados correctamente.
	 * <br>
	 * En los an�lisis realizados, se ha comprobado que el campo id de la
	 * cabecera (header_t::id) contiene la cadena "IDST" en los ficheros
	 * de geometr�a y texturas, e "IDSQ" en los de animaciones, por lo que
	 * podr�a usarse esa informaci�n para anular la carga de uno de animaci�n
	 * si se intenta leer como uno de geometr�a. No obstante no hay una
	 * seguridad total de que esto se cumpla siempre, por lo que no se
	 * utilizar�.
	 */
	bool pack(bool connectedTriangles = false, bool clockwise = false);

	/**
	 * Libera la memoria de las imagenes contenidas en las texturas del
	 * modelo. Esto es pr�ctico si se utiliza una librer�a gr�fica a la que
	 * se le proporcionan las im�genes y que las mantiene. En ese caso puede
	 * no tener sentido que el programa mantenga tambi�n las im�genes
	 * desaprovechando memoria. Tras la llamada a este m�todo la informaci�n
	 * b�sica de las texturas (cuantas son, su altura, anchura, etc.) se
	 * sigue almacenando; pero los datos del color de los p�xeles se libera.
	 * En realidad llama al m�todo MDLTexture::freeImage() con todas las
	 * texturas del modelo.
	 * <p>
	 * Este m�todo s�lo debe ser llamado si se ha realizado con �xito la
	 * inicializaci�n del modelo.
	 */
	void freeTextures();

#ifndef NDEBUG
	/**
	 * Devuelve la diferencia entre el n�mero de transformaciones
	 * necesarias por cada fotograma con la unificaci�n de las matrices
	 * de adyacencia y sin ella. En concreto, es la resta el doble del
	 * n�mero de v�rtices "unificados" (el doble porque hay el mismo
	 * n�mero de v�rtices que de normales) menos el n�mero de v�rtices
	 * y de normales que hab�a en el submodelo seg�n se guardaba en el MDL.<p>
	 * Este m�todo s�lo tiene utilidad para estad�sticas por curiosidad.
	 * De hecho s�lo existe en la compilaci�n en modo depuraci�n. <p>
	 * En realidad, se devuelve la suma de los incrementos en todas los
	 * posibles recubrimientos de todas las partes del cuerpo. En la pr�ctica,
	 * s�lo se utilizar� un recubrimiento para cada parte del cuerpo, por lo
	 * que no se incrementa en ese n�mero por fotograma si hay m�s de un
	 * recubrimiento en alguna parte del cuerpo.
	 * <p>
	 * Este m�todo s�lo debe ser llamado si se ha realizado con �xito la
	 * inicializaci�n del modelo.
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
	 * la informaci�n del modelo. Esta liberaci�n s�lo se realiza si
	 * realmente existe memoria solicitada para ello.<p>
	 * Esta funci�n es auxiliar. Es llamada por MDLCore::freeModel y por
	 * MDLCore::pack.
	 */
	void freeLowLevel();

	/**
	 * Libera la memoria de las estructuras de alto nivel usadas por el
	 * modelo. Por alto nivel nos referimos a los bloques de memoria con
	 * la informaci�n del modelo organizada en clases de un modo c�modo
	 * para proporcionarsela al usuario. Esta liberaci�n s�lo se realiza
	 * si realmente existe memoria solicitada para ello.<p>
	 * Esta funci�n es auxiliar. Es llamada por MDLCore::freeModel y por
	 * MDLCore::pack. Establece MDL:Core::_packInvoked a falso.
	 */
	void freeHighLevel();

	/**
	 * M�todo auxiliar para la implementaci�n del m�todo MDLCore::pack.
	 * En realidad contiene toda la implementaci�n salvo la del control
	 * de la memoria. La implementaci�n de MDLCore::pack llama a este
	 * m�todo, y se ocupa de liberar la memoria de los atributos del objeto
	 * en funci�n del resultado. Eso facilita la implementaci�n sin usar
	 * goto's :-)
	 * @param connectedTriangles Si se desea que las mallas del modelo
	 * mantengan los tri�ngulos conectados, ya sea en modo FAN o en modo
	 * STRIP, se debe pasar cierto en este par�metro. Si el motor no es
	 * capaz de dibujar tri�ngulos compactos, se pasa falso, de modo que
	 * los tri�ngulos se almacenan de forma separada. Vea
	 * HalfLifeMDL::MDLMesh para m�s informaci�n.
	 * @param clockwise Especifica el orden de los v�rtices que crean la
	 * cara delantera en las mallas del modelo que se crear�. Si se desea
	 * que los tri�ngulos devueltos tengan su cara frontal con los v�rtices
	 * en el sentido de las agujas del reloj hay que especificar cierto en
	 * este campo, y falso en caso contrario. <br>
	 * Si se solicitan las caras con los v�rtices en el sentido contrario a
	 * las agujas del reloj, y manteniendo los tri�ngulos conectados, las
	 * mallas en modo STRIP tendr�n un primer tri�ngulo "corrupto", en el
	 * que el primer v�rtice y el tercero ser�n el mismo.
	 * 
	 * @return 
	 *	- Cierto : la informaci�n proporcionada sobre el modelo es
	 * correcta y el modelo puede empezar a utilizarse. 
	 *	- Falso : se ha producido alg�n error.
	 * @see MDLCore::pack
	 */
	bool auxPack(bool connectedTriangles, bool clockwise);

	/**
	 * Enumerado que se utiliza para definir constantes (en lugar de
	 * utilizar defines).
	 */
	enum MDLCoreConstants {
		/** M�ximo n�mero de ficheros externos con animaciones adicionales. */
		MaxExternalSequenceFiles = 32
	};

	/**
	 * Puntero a la cabecera. Realmente no s�lo guarda la cabecera, sino
	 * que a partir de esta posici�n va todo el fichero. El puntero se
	 * utilizar� por lo tanto para acceder a los campos de la cabecera y
	 * para usar los desplazamientos que contiene para buscar la posici�n
	 * en memoria del resto de bloques de informaci�n.
	 * <p>
	 * Este puntero s�lo es v�lido antes de la llamada a MDLCore::pack;
	 */
	header_t* _header;

	/**
	 * Puntero a la cabecera del fichero con las texturas y la informaci�n
	 * de las pieles del modelo. Si las texturas est�n en el mismo fichero
	 * que la geometr�a, este puntero ser� el mismo que _header. En caso
	 * contrario, apuntar� a la informaci�n del fichero modelT.mdl (suponiendo
	 * que el nombre del fichero con la geometr�a es model.mdl). Al igual que
	 * en _header, la zona de memoria apuntada no s�lo contiene la cabecera,
	 * sino que almacena todo el fichero. El puntero se usar� por lo tanto
	 * para acceder a los campos de la cabecera y para usar los
	 * desplazamientos que contiene para buscar la posici�n en memoria del
	 * resto de bloques de informaci�n.
	 * <p>
	 * Este puntero s�lo es v�lido antes de la llamada a MDLCore::pack;
	 */
	header_t* _textureHeader;

	/**
	 * Vector de punteros a las cabeceras de los ficheros con animaciones
	 * adicionales. Un fichero puede almacenar la geometr�a y dejar algunas
	 * animaciones sin definir, y leerlas de otros ficheros. En ese caso
	 * los punteros a las cabeceras de esos ficheros se almacenan aqu�.
	 * En realidad las zonas de memoria apuntadas no s�lo contienen las
	 * cabeceras, sino que almacenan los ficheros completos. Los punteros
	 * se usan por lo tanto para acceder a los campos de las cabeceras y
	 * para usar los desplazamientos que contiene para buscar la posici�n
	 * en memoria del resto de bloques de informaci�n.<p>
	 * El primer elemento apunta al primer fichero externo. Las secuencias
	 * almacenadas por el fichero de la geometr�a deben ser accedidas 
	 * a trav�s de _header.
	 * <p>
	 * Este puntero s�lo es v�lido antes de la llamada a MDLCore::pack;
	 */
	header_t* _animHeader[MaxExternalSequenceFiles];

	/**
	 * Atributo que almacena si se ha realizado la llamada a MDLCore::pack
	 * o no. En funci�n de su valor, los m�todos de acceso a la informaci�n
	 * del modelo ser�n v�lidos o no.
	 * <p>
	 * Tambi�n se utiliza en los m�todos de inicializaci�n para saber qu�
	 * memoria hay que liberar.
	 */
	bool _packInvoked;

	/**
	 * Nombre del modelo. S�lo es correcto tras llamar a MDLCore::pack.
	 */
	std::string _name;

	/**
	 * Vector con los huesos del modelo. Este vector s�lo es correcto tras
	 * llamar a MDLCore::pack. Se almacenan punteros a huesos. Su tama�o se
	 * almacena en MDLCore::_numBones.
	 */
	MDLBone** _bones;

	/**
	 * N�mero de huesos del modelo.
	 */
	unsigned int _numBones;

	/**
	 * Vector con las texturas del modelo. Este vector s�lo es correcto tras
	 * llamar a MDLCore::pack. Se almacenan punteros a texturas. Su tama�o se
	 * almacena en MDLCore::_numTextures.
	 */
	MDLTexture** _textures;

	/**
	 * N�mero de texturas del modelo.
	 */
	unsigned int _numTextures;

	/**
	 * Vector con las secuencias del modelo. Este vector s�lo es correcto tras
	 * llamar a MDLCore::pack. Se almacenan punteros a secuencias. Su tama�o se
	 * almacena en MDLCore::_numSequences.
	 */
	MDLSequence** _sequences;

	/**
	 * N�mero de secuencias del modelo. Este valor s�lo es correcto tras
	 * llamar a MDLCore::pack.
	 */
	unsigned int _numSequences;

	/**
	 * Vector con las partes del modelo. Este vector s�lo es correcto tras
	 * llamar a MDLCore::pack. Se almacenan punteros a partes del cuerpo.
	 * Su tama�o se almacena en MDLCore::_numBodyParts.
	 */
	MDLBodyPart** _bodyParts;

	/**
	 * N�mero de partes del modelo.
	 */
	unsigned int _numBodyParts;

	/**
	 * Vector con los puntos de uni�n del modelo. Este vector s�lo es
	 * correcto tras llamar a MDLCore::pack. Se almacenan punteros a
	 * puntos de uni�n. Su tama�o se almacena en MDLCore::_numAttachments.
	 */
	MDLAttachment** _attachments;

	/**
	 * Informaci�n sobre las pieles del modelo. Este puntero s�lo es
	 * correcto tras llamar a MDLCore::pack.
	 */
	MDLSkins* _skins;

	/**
	 * N�mero de puntos de uni�n del modelo. Este valor s�lo es correcto
	 * tras llamar a MDLCore::pack
	 */
	unsigned int _numAttachments;

	/**
	 * Bounding Box. Te�ricamente, este campo deber�a contener la posici�n
	 * del v�rtice inferior del bounding box alineado con los ejes del modelo
	 * en su posici�n por defecto. La realidad es que este valor se obtiene
	 * directamente del fichero, y en los modelos probados, su valor es 
	 * cero.<br>
	 * @see HalfLifeMDL::header_t::bbmin
	 */
	MDLPoint _bbmin;

	/**
	 * Bounding Box. Te�ricamente, este campo deber�a contener la posici�n
	 * del v�rtice superior del bounding box alineado con los ejes del modelo
	 * en su posici�n por defecto. La realidad es que este valor se obtiene
	 * directamente del fichero, y en los modelos probados, su valor es 
	 * cero.<br>
	 * @see HalfLifeMDL::header_t::bbmax
	 */
	MDLPoint _bbmax;

private:

	/**
	 * Constructor copia. Se declara privado porque no debe utilizarse, pues
	 * no est� implementado. Su implementaci�n deber�a hacer copia de todos
	 * los punteros para no compartir memoria y que los destructures no
	 * fallen.
	 * @param source Objeto a partir del que construir el nuevo.
	 */
	MDLCore(const MDLCore& source) { assert(!"Error: uso de constructor privado"); }

	/**
	 * Operador de asignaci�n. Se declara privado porque no debe utilizarse,
	 * pues no est� implementado. Su implementaci�n deber�a hacer copia de 
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
