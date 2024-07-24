#ifndef N_COLLIDEOBJECT_H
#define N_COLLIDEOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nCollideObject
    @ingroup NebulaCollideModule
    @brief collision system object

    nCollideObject is an actual collision system object which can
    be positioned and oriented in space. It points to an
    nCollideShape which describes the actual shape of the
    object.

    nCollideObjects are kept in sorted list (one for each dimension)
    by the nCollideContext they belong to.
*/

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_NODE_H
#include "util/nnode.h"
#endif

#ifndef N_MATHLIB_H
#include "mathlib/matrix.h"
#endif

#ifndef N_MATHLIB_H
#include "mathlib/sphere.h"
#endif

#ifndef N_COLLIDESERVER_H
#include "collide/ncollideserver.h"
#endif

#ifndef N_COLLIDECONTEXT_H
#include "collide/ncollidecontext.h"
#endif

#ifndef N_COLLIDESHAPE_H
#include "collide/ncollideshape.h"
#endif

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

//------------------------------------------------------------------------------
/**
    @class nCNode
    @ingroup NebulaCollideModule

    A node in a sorted 1-dimensional collision list. Keeps
    a value (x,y or z coordinate of the node) and a back-pointer
    to its nCollideObject.
*/
//------------------------------------------------------------------------------
class nCollideObject;
class nCNode : public nNode {
    float val;

private:
    /**
        Sorting of nodes

        Make sure that the node is still sorted correctly into the
        linked list. Move node forward/backward in the list
        as far as necessary.

        - 26-Jun-00   floh    created
    */
    void sort(void) {
        n_assert(this->IsLinked());
        nCNode *n;

        if ((n = (nCNode *) this->GetPred()) && (n->val > this->val)) {
            // move towards head of list
            do {
                this->Remove();
                this->InsertBefore(n);
            } while ((n = (nCNode *) this->GetPred()) && (n->val > this->val));

        } else if ((n = (nCNode *) this->GetSucc()) && (n->val < this->val)) {

            // move towards tail of list?
            do {
                this->Remove();
                this->InsertAfter(n);
            } while ((n = (nCNode *) this->GetSucc()) && (n->val < this->val));
        }
    };

public:
    nCNode() : val(0.0f) {};
    void SetCollideObject(nCollideObject *co) {
        this->SetPtr(co);
    };
    nCollideObject *GetCollideObject(void) {
        return (nCollideObject *) this->GetPtr();
    };
    /**
        Also makes automatically sure that the node keeps sorted
        correctly in its list.

        - 26-Jun-00   floh    created
    */
    void SetVal(float v) {
        val = v;
        this->sort();
    };
    float GetVal(void) {
        return val;
    };

    /**
        Sort into a list.
    
        - 26-Jun-00   floh    created
    */
    void AddToList(nList& l) {
        l.AddHead(this);
        this->sort();
    };
};

//-------------------------------------------------------------------
class nCollideContext;
class nCollideReport;
class N_PUBLIC nCollideObject : public nNode 
{
    friend class nCollideContext;

protected:
    int id;                     // a unique 32 bit id for this object
    nCollideContext *context;   // the collide context this object is currently attached to
    nCollideServer  *collide_server;    // the almighty collide server
    nNode context_node;         // attached to context with this node 

    float radius;               // radius of the collision object (normally provided by shape)
    nCollideShape *shape;       // the triangle exact collision shape (optional)
    nCNode xmin_cnode;          // the min/max collision node in the X-Dimension
    nCNode xmax_cnode;
    nCollClass coll_class;      // the application defined collision type

    vector3 minv;               // the min/max coordinates in each dimension
    vector3 maxv;

    matrix44 old_matrix;        // the previous position/orientation of the object
    matrix44 new_matrix;        // the new position/orientation of the object
    double old_tstamp;          // the timestamp for 'old_transform'
    double new_tstamp;          // the timestamp for 'new_transform'

