#define N_IMPLEMENTS nWorld
//------------------------------------------------------------------------------
//  (C) 2002	ling
//------------------------------------------------------------------------------
#include "collide/ncollideobject.h"
#include "collide/ncollideserver.h"
#include "gfx/nchannelserver.h"
#include "node/n3dnode.h"
#include "map/nmapnode.h"
#include "map/nmap.h"
#include "toshi/nentity.h"
#include "toshi/nmapa.h"
#include "toshi/nworld.h"

nNebulaScriptClass(nWorld, "nroot");

/// Maximum length of timeslice to process, in seconds
const float nWorld::stepSize = 0.009f;

//------------------------------------------------------------------------------
/**
*/
nWorld::nWorld() :
    channelServer(kernelServer, this),
    collideServer(kernelServer, this),
    collideContext(NULL),
    gameTime(0.0f),
	mapNode(kernelServer, this),
    terrainNode(kernelServer, this),
    mapTerrainNode(NULL)
{
    this->channelServer = "/sys/servers/channel";
    this->collideServer = "/sys/servers/collide";
    this->collideContext = collideServer->NewContext();

    timeChannel = channelServer->GenChannel("time");
}

//------------------------------------------------------------------------------
/**
*/
nWorld::~nWorld()
{
	collideContext->Remove();
	terrainNode->Release();
	mapTerrainNode->Release();
}

/**
    @brief Run world until time.

    Runs the world up to the new game time.  The simulation is run at a fixed
    timeslice so this process might actually process several steps.
*/
void nWorld::Run(float time)
{
    while (gameTime+stepSize < time)
    {
        Step(stepSize);
        gameTime += stepSize;
    }
}

/**
    @brief Process single step of simulation.
*/
void nWorld::Step(float dt)
{
    // Do funky stuff with time channel, everyone else does it
    channelServer->SetChannel1f(timeChannel, gameTime);

    // Assume only nEntity types are attached beneath here,
    // really stupid thing to do but let's hack it for the meanwhile
    nEntity* entity = NULL;
    for (entity = (nEntity*)this->GetHead(); entity != NULL; entity = (nEntity*)entity->GetSucc()) 
	{
		entity->Tick(dt);
    }

    // Detect collisions to be able to compute forces correctly
    // Most sims resolve collision after the integration
    // in order to step back and do something about it.
	collideContext->Collide();
    /*if (0 != collideContext->Collide())
    {
        nEntity* entity = NULL;
        for (entity = (nEntity*)this->GetHead(); entity != NULL; entity = (nEntity*)entity->GetSucc())
        {
				entity->Collide();
        }
    }*/

}

void nWorld::SetMap(const char* map_path)
{
    n_assert(NULL != map_path);

    this->mapNode=map_path;
}

void nWorld::SetTerrain(const char* terrain_path)
{
    n_assert(NULL != terrain_path);

    terrainNode = terrain_path;
    mapTerrainNode = (nMapNode*)terrainNode->Find("map");
    n_assert(NULL != mapTerrainNode);
    n_assert(0 == strcmp(mapTerrainNode->GetClass()->GetName(), "nmapnode"));
}

/**
    @brief Fetch the terrain nVisNode.
    
    Returns the terrain's already transformed nVisNode, unless there's an
    earthquake.  Funny enuff, an earthquake is possible to hack in without
    too much effort.
 */

n3DNode* nWorld::GetTerrainVisNode()
{
    if (terrainNode.isvalid())
		return terrainNode.get();
	else
		return NULL;
}

/**
    @brief Gets the height at a point on the terrain.

    A possible extension is if the world handled tiled terrain, this method
    can encapsulate looking up the correct terrain tile.
*/

float nWorld::GetHeight(const vector3& position) const
{
	if (mapTerrainNode)
		//return mapTerrainNode->GetMap()->GetHeight(position.x+9.0f, position.z+9.0f);
	{
		return mapTerrainNode->GetMap()->GetHeight(position.x, position.z)+0.04f;
	}
	else
		return 0.0f;
}

vector3 nWorld::GetNormal(const vector3& p_position, vector3 &p_normal) const
{
	p_normal.set(0,1,0);
	
	if (mapTerrainNode)
	{
		// Devolveríamos directamente lo que nos devuelve el mapnode, pero como no nos 
		// fiamos vamos a interpolar
		vector3 v_punto1, v_punto2;
		mapTerrainNode->GetMap()->GetNormal(floor(p_position.x), floor(p_position.z),v_punto1);
		mapTerrainNode->GetMap()->GetNormal(ceil(p_position.x), ceil(p_position.z),v_punto2);
        p_normal=(v_punto1+v_punto2)/2;
	}
	return p_normal;
}
//---------------------------------------------------------------------------
// Collision methods

/**
    @brief Creates a new collide shape.
    Called by nEntity::SetCollideShape() to attach a geometry to a shape id.
*/
 nCollideShape* nWorld::NewCollideShape(const char* name, const char* file, bool reload)
{
    nCollideShape* shape = collideServer->NewShape(name);
	if (shape->IsInitialized()) {
		collideServer->ReleaseShape(shape);
		shape->RemRef();
		if (shape->GetRef() == 0) 
			shape->Remove();
		shape = collideServer->NewShape(name);
	}

    if ((false == shape->IsInitialized()) || reload)
        shape->Load((nFileServer2*)kernelServer->Lookup("/sys/servers/file2"), file);
    return shape;
}

/**
    @brief Create a collide object of the given class and shape.
    Usually to be called by nEntity only.
*/

nCollideObject* nWorld::NewCollideObject(nEntity* entity)
{
    nCollideObject* object = collideContext->NewObject();
    collideContext->AddObject(object);
    object->SetClientData(entity);

    return object;
}

