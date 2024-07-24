#define N_IMPLEMENTS nMaquinaEstados

#include "toshi/maquinaestados.h"


/********************************************************************************/
/* M�todo      : siguienteEstado											    */
/* Descripci�n : Eval�a el estado actual de la m�quina de estados, y seg�n la   */
/*				 entrada que le llegue, comprobar� que estados siguientes ser�n */
/*				 correctos.														*/
/* Par�metros  : TAlfabetoEntrada in (ENT). Posibles valores de la transici�n.  */
/* Devuelve    : TRUE si el posible cambio de estado es v�lido.					*/
/*				 FALSE si no es posible.										*/
/********************************************************************************/
bool nMaquinaEstados::siguienteEstado( TAlfabetoEntrada in )
{
	switch (_estadoActual)
	{
		case nMaquinaEstados::TEstado::PARAR :
			switch (in)
			{
				case nMaquinaEstados::TAlfabetoEntrada::MUERTO       :
				case nMaquinaEstados::TAlfabetoEntrada::CONTRATADO   : return true;
				case nMaquinaEstados::TAlfabetoEntrada::LIBERADO     :
				case nMaquinaEstados::TAlfabetoEntrada::FIN_TRAYECTO : return false;
				case nMaquinaEstados::TAlfabetoEntrada::FIN_ESPERA   : return true;
				case nMaquinaEstados::TAlfabetoEntrada::RESUCITADO   : return false;

				default                                              : return false;
			}
			break;
		case nMaquinaEstados::TEstado::MORIR :
			switch (in)
			{
				case nMaquinaEstados::TAlfabetoEntrada::MUERTO       :
				case nMaquinaEstados::TAlfabetoEntrada::CONTRATADO   :
				case nMaquinaEstados::TAlfabetoEntrada::LIBERADO     :
				case nMaquinaEstados::TAlfabetoEntrada::FIN_TRAYECTO :
				case nMaquinaEstados::TAlfabetoEntrada::FIN_ESPERA   : return false;
				case nMaquinaEstados::TAlfabetoEntrada::RESUCITADO   : return true;

				default                                              : return false;
			}
			break;
		case nMaquinaEstados::TEstado::SEGUIR :
			switch (in)
			{
				case nMaquinaEstados::TAlfabetoEntrada::MUERTO       : return true;
				case nMaquinaEstados::TAlfabetoEntrada::CONTRATADO   : return false;
				case nMaquinaEstados::TAlfabetoEntrada::LIBERADO     : return true;
				case nMaquinaEstados::TAlfabetoEntrada::FIN_TRAYECTO :
				case nMaquinaEstados::TAlfabetoEntrada::FIN_ESPERA   :
				case nMaquinaEstados::TAlfabetoEntrada::RESUCITADO   : return false;

				default                                              : return false;
			}
			break;
		case nMaquinaEstados::TEstado::DEAMBULAR :
			switch (in)
			{
				case nMaquinaEstados::TAlfabetoEntrada::MUERTO       :
				case nMaquinaEstados::TAlfabetoEntrada::CONTRATADO   : return true;
				case nMaquinaEstados::TAlfabetoEntrada::LIBERADO     : return false;
				case nMaquinaEstados::TAlfabetoEntrada::FIN_TRAYECTO : return true;
				case nMaquinaEstados::TAlfabetoEntrada::FIN_ESPERA   :
				case nMaquinaEstados::TAlfabetoEntrada::RESUCITADO   : return false;

				default                                              : return false;
			}
			break;
	}
	return false;
}


/********************************************************************************/
/* M�todo      : GetEstado											    		*/
/* Descripci�n : Obtiene el estado actual de la m�quina de estados.			    */
/* Par�metros  : 															    */
/* Devuelve    : El estado actual.												*/
/********************************************************************************/
nMaquinaEstados::TEstado nMaquinaEstados::GetEstado()
{
	return _estadoActual;
}


/********************************************************************************/
/* M�todo      : SetEstado											    		*/
/* Descripci�n : Modifica el estado actual de la m�quina de estados y le asigna */
/*				 el valor pasado por par�metro.								    */
/* Par�metros  : TEstado estado (ENT). Posibles valores de los estados.  		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nMaquinaEstados::SetEstado( TEstado estado )
{
	_estadoActual = estado;
}