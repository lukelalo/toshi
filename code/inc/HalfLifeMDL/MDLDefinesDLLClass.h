//---------------------------------------------------------------------------
// MDLDefinesDLLClass.h
//---------------------------------------------------------------------------

/**
 * @file MDLDefinesDLLClass.h
 * Contiene la definici�n de macros para controlar la generaci�n del c�digo
 * de la librer�a. M�s concretamente, define las macros necesarias para
 * que el c�digo generado de las clases del namespace sirvan para 
 * generar una librer�a est�tica o una din�mica (DLL), as� como
 * para hacer que los ficheros de cabecera utilizados
 * puedan ser utilizados desde c�digo tanto utilizando la librer�a
 * din�mica como est�tica.<p>
 * Las macros que hay que definir externamentes er�n las siguientes:
 * <ul>
 *	<li>MDL_STATIC: si est� definido indica que se est� generando/usando la librer�a de manera
 * est�tica.</li>
 *  <li>MDL_IMPLEMENTS: si est� definido, indica que se est� generando el c�digo
 * de la librer�a, en vez de indicar que se est� utilizando.
 * </ul>
 * @author Marco Antonio G�mez Mart�n
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
