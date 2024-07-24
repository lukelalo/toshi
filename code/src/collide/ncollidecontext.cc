#define N_IMPLEMENTS nCollideContext
//-------------------------------------------------------------------
//  ncollidecontext.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "misc/nquickwatch.h"
#include "collide/ncollideobject.h"
#include "collide/ncollidecontext.h"
#include "mathlib/bbox.h"

//-------------------------------------------------------------------
/**
     - 16-Jun-00   floh    created
*/
//-------------------------------------------------------------------
nCollideContext::~nCollideContext()
{
    // release all owned collide objects
    nCollideObject *co;
    while ((co = (nCollideObject *) this->owned_list.RemHead())) {
        n_assert(co);
        n_assert(co->GetContext() == this);
        if (co->IsAttached())
            this->RemObject(co);
        co->SetContext(NULL);
        delete co;
    };
}

//-------------------------------------------------------------------
/**
    @brief Construct a new collide object.

     - 27-Jun-00   floh    created
*/
//-------------------------------------------------------------------
nCollideObject *nCollideContext::NewObject(void)
{
    nCollideObject *co = new nCollideObject;
    co->SetId(this->unique_id++);
    co->SetContext(this);
    this->owned_list.AddTail(co);
    return co;
}

//-------------------------------------------------------------------
/**
    @brief Kill an owned collide object.

     - 27-Jun-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideContext::ReleaseObject(nCollideObject *co)
{
    n_assert(co);
    n_assert(co->GetContext() == this);
    if (co->IsAttached()) this->RemObject(co);
    co->SetContext(NULL);
    co->Remove();
    delete co;
}

//-------------------------------------------------------------------
/**
     - 26-Jun-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideContext::AddObject(nCollideObject *co)
{
    n_assert(co);
    n_assert(co->GetContext() == this);

    // link the object into our context
    co->SetAttached(true);
    this->attached_list.AddTail(&(co->context_node));

    // add minx/maxx nodes to x-dimensional sorted list
    co->xmin_cnode.AddToList(this->xdim_list);
    co->xmax_cnode.AddToList(this->xdim_list);
}

//-------------------------------------------------------------------
/**
     - 26-Jun-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideContext::RemObject(nCollideObject *co)
{
    n_assert(co);
    n_assert(co->GetContext() == this);
    n_assert(co->IsAttached());
    co->SetAttached(false);
    co->context_node.Remove();
    co->xmin_cnode.Remove();
    co->xmax_cnode.Remove();
}

//-------------------------------------------------------------------
/**
    @brief Call collide on each object in the context.
    
    After this, each object's collision array holds all collisions
    this object was involved with.

     - 26-Jun-00   floh    created
*/
//-------------------------------------------------------------------
int nCollideContext::Collide(void)
{
    // first, clear the collision counters in all collide objects
    nNode *context_node;
    for (context_node = this->attached_list.GetHead();
         context_node;
         context_node = context_node->GetSucc())
    {
        nCollideObject *co = (nCollideObject *) context_node->GetPtr();
        co->ClearCollissions();
    }

    // check the collision status for each object
    this->collideReportHandler.BeginFrame();
    for (context_node = this->attached_list.GetHead();
         context_node;
         context_node = context_node->GetSucc())
    {
        nCollideObject *co = (nCollideObject *) context_node->GetPtr();
        co->Collide();
    }
    this->collideReportHandler.EndFrame();

    int num_coll = this->collideReportHandler.GetNumCollissions();
    return num_coll;
}

//-------------------------------------------------------------------
/**
    Get all collisions an object is involved in.
    Returns pointer to an internal collision array and
    the number of collisions.

     - 04-Jul-00   floh    created
*/
//-------------------------------------------------------------------
int nCollideContext::GetCollissions(nCollideObject *co, nCollideReport **& cr)
{
    if (co->GetNumCollissions() > 0) {
        return this->collideReportHandler.GetCollissions(co,cr);
    } else {
        cr = NULL;
        return 0;
    }
}

