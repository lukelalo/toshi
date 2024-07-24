//---------------------------------------------------------------------------
// MDLQuaternion.h
//---------------------------------------------------------------------------

/**
 * @file MDLQuaternion.h
 * Contiene la declaración de la clase que almacena la información sobre
 * un cuaternión.
 * @see HalfLifeMDL::MDLQuaternion
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */

#ifndef __HalfLifeMDL_MDLQuaternion
#define __HalfLifeMDL_MDLQuaternion


#include "MDLMatrix.h"
#include "MDLBasicType.h"

#include <assert.h>


namespace HalfLifeMDL {

/**
 * Clase que almacena un cuaternión, y dispone de las operaciones habituales
 * sobre ellos.
 * <p>
 * Es utilizado en el namespace HalfLifeMDL internamente, y el usuario nunca
 * tendrá que utilizarlo. Su uso es la interpolación de rotaciones de huesos.
 * <p>
 * La siguiente descripción sobre cuaterniones se ha obtenido de la dirección
 * http://www.martinbr.com/gml_2.php.
 *
 * <p>
 * <b>Introducción</b>
 * <p>
 * Un <i>cuaternion</i> es una forma alternativa de representar
 * <i>rotaciones</i> a través del <i>cualquier eje</i>. Matemáticamente, son
 * una extension del conjunto de <i>numeros complejos</i>. El primero que habló
 * de ellos fue <i>William Hamilton</i>, hace cosa como de 100 años.
 * <br> 
 * Presentan varias ventajas comparado con las rotaciones por matrices.
 * <i>Concatenar quaterniones</i> exige menos operaciones, requieren menos
 * espacio para almacenarlos que una matriz, son mas faciles de
 * <i>interpolar</i> que las matrices, etc. Podemos pensar en un
 * <i>cuaternion</i> como en un <i>vector 4 dimensional</i> con la forma:
 * <p>
 * <center>
 * <b>q</b> = { <i>w</i>, <i>x</i>, <i>y</i>, <i>z</i> } = 
 * <i>w</i> + <i>xi</i> + <i>yj</i> + <i>zk</i>
 * </center>
 * <p>
 * Tambien los podemos encontrar representados como:
 * <b>q</b> = <i>s</i> + <b>v</b>, donde <i>s</i> es un escalar que representa
 * la <i>componente w</i> de <b>q</b>, y <b>v</b> el vector correspondiente a
 * <i>x</i>, <i>y</i> y <i>z</i> que representa su parte <i>imaginaria</i>.
 * Para aprender a operar con <i>quaternions</i> antes debemos refrescar un
 * poco la memoria sobre los <i>numeros imaginarios</i>.
 * <p>
 * <b>Planeta Imaginario</b>
 * <p>
 * Ecuaciones del tipo <b>x</b><sup>2</sup> + <i>1</i> = <i>0</i> no tienen
 * solución en el conjunto de los numeros reales, <b>R</b>, donde no existe
 * ningún número que cumpla <b>x</b><sup>2</sup> = <i>-1</i>. En el conjunto
 * de los numero reales, ningún <i>cuadrado</i> de un número puede ser
 * negativo. Para resolver este tipo de ecuaciones tenemos que ampliar
 * <b>R</b> con el conjunto de números <i>imaginarios</i>, <b>I</b>, donde
 * todos los números (<i>incluidos los negativos</i>) tengan <i>raiz
 * cuadrada</i>.
 * <p>
 * Para hacer posible que el <i>cuadrado</i> de un número pueda ser negativo,
 * nos sacamos de la manga la <i>unidad imaginaria pura i</i>, que cumple:
 * <p>
 * <center><i>i</i><sup>2</sup> = <i>-1</i></center>
 * <p>
 * ¿Cómo es posible esto? Como diría el profesor <i>Hubert J. Farnsworth</i>,
 * "<i>todo es posible si te lo imaginas</i>". Así, la ecuación
 * <b>x</b><sup>2</sup> + <i>9</i> = <i>0</i>, sí tiene solución en
 * <b>C</b>:
 * <p>
 * <center>
 * <b>x</b><sup>2</sup> + <i>9</i> = <i>0</i><br>
 * <b>x</b><sup>2</sup> = <i>-9</i><br>
 * <b>x</b> = sqrt(<i>-9</i>) = sqrt(<i>9</i>) sqrt(<i>-1</i>) =
 * ±<i>3</i><i>i</i>
 * </center>
 * <p>
 * De la unión de <b>R</b> e <b>I</b>, obtenemos el conjunto de los números
 * <i>complejos</i>. Llamaremos número <i>complejo</i> al <i>par</i>
 * (<i>a</i>, <i>b</i>), donde <i>a</i> representa la parte <i>real</i>
 * y <i>b</i> la parte <i>imaginaria</i>. Además cumple:
 * <p>
 * <center>
 * (<i>a</i>, <i>b</i>) = (<i>c</i>, <i>d</i>), si <i>a</i> = <i>c</i>
 * y <i>b</i> = <i>d</i>
 * <br>
 * (<i>a</i>, <i>b</i>) + (<i>c</i>, <i>d</i>) = (<i>a</i> + <i>c</i>,
 * <i>b</i> + <i>d</i>)
 * <br>
 * (<i>a</i>, <i>b</i>) * (<i>c</i>, <i>d</i>) = (<i>ac</i> - <i>bd</i>,
 * <i>ad</i> + <i>bc</i>)
 * <br><br>
 * </center>
 * <p>
 * Podriamos representar el numero <i>imaginario puro i</i> como un número
 * complejo de la forma (<i>0</i>, <i>1</i>), asi que podremos operar con
 * <i>i</i> siguiendo las reglas de arriba, por ejemplo para calcular
 * potencias de <i>i</i>:
 * <p>
 * <center>
 * <i>i</i><sup>2</sup> = (<i>0</i>, <i>1</i>) (<i>0</i>, <i>1</i>) =
 * (<i>0</i> - <i>1</i>, <i>0</i> + <i>0</i>) = (<i>-1</i>, <i>0</i>) =
 * <i>-1</i>
 * <br>
 * <i>i</i><sup>3</sup> = <i>i</i><sup>2</sup><i>i</i> = (<i>-1</i>, <i>0</i>)
 * (<i>0</i>, <i>1</i>) = <i>-i</i>
 * <br>
 * <i>i</i><sup>4</sup> = <i>i</i><sup>3</sup><i>i</i> = (<i>0</i>, <i>-1</i>)
 * (<i>0</i>, <i>1</i>) = <i>1</i>
 * </center>
 * <p>
 * Para <i>sumar</i> dos numeros complejos <i>x</i> = <b>a</b><i>i</i> e
 * <i>y</i> = <b>b</b><i>i</i> (<i>restarlos</i> seria similar) :
 * <p>
 * <center><i>x</i> + <i>y</i> = <b>a</b><i>i</i> + <b>b</b><i>i</i> =
 * (<b>a</b> + <b>b</b>)<i>i</i></center> 
 * <p>
 * <i>Multiplicar</i> los anteriores numeros complejos (y similarmente 
 * tambien <i>dividir</i>) seria:
 * <p>
 * <center><i>xy</i> = <b>a</b><i>i</i><b>b</b><i>i</i> =
 * <b>ab</b><i>i</i><sup>2</sup> = -<b>ab</b></center>
 * <p>
 * Para <i>dividir</i> tened en cuenta que <i>i</i>/<i>i</i> = <i>1</i>.
 * <p>
 * <p>
 * Existe otra forma de representar a los numeros <i>complejos</i>, y es la
 * forma <i>binomica</i>: <i>a</i> + <i>bi</i>, donde <i>a</i> y <i>b</i> son
 * numeros <i>reales</i>. Asi lo podemos representar es un eje
 * <i>cartesiano</i>:
 * <p>
 * <center>
 * @image html quat1.gif
 * </center>
 * <p>
 * Si reemplazamos <i>i</i> por <i>-i</i> de un numero complejo <i>z</i>,
 * invertir la coordenada imaginaria, obtenemos el <i>conjugado</i> de
 * <i>z</i>. Si multiplicamos un numero complejo por su <i>conjugado</i>,
 * eliminaremos su parte <i>imaginaria</i>:
 * <p>
 * <center>
 * (<i>a</i>, <i>b</i>) * (<i>a</i>, <i>-b</i>) = (<i>aa</i> -
 * <i>b</i>(<i>-b</i>), -<i>ab</i> + <i>ba</i>) = (<i>a</i><sup>2</sup> +
 * <i>b</i><sup>2</sup>, 0)
 * </center>
 * <p>
 * El <i>modulo</i> de una numero complejo <i>z</i> = (<i>a</i> + <i>bi</i>),
 * representado como |<i>z</i>|, es:
 * <p>
 * <center>
 * |<i>z</i>| = sqrt(<i>z</i> * <i>z</i>) = sqrt(<i>a</i><sup>2</sup> +
 * <i>b</i><sup>2</sup>)
 * </center>
 * <p>
 * Como ya vimos anteriormente con los <i>vectores</i>, el <i>modulo</i> de
 * un numero complejo es la <i>distancia</i> del punto (<i>a</i>, <i>b</i>)
 * al origen del sistema <i>cartesiano</i> donde un eje representa su parte
 * <i>real</i> y otro, la <i>imaginaria</i>.</p>
 * <br>
 * <b>Numeros hipercomplejos, quaternions y cintas de video</b>
 * <p>Que no te asuste su nombre, un numero <i>hipercomplejo</i> no es mas que
 * la <i>suma</i> de un numero <i>real</i> y <i>varios</i> numeros
 * <i>imaginarios</i>. El subconjunto de los numeros <i>hipercomplejos</i>
 * que nos interesa a nosotros es el de los <i>quaternions</i>, que tienen esta
 * forma:</p>
 * <center>
 * <i>q</i><sub>0</sub> + <i>q</i><sub>1</sub><i>i</i> +
 * <i>q</i><sub>2</sub><i>j</i> + <i>q</i><sub>3</sub><i>k</i> 
 * </center>
 * <p>
 * Donde <i>q</i><sub>0</sub>, <i>q</i><sub>1</sub>, <i>q</i><sub>2</sub> y
 * <i>q</i><sub>3</sub> son numeros <i>reales</i> e <i>i j</i> y <i>k</i> son
 * <i>imaginarios</i>. Si <i>q</i><sub>0</sub> = <i>0</i>, diremos que
 * tenemos un <i>quaternion puro</i>.</p>
 * <p>
 * Para multiplicar las partes <i>imaginarias</i> de un numero
 * <i>hipercomplejo</i>, seguiremos las <i>reglas de Hamilton</i>
 * (inventor de los <i>quaternions</i> en 1843), que dicen lo siguiente:
 * <p>
 * <center>
 * <i>ij</i> = <i>k</i><br>
 * <i>jk</i> = <i>i</i><br>
 * <i>kj</i> = <i>j</i><br>
 * <i>ji</i> = <i>-k</i><br>
 * <i>kj</i> = <i>-i</i><br>
 * <i>ik</i> = <i>-j</i>
 * </center>
 * <p>
 * Antes de saber como nos pueden ayudar los <i>quaternions</i> en nuestros
 * juegos, veremos como realizar algunas operaciones basicas con ellos.
 * <p>
 * <b>Suma y resta de cuaterniones</b>
 * <p><i>Sumar</i> y <i>restar</i> <i>quaternions</i> es muy parecido a lo que
 * hemos hecho antes, teniendo 2 <i>quaternions</i> <i>p</i> =
 * <i>p</i><sub>0</sub> + <i>p</i><sub>1</sub><i>i</i>
 * + <i>p</i><sub>2</sub><i>j</i> + <i>p</i><sub>3</sub><i>k</i> y <i>q</i> = 
 * <i>q</i><sub>0</sub> + <i>q</i><sub>1</sub><i>i</i> + <i>q</i><sub>2</sub>
 * <i>j</i> + <i>q</i><sub>3</sub><i>k</i> seria</p>
 * <center>
 * <i>p</i> + <i>q</i> = (<i>p</i><sub>0</sub> + <i>q</i><sub>0</sub>) +
 * (<i>p</i><sub>1</sub> + <i>q</i><sub>1</sub>)<i>i</i> +
 * (<i>p</i><sub>2</sub> + <i>q</i><sub>2</sub>)<i>j</i> +
 * (<i>p</i><sub>3</sub> + <i>q</i><sub>3</sub>)<i>k</i>  
 * </center>
 * <p>
 * y la <i>resta</i>
 * <p>
 * <center>
 * <i>p</i> - <i>q</i> = (<i>p</i><sub>0</sub> - <i>q</i><sub>0</sub>)
 * + (<i>p</i><sub>1</sub> - <i>q</i><sub>1</sub>)<i>i</i> +
 * (<i>p</i><sub>2</sub> - <i>q</i><sub>2</sub>)<i>j</i> +
 * (<i>p</i><sub>3</sub> - <i>q</i><sub>3</sub>)<i>k</i>
 * </center>
 * <p>
 * La verdad es que estas operaciones apenas la usaremos, siendo mucho mas
 * importante la <i>multiplicacion</i>.
 * <p>
 * <br>
 * <b>Multiplicar cuaterniones</b>
 * <p>
 * Sera la operacion que mas usemos, como veremos mas adelante. Para
 * multiplicar <i>quaternions</i> usaremos la multiplicacion de numeros
 * <i>imaginarios</i>. * La codificaremos de la siguiente forma:
 * <p>
 * <br>
 * <table border="0" cellspacing="1" cellpadding="1" align="center">
 * <tr><td>
 * <tt><pre>
 * quat &quat::operator *(const quat &q)
 * {
 *   quat t;

 *   t.x = (w * q.x + x * q.w + y * q.z - z * q.y);
 *   t.y = (w * q.y - x * q.z + y * q.w + z * q.x);
 *   t.z = (w * q.z + x * q.y - y * q.x + z * q.w);
 *   t.w = (w * q.w - x * q.x - y * q.y - z * q.z);

 *   return t;
 * }
 * </pre></tt>
 * </td></tr></table>
 * <br>
 * <br>
 * <b>Conjugado</b>
 * <p>
 * Es similar a la forma en la que hizimos el <i>conjugado</i> de numeros
 * complejos. Para hacer el <i>conjugado</i> de un <i>quaternion</i>
 * simplemente cambiaremos cada parte <i>imaginaria</i> por su
 * <i>negativo</i>. Asi, si tenemos el <i>quaternion</i> <i>q</i> =
 * <i>q</i><sub>0</sub> + <i>q</i><sub>1</sub><i>i</i> +
 * <i>q</i><sub>2</sub><i>j</i> + <i>q</i><sub>3</sub><i>k</i> su
 * <i>conjugado</i>, <i>q</i><sup>*</sup>, seria <i>q</i><sub>0</sub> -
 * <i>q</i><sub>1</sub><i>i</i> - <i>q</i><sub>2</sub><i>j</i> -
 * <i>q</i><sub>3</sub><i>k</i>
 * <p>
 * <p>El <i>conjugado</i> cumple (<i>pq</i>)<sup>*</sup> =
 * <i>q</i><sup>*</sup><i>p</i><sup>*</sup>. Osea, que el <i>conjugado</i>
 * del producto de dos <i>quaternions</i> es igual al producto de los
 * <i>conjugados</i>, pero en <i>distinto orden</i>.
 * <p>
 * <p>
 * Otra propiedad interesante es que <i>q</i><sup>*</sup><i>q</i> es igual
 * a <i>q</i><sub>0</sub><sup>2</sup> + <i>q</i><sub>1</sub><sup>2</sup> +
 * <i>q</i><sub>2</sub><sup>2</sup> + <i>q</i><sub>3</sub><sup>2</sup>, que
 * vemos que es un numero <i>real</i>.
 * <p>
 * <br>
 * <b>Modulo</b>
 * <p>El <i>modulo</i> de un <i>quaternion</i>, |<i>q</i>|, es
 * <i>sqrt</i>(<i>qq</i>). Asi que |<i>q</i>|<sup>2</sup> = <i>qq</i>.</p>
 * <br>
 * <p>
 * <b>Inversa</b>
 * <p>
 * Si multiplicamos un <i>quaternion</i> por su <i>inverso</i> obtendremos el
 * numero <i>real</i> <b>1</b>. Para calcular el <i>inverso</i> de un
 * <i>quaternion</i>, llamado <i>q</i><sup>-1</sup>, haremos
 * <i>q</i><sup>*</sup> / |<i>q</i>|<sup>2</sup>
 * <p>
 * <br>
 * <b>Rotando a <i>Miss Daisy</i></b>
 * <p>
 * Despues de ver las operaciones basicas que podemos hacer con
 * <i>quaternions</i>, estamos en condiciones para afrontar lo que nos
 * interesa de los <i>quarternions</i>, <i>rotar</i> puntos con respecto a
 * <i>cualquier</i> eje. Usaremos los <i>quatarnions</i> casi siempre
 * de la siguiente forma:
 * <p>
 * <ol>
 * <li>"<i>Meteremos</i>" una rotacion en un <i>quaternion</i>, osea
 * que haremos que represente la rotacion que queremos. Para esto
 * partiremos de un <i>vector</i>, o una <i>matriz</i>, o unos <i>angulos
 * Euler</i> (pronunciado <i>Oiler</i>), que representen esa rotacion que
 * queremos.</li>
 * <li>Teniendo ya la rotacion "<i>dentro</i>" del <i>quatarnion</i>,
 * operaremos sobre el este. Normalmente haremos <i>interpolaciones</i>,
 * que son mucho mas faciles de hacer que con matrices y es la verdadera
 * causa de usar <i>quaternions</i>.</li>
 * <li>Transformaremos nuestro <i>quaternion</i> es su forma de <i>matriz</i>,
 * para poder seguir con los calculos.</li>
 * </ol>
 * <p>
 * Primero veremos como representar esa rotacion en un <i>quaternion</i>
 * dado un vector que define el eje de rotacion. Tenemos el <i>quaternion</i>
 * <i>q</i> = <i>q</i><sub>0</sub> + <i>q</i><sub>1</sub><i>i</i> +
 * <i>q</i><sub>2</sub><i>j</i> + <i>q</i><sub>3</sub><i>k</i>, podemos
 * pensar en <i>i</i>, <i>j</i> y <i>k</i> como en <i>vectores
 * imaginarios</i> <b>i</b>, <b>j</b> y <b>k</b>. Asi, <i>q</i><sub>1</sub>,
 * <i>q</i><sub>2</sub> y <i>q</i><sub>3</sub> serian las <i>componentes</i>
 * de dichos vectores. Asi que nuestro <i>quaternion</i> podria representarse
 * ahora como:
 * <p>
 * <center>
 * <i>q</i> = <i>q</i><sub>0</sub> + <b>q</b>
 * </center>
 * <p>
 * Donde <i>q</i><sub>0</sub> es un <i>escalar</i> y <b>q</b> es un <i>vector</i> que representa la direccion en la cual el eje de la rotacion señala. Dada una
rotacion <i>ß</i> con respecto al eje <b>u</b> (vector <i>normalizado</i>), el <i>quaternion</i> que representa esa rotacion es:</p>

<center>
<i>q</i> = <i>cos</i>(<i>ß</i>/2) + <i>sin</i>(<i>ß</i>/2)<b>u</b>
</center>

<p>Sabiendo esto, si queremos representar una rotacion sobre un eje determinado en un <i>quaternion</i>, la funcion que usaremos seria:</p>

<br>
	<table border="0" cellspacing="1" cellpadding="1" align="center">
	<tr>
	<td>
	<pre>					
quat &quat::fromAngles(const vec3 &axis, real angle)
{
  vec3 v;
  real s;

  v = vec3::normalize(axis);

  angle /= 2.0f;

  s = calc::sin(angle);

  x = v.x * s;
  y = v.y * s;
  z = v.z * s;
  w = calc::cos(angle);
}
	</pre>

	</td>
</tr>
</table>
<br>

<p>Ademas de con un <i>vector</i>, tambien nos puede interesar pasarle una <i>matriz</i> para definir la rotacion. La forma de hacer esto la saque de
<b>Game Developer Magazine</b>, y la podreis ver en el codigo que acompaña a este capitulo (<i>::fromMatrix()</i>).</p>

<p>Otro metodo es mediente <i>angulos Euler</i>. Mediante este sistema se define una rotacion a traves de <i>tres</i> simples rotaciones de los
ejes:</p>

 * <center>
 * @image html quat3.gif
 * </center>
 * <p>
Este grafico vemos como primero se rotaria el sobre el eje <i>z</i> (rotacion azul), luego sobre el nuevo eje <i>x'</i> (rotacion verde) y finalmente sobre el nuevo eje

<i>z''</i> (rotacion roja). Este es el orden mas utilizado, aunque puede haber otras convenciones distintas. Para usar <i>angulos Euler</i> me base nuevamente en
una funcion de <b>Game Developer Magazine</b> (<i>::fromEuler()</i>)</p>

<p>Ya sabemos como representar una rotacion en un <i>quaternion</i>. Ahora veremos como rotar un punto, vector, teniendo un <i>quaternion</i> con una rotacion ya
definida. Definimos el <i>quternion puro</i> <b>p</b>:</p>

<center>
<i>p</i> = 0 + <i>r</i>
</center>

<p>Donde <i>r</i> es el vector a rotar. Pues bien, la parte <i>imaginaria</i> de la operacion <b>qpq<sup>*</sup></b>, es nuestro ansiado vector <i>r</i> rotado.</p>


<p>Podemos aplicar muchas rotaciones en <i>un solo quaternion</i>. Imaginemos que queremos rotar un vector representado por el <i>quaternion puro v</i> por <i>q</i> y luego por
<i>p</i>. Como vimos arriba, para calcular lo primero, hacemos <i>qvq</i><sup>*</sup>. Para hacer la segunda rotacion, <i>p</i>(<i>qvq</i><sup>*</sup>)<i>p</i><sup>*</sup>. Como las
multiplicaciones son <i>asociativas</i>, lo anterior puede escribirse tambien como (<i>pq</i>)<i>v</i>(<i>q</i><sup>*</sup><i>p</i><sup>*</sup>). Tambien sabemos
que (<i>q</i><sup>*</sup><i>p</i><sup>*</sup>) = (<i>pq</i>)<sup>*</sup>, asi que nos queda (<i>pq</i>)<i>v</i>(<i>pq</i>)<sup>*</sup>. Para simplificar aun mas, llamaremos <i>r</i> al producto
 <i>pq</i>, con lo que nuestro vector final es simplemente <i>rvr</i><sup>*</sup>.

</p>

<br>
<b>3.8 Interpola esto!</b>

<p>Ya que los <i>quaternions</i> pueden ser representados como vectores, son idoneos para <i>interpolarlos</i>. La mayoria de las utilidades de edicion 3D exportan las <i>animaciones</i> usando <i>quaternions</i> y <i>no</i> matrices de rotacion. Si queremos
optener el <i>frame</i> que esta entre dos de las animaciones que nos dan estos programas, solo tenemos que <i>interpolar</i> esos <i>quaternions</i>. La forma mas simple
(aunque no muy util) seria una <i>interpolacion lineal</i>:</p>

<center>
<i>q</i>(<i>t</i>) = (<b>1</b> - <i>t</i>)<i>q</i><sub>1</sub> + <i>t</i><i>q</i><sub>2</sub>

</center>

<p>Donde <i>q</i><sub>1</sub> y <i>q</i><sub>2</sub> son los dos <i>quaternions</i> que queremos interpolar y <i>t</i> va de <b>0</b> a <b>1</b>. La funcion <i>q</i>(<i>t</i>) recorre la
linea que conecta ambos <i>quaternions</i> (pensemos que son vectores). Si el angulo es grande, esta interpolacion no daria unos resultados muy buenos.

</p>

<p>Otro tipo de interpolacion mas interesante es la <i>esferica</i>, o <b>SLERP</b> (<b>S</b>pherical <b>L</b>inear int<b>ERP</b>olation), que recorreria ambos <i>quaterniones</i> siguiendo
el <i>arco</i> (y no la linea) que une ambos <i>quaterniones</i> (si los
dos <i>quaternions</i> estan muy cerca, podemos usar una interpolacion <i>lineal</i>, nadie lo notara...). Seria asi:

</p>

<center>
<i>q</i>(<i>t</i>) = (<i><i>q</i><sub>1</sub>sin</i>((<b>1</b> - <i>t</i>)ß) + <i><i>q</i><sub>2</sub>sin</i>(<i>t</i>ß) ) / <i>sin</i>(ß)

</center>

 *
 * @see http://www.martinbr.com/gml_2.php
 * @todo Repasar toda esta explicación. Hay cosas que se podrían quitar, y
 * luego habría que arreglar el código en el fuente; lo empecé a colocar, pero
 * acabé hasta las narices y está hecho una pena.
 * 
 * @author Pedro Pablo Gómez Martín
 * @date Noviembre, 2003.
 */
class MDLQuaternion {
public:

