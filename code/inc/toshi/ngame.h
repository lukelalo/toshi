#ifndef N_GAME_H
#define N_GAME_H
//------------------------------------------------------------------------------
/**
    @class nGame

    @brief Contiene el bucle principal del juego.

*/

//#define USA_CAMARA_CON_RATON

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/matrix.h"		// LO QUITAREMOS CUANDO LA CAMARA SEA EXTERNA
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#ifndef N_FMOD_H
#include "toshi/fmod.h"
#endif

#ifdef USA_CAMARA_CON_RATON
#ifndef N_PRIMITIVESERVER_H
#include "gfx/nprimitiveserver.h"
#endif
#endif

#undef N_DEFINES
#define N_DEFINES nGame
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nScriptServer;
class nGfxServer;
class nInputServer;
class nConServer;
class nSceneGraph2;
class nSpecialFxServer;
class nChannelServer;
class n3DNode;
class nAudioServer2;
class nIA;
class nWorld;
class nEntity;
class nCamera;
class nJugador;
class nAyuntamiento;
class nCasaMoneda;
class nCasaPlanos;
class nConstruccion;
class nNet;
class nPartida;
class nMenu;
class nVideo;

class N_PUBLIC nGame : public nRoot
{
public:
    /// constructor
    nGame();
    /// destructor
    virtual ~nGame();
    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

	/// Inicializa el juego
	void Init(const char *script_path);

    /// Start game
    void Run();
    /// Stop game
    void Stop();
    /// Set scene
	void SetScene(const char*scene_path);
	// Set World
	void SetWorld(const char*world_path);
    /// Set render camera
	void SetCamera(const char*camera_path);
	// Set/Get IAs
	void SetIA(const char*ia_path);
    inline const char*GetIA() const;
	// Set/Get NET
	void SetNet(const char *net_path);
	inline const char *GetNet() const;
	// Set/Get MENU
	void SetMenu(const char *menu_path);
	inline const char *GetMenu() const;
	// Set/Get VIDEO
	void SetVideo(const char *video_path);
	inline const char *GetVideo() const;

	// Devuelve la posición del ratón en coordenadas mundo
	vector3 GetPosMouseWorld();
	// Devuelve la entidad seleccionada en la posición del ratón
	nEntity * GetEntMouseWorld();
	// Devuelve un puntero a la partida actual
	nPartida * GetPartida();
	// Devuelve las coordenadas del ratón en posición de pantalla
	vector3 GetPosMouse();
	// Trata un click de ratón princi
	void ManejadorBotonPrincipal();
	// Trata un click de ratón
	void ManejadorBotonSecundario();

	// Muestra o desactiva la ayuda al jugador
	void MostrarAyuda(bool p_mostrar);

	// Cambia el cursor de la pantalla
	void CambiarCursor();

	// Indica si se debe o no mostrar el renderizado del juego
	void MostrarRender(bool mostrar);

	// Comienza una partida en un equipo local, sin red
	void ComenzarPartidaLocal(int num_jugadores);

	// Comienza una partida por red
	void ComenzarPartidaRed(bool servidor);

	// Actualiza el estado visual y de sonido
	void Update();

	// Apaga el sonido para que no haga cosas extrañas
	void Mute( bool apagar );

	// Pausa el juego
	void Pause( bool pausar );

    // Debug methods
    void ToggleVisualiseCollide();

	// Funciones para la música del juego (FMOD)
	void StopSong(int canal);
	void StopSound(int canal);
	void StopAllSounds();
	void PauseAllSounds(bool p_pausa);
	void PlaySong(int canal, const char *song);
	void PauseSong(int canal, bool p_pausa);
	void PlaySound(int canal, const char *sound);
	void Play3DSound(int canal, const char *sound, vector3 p_pos);
	int  Play3DSoundEx(int canal, const char *sound, vector3 p_pos, float p_mind, float p_maxd, bool p_loop, int p_prioridad);
	void SetPosSound(int canal, vector3 p_pos);
	void SetMusicVolume(int canal, float vol);
	void SetSoundVolume(int canal, float vol);

