#define N_IMPLEMENTS nIAJugador

#include "toshi/niajugador.h"
#include "toshi/njugador.h"
#include "toshi/ngame.h"
#include "toshi/npartida.h"
#include "toshi/njugador.h"
#include "toshi/ncampesino.h"

nNebulaScriptClass( nIAJugador, "nroot" );

// Constructor general
nIAJugador::nIAJugador() : refScriptServer(kernelServer, this),
						   refGame(kernelServer,this)
{
	refScriptServer		  = "/sys/servers/script";
	refGame				  = "/game";
	partida				  = refGame->GetPartida();
	jugador				  = partida->AsignarJugadorIA();
	personajeSeleccionado = nPersonaje::PERSONAJE_CAMPESINO;
	comportamiento        = T_Comportamiento::NEUTRAL;
	srand( (unsigned int) time( NULL ) );
}

// Destructor
nIAJugador::~nIAJugador()
{
	acciones_jugador.Clear();
}

void nIAJugador::Trigger()
{
	if ( this->jugador->GetPersonaje()->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO &&
		 this->jugador->GetSiguientePersonaje()->GetTipoPersonaje() == nPersonaje::PERSONAJE_CAMPESINO )
	{
		// Si no queda ningún personaje para elegir esperamos
		if ( partida->TodosLosPersonajesOcupados() )
			return;

		// Se escoge un personaje según ciertos criterios y según el comportamiento que esté siguiendo
		if ( EscogerPersonaje() )
		{
			// Borramos la lista de acciones (para no hacer acciones con personajes que no nos corresponden)
			acciones_jugador.Clear();

			// Se ha seleccionado un personaje
			partida->BloqueaPersonaje( personajeSeleccionado, jugador->GetId() );

			// Coger el personaje
			EncolarAccion( ACCION_COGER_PERSONAJE );

			// Coger dinero y/o plano/s
			switch ( comportamiento )
			{
				case NEUTRAL :
				case CONSTRUCTIVO :
				case DESTRUCTIVO :
					{
						// No existe predilección por una acción
						int num_accion = rand() % 100;
						if ( num_accion < 60 )
							EncolarAccion( ACCION_COGER_DINERO );
						else
						{
							if ( this->jugador->GetNumPlanos() < N_MAXPLANOS )
								EncolarAccion( ACCION_COGER_PLANO );
							if ( personajeSeleccionado == nPersonaje::PERSONAJE_INGENIERO )
							{
								if ( this->jugador->GetNumPlanos() < N_MAXPLANOS - 1 )
									EncolarAccion( ACCION_COGER_PLANO );
								if ( jugador->LoTieneConstruido( "ESCUELA DE INGENIERIA" ) )
									EncolarAccion( ACCION_COGER_DINERO );
							}
						}
					}
					break;
			}

			// Acción especial de los personajes
			switch ( personajeSeleccionado )
			{
				case nPersonaje::PERSONAJE_KUNOICHI :
					{
						switch ( comportamiento )
						{
							case NEUTRAL :
							case CONSTRUCTIVO :
								{
									EncolarAccion( ACCION_MATAR_JUGADOR_MAS_CERCANO );
									if ( jugador->LoTieneConstruido( "ESCUELA DE ASESINOS" ) )
										EncolarAccion( ACCION_MATAR_JUGADOR_MAS_CERCANO );
								}
								break;
							case DESTRUCTIVO :
								{
									EncolarAccion( ACCION_MATAR_JUGADOR_MEJOR );
									if ( jugador->LoTieneConstruido( "ESCUELA DE ASESINOS" ) )
										EncolarAccion( ACCION_MATAR_JUGADOR_MEJOR );
								}
								break;
						}
					}
					break;
				case nPersonaje::PERSONAJE_LADRONA :
					{
						switch ( comportamiento )
						{
							case NEUTRAL :
							case CONSTRUCTIVO :
								{
									EncolarAccion( ACCION_ROBAR_PERSONAJE_MAS_CERCANO );
									if ( jugador->LoTieneConstruido( "ESCUELA DE LADRONES" ) )
										EncolarAccion( ACCION_ROBAR_PERSONAJE_MAS_CERCANO );
								}
								break;
							case DESTRUCTIVO :
								{
									EncolarAccion( ACCION_ROBAR_PERSONAJE_MEJOR );
									if ( jugador->LoTieneConstruido( "ESCUELA DE LADRONES" ) )
										EncolarAccion( ACCION_ROBAR_PERSONAJE_MEJOR );
								}
								break;
						}
					}
					break;
				case nPersonaje::PERSONAJE_SHUGENJA :
					{
						switch ( comportamiento )
						{
							case NEUTRAL :
								{
									if ( !PuedeConstruir() && jugador->GetNumPlanos() > 0 )
										EncolarAccion( ACCION_INTERCAMBIAR_PLANOS );
								}
								break;
							case CONSTRUCTIVO :
								{
									if ( !PuedeConstruir() )
									{
										int num_accion2 = rand() % 2;
										if ( num_accion2 == 0 && jugador->GetNumPlanos() > 0 )
											EncolarAccion( ACCION_INTERCAMBIAR_PLANOS );
										else
										{
											if ( PuedeCambiarPlanos() )
												EncolarAccion( ACCION_INTERCAMBIAR_PLANOS_CON_JUGADOR );
											else
											{
												if ( jugador->GetNumPlanos() > 0 )
													EncolarAccion( ACCION_INTERCAMBIAR_PLANOS );
											}
										}
									}
									if ( PuedeResucitar() )
										EncolarAccion( ACCION_RESUCITAR );
								}
								break;
							case DESTRUCTIVO :
								{
									if ( PuedeCambiarPlanos() )
										EncolarAccion( ACCION_INTERCAMBIAR_PLANOS_CON_JUGADOR );
									else
									{
										if ( jugador->GetNumPlanos() > 0 )
											EncolarAccion( ACCION_INTERCAMBIAR_PLANOS );
									}
								}
								break;
						}
					}
					break;
				case nPersonaje::PERSONAJE_SHOGUN :
					{
						if ( PuedeRecaudar() )
							EncolarAccion( ACCION_RECAUDAR );
						switch ( comportamiento )
						{
							case NEUTRAL :
								{
									int num_accion3 = rand() % 2;
									if ( num_accion3 == 0 )
										EncolarAccion( ACCION_CONTRATAR );
								}
								break;
							case CONSTRUCTIVO :
							case DESTRUCTIVO :
								{
									if ( PuedeRecaudarFuera() )
									{
										int num_contratos = (rand() % 3) + 1;
										for ( int i = 0; i < num_contratos; i++ )
										{
											EncolarAccion( ACCION_CONTRATAR );
											EncolarAccion( ACCION_RECAUDAR_FUERA );
										}
									}
								}
								break;
						}
					}
					break;
				case nPersonaje::PERSONAJE_MAESTRO :
					{
						if ( PuedeRecaudar() )
							EncolarAccion( ACCION_RECAUDAR );
						switch ( comportamiento )
						{
							case NEUTRAL :
								{
									int num_accion4 = rand() % 2;
									if ( num_accion4 == 0 )
										EncolarAccion( ACCION_CONTRATAR );
								}
								break;
						}
					}
					break;
				case nPersonaje::PERSONAJE_MERCADER :
					{
						switch ( comportamiento )
						{
							case NEUTRAL :
								{
									int num_accion5 = rand() % 2;
									if ( num_accion5 == 0 )
										EncolarAccion( ACCION_CONTRATAR );
									if ( PuedeRecaudar() )
										EncolarAccion( ACCION_RECAUDAR );
								}
								break;
							case CONSTRUCTIVO :
								{
									if ( PuedeRecaudar() )
									{
										int num_contratos2 = (rand() % 3) + 1;
										for ( int i = 0; i < num_contratos2; i++ )
											EncolarAccion( ACCION_CONTRATAR );
										EncolarAccion( ACCION_RECAUDAR );
									}
								}
								break;
							default : break;
						}
					}
					break;
				case nPersonaje::PERSONAJE_INGENIERO :
					{
						switch ( comportamiento )
						{
							case NEUTRAL :
								{
									int num_accion6 = rand() % 2;
									if ( num_accion6 == 0 )
										EncolarAccion( ACCION_CONTRATAR );
								}
								break;
							case CONSTRUCTIVO :
								{
									if ( PuedeConstruir() )
									{
										int num_contratos3 = (rand() % 3) + 1;
										for ( int i = 0; i < num_contratos3; i++ )
											EncolarAccion( ACCION_CONTRATAR );
									}
								}
								break;
							default : break;
						}
					}
					break;
				case nPersonaje::PERSONAJE_SAMURAI :
					{
						if ( PuedeRecaudar() )
							EncolarAccion( ACCION_RECAUDAR );
						switch ( comportamiento )
						{
							case NEUTRAL :
								{
									if ( PuedeDestruir() )
									{
										int num_accion7 = rand() % 2;
										if ( num_accion7 == 0 )
											EncolarAccion( ACCION_CONTRATAR );
										EncolarAccion( ACCION_DESTRUIR_EDIFICIO );
									}
								}
								break;
							case CONSTRUCTIVO :
								{
									if ( PuedeDestruir() )
									{
										int num_contratos4 = (rand() % 3) + 1;
										for ( int i = 0; i < num_contratos4; i++ )
											EncolarAccion( ACCION_CONTRATAR );
										EncolarAccion( ACCION_DESTRUIR_EDIFICIO );
									}
								}
								break;
							case DESTRUCTIVO :
								{
									if ( PuedeDestruir() )
										EncolarAccion( ACCION_DESTRUIR_EDIFICIO );
									if ( PuedeDestruir() )
										EncolarAccion( ACCION_DESTRUIR_EDIFICIO );
								}
								break;
						}
					}
					break;
			}

			// Si puede construir, construye tantas veces como pueda
			if ( PuedeConstruir() )
			{
				// Construir Plano
				EncolarAccion( ACCION_CONSTRUIR_PLANO );
				EncolarAccion( ACCION_CONSTRUIR_PLANO );
				if ( personajeSeleccionado == nPersonaje::PERSONAJE_INGENIERO )
					EncolarAccion( ACCION_CONSTRUIR_PLANO );
			}
			else
			{
				int num_accion8 = rand() % 2;
				if ( num_accion8 == 0 )
					EncolarAccion( ACCION_IR_A_AYUNTAMIENTO );
			}

			// Liberar Personaje
			EncolarAccion( ACCION_LIBERAR_PERSONAJE );
			EncolarAccion( ACCION_BORRAR_PERSONAJE );
		}
	}
	else if ( jugador->GetPersonaje()->EstaMuerto() && acciones_jugador.Size() > 1 )
	{
		//n_printf("Como la IA esta muerta, libero su cola de acciones\n");
		acciones_jugador.Clear();

		EncolarAccion( ACCION_LIBERAR_PERSONAJE );
		EncolarAccion( ACCION_BORRAR_PERSONAJE );
	}
	else
	{
		if ( jugador->JugadorInactivo() )
			EjecutarSiguienteAccion();
	}
}

