//---------------------------------------------------------------------------
// MDLCore.cpp
//---------------------------------------------------------------------------

/**
 * @file MDLCore.cpp
 * Contiene la definici�n (implementaci�n) de la clase b�sica que almacena
 * la informaci�n de un modelo de Half Life. (HalfLifeMDL::MDLCore). Esta clase tiene
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
 * Constructor sin par�metros. Inicializa todos los punteros a
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
 * 
 * @return 
 *	- Cierto : la informaci�n proporcionada previamente es
 * correcta y el modelo puede empezar a utilizarse. 
 *	- Falso : se ha producido alg�n error.
 * @param clockwise Especifica el orden de los v�rtices que crean la
 * cara delantera en las mallas del modelo que se crear�. Si se desea
 * que los tri�ngulos devueltos tengan su cara frontal con los v�rtices
 * en el sentido de las agujas del reloj hay que especificar cierto en
 * este campo, y falso en caso contrario.
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
 * la informaci�n del modelo. Esta liberaci�n s�lo se realiza si
 * realmente existe memoria solicitada para ello.<p>
 * Esta funci�n es auxiliar. Es llamada por MDLCore::freeModel y por
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
 * la informaci�n del modelo organizada en clases de un modo c�modo
 * para proporcionarsela al usuario. Esta liberaci�n s�lo se realiza
 * si realmente existe memoria solicitada para ello.<p>
 * Esta funci�n es auxiliar. Es llamada por MDLCore::freeModel y por
 * MDLCore::pack. Establece MDL:Core::_packInvoked a falso.
 */