	// Devuelve si un jugador es jugador principal
	bool EsJugadorPrincipal(int jugador);

	// Devuelve los puntos por acciones del jugador
	int GetPuntosPorAccion(int jugador);

	// Devuelve los puntos por construcciones del jugador
	int GetPuntosPorConstruccion(int jugador);

	// Devuelve el número de construcciones del jugador
	int GetNumConstrucciones(int jugador);

	// Devuelve los puntos por distintivo del jugador
	int GetPuntosPorDistintivo(int jugador);

	// Devuelve los puntos por dinero del jugador
	int GetPuntosPorOro(int jugador);

	// Devuelve los puntos por tiempo del jugador
	int GetPuntosPorTiempo(int jugador);

	// Devuelve el total del tiempo del jugador
	int GetPuntosTotales(int jugador);

	// Indica si el juego ha iniciado
	void SetIniciado(bool p_iniciado);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    bool Trigger();
    void Render();

    nAutoRef<nScriptServer> scriptServer;
    nAutoRef<nGfxServer> gfxServer;
    nAutoRef<nInputServer> inputServer;
    nAutoRef<nConServer> consoleServer;
    nAutoRef<nSceneGraph2> sceneGraph;
    nAutoRef<nSpecialFxServer> specialfxServer;
    nAutoRef<nChannelServer> channelServer;
	nAutoRef<nRoot> ias;
	nAutoRef<nWorld> world;
	nAutoRef<nCamera> renderCamera;

	nAudioServer2* as2;
	nJugador *jugador;

    nAutoRef<n3DNode> renderScene;

	nPartida *partida_actual;

    // Game should keep running
    bool stop;
	bool mostrarRender;
	bool pausa;
	bool JuegoEnRed;
	bool EsServidor;
	bool AyudaActiva;
	bool iniciado; // Indica si el juego ha comenzado ya

	// Ruta del directorio que contiene todos los nIA
	nString iaPath;

  	//Controlador Red
	nAutoRef<nRoot> nets;
	nString netPath;

	// Controlador menus
	nAutoRef<nRoot> menus;
	nString menuPath;
	nMenu *menuSeleccionado;

	// Controlador videos
	nAutoRef<nRoot> videos;
	nString videoPath;

	// Para utilizar FMOD
	int canalMusica[8];
	int canalSonido[8];
	int volumenMusica[8];
	int volumenSonido[8];
	FSOUND_STREAM *p_cancion[8];
	FSOUND_SAMPLE *p_sonido[8];
	nArray<FSOUND_SAMPLE *> pt_samples;


	// Jugador del cliente
	nString jugadorPath;

	// Identificadores de los canales de tiempo.
	int timeChannel;
	int globalTimeChannel;


    nAutoRef<n3DNode> ref_camera;
    nAutoRef<n3DNode> ref_lookat;
 //   nAutoRef<nPrimitiveServer>  ref_prim;
	nString entidadColisionada;

    int mouse_old_x, mouse_old_y;
    int mouse_cur_x, mouse_cur_y;
    int mouse_rel_x, mouse_rel_y;
	float g_rueda;
	float altura_camara;
	float TiempoIA;

	bool MostrandoInformacion;

    void get_mouse_input(nInputServer *);
    void handle_input(void);
    void render_grid(nGfxServer *, matrix44&);
    void render_node(n3DNode *, matrix44&);
    void place_camera(matrix44&, matrix44&, float);
};

inline const char*nGame::GetIA() const {
	return iaPath.Get();
}

inline const char *nGame::GetNet() const {
	return netPath.Get();
}

inline const char *nGame::GetMenu() const {
	return menuPath.Get();
}

inline const char *nGame::GetVideo() const {
	return videoPath.Get();
}

#endif