void nIAJugador::EncolarAccion( T_Accion accion )
{
	this->acciones_jugador.PushBack( accion );
}

void nIAJugador::EjecutarSiguienteAccion()
{
	bool conseguido = true;
	if ( this->acciones_jugador.Size() > 0 )
	{
		T_Accion accionActual = this->acciones_jugador.At( 0 );

		switch ( accionActual )
		{
			case ACCION_COGER_PERSONAJE:
				jugador->IA_CambiarPersonaje( personajeSeleccionado );
				break;
			case ACCION_ROBAR_PERSONAJE_MAS_CERCANO:
				jugador->IA_RobarPersonajeMasCercano();
				break;
			case ACCION_ROBAR_PERSONAJE_MEJOR:
				jugador->IA_RobarPersonajeMejor();
				break;
			case ACCION_COGER_PLANO:
				jugador->IA_CogerPlano();
				break;
			case ACCION_LIBERAR_PERSONAJE:
				conseguido = jugador->IA_LiberarPersonaje();
				break;
			case ACCION_MATAR_JUGADOR_MAS_CERCANO:
				jugador->IA_MatarJugadorMasCercano();
				break;
			case ACCION_MATAR_JUGADOR_MEJOR:
				jugador->IA_MatarJugadorMejor();
				break;
			case ACCION_CONSTRUIR_PLANO:
				jugador->IA_ConstruirPlano();
				break;
			case ACCION_COGER_DINERO:
				jugador->IA_CogerDinero();
				break;
			case ACCION_INTERCAMBIAR_PLANOS:
				jugador->IA_IntercambiarPlanos();
				break;
			case ACCION_INTERCAMBIAR_PLANOS_CON_JUGADOR:
				jugador->IA_IntercambiarPlanosConJugador();
				break;
			case ACCION_DESTRUIR_EDIFICIO:
				jugador->IA_DestruirEdificio();
				break;
			case ACCION_IR_A_AYUNTAMIENTO:
				jugador->IA_Ir_A_Ayuntamiento();
				break;
			case ACCION_BORRAR_PERSONAJE:
				personajeSeleccionado = nPersonaje::PERSONAJE_CAMPESINO;
				break;
			case ACCION_RECAUDAR:
				jugador->IA_Recaudar();
				break;
			case ACCION_RESUCITAR:
				jugador->IA_Resucitar();
				break;
			case ACCION_RECAUDAR_FUERA:
				jugador->IA_RecaudarEnAyuntamientoMasCercano();
				break;
			case ACCION_CONTRATAR:
				jugador->IA_Contratar();
				break;
		}
		if ( conseguido )
			this->acciones_jugador.Erase( 0 );
	}
}