    void *client_data;          // user defined client data field
    bool is_attached;           // currently attached to a context

    int num_colls;              // number of collisions this object is involved in
    vector4 visColor;           // color for visualization

public:
    nCollideObject() 
        : context(NULL),
          collide_server(NULL),
          radius(0.0f),
          shape(NULL),
          coll_class(0),
          old_tstamp(0.0),
          new_tstamp(0.0),
          client_data(NULL),
          is_attached(false),
          num_colls(0),
          visColor(0.0f, 0.0f, 1.0f, 1.0f)
    {
        context_node.SetPtr(this);
        xmin_cnode.SetCollideObject(this);
        xmax_cnode.SetCollideObject(this);
    };

    //--- set/get visulization color ---
    void SetVisualizeColor(const vector4& color)
    {
        this->visColor = color;
    }
    const vector4& GetVisualizeColor()
    {
        return this->visColor;
    }

    //--- set/get attributes ---
    void SetContext(nCollideContext *c) {
        // c may be NULL!!!
        this->context = c;
        if (c) {
            this->collide_server = c->GetCollideServer();
        } else {
            this->collide_server = NULL;
        }
    };
    nCollideContext *GetContext(void) {
        return this->context;
    };

    //---------------------------------------------------------------
    void SetId(int i) {
        this->id = i;
    };
    int GetId(void) {
        return this->id;
    };
    //---------------------------------------------------------------
    void SetAttached(bool b) {
        this->is_attached = b;
    };
    bool IsAttached(void) {
        return this->is_attached;
    };
    //---------------------------------------------------------------
    void SetRadius(float f) {
        this->radius = f;
    };
    float GetRadius(void) {
        return this->radius;
    };
    //---------------------------------------------------------------
    void SetShape(nCollideShape *s) {
        this->shape = s;
        if (s)
        {
            this->SetRadius(s->GetRadius());
        }
        else
        {
            this->SetRadius(0.0f);
        }
    };
    nCollideShape *GetShape(void) {
        return this->shape;
    };
    //---------------------------------------------------------------
    void SetCollClass(nCollClass cc) {
        this->coll_class = cc;
    };
    nCollClass GetCollClass(void) {
        return this->coll_class;
    };
    //---------------------------------------------------------------
    void SetClientData(void *d) {
        this->client_data = d;
    };
    void *GetClientData(void) {
        return this->client_data;
    };
    //---------------------------------------------------------------
    const matrix44& GetTransform(void) {
        return this->new_matrix;
    };
    const matrix44& GetPrevTransform(void) {
        return this->old_matrix;
    };
    double GetTimeStamp(void) {
        return this->new_tstamp;
    };
    double GetPrevTimeStamp(void) {
        return this->old_tstamp;
    };
    
    //---------------------------------------------------------------
    void ClearCollissions(void) {
        this->num_colls = 0;
    };
    int GetNumCollissions(void) {
        return this->num_colls;
    };

