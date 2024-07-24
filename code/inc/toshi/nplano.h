#ifndef N_PLANO_H
#define N_PLANO_H
//------------------------------------------------------------------------------
/**
    @class classname

    @brief a brief description of the class

    a detailed description of the class

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_N3DNODE_H
#include "node/n3dnode.h"
#endif

#ifndef N_CSPRITE_H
#include "CSprite/ncsprite.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#include "kernel/nscriptserver.h"

#include <string>
using namespace std;

#undef N_DEFINES
#define N_DEFINES nPlano
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------

class n3DNode;
class nCSprite;
class nTexArrayNode;

class nPlano : public n3DNode
{
	public:
	
		enum TPlano
		{
			ESPECIAL,
			COMERCIAL,
			MILITAR,
			RELIGIOSO,
			IMPERIAL,
			DESCONOCIDO
		};

		// constructor por defecto
		nPlano();
		// constructor general
		nPlano( int id_plano, int id_edificio, string nombre_plano, TPlano distintivo_plano, 
				string descripcion_plano, string descripcion_plano2, int coste_plano, int valor_plano );
		// destructor
		virtual ~nPlano();
		// persistency
		virtual bool SaveCmds( nPersistServer* persistServer );
		// Identificador único del plano
		void SetIdHUD( int id_plano );
		inline const int GetIdHUD() const;
		// Identificador del edificio a construir
		void SetIdEdificio( int id_edificio );
		inline const int GetIdEdificio() const;
		// Distintivo del plano: 0 IMPERIAL, 1 MILITAR, 2 RELIGIOSO, 3 COMERCIAL, 4 ESPECIAL
		void SetDistintivo( TPlano distintivo );
		inline const TPlano GetDistintivo() const;
		// Nombre del plano
		void SetNombre( string nombre_plano );
		inline string GetNombre() const;
		// Descripción del plano
		void SetDescripcion( string descripcion_plano );
		inline string GetDescripcion() const;
		// Descripción del plano (segunda línea)
		void SetDescripcion2( string descripcion_plano );
		inline string GetDescripcion2() const;
		// Valor del plano
		void SetValor( int valor_plano );
		inline const int GetValor() const;
		// Coste del plano
		void SetCoste( int coste_plano );
		inline const int GetCoste() const;
		// Está construido el plano
		bool EstaConstruido();
		// Construir el plano
		const char* Construir();
		// Comprueba la posicion del ratón por si coincide con la del plano
		bool Click( float x, float y );
		// Pinta el plano en la pantalla
		void Pintar();
		// Genera el modelo del edificio al que hace referencia el plano
		void GenerarModeloEdificio();
		// Cambia la posición del plano en la pantalla
		void SetPosition( vector2 posicion );
		inline vector2 GetPosition() const;
		// Cambia el tamaño del plano
		void SetMedidas( vector2 medidas );
		inline vector2 GetMedidas() const;
		// Linka la representación gráfica a la lógica
		void SetPlanoHUD();
		n3DNode* GetPlanoHUD();
		// pointer to nKernelServer
		static nKernelServer* kernelServer;

	private:

		nAutoRef<nScriptServer> refScriptServer;
		// Características lógicas
		int				IdEdificio;
		string			Nombre;
		nPlano::TPlano  Distintivo;
		string			Descripcion;
		string			Descripcion2;
		int			    Coste;
		int				Valor;
		bool			Construido;
		bool            Visible;
		// Características físicas;
		int				IdHUD;
		vector2         medidas;
		vector2         posicion;
		n3DNode*        PlanoHUD;
};

inline const int nPlano::GetIdHUD() const
{
	return IdHUD;
}

inline const int nPlano::GetIdEdificio() const
{
	return IdEdificio;
}

inline string nPlano::GetNombre() const
{
	return Nombre;
}

inline string nPlano::GetDescripcion() const
{
	return Descripcion;
}

inline string nPlano::GetDescripcion2() const
{
	return Descripcion2;
}

inline const int nPlano::GetCoste() const
{
	return Coste;
}

inline const int nPlano::GetValor() const
{
	return Valor;
}

inline const nPlano::TPlano nPlano::GetDistintivo() const
{
	return Distintivo;
}

inline vector2 nPlano::GetPosition() const
{
	return posicion;
}

inline vector2 nPlano::GetMedidas() const
{
	return medidas;
}
//------------------------------------------------------------------------------
#endif