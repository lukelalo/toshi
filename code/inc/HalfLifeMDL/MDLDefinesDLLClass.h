//---------------------------------------------------------------------------
// MDLDefinesDLLClass.h
//---------------------------------------------------------------------------

/**
 * @file MDLDefinesDLLClass.h
 * Contiene la definición de macros para controlar la generación del código
 * de la librería. Más concretamente, define las macros necesarias para
 * que el código generado de las clases del namespace sirvan para 
 * generar una librería estática o una dinámica (DLL), así como
 * para hacer que los ficheros de cabecera utilizados
 * puedan ser utilizados desde código tanto utilizando la librería
 * dinámica como estática.<p>
 * Las macros que hay que definir externamentes erán las siguientes:
 * <ul>
 *	<li>MDL_STATIC: si está definido indica que se está generando/usando la librería de manera
 * estática.</li>
 *  <li>MDL_IMPLEMENTS: si está definido, indica que se está generando el código
 * de la librería, en vez de indicar que se está utilizando.
 * </ul>
 * @author Marco Antonio Gómez Martín
 * @date Abril, 2004.
 */

#ifndef __HalfLifeMDL_MDLDefinesDLLClass
#define __HalfLifeMDL_MDLDefinesDLLClass


#ifdef WIN32
#	ifdef MDL_STATIC
#		define MDL_EXPORT
#		define MDL_IMPORT
#	else
#		define MDL_EXPORT __declspec(dllexport)
#		define MDL_IMPORT __declspec(dllimport)
#	endif
#else
#	define MDL_EXPORT
#	define MDL_IMPORT
#endif

#ifdef WIN32
#	ifndef MDL_STATIC
#		ifdef MDL_IMPLEMENTS
#			define MDL_PUBLIC MDL_EXPORT
#		else
#			define MDL_PUBLIC MDL_IMPORT
#		endif
#	endif
#endif

#ifndef MDL_PUBLIC
#define MDL_PUBLIC
#endif

#ifdef WIN32
#pragma warning( disable : 4251 )       // class XX needs DLL interface to be used...
#pragma warning( disable : 4355 )       // initialization list uses 'this' 
#pragma warning( disable : 4275 )       // base class has not dll interface...
#pragma warning( disable : 4786 )       // symbol truncated to 255 characters
#endif

#endif