    /**
        Transform the object to its new position/orientation, update
        the dimensional nodes and the bounding box.
    
        - 26-Jun-00   floh      created
        - 19-Jul-00   floh    bounding box now covers the range
                                of motion between the last position
                                and the current position
        - 14-Oct-00   floh    added the max velocity position
                                exception (this fixes the
                    position-set problem at the case
                    of an collision if an object
                                        travels at near-infinity speed (it
                    will basically be reset to its
                    previous pos).
                - 04-Mar-02   floh      removed max velocity position
                                exception, instead of doing a position
                    exception the maximum number of
                    intermediate tests is now capped
    */
    void Transform(double t, const matrix44& m) {
        n_assert(this->is_attached);

        // if old_matrix and old_tstamp are not yet valid,
        // they will be initialized with the current
        // values, to prevent "startup popping"
        if (this->old_tstamp == 0.0) {
            this->old_matrix = m;
            this->old_tstamp = t;
       } else if (this->old_tstamp == t) {
            this->old_matrix = m;
            this->old_tstamp = t;
       } else {
            this->old_matrix = this->new_matrix;
            this->old_tstamp = this->new_tstamp;
        }
        this->new_matrix = m;
        this->new_tstamp = t;
        
        // update the bounding-box
        vector3 p0(this->old_matrix.pos_component());
        vector3 p1(this->new_matrix.pos_component());
        this->minv.set(n_min(p0.x,p1.x)-this->radius, 
                       n_min(p0.y,p1.y)-this->radius, 
                       n_min(p0.z,p1.z)-this->radius);
        this->maxv.set(n_max(p0.x,p1.x)+this->radius, 
                       n_max(p0.y,p1.y)+this->radius, 
                       n_max(p0.z,p1.z)+this->radius);

        // update the x-dimension node, nCNode::SetVal() automatically
        // makes sure that the nodes keep their correct orders
        // in the list
        this->xmin_cnode.SetVal(this->minv.x);
        this->xmax_cnode.SetVal(this->maxv.x);
    };

    /**
        Check whether 2 moving collide objects have contact.

        - 08-Jul-00   floh    created
    */
    bool Contact(nCollideObject *other,     // the other object
                 nCollType ct,
                 nCollideReport& cr)
    {
        vector3 p0(this->old_matrix.pos_component());
        vector3 p1(other->old_matrix.pos_component());
        vector3 v0(this->new_matrix.pos_component()  - p0);
        vector3 v1(other->new_matrix.pos_component() - p1);

        bool has_contact = false;
        switch (ct) {
            case COLLTYPE_QUICK:
                {
                    // do a contact check between 'moving spheres'
                    sphere s0(p0,this->radius);
                    sphere s1(p1,other->radius);
                    float u0,u1;
                    if (s0.intersect_sweep(v0,s1,v1,u0,u1)) {
                        // there may be a valid contact somewhere along the path
                        if ((u0>=0.0f) && (u0<1.0f)) {
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

                            // compute the contact point
                            cr.contact = (d*this->radius) + c0;

                            // compute the collide normals
                            cr.co1_normal = d;
                            cr.co2_normal = -d;

                            // compute the timestamp where the collision happended
                            cr.tstamp = this->old_tstamp + (this->new_tstamp-this->old_tstamp)*u0;
                            has_contact = true;
                        }
                    }
                }
                break;

            case COLLTYPE_EXACT:
            case COLLTYPE_CONTACT:
                {
                    // If distance travelled is more then 1/8 then each of the object's
                    // radii, then we do several tests along the line
                    // of movements.
                    float rq0 = this->radius * 0.125f;
                    float rq1 = other->radius * 0.125f;
                    float v0_len = v0.len();
                    float v1_len = v1.len();
                    int num = (int) n_max((v0_len/rq0), (v1_len/rq1));
                    const int maxChecks = 16;
                    if (num == 0) {
                        num = 1;
                    } else if (num > maxChecks) {
                        num = maxChecks;
                    }
                    vector3 d0(v0 / float(num));
                    vector3 d1(v1 / float(num));
                    matrix44 self_matrix = this->old_matrix;
                    matrix44 other_matrix = other->old_matrix;
                    int i;

                    for (i=0; i<num; i++) {
                        p0 += d0;
                        p1 += d1;
                        self_matrix.M41 = p0.x;
                        self_matrix.M42 = p0.y;
                        self_matrix.M43 = p0.z;
                        other_matrix.M41 = p1.x;
                        other_matrix.M42 = p1.y;
                        other_matrix.M43 = p1.z;
                        if (this->shape->Collide(ct, self_matrix, other->shape, other_matrix, cr)) {
                            // CONTACT!!!
                            double dt = (this->new_tstamp - this->old_tstamp) / num;
                            cr.tstamp = this->old_tstamp + dt*i;
                            return true;
                        }
                    }
                }
                break;  

            default:
                break;
        }
        return has_contact;
    };