	/**
	 * Constructor sin parámetros.
	 */
	MDLQuaternion() { _coords[0] = _coords[1] = _coords[2] = _coords[3] = 0; }

	/**
	 * Constructor de un cuaternión a partir de tres ángulos de rotación.
	 * El código usado para generar el cuaternión se ha extraído del visor
	 * de MDL de referencia. No me he preocupado mucho de mirar qué hace
	 * exactamente el código, por lo que no puedo detallar qué hace el método
	 * O:-). Sospecho que tiene que ver con lo que en el comentario inicial
	 * se llaman ángulos Euler.
	 * @param x Rotación en el eje x
	 * @param y Rotación en el eje y
	 * @param z Rotación en el eje z.
	 */
	MDLQuaternion(MDLBasicType x, MDLBasicType y, MDLBasicType z) {
					setAngles(x, y, z); }

	/**
	 * Devuelve el valor de la coordenada X
	 * @return Coordenada x.
	 */
	MDLBasicType x() const { return _coords[0]; }

	/**
	 * Devuelve una referencia a la coordenada X.
	 * @return Coordenadad x.
	 */
	MDLBasicType& x() { return _coords[0]; }

	/**
	 * Devuelve el valor de la coordenada Y
	 * @return Coordenadad y.
	 */
	MDLBasicType y() const { return _coords[1]; }

