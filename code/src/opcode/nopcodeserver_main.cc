#define N_IMPLEMENTS nOpcodeServer
//------------------------------------------------------------------------------
//  nopcodeserver_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "opcode/nopcodeserver.h"
#include "opcode/nopcodeshape.h"

//------------------------------------------------------------------------------
/**
*/
nOpcodeServer::nOpcodeServer()
{
    // setup the tree collider
    this->opcTreeCollider.SetFirstContact(false);       // report all contacts
    this->opcTreeCollider.SetFullBoxBoxTest(false);     // use coarse BV-BV tests
    this->opcTreeCollider.SetFullPrimBoxTest(false);    // use coarse primitive-BV tests
    this->opcTreeCollider.SetTemporalCoherence(false);  // don't use temporal coherence

    // setup the ray collider
    this->opcRayCollider.SetFirstContact(false);                // report all contacts
    this->opcRayCollider.SetTemporalCoherence(false);           // no temporal coherence
    this->opcRayCollider.SetClosestHit(false);                  // all hits
    this->opcRayCollider.SetCulling(true);                      // with backface culling
    this->opcRayCollider.SetMaxDist(100000.0f);                 // max dist 100 km
    this->opcRayCollider.SetDestination(&(this->opcFaceCache)); // detected hits go here

    // setup the sphere collider
    this->opcSphereCollider.SetFirstContact(false);             // report all contacts
    this->opcSphereCollider.SetTemporalCoherence(false);        // no temporal coherence
}

//------------------------------------------------------------------------------
/**
*/
nOpcodeServer::~nOpcodeServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCollideShape*
nOpcodeServer::NewShape(const char* id)
{
    n_assert(id);

    char buf[N_MAXPATH];
    this->getResourceID(id, buf, sizeof(buf));

    // does this shape already exist?
    nCollideShape* collShape = (nCollideShape*) this->shape_list.Find(buf);
    if (!collShape)
    {
        collShape = (nCollideShape*) new nOpcodeShape(buf);
        this->shape_list.AddTail(collShape);
    }
    collShape->AddRef();
    return collShape;
}

//------------------------------------------------------------------------------