    /**
        For each overlapping object in all 3 dimensions, 
        which doesn't fall into the ignore_types category,
        do a collision check, and if the check is positive,
        record collision by doing an AddCollission().

        - 26-Jun-00   floh    created
    */
    void Collide(void) {
        n_assert(this->is_attached);
        n_assert(this->context);

        nCollideReportHandler *crh = &(this->context->collideReportHandler);

        // for each overlapping object in the X dimension...
        nCNode *cnx = &(this->xmin_cnode);
        while ((cnx = (nCNode *) cnx->GetSucc()) &&
               (cnx != &(this->xmax_cnode)))
        {
            // is the candidate in the ignore types set?
            nCollideObject *other = cnx->GetCollideObject();
            
            // query the collision type defined for those two objects
            nCollType ct = this->collide_server->QueryCollType(this->GetCollClass(),other->GetCollClass());
            
            // ignore collision?
            if (COLLTYPE_IGNORE == ct) continue;

            // nope, see if it overlaps in the other 2 dimensions as well...
            if ((!((other->maxv.y<this->minv.y) || (other->minv.y>this->maxv.y))) &&
                (!((other->maxv.z<this->minv.z) || (other->minv.z>this->maxv.z))))
            {
                // we have an overlap, mister 
                
                // has this collision already been detected by the
                // other object?
                if (!crh->CollissionExists(this->id,other->id)) {
                    // no, we're first...

                    // ask objects whether they collide...
                    // FIXME: probably do velocity-based finer
                    // grained control here ?!?!?!
                    nCollideReport cr;
                    if (this->Contact(other,ct,cr)) {
                        cr.co1 = this;
                        cr.co2 = other;
                        crh->AddCollission(cr,this->id,other->id);
                        this->num_colls++;
                        other->num_colls++;
                    }
                }
            }
        }
    };

    /**
        Return collision reports for all collisions this
        object is involved in.

        - 27-Jun-00   floh    created
    */
    int GetCollissions(nCollideReport **&crp) {
        n_assert(this->context);
        n_assert(this->is_attached);
        return this->context->collideReportHandler.GetCollissions(this,crp);
    };

    /**
        Visualize stuff in local coordinate space.

        - 27-Jun-00   floh    created
        - 10-Sep-00   floh    oops, there was a missing gs->End()
    */
    void VisualizeLocal(nGfxServer *gs) {
        n_assert(gs);
        gs->Begin(N_PTYPE_LINE_LIST);
        if (this->num_colls > 0) 
        {
            gs->Rgba(0.0f,0.7f,0.0f,1.0f);
        }
        else
        {
            gs->Rgba(this->visColor.x, this->visColor.y, this->visColor.z, this->visColor.w);
        }

        // render the objects radii
        int dim;
        float dr = n_deg2rad(5.0f);
        for (dim=0; dim<3; dim++) {
            float r;
            for (r=0.0f; r<n_deg2rad(360.0f); r+=dr) {
                float sin_r0 = (float) n_sin(r);
                float cos_r0 = (float) n_cos(r);
                float sin_r1 = (float) n_sin(r+dr);
                float cos_r1 = (float) n_cos(r+dr);
                vector3 v0_x(0.0f, sin_r0*this->radius, cos_r0*this->radius);
                vector3 v1_x(0.0f, sin_r1*this->radius, cos_r1*this->radius);
                vector3 v0_y(sin_r0*this->radius, 0.0f, cos_r0*this->radius);
                vector3 v1_y(sin_r1*this->radius, 0.0f, cos_r1*this->radius);
                vector3 v0_z(sin_r0*this->radius, cos_r0*this->radius, 0.0f);
                vector3 v1_z(sin_r1*this->radius, cos_r1*this->radius, 0.0f);
                gs->Coord(v0_x.x,v0_x.y,v0_x.z); gs->Coord(v1_x.x,v1_x.y,v1_x.z);
                gs->Coord(v0_y.x,v0_y.y,v0_y.z); gs->Coord(v1_y.x,v1_y.y,v1_y.z);
                gs->Coord(v0_z.x,v0_z.y,v0_z.z); gs->Coord(v1_z.x,v1_z.y,v1_z.z);
            }
        }
        gs->End();

        // optionally, render the object's shape
        if (this->shape) {
            this->shape->Visualize(gs);
        }
    };