bool nIAJugador::PuedeCambiarPlanos()
{
	int num_max_planos = this->jugador->GetNumPlanos();

	// Se busca entre todos los jugadores
	for ( int i = 0; i < partida->GetJugadores()->Size(); i++ )
	{
		// el que tenga más planos
		if ( partida->GetJugadores()->At( i )->GetNumPlanos() > num_max_planos )
			return true;
		else
		{
			// Y a igual número de planos
			if ( partida->GetJugadores()->At( i )->GetNumPlanos() == num_max_planos &&
				 partida->GetJugadores()->At( i )->GetId() != this->jugador->GetId() )
			{
				int valor_max_planos     = 0;
				int valor_max_planos_aux = 0;

				// se elige el jugador con mejor combinación de planos
				for ( int j = 0; j < partida->GetJugadores()->At( i )->GetNumPlanos(); j++ )
				{
					// Nos aseguramos de que sea un objeto válido
					if ( partida->GetJugadores()->At( i )->GetPlanos().At( j ) != NULL )
						valor_max_planos += partida->GetJugadores()->At( i )->GetPlanos().At( j )->GetValor();
				}

				for ( int k = 0; k < this->jugador->GetNumPlanos(); k++ )
				{
					// Nos aseguramos de que sea un objeto válido
					if ( this->jugador->GetPlanos().At( k ) != NULL )
						valor_max_planos_aux += this->jugador->GetPlanos().At( k )->GetValor();
				}

				if ( valor_max_planos > valor_max_planos_aux )
					return true;
			}
		}
	}
	return false;
}