//-------------------------------------------------------------------
/**
    Visualize all objects in the context.

     - 26-Jun-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideContext::Visualize(nGfxServer *gs)
{
    n_assert(gs);
    if (!this->attached_list.IsEmpty()) {
        nRState rs;
        rs.Set(N_RS_LIGHTING,N_FALSE);          gs->SetState(rs);
        rs.Set(N_RS_TEXTUREHANDLE,0);           gs->SetState(rs);
        rs.Set(N_RS_ALPHABLENDENABLE,N_FALSE),  gs->SetState(rs);
        rs.Set(N_RS_ZFUNC,N_CMP_LESSEQUAL);     gs->SetState(rs);
        rs.Set(N_RS_ZBIAS,1.0f);                gs->SetState(rs);
        matrix44 ivm;

        // get the current inverse view matrix
        gs->GetMatrix(N_MXM_INVVIEWER,ivm);

        // for each object in the system...
        nNode *context_node;
        for (context_node = this->attached_list.GetHead();
             context_node;
             context_node = context_node->GetSucc())
        {
            nCollideObject *co = (nCollideObject *) context_node->GetPtr();

            // get the current transformation
            matrix44 m = co->GetTransform();

            // multiply by inverse viewer matrix
            m *= ivm;

            // render the object
            gs->SetMatrix(N_MXM_MODELVIEW,m);
            co->VisualizeLocal(gs);

            // render any collision contacts
            gs->SetMatrix(N_MXM_MODELVIEW,ivm);
            co->VisualizeGlobal(gs);
        }
        rs.Set(N_RS_ZBIAS,0.0f); gs->SetState(rs);
    }
}

//-------------------------------------------------------------------
/**
    Do an instant check of a moving sphere in the collision
    volume. Fills out the provided collide report array and
    returns number of detected collisions.

    @param p0     [in] starting position
    @param v0     [in] vector to ending position
    @param radius [in] radius
    @param cc     [in] collision class for collision type query
    @param cr_ptr [out] pointer to array of pointers to nCollideReport's
    @return       number of detected contacts (1 per collide object)

     - 25-Sep-00   floh    created
     - 26-Sep-00   floh    + now reuses collision report handler
     - 08-Nov-00   floh    + bugfix: collision check now more robust,
                             if one volume was completely inside another,
                             it could happen that the collision was
                             not reported.
*/
//-------------------------------------------------------------------
int nCollideContext::MovingSphereCheck(const vector3& p0,
                                       const vector3& v0,
                                       float radius,
                                       nCollClass cc,
                                       nCollideReport **& cr_ptr)
{
    // create a bounding box from the start and end position
    vector3 p0v0(p0+v0);
    vector3 minv(n_min(p0.x,p0v0.x)-radius, 
                 n_min(p0.y,p0v0.y)-radius, 
                 n_min(p0.z,p0v0.z)-radius);
    vector3 maxv(n_max(p0.x,p0v0.x)+radius, 
                 n_max(p0.y,p0v0.y)+radius, 
                 n_max(p0.z,p0v0.z)+radius);

    const int own_id = 0xffff;

    // initialize collision report handler
    this->checkReportHandler.BeginFrame();

    // This simply goes through all attached objects, and
    // checks them for overlap, so ITS SLOW! Every object is
    // tested exactly once
    nNode *context_node;
    for (context_node = this->attached_list.GetHead();
         context_node;
         context_node = context_node->GetSucc())
    {
        nCollideObject *other = (nCollideObject *) context_node->GetPtr();

        // see if we have overlaps in all 3 dimensions
        if ((minv.x < other->maxv.x) && (maxv.x > other->minv.x) &&
            (minv.y < other->maxv.y) && (maxv.y > other->minv.y) &&
            (minv.z < other->maxv.z) && (maxv.z > other->minv.z))
        {
            // see if the candidate is in the ignore types set
            nCollType ct = this->cs->QueryCollType(cc,other->GetCollClass());
            if (COLLTYPE_IGNORE == ct) continue;

            vector3 p1(other->old_matrix.pos_component());
            vector3 v1(other->new_matrix.pos_component() - p1);

            // do the contact check between 2 moving spheres
            sphere s0(p0,radius);
            sphere s1(p1,other->radius);
            float u0,u1;
            if (s0.intersect_sweep(v0,s1,v1,u0,u1)) {
                if ((u0>=0.0f) && (u0<1.0f)) {

                    // we have contact!

                    // compute the 2 midpoints at the time of collision
                    vector3 c0(p0 + v0*u0);
                    vector3 c1(p1 + v1*u0);
                
                    // compute the collide normal
                    vector3 d(c1-c0);
                    if (d.len() > TINY) {
                        d.norm();
                    } else {
                        d.set(0.0f, 1.0f, 0.0f);
                    }

                    // fill out a collide report and add to report handler
                    nCollideReport cr;
                    cr.co1     = other;
                    cr.co2     = other;
                    cr.tstamp  = 0.0;
                    cr.contact = (d*radius) + c0;
                    cr.co1_normal = d;
                    cr.co2_normal = -d;
                    this->checkReportHandler.AddCollission(cr,own_id,other->id);
                }
            }
        }
    }
    this->checkReportHandler.EndFrame();
    return this->checkReportHandler.GetAllCollissions(cr_ptr);
}