    /**
        Visualize stuff in global space.
    
        - 04-Jul-00   floh    created
    */
    void VisualizeGlobal(nGfxServer *gs) {
        n_assert(gs);

        gs->Begin(N_PTYPE_LINE_LIST);
        
        // render any collision contact points
        if (this->num_colls > 0) {
            nCollideReport **pcr;
            int num = this->context->GetCollissions(this,pcr);
            int i;
            for (i=0; i<num; i++) {
                nCollideReport *cr = pcr[i];
                vector3& cnt = cr->contact;
                gs->Rgba(1.0f,1.0f,0.0f,1.0f);
                gs->Coord(cnt.x-0.5f,cnt.y,cnt.z); gs->Coord(cnt.x+0.5f,cnt.y,cnt.z);
                gs->Coord(cnt.x,cnt.y-0.5f,cnt.z); gs->Coord(cnt.x,cnt.y+0.5f,cnt.z);
                gs->Coord(cnt.x,cnt.y,cnt.z-0.5f); gs->Coord(cnt.x,cnt.y,cnt.z+0.5f);
                vector3& n = cr->co1_normal;
                gs->Rgba(1.0f,0.0f,0.0f,1.0f);
                gs->Coord(cnt.x,cnt.y,cnt.z); gs->Coord(cnt.x+n.x,cnt.y+n.y,cnt.z+n.z);
                n = cr->co2_normal;
                gs->Rgba(1.0f,0.0f,0.0f,1.0f);
                gs->Coord(cnt.x,cnt.y,cnt.z); gs->Coord(cnt.x+n.x,cnt.y+n.y,cnt.z+n.z);
            }
        }

        // render the objects bounding boxes (stretched by their movement)
        gs->Rgba(this->visColor.x, this->visColor.y, this->visColor.z, this->visColor.w);
        vector3& v0 = this->minv;
        vector3& v1 = this->maxv;
        gs->Coord(v0.x,v0.y,v0.z); gs->Coord(v1.x,v0.y,v0.z);
        gs->Coord(v1.x,v0.y,v0.z); gs->Coord(v1.x,v1.y,v0.z);
        gs->Coord(v1.x,v1.y,v0.z); gs->Coord(v0.x,v1.y,v0.z);
        gs->Coord(v0.x,v1.y,v0.z); gs->Coord(v0.x,v0.y,v0.z);
        gs->Coord(v0.x,v0.y,v1.z); gs->Coord(v1.x,v0.y,v1.z);
        gs->Coord(v1.x,v0.y,v1.z); gs->Coord(v1.x,v1.y,v1.z);
        gs->Coord(v1.x,v1.y,v1.z); gs->Coord(v0.x,v1.y,v1.z);
        gs->Coord(v0.x,v1.y,v1.z); gs->Coord(v0.x,v0.y,v1.z);
        gs->Coord(v0.x,v0.y,v0.z); gs->Coord(v0.x,v0.y,v1.z);
        gs->Coord(v1.x,v0.y,v0.z); gs->Coord(v1.x,v0.y,v1.z);
        gs->Coord(v1.x,v1.y,v0.z); gs->Coord(v1.x,v1.y,v1.z);
        gs->Coord(v0.x,v1.y,v0.z); gs->Coord(v0.x,v1.y,v1.z);
        gs->End();
    };
};
//-------------------------------------------------------------------
#endif