bool nIAJugador::InteresaDinero()
{
	bool encontrado = false;

	// Si no tiene dinero
	if ( this->jugador->GetOro() == 0 )
		return true;

	// Para cada plano
	for ( int i = 0; i < this->jugador->GetPlanos().Size(); i++ )
	{
		// Nos aseguramos de que sea un objeto válido
		if ( this->jugador->GetPlanos().At( i ) != NULL )
		{
			// Comprueba si puede construirlo
			if ( this->jugador->GetPlanos().At( i )->GetCoste() <= this->jugador->GetOro() &&
				 ( !this->jugador->LoTieneConstruido( this->jugador->GetPlanos().At( i )->GetNombre().c_str() ) ||
				   this->jugador->LoTieneConstruido( "ESCUELA IMPERIAL" ) ) )
			{
				encontrado = true;
				break;
			}
		}
	}
	return !encontrado;
}

bool nIAJugador::InteresaPlano()
{
	bool encontrado = false;

	// Si no tiene planos
	if ( this->jugador->GetNumPlanos() == 0 )
		return true;

	// Para cada plano
	for ( int i = 0; i < this->jugador->GetPlanos().Size(); i++ )
	{
		// Nos aseguramos de que sea un objeto válido
		if ( this->jugador->GetPlanos().At( i ) != NULL )
		{
			// Comprueba si puede construirlo
			if ( this->jugador->GetPlanos().At( i )->GetCoste() <= this->jugador->GetOro() &&
				 ( !this->jugador->LoTieneConstruido( this->jugador->GetPlanos().At( i )->GetNombre().c_str() ) ||
				   this->jugador->LoTieneConstruido( "ESCUELA IMPERIAL" ) ) )
			{
				encontrado = true;
				break;
			}
		}
	}
	return !encontrado;
}

bool nIAJugador::PuedeConstruir()
{
	bool encontrado = false;

	// Si no tiene planos o no tiene dinero
	if ( this->jugador->GetNumPlanos() == 0 || this->jugador->GetOro() == 0 )
		return false;

	// Para cada plano
	for ( int i = 0; i < this->jugador->GetPlanos().Size(); i++ )
	{
		// Nos aseguramos de que sea un objeto válido
		if ( this->jugador->GetPlanos().At( i ) != NULL )
		{
			// Comprueba si puede construirlo
			if ( this->jugador->GetPlanos().At( i )->GetCoste() <= this->jugador->GetOro() &&
				 ( !this->jugador->LoTieneConstruido( this->jugador->GetPlanos().At( i )->GetNombre().c_str() ) ||
				   this->jugador->LoTieneConstruido( "ESCUELA IMPERIAL" ) ) )
			{
				encontrado = true;
				break;
			}
		}
	}
	return encontrado;
}

bool nIAJugador::PuedeDestruir()
{
	bool encontrado = false;

	// Si no tiene planos o no tiene dinero
	if ( jugador->GetOro() == 0 )
		return false;

	// Para el jugador con más edificios construidos
	nJugador *jugador_sel = partida->JugadorMejor( jugador->GetId() );
	if ( jugador_sel != NULL )
	{
		for ( int i = 0; i < jugador_sel->GetDistrito()->GetListaPlanos().Size(); i++ )
		{
			// Nos aseguramos de que sea un objeto válido
			if ( jugador_sel->GetDistrito()->GetListaPlanos().At( i ) != NULL )
			{
				// Comprueba si puede destruirlo
				if ( jugador_sel->GetDistrito()->GetListaPlanos().At( i )->GetCoste() <= jugador->GetOro() &&
					( jugador_sel->GetPersonaje()->GetTipoPersonaje() != nPersonaje::TPersonaje::PERSONAJE_MAESTRO ||
					( jugador_sel->GetPersonaje()->GetTipoPersonaje() == nPersonaje::TPersonaje::PERSONAJE_MAESTRO &&
						jugador->LoTieneConstruido( "ESCUELA DE ASEDIO" ) ) ) )
				{
					encontrado = true;
					break;
				}
			}
		}
	}
	return encontrado;
}

