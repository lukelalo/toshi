#ifndef N_GAME_H
#define N_GAME_H
//------------------------------------------------------------------------------
/**
    @class nGame

    @brief Contiene el bucle principal del juego.

*/

#define USA_CAMARA_CON_RATON


#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/matrix.h"		// LO QUITAREMOS CUANDO LA CAMARA SEA EXTERNA
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
class nCamera;


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

    // Debug methods
    void ToggleVisualiseCollide();

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

    nAutoRef<n3DNode> renderScene;

    // Game should keep running
    bool stop;

	// Ruta del directorio que contiene todos los nIA
	nString iaPath;

	// Identificadores de los canales de tiempo.
	int timeChannel;
	int globalTimeChannel;


    nAutoRef<n3DNode> ref_camera;
    nAutoRef<n3DNode> ref_lookat;
    nAutoRef<nPrimitiveServer>  ref_prim;

    int mouse_old_x, mouse_old_y;
    int mouse_cur_x, mouse_cur_y;
    int mouse_rel_x, mouse_rel_y;

    void get_mouse_input(nInputServer *);
    void handle_input(void);
    void render_grid(nGfxServer *, matrix44&);
    void render_node(n3DNode *, matrix44&);
    void place_camera(matrix44&, matrix44&, float);
};

inline const char*nGame::GetIA() const {
	return iaPath.Get();
}

#endif