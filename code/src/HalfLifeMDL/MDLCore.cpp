//---------------------------------------------------------------------------
// MDLCore.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLCore.cpp
 * Contiene la definición (implementación) de la clase básica que almacena
 * la información de un modelo de Half Life. (HalfLifeMDL::MDLCore). Esta clase tiene
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

#include <iostream>
#include <assert.h>

#include "MDLCore.h"

#ifndef NULL
/**
 * Valor nulo para los punteros.
 */
#define NULL 0
#endif

namespace HalfLifeMDL {

/**
 * Constructor sin parámetros. Inicializa todos los punteros a
 * NULL.
 */
MDLCore::MDLCore() {

	_header = NULL;
	_textureHeader = NULL;
	_packInvoked = false;
	for (unsigned int i = 0; i < MaxExternalSequenceFiles; i++)
		_animHeader[i] = NULL;

	_bones = NULL;
	_textures = NULL;
	_sequences = NULL;
	_bodyParts = NULL;
	_attachments = NULL;
	_skins = NULL;

} // Constructor

//---------------------------------------------------------------------------

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
bool MDLCore::setMainFile(MDLchar* buffer) {

	if (buffer == NULL) {
		freeModel();
		return true;
	}

	if ((_header != NULL) || (_packInvoked))
		freeModel();

	_header = (header_t*) buffer;

	if ((_header->id != 0x54534449) &&	// "IDST"
		(_header->id != 0x51534449)) {	// "IDSQ"
		_header = NULL;
		return false;
	}

	if (_header->version != 10) {
		_header = NULL;
		return false;
	}

	_textureHeader = _header;

	return true;

} // setMainFile

//---------------------------------------------------------------------------

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
bool MDLCore::setTextureFile(MDLchar* buffer) {

	if ((buffer == NULL) || (_header == NULL)
			|| (_header != _textureHeader) || _packInvoked)
		return false;

	if (_header->numtextures != 0)
		return false;

	_textureHeader = (header_t*) buffer;

	if ((_textureHeader->id != 0x54534449) &&	// "IDST"
		(_textureHeader->id != 0x51534449)) {	// "IDSQ"
		_textureHeader = NULL;
		return false;
	}

	if (_textureHeader->version != 10) {
		_textureHeader = NULL;
		return false;
	}

	return true;

} // setTextureFile

//---------------------------------------------------------------------------

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
bool MDLCore::setSeqGroupFile(MDLchar* buffer, unsigned int index) {

	if ((buffer == NULL) || (_header == NULL) || _packInvoked)
		return false;

	if ((index > _header->numseqgroups) || (index < 1))
		return false;

	_animHeader[index - 1] = (header_t*) buffer;

	if (_animHeader[index - 1]->id != 0x51534449) {	// "IDSQ".
		_animHeader[index - 1] = NULL;				//  "IDST" no permitido.
		return false;
	}

	if (_animHeader[index - 1]->version != 10) {
		_animHeader[index - 1] = NULL;
		return false;
	}

	return true;

} // setSeqGroupFile

//---------------------------------------------------------------------------

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
 * 
 * @return 
 *	- Cierto : la información proporcionada previamente es
 * correcta y el modelo puede empezar a utilizarse. 
 *	- Falso : se ha producido algún error.
 * @param clockwise Especifica el orden de los vértices que crean la
 * cara delantera en las mallas del modelo que se creará. Si se desea
 * que los triángulos devueltos tengan su cara frontal con los vértices
 * en el sentido de las agujas del reloj hay que especificar cierto en
 * este campo, y falso en caso contrario.
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
bool MDLCore::pack(bool connectedTriangles, bool clockwise) {

	bool result;
	result = auxPack(connectedTriangles, clockwise);

	if (result) {
		// Todo ha ido bien. Liberamos la memoria de la parte del fichero.
		freeLowLevel();
		_packInvoked = true;
		return true;
	}
	else {
		// Algo ha fallado. Liberamos la memoria que se haya podido pedir
		// en la parte de alto nivel, y acabamos.
		freeHighLevel();
		_packInvoked = false;
		return false;
	}

} // pack

//---------------------------------------------------------------------------

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

void MDLCore::freeTextures() {
	assert(_packInvoked || !"Modelo no inicializado");
	
	if (_textures == NULL)
		return;
	
	for (unsigned int cont = 0; cont < _numTextures; cont++)
		_textures[cont]->freeImage();

} // freeTextures

//---------------------------------------------------------------------------

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
int MDLCore::getIncrTransformations() {
	assert(_packInvoked);
	int result = 0;

	if (_bodyParts != NULL) {
		for (unsigned int c = 0; c < _numBodyParts; c++)
			result += _bodyParts[c]->getIncrTransformations();
	}
	return result;

} // getIncrTransformations

#endif

//---------------------------------------------------------------------------

/*
 * Libera la memoria de las estructuras usadas por el modelo.
 */
void MDLCore::freeModel() {

	freeLowLevel();
	freeHighLevel();

} // freeModel

//---------------------------------------------------------------------------

/**
 * Libera la memoria de las estructuras de bajo nivel usadas por el
 * modelo. Por bajo nivel nos referimos a los bloques de memoria donde
 * se almacena el contenido de los ficheros desde los que se ha obtenido
 * la información del modelo. Esta liberación sólo se realiza si
 * realmente existe memoria solicitada para ello.<p>
 * Esta función es auxiliar. Es llamada por MDLCore::freeModel y por
 * MDLCore::pack.
 */
void MDLCore::freeLowLevel() {

	if ((_textureHeader != _header) && (_textureHeader != NULL))
		delete[] (MDLchar*) _textureHeader;

	_textureHeader = NULL;

	if (_header != NULL) {
		delete[] (MDLchar*) _header;
		_header = NULL;
	}

	for (unsigned int i = 0; i < MaxExternalSequenceFiles; i++) {
		if (_animHeader[i] != NULL) {
			delete[] (MDLchar*) _animHeader[i];
			_animHeader[i] = NULL;
		}

	}

} // freeLowLevel

//---------------------------------------------------------------------------

/**
 * Libera la memoria de las estructuras de alto nivel usadas por el
 * modelo. Por alto nivel nos referimos a los bloques de memoria con
 * la información del modelo organizada en clases de un modo cómodo
 * para proporcionarsela al usuario. Esta liberación sólo se realiza
 * si realmente existe memoria solicitada para ello.<p>
 * Esta función es auxiliar. Es llamada por MDLCore::freeModel y por
 * MDLCore::pack. Establece MDL:Core::_packInvoked a falso.
 */
void MDLCore::freeHighLevel() {

	unsigned int cont;
/*
	// Originalmente utilicé una macro con la siguiente definición para
	// ahorrar código en la liberación de arrays. Finalmente lo eliminé por
	// tres razones:
	//    - Stroustrup desaconseja su uso.
	//    - Al ser una macro de varias líneas, había una alta probabilidad
	//      de que los depuradores o mensajes de error no funcionaran como
	//      es debido.
	//    - Doxygen lanzaba un warning por no documentarla (y, desde luego,
	//      no quería documentarla) :-)
	// Por tanto, eliminé la macro, y hay mucho código similar dentro de
	// este método.
	if (array != NULL) {
		for (cont = 0; cont < tamanyo; ++cont)
			if (array[cont] != NULL)
				delete array[cont];
		delete[] array;
		array = NULL;
	}
*/

	// HUESOS
	if (_bones != NULL) {
		for (cont = 0; cont < _numBones; ++cont)
			if (_bones[cont] != NULL)
				delete _bones[cont];
		delete[] _bones;
		_bones = NULL;
	}

	// TEXTURAS
	if (_textures != NULL) {
		for (cont = 0; cont < _numTextures; ++cont)
			if (_textures[cont] != NULL)
				delete _textures[cont];
		delete[] _textures;
		_textures = NULL;
	}

	// SECUENCIAS
	if (_sequences != NULL) {
		for (cont = 0; cont < _numSequences; ++cont)
			if (_sequences[cont] != NULL)
				delete _sequences[cont];
		delete[] _sequences;
		_sequences = NULL;
	}

	// PARTES DEL CUERPO
	if (_bodyParts != NULL) {
		for (cont = 0; cont < _numBodyParts; ++cont)
			if (_bodyParts[cont] != NULL)
				delete _bodyParts[cont];
		delete[] _bodyParts;
		_bodyParts = NULL;
	}

	// Attachments
	if (_attachments != NULL) {
		for (cont = 0; cont < _numAttachments; ++cont)
			if (_attachments[cont] != NULL)
				delete _attachments[cont];
		delete[] _attachments;
		_attachments = NULL;
	}

	// Pieles
	if (_skins != NULL) {
		delete _skins;
		_skins = NULL;
	}

	_packInvoked = false;

} // freeHighLevel

//---------------------------------------------------------------------------

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
 * @return 
 *	- Cierto : la información proporcionada sobre el modelo es
 * correcta y el modelo puede empezar a utilizarse. 
 *	- Falso : se ha producido algún error.
 * @see MDLCore::pack
 */
bool MDLCore::auxPack(bool connectedTriangles, bool clockwise) {

	unsigned int cont;

	// Información general
	_name = _header->name;

	// BOUNDING BOXES
	_bbmin.set(_header->bbmin[0], _header->bbmin[1], _header->bbmin[2]);
	_bbmax.set(_header->bbmax[0], _header->bbmax[1], _header->bbmax[2]);

	// HUESOS

	// Algunos ficheros (los que contienen animaciones, no geometría)
	// tienen sin inicializar con un valor con sentido el campo del
	// número de huesos. Eso es problemático, porque suponer que el
	// valor leído es correcto puede llevar a pedir muchísima memoria,
	// intentar realizar bucles larguísimos, o realizar acceso a memoria
	// incorrectos. Para intentar detectar el problema, se comprueba
	// que el número de huesos declarado tiene sentido, mirando que
	// entra en el espacio ocupado por el fichero. Si alguna de las
	// pruebas de integridad falla, se devuelve falso.
	// Hay más información en la documentación del método pack.
	if ((_header->boneindex > _header->length) ||
			(_header->numbones * sizeof(bone_t)
						+ _header->boneindex > _header->length))
		return false;

	// Recorremos el array apuntado por _header->boneindex para
	// construir los objetos de la clase MDLBone que son más cómodos
	// para almacenar la información de la jerarquía de huesos.
	// Se debe cumplir que la lista de huesos del fichero esté en
	// preorden... más concretamente que antes de llegar a la información
	// de un determinado hueso se haya pasado por el padre.
	// En realidad en todos los ficheros analizados están en preorden,
	// por lo que no se comprueba salvo que se esté en modo depuración.
	bone_t* bone;
	bone = (bone_t*)((MDLchar*)_header + _header->boneindex);
	_numBones = _header->numbones;
	// Pedimos memoria para los huesos.
	if (_numBones > 0)
		_bones = new MDLBone*[_numBones];

#ifndef NDEBUG
	for (cont = 0; cont < _numBones; ++cont)
		// Por si acaso salimos del array siguiente antes de tiempo
		// para no tener problemas en la liberación de la memoria
		// ponemos a NULL todos los punteros a los huesos.
		// Esto sólo es necesario si se está en modo depuración
		// y la salida es por algún assert.
		_bones[cont] = NULL;
#endif

	for (cont = 0; cont < _numBones; ++cont, ++bone) {
		assert (bone->parent < (int) cont ||
					!"Los huesos del fichero no están en preorden.");
		assert (bone->parent < (int) _numBones ||
					!"Hueso indefinido en la jerarquía.");
		_bones[cont] = new MDLBone(bone,
					(bone->parent < 0) ? NULL : _bones[bone->parent]);
	}

	// PUNTOS DE UNIÓN
	// Algunos ficheros (los que contienen animaciones, no geometría)
	// tienen sin inicializar con un valor con sentido el campo del
	// número de puntos de unión. Hacemos las comprobaciones obvias
	// igual que con las texturas.
	// En realidad la documentación de StudioMDL indica que sólo puede
	// haber como mucho cuatro puntos de unión, por lo que podríamos
	// usar eso como comprobación. No obstante no nos impondremos 
	// nosotros aquí ese límite (no está establecido en el resto de
	// la librería), y supondremos que puede haber cualquier número.
	// Eso complica ligeramente la comprobación, pues debe usar el
	// tamaño del fichero para saber si el número está mal.
	if ((_header->attachmentindex > _header->length) ||
			(_header->numattachments * sizeof(attachment_t)
						+ _header->attachmentindex > _header->length))
		return false;
	// Recorremos el array apuntado por _header->attachmentindex para
	// construir los objetos de la clase MDLAttachment que son más cómodos
	// para almacenar la información.
	attachment_t* attach;
	attach = (attachment_t*) ((MDLchar*)_header + _header->attachmentindex);
	_numAttachments = _header->numattachments;
	if (_numAttachments > 0)
		_attachments = new MDLAttachment*[_numAttachments];
	for (cont = 0; cont < _numAttachments; ++cont, ++attach)
		_attachments[cont] = new MDLAttachment(attach,
												(const MDLBone**)_bones);

	// SECUENCIAS
	seqdesc_t* seq;
	seq = (seqdesc_t*)((MDLchar*)_header + _header->seqindex);
	_numSequences = _header->numseq;
	if (_numSequences > 0) {
		_sequences = new MDLSequence*[_numSequences];
		for (cont = 0; cont < _numSequences; ++cont, ++seq)
			_sequences[cont] = new MDLSequence(seq,
								(event_t*)((MDLchar*)_header + seq->eventindex),
								_numBones, 
								(anim_t*)((MDLchar*)_header + seq->animindex),
								(const MDLBone**)_bones);
	}
	else
		_sequences = NULL;

	// TEXTURAS
	texture_t* tex;
	tex = (texture_t*)((MDLchar*)_textureHeader +
							_textureHeader->textureindex);
	_numTextures = _textureHeader->numtextures;
	if (_numTextures > 0) {
		_textures = new MDLTexture*[_numTextures];
		for (cont = 0; cont < _numTextures; ++cont, ++tex)
			_textures[cont] = new MDLTexture(tex,
								(MDLuchar*)_textureHeader + tex->index);
	}
	else
		if (AllowModelsWithoutTextures == 0)
			// No soportamos modelos sin texturas. Acabamos con error.
			return false;

	// PIELES
	if ((AllowModelsWithoutTextures == 0) && 
		((_textureHeader->numskinfamilies == 0) ||
		(_textureHeader->numskinref == 0) ||
		(_textureHeader->skinindex == 0)))
		// No hay información correcta de pieles. No lo admitimos, pues
		// las mallas referencian pieles
		return false;
	_skins = new MDLSkins(_textureHeader->numskinfamilies,
							_textureHeader->numskinref, 
							(MDLushort*)((MDLchar*)_textureHeader +
											_textureHeader->skinindex),
							(const MDLTexture**) _textures);

	// PARTES DEL CUERPO
	bodyparts_t* bodyPart;
	bodyPart = (bodyparts_t*)((MDLchar*)_header + _header->bodypartindex);
	_numBodyParts = _header->numbodyparts;
	if (_numBodyParts > 0) {
		_bodyParts = new MDLBodyPart*[_numBodyParts];
		for (cont = 0; cont < _numBodyParts; ++cont, ++bodyPart)
			_bodyParts[cont] = new MDLBodyPart(bodyPart,
											(unsigned int) _header,
											connectedTriangles,
											clockwise, _skins);
	}

	return true;

} // auxPack

//---------------------------------------------------------------------------

/**
 * Método que busca cosas en el fichero. Es un método de depuración
 * usado para analizar modelos e intentar deducir qué significan algunos
 * campos.
 * @todo Eliminar el método :-) También habría que quitar el #include del
 * cpp para poder escribir por pantalla en este método.
 */
void MDLCore::analizar() {

	// Analizamos las secuencias para ver los valores de motiontype.
	header_t* cabecera;
	seqdesc_t* secuencia;
//	anim_t* animacion;

	cabecera = _header;
/*
	// Escribimos la posición de la articulación padre.
	bone_t* padre;	// Hueso raíz.
	unsigned int idPadre;	// Índice del hueso padre en el array de huesos.
	padre = ((bone_t*)(((char*)cabecera) + cabecera->boneindex));
	idPadre = 0;
	while (padre->parent != -1) {
		padre++;
		idPadre++;
	}

	std::cout << "Hueso padre por defecto: (" << padre->value[DoF_X] << ", "
			<< padre->value[DoF_Y] << ", "
			<< padre->value[DoF_Z] << ")" << std::endl;
*/
	int grupoDeSecuencia = 0;
	std::cout << cabecera->name << std::endl;
/*
	if (cabecera->numseqgroups == 0)
		std::cout << "NÚMERO DE GRUPOS DE SECUENCIA 0!!!!!!!!\n";
	else {
		for (unsigned int s = 0; s < cabecera->numseqgroups; s++) {
			std::cout << "Grupo " << s << ". Label: ";
			std::cout << ((seqgroup_t*)((char*) cabecera + cabecera->seqgroupindex))[s].label;
			std::cout << " name: ";
			std::cout << ((seqgroup_t*)((char*) cabecera + cabecera->seqgroupindex))[s].name;
			std::cout << " data: ";
			std::cout << ((seqgroup_t*)((char*) cabecera + cabecera->seqgroupindex))[0].data;
			std::cout << "\n";
			if (((seqgroup_t*)((char*) cabecera + cabecera->seqgroupindex))[0].data != 0)
				std::cout << "¡¡¡¡¡¡DATA DIFERENTE DE 0!!!!!!\n";
		}
	}
*/
/*
	bonecontroller_t *controladorHueso;
	if (cabecera->numbonecontrollers > 0) {
		controladorHueso = (bonecontroller_t*)((char*)cabecera + cabecera->bonecontrollerindex);

	}
*/

//	std::cout << "Número de secuencias: " << _header->numseq << std::endl;
	// Ñapa.
	if (_header->numseqgroups == 0) _header->numseq = 0;
	unsigned int i;
/*
	if (_header->numattachments != 0) {
		std::cout << "Hay " << _header->numattachments << " attachments."
				<< " Por defecto el tipo es 0 y sin nombre." << std::endl;
		attachment_t* attach;
		attach = (attachment_t*) ((char*) _header + _header->attachmentindex);
		for (i = 0; i < _header->numattachments; i++, attach++) {
				std::cout << i << std::endl;
				if (attach->name[0] != '\0')
					std::cout << "\tNombre: \"" << attach->name << "\"" << std::endl;
				if (attach->type != 0)
					std::cout << "\tTipo: " << attach->type << std::endl;
				std::cout << "\tHueso: " << attach->bone << std::endl;
				std::cout << "\tOrg: (" << attach->org[0] << 
							 ", " << attach->org[1] << ", " << attach->org[2] << ")" << std::endl;
				for (unsigned int v = 0; v < 3; v++) {
					std::cout << "\tvector["<<v<<"] : " << attach->vectors[v][0] << 
							 ", " << attach->vectors[v][1] << ", " 
							 << attach->vectors[v][2] << ")" << std::endl;
				}
		}
	}
*/
	for (i = 0; i < _header->numseq; i++) {
		secuencia = &((seqdesc_t*)((char*) _header + _header->seqindex))[i];
/*
		// Obtenemos la posición del hueso raíz en el primer fotograma de la
		// animación. No nos preocupamos por mezcla de animaciones.
		seqgroup_t	*pseqgroup;
		pseqgroup = (seqgroup_t *)((char *)_header + _header->seqgroupindex)
						+ secuencia->seqgroup;
		if (secuencia->seqgroup == 0) 
			animacion = (anim_t*) ((char*)_header + pseqgroup->data + secuencia->animindex);
		else
			animacion = (anim_t*) ((char*)_animHeader[secuencia->seqgroup - 1] + secuencia->animindex);

		// animacion apunta a la información sobre las animaciones del hueso 0
		vec3_t pos;
		// Hacemos que apunte a la información del hueso padre.
		animacion += idPadre;
		for (int eje = 0; eje < 3; eje++) {
			pos[eje] = padre->value[eje];
			if (animacion->offset[eje] != 0) {
				pos[eje] += *((short*)(((char*) animacion) + animacion->offset[eje]) + 1);
			}
		}
		std::cout << "\tHueso padre: (" << pos[0] << ", " << pos[1] << ", " << pos[2] << ")\n";
*/

		if ((secuencia->motiontype != 0) || (secuencia->automoveangleindex != 0)
				|| (secuencia->automoveposindex != 0) || (secuencia->numblends != 1)) {
			std::cout << secuencia->label << "(" << secuencia->motiontype << ")\n";
/*			if (secuencia->motionbone != 0) {
				std::cout << "\tmotiontype = " << secuencia->motiontype << std::endl;
				std::cout << "\tmotionbone = " << secuencia->motionbone << std::endl;
			}
			if (secuencia->motiontype != 0) {
				std::cout << "\tlinearmovement = (" << secuencia->linearmovement[0] << ", "
					 << secuencia->linearmovement[1] << ", "
					  << secuencia->linearmovement[2] << ")\n";
			}
			if (secuencia->automoveposindex != 0)
				std::cout << "\tautomoveposindex = "
							<< secuencia->automoveposindex << std::endl;
			if (secuencia->automoveangleindex != 0)
				std::cout << "\tautomoveangleindex = "
							<< secuencia->automoveangleindex << std::endl;
*/
			if (secuencia->numblends > 2) {
				std::cout << "\tnumblends = " << secuencia->numblends << std::endl;
				std::cout << "\t\tblendparent: " << secuencia->blendparent << "\n";
				for (unsigned int nb = 0; nb < secuencia->numblends; nb++) {
					std::cout << "\t\t" << nb << ": Tipo = " <<
								secuencia->blendtype[nb] << " Rango: (" <<
								secuencia->blendstart[nb] << ", " <<
								secuencia->blendend[nb] << ")\n";
				}
			}
		} // if (secuencia->motiontype != 0)

/*
		if (secuencia->entrynode != 0)
			std::cout << "entrynode = " << secuencia->entrynode << std::endl;
		if (secuencia->exitnode != 0)
			std::cout << "exitnode = " << secuencia->exitnode << std::endl;
		if (secuencia->nodeflags != 0)
			std::cout << "nodeflags = " << secuencia->nodeflags << std::endl;
		if (secuencia->blendparent != 0)
			std::cout << "blendparent = " << secuencia->blendparent << std::endl;

		if (secuencia->nextseq != 0)
			std::cout << "nextseq = " << secuencia->nextseq << "(" 
						<< &((seqdesc_t*)((char*) _header + _header->seqindex))[secuencia->nextseq].label 
						<< ")" << std::endl;
		if (secuencia->numpivots != 0) {
			std::cout << "numpivots = " << secuencia->numpivots << std::endl;
			std::cout << "pivotindex = " << secuencia->pivotindex << std::endl;
		}

		for (int e = 0; e < secuencia->numevents; ++e) {
			event_t* evento;
			evento = &(((event_t*)(((MDLchar*)cabecera) + secuencia->eventindex)))[e];
			if (evento->type != 0) {
				std::cout << "TIPO DE UN EVENTO != 0\n";
			}
		}
*/
	} // for que recorría todas las secuencias.
/*
	unsigned int bp;
	bodyparts_t* bodyPart = (bodyparts_t*) 
							(((char*)_header) + _header->bodypartindex);
	std::cout << "Partes del cuerpo:\n";
	for (bp = 0; bp < _header->numbodyparts; bp++, bodyPart++) {
		std::cout << "\t" << bp << ": " << bodyPart->nummodels << " modelos\n";
		model_t* model;
		model = (model_t*) (((char*)_header) + bodyPart->modelindex);
		for (i = 0; i < bodyPart->nummodels; i++, model++) {
			std::cout << "\t\tMalla " << i <<": " << model->numverts << " vértices (";
			std::cout << model->vertindex << ")\n";
		}
	}
*/
} // analizar


} // namespace HalfLifeMDL

