//---------------------------------------------------------------------------
// MDLBasicType.h
//---------------------------------------------------------------------------

/**
 * @file MDLBasicType.h
 * Contiene la declaración del tipo MDLBasicType.
 * @see HalfLifeMDL::MDLBasicType
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLBasicType
#define __HalfLifeMDL_MDLBasicType

namespace HalfLifeMDL {

/**
 * Tipo básico utilizado para guardar coordenadas en la parte de alto
 * nivel del namespace HalfLifeMDL. Se utiliza para los puntos o vértices
 * para los cuaterniones, y para las matrices de transformación. <br>
 * Se ha creado este tipo para facilitar su cambio. Es posible establer
 * cualquier tipo que tenga implementadas las operaciones aritméticas
 * básicas.
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
	 * Constante usada en la librería que permite modificar su funcionamiento
	 * respecto a los modelos que no tienen texturas. <p>
	 * <p>
	 * Si esta constante se deja a 0, no se permitirá empaquetar un modelo
	 * para el que no se haya proporcionado información sobre texturas, ya
	 * sea con el fichero de la geometría o con otro. <p>
	 * Si se desea admitir esa posibilidad, se tendrá que definir esta
	 * constante con el valor 1, en cuyo caso no se generará error al
	 * empaquetar un modelo sin texturas o pieles. El modelo podrá pintarse
	 * sin usar texturas, o utilizando texturas externas. Será responsabilidad
	 * del usuario encargarse del trabajo de la gestión de pieles y texturas.
	 * <p>
	 * La posibilidad de admitir empaquetar modelos sin texturas o pieles
	 * se deja abierta y sencilla de conseguir porque algunos modelos del
	 * juego original de Half Life no <em>parecen</em> tener texturas o
	 * pieles. En concreto, el modelo <code>player.mdl</code> no posee
	 * texturas, y no existe un <code>playerT.mdl</code> que, normalmente,
	 * sería usado para buscarlas. Si se quiere representar ese modelo,
	 * no podrían aplicarse texturas sobre él, pues no están disponibles. <br>
	 * Ahora bien, eso no significa que el modelo no haga referencia a
	 * texturas. Es decir, las mallas del modelo indican coordenadas de
	 * textura, y referencian a texturas dentro de la piel actual, por lo que
	 * el usuario de la librería podría arreglarselas para leer las texturas
	 * de algún otro lugar y usarlas para proyectarlas en el modelo. En este
	 * caso, las coordenadas de textura indicadas por los vértices de la malla
	 * se proporcionan "crudas" tal y como se almacenan en el fichero MDL.
	 * En condiciones normales (si hay texturas), las coordenadas del fichero
	 * se <b>dividen</b> por el ancho (<code>u</code>) o el alto
	 * (<code>v</code>) de la textura que se mapearía en la malla. Si no
	 * hay información sobre las dimensiones (porque no hay texturas), el
	 * usuario tendrá que arreglarselas para pasar las coordenadas dadas
	 * a valores (normalmente entre 0 y 1) que tengan sentido. <p>
	 * Como curiosidad, se ha dicho que el modelo <code>player.mdl</code>
	 * <em>parece</em> no tener texturas. Realmente es así: el fichero no
	 * contiene imagenes, y no existe un <code>playerT.mdl</code> en el
	 * arbol de directorios creado por la instalación de Half Life. Sin
	 * embargo, al conocer el formato del fichero de los modelos, se sabe
	 * que empiezan o bien por la cadena "IDST" o por "IDSQ" (vease
	 * HalfLifeMDL::header_t::id). Si se tiene un poco de curiosidad, surge
	 * pronto la idea de buscar esas cadenas en todos los ficheros del juego
	 * por si estuvieran encerrados en algún fichero sin extensión 
	 * <code>.mdl</code>. Y, efectivamente, los ficheros .PAK parecen tener
	 * en su interior modelos. <br>
	 * Abriendo con un editor hexadecimal esos ficheros, se intuye que
	 * contienen ficheros de diferente tipo, junto con algún tipo de
	 * información adicional para saber cual es cual y donde empiezan. Pero
	 * los ficheros no están ni comprimidos ni encriptados. Buscando la
	 * cadena "IDST" se encuentra la posición de inicio de modelos, o ficheros
	 * con texturas para otros modelos. Buscando la cadena "IDSQ" se
	 * encuentran "ficheros" que contienen secuencias para otros modelos
	 * (en realidad no en todas las apariciones de la cadena IDSQ comienza
	 * un fichero MDL; lo usarán para alguna otra cosa). <br>
	 * El siguiente paso es extraerlos... :-) Para eso es necesario conocer
	 * la longitud del fichero MDL para no sacar más de la cuenta. Eso se
	 * obtiene del campo longitud de la cabecera. Es sencillo encontrar ese
	 * campo desplazandose hacia adelante 64 bytes (0x40) a partir del
	 * primer caracter del nombre, que se identifica fácilmente. El campo
	 * del tamaño es un entero de 32 bits, e indica el número de bytes
	 * desde el principio (posición de la I de IDST) hasta el final del
	 * fichero. Como además el nombre del fichero se incluye dentro de la
	 * cabecera, el resto es trivial. <p>
	 * Hay algunos ficheros que no indican nombre. El campo del nombre
	 * sigue ocupando los 64 bytes, pero está vacío. Esos tienen todas las
	 * papeletas para ser ficheros que sólo poseen texturas. Se puede
	 * comprobar mirando por encima lo que sigue en el fichero. Si se ven
	 * algunas cadenas con nombres de ficheros de imagenes (con extensión
	 * .BMP), el MDL sólo contendrá texturas. Si las cadenas tienen nombres
	 * sospechosos de pertenecer a huesos (como "Bone12" o "Bip01 Pelvis")
	 * entonces el fichero contiene geometría. <p>
	 * La cuestión interesante es que algunos de los modelos almacenados
	 * que sólo contienen texturas encajan perfectamente con los requisitos
	 * de <code>player.mdl</code>, como por ejemplo el que empieza en la
	 * posición 221683084 (en decimal) de .\\valve\\pak0.pak. Si se extrae y
	 * se utiliza como fichero de texturas, el modelo se representará
	 * correctamente. <p>
	 * Como los ficheros tienen bastante información textual (sobre su nombre,
	 * el nombre de los ficheros de imagenes de los que se han sacado las
	 * texturas que contiene y cosas así), en general no es difícil
	 * sacar todos los ficheros (los de geometría, de textura y de
	 * secuencias) conociendo el "convenio" de nombres usado por el juego
	 * original descrito en MDLCore.
	 */
	AllowModelsWithoutTextures = 0
};

} // namespace HalfLifeMDL

#endif