	/**
	 * Devuelve una referencia a la coordenada Y.
	 * @return Coordenadad y.
	 */
	MDLBasicType& y() { return _coords[1]; }

	/**
	 * Devuelve el valor de la coordenada Z.
	 * @return Coordenadad z.
	 */
	MDLBasicType z() const { return _coords[2]; }

	/**
	 * Devuelve una referencia a la coordenada Z.
	 * @return Coordenadad z.
	 */
	MDLBasicType& z() { return _coords[2]; }

	/**
	 * Devuelve el valor de la coordenada W (el escalar del
	 * cuaternión).
	 * @return Coordenadad z.
	 */
	MDLBasicType w() const { return _coords[3]; }

	/**
	 * Devuelve el valor de la coordenada W (el escalar del
	 * cuaternión).
	 * @return Coordenadad z.
	 */
	MDLBasicType& w() { return _coords[3]; }

	/**
	 * Devuelve la dimensión del punto solicitada. Se supone
	 * que 0 es la coordenada X, 1 la Y, 2 la Z y la 3 la
	 * W (el escalar del cuaternión).
	 * @return Valor de la coordenada solicitada.
	 */
	MDLBasicType& operator[](int v) {
					assert((v >= 0) && (v <= 3));
					return _coords[v]; }

	/**
	 * Devuelve la dimensión del punto solicitada. Se supone
	 * que 0 es la coordenada X, 1 la Y, 2 la Z y la 3 la
	 * W (el escalar del cuaternión).
	 * @return Valor de la coordenada solicitada.
	 */
	MDLBasicType operator[](int v) const {
					assert((v >= 0) && (v <= 3));
					return _coords[v]; }