//------------------------------------------------------------------------------
/**
    Test a line against the collide objects in the collide context.
    The collType will be interpreted as follows:

      - COLLTYPE_IGNORE:        illegal (makes no sense)
      - COLLTYPE_QUICK:         occlusion check only
      - COLLTYPE_CONTACT:       return closest contact only
      - COLLTYPE_EXACT:         return all contacts (unsorted)

    @param  line        [in]  the line to test in global space
    @param  collType    [in]  the collision type
    @param  collClass   [in]  optional coll class (COLLCLASS_ALWAYS_* if no coll class filtering wanted)
    @param  crPtr       [out] will be filled with pointer to collide report pointers
    @return             number of detected contacts (1 per collide object)
*/
int
nCollideContext::LineCheck(const line3& line, 
                           nCollType collType, 
                           nCollClass collClass,
                           nCollideReport**& crPtr)
{
    n_assert(collType != COLLTYPE_IGNORE);

    // create a bounding box from the line
    bbox3 bbox;
    bbox.begin_grow();
    bbox.grow(line.start());
    bbox.grow(line.end());
    const int ownId = 0xffff;

    // initialize collision report handler
    this->checkReportHandler.BeginFrame();

    // go through all attached collide objects
    nNode *contextNode;
    for (contextNode = this->attached_list.GetHead();
         contextNode;
         contextNode = contextNode->GetSucc())
    {
        nCollideObject *co = (nCollideObject *) contextNode->GetPtr();

        // see if we have overlaps in all 3 dimensions
        if ((bbox.vmin.x < co->maxv.x) && (bbox.vmax.x > co->minv.x) &&
            (bbox.vmin.y < co->maxv.y) && (bbox.vmax.y > co->minv.y) &&
            (bbox.vmin.z < co->maxv.z) && (bbox.vmax.z > co->minv.z))
        {

            // see if the candidate is in the ignore types set
            nCollType ct = this->cs->QueryCollType(collClass, co->GetCollClass());
            if (COLLTYPE_IGNORE == ct) 
            {
                continue;
            }

            // check collision
            nCollideShape* shape = co->GetShape();
            if (shape)
            {
                nCollideReport cr;
                if (shape->LineCheck(collType, co->GetTransform(), line, cr))
                {
                    cr.co1 = co;
                    cr.co2 = co;
                    this->checkReportHandler.AddCollission(cr, ownId, co->id);
                    if (COLLTYPE_QUICK == collType)
                    {
                        // break out of loop
                        break;
                    }
                }
            }
        }
    }
    this->checkReportHandler.EndFrame();
    
    if (COLLTYPE_CONTACT == collType)
    {
        // get closest contact only
        return this->checkReportHandler.GetClosestCollission(line.start(), crPtr);
    }
    else
    {
        // get all contacts (unsorted)
        return this->checkReportHandler.GetAllCollissions(crPtr);
    }
}