void MDLCore::freeHighLevel() {

	unsigned int cont;
/*
	// Originalmente utilic� una macro con la siguiente definici�n para
	// ahorrar c�digo en la liberaci�n de arrays. Finalmente lo elimin� por
	// tres razones:
	//    - Stroustrup desaconseja su uso.
	//    - Al ser una macro de varias l�neas, hab�a una alta probabilidad
	//      de que los depuradores o mensajes de error no funcionaran como
	//      es debido.
	//    - Doxygen lanzaba un warning por no documentarla (y, desde luego,
	//      no quer�a documentarla) :-)
	// Por tanto, elimin� la macro, y hay mucho c�digo similar dentro de
	// este m�todo.
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
 * @return 
 *	- Cierto : la informaci�n proporcionada sobre el modelo es
 * correcta y el modelo puede empezar a utilizarse. 
 *	- Falso : se ha producido alg�n error.
 * @see MDLCore::pack
 */
bool MDLCore::auxPack(bool connectedTriangles, bool clockwise) {

	unsigned int cont;

	// Informaci�n general
	_name = _header->name;

	// BOUNDING BOXES
	_bbmin.set(_header->bbmin[0], _header->bbmin[1], _header->bbmin[2]);
	_bbmax.set(_header->bbmax[0], _header->bbmax[1], _header->bbmax[2]);

	// HUESOS

	// Algunos ficheros (los que contienen animaciones, no geometr�a)
	// tienen sin inicializar con un valor con sentido el campo del
	// n�mero de huesos. Eso es problem�tico, porque suponer que el
	// valor le�do es correcto puede llevar a pedir much�sima memoria,
	// intentar realizar bucles largu�simos, o realizar acceso a memoria
	// incorrectos. Para intentar detectar el problema, se comprueba
	// que el n�mero de huesos declarado tiene sentido, mirando que
	// entra en el espacio ocupado por el fichero. Si alguna de las
	// pruebas de integridad falla, se devuelve falso.
	// Hay m�s informaci�n en la documentaci�n del m�todo pack.
	if ((_header->boneindex > _header->length) ||
			(_header->numbones * sizeof(bone_t)
						+ _header->boneindex > _header->length))
		return false;

	// Recorremos el array apuntado por _header->boneindex para
	// construir los objetos de la clase MDLBone que son m�s c�modos
	// para almacenar la informaci�n de la jerarqu�a de huesos.
	// Se debe cumplir que la lista de huesos del fichero est� en
	// preorden... m�s concretamente que antes de llegar a la informaci�n
	// de un determinado hueso se haya pasado por el padre.
	// En realidad en todos los ficheros analizados est�n en preorden,
	// por lo que no se comprueba salvo que se est� en modo depuraci�n.
	bone_t* bone;
	bone = (bone_t*)((MDLchar*)_header + _header->boneindex);
	_numBones = _header->numbones;
	// Pedimos memoria para los huesos.
	if (_numBones > 0)
		_bones = new MDLBone*[_numBones];

#ifndef NDEBUG
	for (cont = 0; cont < _numBones; ++cont)
		// Por si acaso salimos del array siguiente antes de tiempo
		// para no tener problemas en la liberaci�n de la memoria
		// ponemos a NULL todos los punteros a los huesos.
		// Esto s�lo es necesario si se est� en modo depuraci�n
		// y la salida es por alg�n assert.
		_bones[cont] = NULL;
#endif

	for (cont = 0; cont < _numBones; ++cont, ++bone) {
		assert (bone->parent < (int) cont ||
					!"Los huesos del fichero no est�n en preorden.");
		assert (bone->parent < (int) _numBones ||
					!"Hueso indefinido en la jerarqu�a.");
		_bones[cont] = new MDLBone(bone,
					(bone->parent < 0) ? NULL : _bones[bone->parent]);
	}

	// PUNTOS DE UNI�N
	// Algunos ficheros (los que contienen animaciones, no geometr�a)
	// tienen sin inicializar con un valor con sentido el campo del
	// n�mero de puntos de uni�n. Hacemos las comprobaciones obvias
	// igual que con las texturas.
	// En realidad la documentaci�n de StudioMDL indica que s�lo puede
	// haber como mucho cuatro puntos de uni�n, por lo que podr�amos
	// usar eso como comprobaci�n. No obstante no nos impondremos 
	// nosotros aqu� ese l�mite (no est� establecido en el resto de
	// la librer�a), y supondremos que puede haber cualquier n�mero.
	// Eso complica ligeramente la comprobaci�n, pues debe usar el
	// tama�o del fichero para saber si el n�mero est� mal.
	if ((_header->attachmentindex > _header->length) ||
			(_header->numattachments * sizeof(attachment_t)
						+ _header->attachmentindex > _header->length))
		return false;
	// Recorremos el array apuntado por _header->attachmentindex para
	// construir los objetos de la clase MDLAttachment que son m�s c�modos
	// para almacenar la informaci�n.
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
		// No hay informaci�n correcta de pieles. No lo admitimos, pues
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
 * M�todo que busca cosas en el fichero. Es un m�todo de depuraci�n
 * usado para analizar modelos e intentar deducir qu� significan algunos
 * campos.
 * @todo Eliminar el m�todo :-) Tambi�n habr�a que quitar el #include del
 * cpp para poder escribir por pantalla en este m�todo.
 */
void MDLCore::analizar() {

	// Analizamos las secuencias para ver los valores de motiontype.
	header_t* cabecera;
	seqdesc_t* secuencia;
//	anim_t* animacion;

	cabecera = _header;
/*
	// Escribimos la posici�n de la articulaci�n padre.
	bone_t* padre;	// Hueso ra�z.
	unsigned int idPadre;	// �ndice del hueso padre en el array de huesos.
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
		std::cout << "N�MERO DE GRUPOS DE SECUENCIA 0!!!!!!!!\n";
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
				std::cout << "������DATA DIFERENTE DE 0!!!!!!\n";
		}
	}
*/
/*
	bonecontroller_t *controladorHueso;
	if (cabecera->numbonecontrollers > 0) {
		controladorHueso = (bonecontroller_t*)((char*)cabecera + cabecera->bonecontrollerindex);

	}
*/

//	std::cout << "N�mero de secuencias: " << _header->numseq << std::endl;
	// �apa.
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
		// Obtenemos la posici�n del hueso ra�z en el primer fotograma de la
		// animaci�n. No nos preocupamos por mezcla de animaciones.
		seqgroup_t	*pseqgroup;
		pseqgroup = (seqgroup_t *)((char *)_header + _header->seqgroupindex)
						+ secuencia->seqgroup;
		if (secuencia->seqgroup == 0) 
			animacion = (anim_t*) ((char*)_header + pseqgroup->data + secuencia->animindex);
		else
			animacion = (anim_t*) ((char*)_animHeader[secuencia->seqgroup - 1] + secuencia->animindex);

		// animacion apunta a la informaci�n sobre las animaciones del hueso 0
		vec3_t pos;
		// Hacemos que apunte a la informaci�n del hueso padre.
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
	} // for que recorr�a todas las secuencias.
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
			std::cout << "\t\tMalla " << i <<": " << model->numverts << " v�rtices (";
			std::cout << model->vertindex << ")\n";
		}
	}
*/
} // analizar


} // namespace HalfLifeMDL