bool nIAJugador::PuedeRecaudar()
{
	bool encontrado = false;

	// Para cada plano
	for ( int i = 0; i < this->jugador->GetDistrito()->GetListaPlanos().Size(); i++ )
	{
		// Comprueba si puede recaudar
		if ( ( jugador->GetDistrito()->GetListaPlanos().At( i )->GetDistintivo() == nPlano::TPlano::COMERCIAL &&
			   jugador->GetPersonaje()->GetTipoPersonaje() == nPersonaje::TPersonaje::PERSONAJE_MERCADER ) ||
			 ( jugador->GetDistrito()->GetListaPlanos().At( i )->GetDistintivo() == nPlano::TPlano::IMPERIAL &&
			   jugador->GetPersonaje()->GetTipoPersonaje() == nPersonaje::TPersonaje::PERSONAJE_SHOGUN ) ||
			 ( jugador->GetDistrito()->GetListaPlanos().At( i )->GetDistintivo() == nPlano::TPlano::RELIGIOSO &&
			   jugador->GetPersonaje()->GetTipoPersonaje() == nPersonaje::TPersonaje::PERSONAJE_MAESTRO ) ||
			 ( jugador->GetDistrito()->GetListaPlanos().At( i )->GetDistintivo() == nPlano::TPlano::MILITAR &&
			   jugador->GetPersonaje()->GetTipoPersonaje() == nPersonaje::TPersonaje::PERSONAJE_SAMURAI ) )
		{
			encontrado = true;
			break;
		}
	}
	return encontrado;
}

bool nIAJugador::PuedeRecaudarFuera()
{
	bool encontrado = false;

	// Para cada jugador
	for ( int i = 0; i < partida->GetJugadores()->Size(); i++ )
	{
		// diferente al correspondiente a la IA
		if ( jugador->GetId() != partida->GetJugadores()->At( i )->GetId() )
		{
			// Para cada plano
			for ( int j = 0; j < partida->GetJugadores()->At( i )->GetDistrito()->GetListaPlanos().Size(); j++ )
			{
				// Comprueba si puede recaudar
				if ( partida->GetJugadores()->At( i )->GetDistrito()->GetListaPlanos().At( j )->GetDistintivo() == nPlano::TPlano::IMPERIAL )
				{
					encontrado = true;
					break;
				}
			}
		}
	}
	return encontrado;
}

bool nIAJugador::PuedeResucitar()
{
	bool encontrado = false;

	// Para cada campesino
	for ( int i = 0; i < this->partida->GetCampesinos()->Size(); i++ )
	{
		// Comprueba si el campesino está muerto
		if ( this->partida->GetCampesinos()->At( i )->EstaMuerto() )
		{
			encontrado = true;
			break;
		}
	}
	return encontrado;
}

nIAJugador::T_Comportamiento nIAJugador::GetComportamiento()
{
	return this->comportamiento;
}

void nIAJugador::SetComportamiento( T_Comportamiento estado )
{
	this->comportamiento = estado;
}