//------------------------------------------------------------------------------
/**
    Test a sphere against the collide objects in the collide context.
    The collType will be interpreted as follows:

     - COLLTYPE_IGNORE:        illegal (makes no sense)
     - COLLTYPE_QUICK:         return all contacts, do sphere-sphere check
     - COLLTYPE_CONTACT:       return closest contact only, sphere-shape
     - COLLTYPE_EXACT:         return all contacts (unsorted), sphere-shape

    @param  sphere      [in]  the sphere to test in global space
    @param  collType    [in]  the collission type
    @param  collClass   [in]  optional coll class (COLLCLASS_ALWAYS_* if no coll class filtering wanted)
    @param  crPtr       [out] will be filled with pointer to collide report pointers
    @return             number of detected contacts (1 per collide object)
*/
int
nCollideContext::SphereCheck(const sphere& ball,
                             nCollType collType,
                             nCollClass collClass,
                             nCollideReport**& crPtr)
{
    n_assert(collType != COLLTYPE_IGNORE);

    // create a bounding box from the sphere
    vector3 vmin(ball.p.x - ball.r, ball.p.y - ball.r, ball.p.z - ball.r);
    vector3 vmax(ball.p.x + ball.r, ball.p.y + ball.r, ball.p.z + ball.r);
    bbox3 bbox(vmin, vmax);
    const int ownId = 0xffff;

    // initialize collission report handler
    this->checkReportHandler.BeginFrame();

    // go through all attached collide objects
    sphere s0;
    nNode *contextNode;
    for (contextNode = this->attached_list.GetHead();
         contextNode;
         contextNode = contextNode->GetSucc())
    {
        nCollideObject *co = (nCollideObject *) contextNode->GetPtr();

        // see if we have overlaps in all 3 dimensions
        if ((bbox.vmin.x < co->maxv.x) && (bbox.vmax.x > co->minv.x) &&
            (bbox.vmin.y < co->maxv.y) && (bbox.vmax.y > co->minv.y) &&
            (bbox.vmin.z < co->maxv.z) && (bbox.vmax.z > co->minv.z))
        {
            // see if the candidate is in the ignore types set
            nCollType ct = this->cs->QueryCollType(collClass, co->GetCollClass());
            if (COLLTYPE_IGNORE == ct)
            {
                continue;
            }

            if (COLLTYPE_QUICK == ct)
            {
                // do sphere-sphere collision check
                const matrix44 coTrans = co->GetTransform();
                s0.set(coTrans.M41, coTrans.M42, coTrans.M43, co->GetRadius());
                if (ball.intersects(s0))
                {
                    nCollideReport cr;
                    cr.co1 = co;
                    cr.co2 = co;
                    this->checkReportHandler.AddCollission(cr, ownId, co->id);
                }
            }
            else
            {
                // do sphere-shape collision check
                nCollideShape* shape = co->GetShape();
                if (shape)
                {
                    nCollideReport cr;
                    if (shape->SphereCheck(collType, co->GetTransform(), ball, cr))
                    {
                        cr.co1 = co;
                        cr.co2 = co;
                        this->checkReportHandler.AddCollission(cr, ownId, co->id);
                    }
                }
            }
        }
    }
    this->checkReportHandler.EndFrame();

    if (COLLTYPE_CONTACT == collType)
    {
        // get closest contact only
        return this->checkReportHandler.GetClosestCollission(ball.p, crPtr);
    }
    else
    {
        // get all contacts (unsorted)
        return this->checkReportHandler.GetAllCollissions(crPtr);
    }
}

//------------------------------------------------------------------------------
/**
    Reset position and timestamp of all attached collide objects to 0.0.
    This is useful at the beginning of a level to prevent phantom collissions
    (when objects are repositioned to their starting point in the level).
*/
void
nCollideContext::Reset()
{
    n_printf("*** nCollideContext::Reset() called\n");

    matrix44 identity;
    nNode *context_node;
    for (context_node = this->attached_list.GetHead();
         context_node;
         context_node = context_node->GetSucc())
    {
        nCollideObject *co = (nCollideObject *) context_node->GetPtr();
        // This must be done twice, so that old timestamp also becomes 0
        co->Transform(0.0, identity);
        co->Transform(0.0, identity);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
