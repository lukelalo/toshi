#define N_IMPLEMENTS nMaquinaEstados

#include "toshi/maquinaestados.h"


/********************************************************************************/
/* Método      : siguienteEstado											    */
/* Descripción : Evalúa el estado actual de la máquina de estados, y según la   */
/*				 entrada que le llegue, comprobará que estados siguientes serán */
/*				 correctos.														*/
/* Parámetros  : TAlfabetoEntrada in (ENT). Posibles valores de la transición.  */
/* Devuelve    : TRUE si el posible cambio de estado es válido.					*/
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
/* Método      : GetEstado											    		*/
/* Descripción : Obtiene el estado actual de la máquina de estados.			    */
/* Parámetros  : 															    */
/* Devuelve    : El estado actual.												*/
/********************************************************************************/
nMaquinaEstados::TEstado nMaquinaEstados::GetEstado()
{
	return _estadoActual;
}


/********************************************************************************/
/* Método      : SetEstado											    		*/
/* Descripción : Modifica el estado actual de la máquina de estados y le asigna */
/*				 el valor pasado por parámetro.								    */
/* Parámetros  : TEstado estado (ENT). Posibles valores de los estados.  		*/
/* Devuelve    : 																*/
/********************************************************************************/
void nMaquinaEstados::SetEstado( TEstado estado )
{
	_estadoActual = estado;
}