bool nIAJugador::EscogerPersonaje()
{
	bool personajeElegido = false;

	// Está interesado en elegir al personaje que más pueda construir en el turno
	if ( PuedeConstruir() )
	{
		switch ( comportamiento )
		{
			case NEUTRAL :
				{
					while ( !personajeElegido )
						personajeElegido = PersonajeAlAzar();
				}
				break;
			case CONSTRUCTIVO :
				{
					// 1ª OPCIÓN (de momento) -> INGENIERO (3 construcciones)
					if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
						jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
					{
						personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
						personajeElegido      = true;
					}
					else
					{
						// 2ª OPCIÓN (de momento) -> Cualesquiera de los personajes restantes (2 construcciones)
						if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) &&
							jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MERCADER )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_MERCADER;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHOGUN )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_SHOGUN;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MAESTRO )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_MAESTRO;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_LADRONA )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_LADRONA;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SAMURAI )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_SAMURAI;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
							personajeElegido      = true;
						}
					}
				}
				break;
			case DESTRUCTIVO :
				{
					// 1ª OPCIÓN (de momento) -> LADRONA (2 construcciones)
					if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) &&
						jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_LADRONA )
					{
						personajeSeleccionado = nPersonaje::PERSONAJE_LADRONA;
						personajeElegido      = true;
					}
					else
					{
						// 2ª OPCIÓN (de momento) -> Cualesquiera de los personajes restantes (2 construcciones)
						if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
						 	 jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SAMURAI )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_SAMURAI;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) &&
							jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MERCADER )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_MERCADER;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHOGUN )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_SHOGUN;
							personajeElegido      = true;
						}
						else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MAESTRO )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_MAESTRO;
							personajeElegido      = true;
						}
					}
				}
				break;
		}
	}
	else	// No puede construir
	{
		// Está interesado en elegir al personaje que más dinero pueda obtener en el turno
		if ( InteresaDinero() )
		{
			switch ( comportamiento )
			{
				case NEUTRAL :
					{
						while ( !personajeElegido )
							personajeElegido = PersonajeAlAzar();
					}
					break;
				case CONSTRUCTIVO :
					{
						// 1ª OPCIÓN (de momento) -> MERCADER (20 monedas + "10 monedas(ESCUELA DE COMERCIO)" + recaudo)
						if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) &&
							jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MERCADER )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_MERCADER;
							personajeElegido      = true;
						}
						else
						{
							// 2ª OPCIÓN (de momento) -> LADRONA (10 monedas + "10 monedas(ESCUELA DE COMERCIO)" + robo + "robo(ESCUELA DE LADRONES)")
							if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_LADRONA )
							{
								personajeSeleccionado = nPersonaje::PERSONAJE_LADRONA;
								personajeElegido      = true;
							}
							else
							{
								// Si tiene edificios construidos por los que recaudar
								if ( PuedeRecaudar() )
								{
									// 3ª OPCIÓN (de momento) -> SHOGUN, MAESTRO o SAMURAI (10 monedas + "10 monedas(ESCUELA DE COMERCIO)" + recaudo)
									if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) &&
										jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHOGUN )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_SHOGUN;
										personajeElegido      = true;
									}
									else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) &&
											jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MAESTRO )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_MAESTRO;
										personajeElegido      = true;
									}
									else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) &&
											jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SAMURAI )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_SAMURAI;
										personajeElegido      = true;
									}
									else
									{
										// 4ª OPCIÓN (de momento) -> Cualesquiera de los personajes restantes (10 monedas + "10 monedas(ESCUELA DE COMERCIO)")
										if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
											jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
											personajeElegido      = true;
										}
									}
								}
								else	// No tiene edificios por los que recaudar
								{	// 4ª OPCIÓN (de momento) -> INGENIERO, KUNOICHI ó SHUGENJA (10 monedas + "10 monedas(ESCUELA DE COMERCIO)")
									if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
										jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
										personajeElegido      = true;
									}
									else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
											jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
										personajeElegido      = true;
									}
									else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
											jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
										personajeElegido      = true;
									}
								}
							}
						}
					}
					break;
				case DESTRUCTIVO :
					{
						// 1ª OPCIÓN (de momento) -> LADRONA (10 monedas + "10 monedas(ESCUELA DE COMERCIO)" + robo + "robo(ESCUELA DE LADRONES)")
						if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) &&
							jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_LADRONA )
						{
							personajeSeleccionado = nPersonaje::PERSONAJE_LADRONA;
							personajeElegido      = true;
						}
						else
						{
							// 2ª OPCIÓN (de momento) -> MERCADER (20 monedas + "10 monedas(ESCUELA DE COMERCIO)" + recaudo)
							if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) &&
								jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MERCADER )
							{
								personajeSeleccionado = nPersonaje::PERSONAJE_MERCADER;
								personajeElegido      = true;
							}
							else
							{
								// Si tiene edificios construidos por los que recaudar
								if ( PuedeRecaudar() )
								{
									// 3ª OPCIÓN (de momento) -> SHOGUN, MAESTRO o SAMURAI (10 monedas + "10 monedas(ESCUELA DE COMERCIO)" + recaudo)
									if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) &&
										 jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SAMURAI )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_SAMURAI;
										personajeElegido      = true;
									}
									else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) &&
										jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHOGUN )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_SHOGUN;
										personajeElegido      = true;
									}
									else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) &&
										 	  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MAESTRO )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_MAESTRO;
										personajeElegido      = true;
									}
									else
									{
										// 4ª OPCIÓN (de momento) -> Cualesquiera de los personajes restantes (10 monedas + "10 monedas(ESCUELA DE COMERCIO)")
										if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
											 jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
												  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
											      jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
											personajeElegido      = true;
										}
									}
								}
								else	// No tiene edificios por los que recaudar
								{	// 4ª OPCIÓN (de momento) -> INGENIERO, KUNOICHI ó SHUGENJA (10 monedas + "10 monedas(ESCUELA DE COMERCIO)")
									if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
										 jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
										personajeElegido      = true;
									}
									else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
											jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
										personajeElegido      = true;
									}
									else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
										      jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
										personajeElegido      = true;
									}
								}
							}
						}
					}
					break;
			}
		}
		else	// No interesa el dinero
		{
			// Está interesado en elegir al personaje que más planos pueda obtener en el turno
			if ( InteresaPlano() )
			{
				switch ( comportamiento )
				{
					case NEUTRAL :
						{
							while ( !personajeElegido )
								personajeElegido = PersonajeAlAzar();
						}
						break;
					case CONSTRUCTIVO :
						{
							// Está interesado en elegir al shugenja para cambiar los planos con el jugador que más planos tenga
							if ( PuedeCambiarPlanos() )
							{
								// 1ª OPCIÓN (de momento) -> SHUGENJA (n planos)
								if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
									jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
								{
									personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
									personajeElegido      = true;
								}
								else
								{
									// 2ª OPCIÓN (de momento) -> INGENIERO (2 planos)
									if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
										jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
										personajeElegido      = true;
									}
									else
									{
										// 3ª OPCIÓN (de momento) -> Cualesquiera de los personajes restantes (1 plano)
										if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) &&
											 jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MERCADER )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_MERCADER;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
											      jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHOGUN )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_SHOGUN;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_LADRONA )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_LADRONA;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MAESTRO )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_MAESTRO;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SAMURAI )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_SAMURAI;
											personajeElegido      = true;
										}
									}
								}
							}
							else	// No le interesa cambiar planos porque es el jugador con más planos
							{
								// 1ª OPCIÓN (de momento) -> INGENIERO (2 planos)
								if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
									jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
								{
									personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
									personajeElegido      = true;
								}
								else
								{
									// 2ª OPCIÓN (de momento) -> SHUGENJA (n planos)
									if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
										jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
										personajeElegido      = true;
									}
									else
									{
										// 3ª OPCIÓN (de momento) -> Cualesquiera de los personajes restantes (1 plano)
										if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) &&
											 jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MERCADER )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_MERCADER;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
											      jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHOGUN )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_SHOGUN;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_LADRONA )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_LADRONA;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MAESTRO )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_MAESTRO;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SAMURAI )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_SAMURAI;
											personajeElegido      = true;
										}
									}
								}
							}
						}
						break;
					case DESTRUCTIVO :
						{
							// Está interesado en elegir al shugenja para cambiar los planos con el jugador que más planos tenga
							if ( PuedeCambiarPlanos() )
							{
								// 1ª OPCIÓN (de momento) -> SHUGENJA (n planos)
								if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
									jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
								{
									personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
									personajeElegido      = true;
								}
								else
								{
									// 2ª OPCIÓN (de momento) -> INGENIERO (2 planos)
									if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
										jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
										personajeElegido      = true;
									}
									else
									{
										// 3ª OPCIÓN (de momento) -> Cualesquiera de los personajes restantes (1 plano)
										if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
											 jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) &&
												  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_LADRONA )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_LADRONA;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) &&
												  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SAMURAI )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_SAMURAI;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) &&
												  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHOGUN )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_SHOGUN;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) &&
												  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MAESTRO )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_MAESTRO;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) &&
												jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MERCADER )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_MERCADER;
											personajeElegido      = true;
										}
									}
								}
							}
							else	// No le interesa cambiar planos porque es el jugador con más planos
							{
								// 1ª OPCIÓN (de momento) -> INGENIERO (2 planos)
								if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) &&
									 jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
								{
									personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
									personajeElegido      = true;
								}
								else
								{
									// 2ª OPCIÓN (de momento) -> SHUGENJA (n planos)
									if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) &&
										 jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
									{
										personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
										personajeElegido      = true;
									}
									else
									{
										// 3ª OPCIÓN (de momento) -> Cualesquiera de los personajes restantes (1 plano)
										if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) &&
											 jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) &&
												  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_LADRONA )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_LADRONA;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) &&
												  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SAMURAI )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_SAMURAI;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) &&
												  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHOGUN )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_SHOGUN;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) &&
												  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MAESTRO )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_MAESTRO;
											personajeElegido      = true;
										}
										else if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) &&
												  jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MERCADER )
										{
											personajeSeleccionado = nPersonaje::PERSONAJE_MERCADER;
											personajeElegido      = true;
										}
									}
								}
							}
						}
						break;
				}
			}
			else
			{
				while ( !personajeElegido )
					personajeElegido = PersonajeAlAzar();
			}
		}
	}
	/*if ( personajeSeleccionado == nPersonaje::PERSONAJE_SHOGUN )
		n_printf("EscogerPersonaje:SHOGUN\n");
	else if ( personajeSeleccionado == nPersonaje::PERSONAJE_MAESTRO )
		n_printf("EscogerPersonaje:MAESTRO\n");
	else if ( personajeSeleccionado == nPersonaje::PERSONAJE_MERCADER )
		n_printf("EscogerPersonaje:MERCADER\n");
	else if ( personajeSeleccionado == nPersonaje::PERSONAJE_SAMURAI )
		n_printf("EscogerPersonaje:SAMURAI\n");
	else if ( personajeSeleccionado == nPersonaje::PERSONAJE_KUNOICHI )
		n_printf("EscogerPersonaje:KUNOICHI\n");
	else if ( personajeSeleccionado == nPersonaje::PERSONAJE_LADRONA )
		n_printf("EscogerPersonaje:LADRONA\n");
	else if ( personajeSeleccionado == nPersonaje::PERSONAJE_INGENIERO )
		n_printf("EscogerPersonaje:INGENIERO\n");
	else if ( personajeSeleccionado == nPersonaje::PERSONAJE_SHUGENJA )
		n_printf("EscogerPersonaje:SHUGENJA\n");*/

	nJugador *jugador_mejor = this->partida->JugadorMejor( this->jugador->GetId() );
	if ( jugador_mejor != NULL )
	{
		//n_printf("MEJOR JUGADOR = %d CON %d EDIFICIOS CONSTRUIDOS\n",jugador_mejor->GetId(),jugador_mejor->GetDistrito()->NumeroConstrucciones());
		if ( abs( this->jugador->GetDistrito()->NumeroConstrucciones() - jugador_mejor->GetDistrito()->NumeroConstrucciones() ) < 2 &&
			 this->GetComportamiento() != CONSTRUCTIVO )
		{
			this->SetComportamiento( CONSTRUCTIVO );
			//n_printf("IA %d NEUTRAL\n",this->jugador->GetId());
		}
		else if ( abs( this->jugador->GetDistrito()->NumeroConstrucciones() - jugador_mejor->GetDistrito()->NumeroConstrucciones() ) < 4 &&
				  this->GetComportamiento() != NEUTRAL )
		{
			this->SetComportamiento( NEUTRAL );
			//n_printf("IA %d CONSTRUCTIVO\n",this->jugador->GetId());
		}
		else if ( abs( this->jugador->GetDistrito()->NumeroConstrucciones() - jugador_mejor->GetDistrito()->NumeroConstrucciones() ) > 3 &&
				  this->GetComportamiento() != DESTRUCTIVO )
		{
			this->SetComportamiento( DESTRUCTIVO );
			//n_printf("IA %d DESTRUCTIVO\n",this->jugador->GetId());
		}
		/*else
			n_printf("Sin modificar el comportamiento de la IA\n");*/
	}

	return personajeElegido;
}