	/**
	 * Establece el cuaternión a partir de tres ángulos.
	 * El código usado para generar el cuaternión se ha extraído del visor
	 * de MDL de referencia. No me he preocupado mucho de mirar qué hace
	 * exactamente el código, por lo que no puedo detallar qué hace el método
	 * O:-). Sospecho que tiene que ver con lo que en el comentario inicial
	 * se llaman ángulos Euler.
	 * @param x Rotación en el eje x
	 * @param y Rotación en el eje y
	 * @param z Rotación en el eje z.
	 */
	void setAngles(MDLBasicType x, MDLBasicType y, MDLBasicType z);

	/**
	 * Interpolación esférica entre el cuaternión actual y el pasado como
	 * parámetro (Spherical linear interpolation).
	 * @param op2 Segundo cuaternión a utilizar en la interpolación
	 * @param t Peso de la interpolación. Un 1.0 da todo el peso al
	 * cuaternión del parámetro, y ninguno al del objeto.
	 * @return El cuaternión resultado de la interpolación.
	 */
	MDLQuaternion slerp(const MDLQuaternion& op2, MDLBasicType t) {
					MDLQuaternion result;
					return slerp(op2, t, result); }

	/**
	 * Interpolación esférica entre el cuaternión actual y el pasado como
	 * parámetro (Spherical linear interpolation).
	 * Puede considerarse una versión optimizada de slerp(MDLQuaternion,
	 * MDLBasicType), porque el resultado lo deposita en un parámetro de
	 * salida, ahorrandose una copia. La desventaja es que es más
	 * incómodo de usar.
	 * @param t Peso de la interpolación. Un 1.0 da todo el peso al
	 * cuaternión del parámetro, y ninguno al del objeto.
	 * @param result Cuaternión donde depositar el resultado.
	 * @return El cuaternión resultado de la interpolación (pasado como
	 * último parámetro).
	 */
	MDLQuaternion& slerp(const MDLQuaternion& op2, MDLBasicType t,
												MDLQuaternion& result);


	/**
	 * Devuelve la amtriz de rotación asociada al cuaternión actual.
	 * @return Matriz de rotación asociada al cuaternión.
	 */
	MDLMatrix toMatrix();// { MDLMatrix result; return toMatrix(result); }

	/**
	 * Devuelve la matriz de rotación asociada al cuaternión actual.
	 * Se puede considerar más eficiente que toMatrix() porque el resultado
	 * se almacena en un parámetro de salida, evitandose la copia.
	 * @param result Matriz resultado. Sólo se modifica la parte relativa
	 * a la matriz de rotación. La parte de la traslación no se modifica.
	 * @return La matriz recibida en el parámetro.
	 */
	MDLMatrix& toMatrix(MDLMatrix& result);

protected:

	/**
	 * Coordenadas del quaternión. Las tres primeras dimensiones son la
	 * parte imaginaria (xi + yj + zk), y la última el escalar.
	 */
	MDLBasicType _coords[4];

}; // class MDLQuaternion

} // namespace HalfLifeMDL

#endif
