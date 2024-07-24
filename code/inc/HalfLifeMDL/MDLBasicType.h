//---------------------------------------------------------------------------
// MDLBasicType.h
//---------------------------------------------------------------------------

/**
 * @file MDLBasicType.h
 * Contiene la declaraci�n del tipo MDLBasicType.
 * @see HalfLifeMDL::MDLBasicType
 * @author Pedro Pablo G�mez Mart�n
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLBasicType
#define __HalfLifeMDL_MDLBasicType

namespace HalfLifeMDL {

/**
 * Tipo b�sico utilizado para guardar coordenadas en la parte de alto
 * nivel del namespace HalfLifeMDL. Se utiliza para los puntos o v�rtices
 * para los cuaterniones, y para las matrices de transformaci�n. <br>
 * Se ha creado este tipo para facilitar su cambio. Es posible establer
 * cualquier tipo que tenga implementadas las operaciones aritm�ticas
 * b�sicas.
 *
 * @see HalfLifeMDL::MDLPoint
 * @see HalfLifeMDL::MDLMatrix
 * @see HalfLifeMDL::MDLQuaternion
 */
typedef float MDLBasicType;

/**
 * Enumerado sin sentido como tipo, que se usa para definir la constante
 * que contiene como posibles valores.
 */
enum MDLConstants {
	/**
	 * Constante usada en la librer�a que permite modificar su funcionamiento
	 * respecto a los modelos que no tienen texturas. <p>
	 * <p>
	 * Si esta constante se deja a 0, no se permitir� empaquetar un modelo
	 * para el que no se haya proporcionado informaci�n sobre texturas, ya
	 * sea con el fichero de la geometr�a o con otro. <p>
	 * Si se desea admitir esa posibilidad, se tendr� que definir esta
	 * constante con el valor 1, en cuyo caso no se generar� error al
	 * empaquetar un modelo sin texturas o pieles. El modelo podr� pintarse
	 * sin usar texturas, o utilizando texturas externas. Ser� responsabilidad
	 * del usuario encargarse del trabajo de la gesti�n de pieles y texturas.
	 * <p>
	 * La posibilidad de admitir empaquetar modelos sin texturas o pieles
	 * se deja abierta y sencilla de conseguir porque algunos modelos del
	 * juego original de Half Life no <em>parecen</em> tener texturas o
	 * pieles. En concreto, el modelo <code>player.mdl</code> no posee
	 * texturas, y no existe un <code>playerT.mdl</code> que, normalmente,
	 * ser�a usado para buscarlas. Si se quiere representar ese modelo,
	 * no podr�an aplicarse texturas sobre �l, pues no est�n disponibles. <br>
	 * Ahora bien, eso no significa que el modelo no haga referencia a
	 * texturas. Es decir, las mallas del modelo indican coordenadas de
	 * textura, y referencian a texturas dentro de la piel actual, por lo que
	 * el usuario de la librer�a podr�a arreglarselas para leer las texturas
	 * de alg�n otro lugar y usarlas para proyectarlas en el modelo. En este
	 * caso, las coordenadas de textura indicadas por los v�rtices de la malla
	 * se proporcionan "crudas" tal y como se almacenan en el fichero MDL.
	 * En condiciones normales (si hay texturas), las coordenadas del fichero
	 * se <b>dividen</b> por el ancho (<code>u</code>) o el alto
	 * (<code>v</code>) de la textura que se mapear�a en la malla. Si no
	 * hay informaci�n sobre las dimensiones (porque no hay texturas), el
	 * usuario tendr� que arreglarselas para pasar las coordenadas dadas
	 * a valores (normalmente entre 0 y 1) que tengan sentido. <p>
	 * Como curiosidad, se ha dicho que el modelo <code>player.mdl</code>
	 * <em>parece</em> no tener texturas. Realmente es as�: el fichero no
	 * contiene imagenes, y no existe un <code>playerT.mdl</code> en el
	 * arbol de directorios creado por la instalaci�n de Half Life. Sin
	 * embargo, al conocer el formato del fichero de los modelos, se sabe
	 * que empiezan o bien por la cadena "IDST" o por "IDSQ" (vease
	 * HalfLifeMDL::header_t::id). Si se tiene un poco de curiosidad, surge
	 * pronto la idea de buscar esas cadenas en todos los ficheros del juego
	 * por si estuvieran encerrados en alg�n fichero sin extensi�n 
	 * <code>.mdl</code>. Y, efectivamente, los ficheros .PAK parecen tener
	 * en su interior modelos. <br>
	 * Abriendo con un editor hexadecimal esos ficheros, se intuye que
	 * contienen ficheros de diferente tipo, junto con alg�n tipo de
	 * informaci�n adicional para saber cual es cual y donde empiezan. Pero
	 * los ficheros no est�n ni comprimidos ni encriptados. Buscando la
	 * cadena "IDST" se encuentra la posici�n de inicio de modelos, o ficheros
	 * con texturas para otros modelos. Buscando la cadena "IDSQ" se
	 * encuentran "ficheros" que contienen secuencias para otros modelos
	 * (en realidad no en todas las apariciones de la cadena IDSQ comienza
	 * un fichero MDL; lo usar�n para alguna otra cosa). <br>
	 * El siguiente paso es extraerlos... :-) Para eso es necesario conocer
	 * la longitud del fichero MDL para no sacar m�s de la cuenta. Eso se
	 * obtiene del campo longitud de la cabecera. Es sencillo encontrar ese
	 * campo desplazandose hacia adelante 64 bytes (0x40) a partir del
	 * primer caracter del nombre, que se identifica f�cilmente. El campo
	 * del tama�o es un entero de 32 bits, e indica el n�mero de bytes
	 * desde el principio (posici�n de la I de IDST) hasta el final del
	 * fichero. Como adem�s el nombre del fichero se incluye dentro de la
	 * cabecera, el resto es trivial. <p>
	 * Hay algunos ficheros que no indican nombre. El campo del nombre
	 * sigue ocupando los 64 bytes, pero est� vac�o. Esos tienen todas las
	 * papeletas para ser ficheros que s�lo poseen texturas. Se puede
	 * comprobar mirando por encima lo que sigue en el fichero. Si se ven
	 * algunas cadenas con nombres de ficheros de imagenes (con extensi�n
	 * .BMP), el MDL s�lo contendr� texturas. Si las cadenas tienen nombres
	 * sospechosos de pertenecer a huesos (como "Bone12" o "Bip01 Pelvis")
	 * entonces el fichero contiene geometr�a. <p>
	 * La cuesti�n interesante es que algunos de los modelos almacenados
	 * que s�lo contienen texturas encajan perfectamente con los requisitos
	 * de <code>player.mdl</code>, como por ejemplo el que empieza en la
	 * posici�n 221683084 (en decimal) de .\\valve\\pak0.pak. Si se extrae y
	 * se utiliza como fichero de texturas, el modelo se representar�
	 * correctamente. <p>
	 * Como los ficheros tienen bastante informaci�n textual (sobre su nombre,
	 * el nombre de los ficheros de imagenes de los que se han sacado las
	 * texturas que contiene y cosas as�), en general no es dif�cil
	 * sacar todos los ficheros (los de geometr�a, de textura y de
	 * secuencias) conociendo el "convenio" de nombres usado por el juego
	 * original descrito en MDLCore.
	 */
	AllowModelsWithoutTextures = 0
};

} // namespace HalfLifeMDL

#endif