bool nIAJugador::PersonajeAlAzar()
{
	// Elegimos un personaje al azar
	bool personajeElegido = false;
	int  numPersonaje     = rand() % 8;
	switch ( numPersonaje )
	{
		case 0:
			if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_LADRONA ) && jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_LADRONA )
			{
				personajeSeleccionado = nPersonaje::PERSONAJE_LADRONA;
				personajeElegido      = true;
			}
			break;
		case 1:
			if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHOGUN ) && jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHOGUN )
			{
				personajeSeleccionado = nPersonaje::PERSONAJE_SHOGUN;
				personajeElegido      = true;
			}
			break;
		case 2:
			if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_KUNOICHI ) && jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_KUNOICHI )
			{
				personajeSeleccionado = nPersonaje::PERSONAJE_KUNOICHI;
				personajeElegido      = true;
			}
			break;
		case 3:
			if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MAESTRO ) && jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MAESTRO )
			{
				personajeSeleccionado = nPersonaje::PERSONAJE_MAESTRO;
				personajeElegido      = true;
			}
			break;
		case 4:
			if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_INGENIERO ) && jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_INGENIERO )
			{
				personajeSeleccionado = nPersonaje::PERSONAJE_INGENIERO;
				personajeElegido      = true;
			}
			break;
		case 5:
			if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_MERCADER ) && jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_MERCADER )
			{
				personajeSeleccionado = nPersonaje::PERSONAJE_MERCADER;
				personajeElegido      = true;
			}
			break;
		case 6:
			if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SHUGENJA ) && jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SHUGENJA )
			{
				personajeSeleccionado = nPersonaje::PERSONAJE_SHUGENJA;
				personajeElegido      = true;
			}
			break;
		case 7:
			if ( partida->PersonajeLibre( nPersonaje::PERSONAJE_SAMURAI ) && jugador->GetPersonajeAnterior() != nPersonaje::PERSONAJE_SAMURAI )
			{
				personajeSeleccionado = nPersonaje::PERSONAJE_SAMURAI;
				personajeElegido      = true;
			}
			break;
	}
	return personajeElegido;
}