#ifndef N_WAVEFRONT_H
#define N_WAVEFRONT_H
//--------------------------------------------------------------------
/**
    @class nWavefront

    @brief einfache Klasse zum Laden/Manipulieren/Saven von Wavefront-Files

    Damit nicht jedes Tool seinen eigenen Wavefront-Loader/Saver
    hat, hier eine Klasse, die ein Wavefront-Objekt kapselt
    und manipuliert.
*/
//--------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_NODE_H
#include "util/nnode.h"
#endif

//--------------------------------------------------------------------
class nWavefront;
class nFace;

//--------------------------------------------------------------------
//  Index Liste
//--------------------------------------------------------------------
class nIList : public nList {
public:
    nNode *NodeAt(long);
    long IndexOf(nNode *);
};

//--------------------------------------------------------------------
// ein Vertex
//--------------------------------------------------------------------
class nV : public nNode {
public:
    long use_count;
    float x,y,z;

    nV(nIList *l) {
        use_count=0;
        x=0.0f; y=0.0f; z=0.0f;
        l->AddTail(this);
    };
    nV(nIList *l, float _x, float _y, float _z) {
        use_count=0;
        x=_x; y=_y; z=_z;
        l->AddTail(this);
    };
    ~nV() {
        n_assert(use_count==0);
        this->Remove();
    };
    
    bool IsEqual(nV *v, float tol) {
        if ((fabs(v->x-x)<=tol) &&
            (fabs(v->y-y)<=tol) &&
            (fabs(v->z-z)<=tol))
        {
            return true;
        } else {
            return false;
        }
    };
    bool Print(FILE *fp) {
        fprintf(fp,"v %f %f %f\n",x,y,z);
        return true;
    };
};

//--------------------------------------------------------------------
// eine Normale
//--------------------------------------------------------------------
class nVN : public nNode {
public:    
    long use_count;
    float nx,ny,nz;
    nVN(nIList *l) {
        use_count = 0;
        nx=0.0f; ny=0.0f; nz=0.0f;
        l->AddTail(this);
    };
    nVN(nIList *l, float _nx, float _ny, float _nz) {
        use_count = 0;
        nx=_nx; ny=_ny; nz=_nz;
        l->AddTail(this);
    };
    ~nVN() {
        n_assert(use_count==0);
        this->Remove();
    }
    bool IsEqual(nVN *vn, float tol) {
        if ((fabs(vn->nx-nx)<=tol) &&
            (fabs(vn->ny-ny)<=tol) &&
            (fabs(vn->nz-nz)<=tol))
        {
            return true;
        } else {
            return false;
        }
    };    
    bool Print(FILE *fp) {
        fprintf(fp,"vn %f %f %f\n",nx,ny,nz);
        return true;
    };
};

//--------------------------------------------------------------------
// eine UV-Koordinate
//--------------------------------------------------------------------
class nVT : public nNode {
public:
    long use_count;    
    float u,v;
    nVT(nIList *l) {
        use_count = 0;        
        u=0.0f; v=0.0f;
        l->AddTail(this);
    };
    nVT(nIList *l, float _u, float _v) {
        use_count = 0;
        u=_u; v=_v;
        l->AddTail(this);
    };
    ~nVT() {
        n_assert(use_count == 0);
        this->Remove();
    };         
    bool IsEqual(nVT *vt, float tol) {
        if ((fabs(vt->u-u)<=tol) &&
            (fabs(vt->v-v)<=tol))
        {
            return true;
        } else {
            return false;
        }
    };
    bool Print(FILE *fp) {
        fprintf(fp,"vt %f %f\n",u,v);
        return true;
    };
};

//--------------------------------------------------------------------
// eine RGBA-Koordinate
//--------------------------------------------------------------------
class nRgba : public nNode {
public:
    long use_count;    
    float r,g,b,a;
    nRgba(nIList *l) {
        use_count = 0;        
        r=0.0f; g=0.0f; b=0.0f; a=0.0f;
        l->AddTail(this);
    };
    nRgba(nIList *l, float _r, float _g, float _b, float _a) {
        use_count = 0;
        r=_r; g=_g; b=_b; a=_a;
        l->AddTail(this);
    };
    ~nRgba() {
        n_assert(use_count == 0);
        this->Remove();
    };         
    bool IsEqual(nRgba *rgba, float tol) {
        if ((fabs(rgba->r-r)<=tol) &&
            (fabs(rgba->g-g)<=tol) &&
            (fabs(rgba->b-b)<=tol) &&
            (fabs(rgba->a-a)<=tol))
        {
            return true;
        } else {
            return false;
        }
    };
    bool Print(FILE *fp) {
        fprintf(fp,"rgba %f %f %f %f\n",r,g,b,a);
        return true;
    };
};

//--------------------------------------------------------------------
// ein "Eckpunkt" mit "Indices" in die Attribute-Arrays
//--------------------------------------------------------------------
class nPoint : public nNode {
public:
    nWavefront *wf;
    nFace *f;
    nV  *v;
    nVN *vn;
    nVT *vt;
    nRgba *rgba;
    nPoint(nWavefront *, nFace *, nV *, nVN *, nVT *, nRgba *);
    ~nPoint();
    inline bool IsEqual(nPoint *p) {
        if ((p->v  == v)  &&
            (p->vn == vn) &&
            (p->vt == vt) &&
            (p->rgba == rgba))
        {
            return true;
        } else {
            return false;
        }
    }; 
    bool Print(FILE *);
    bool PrintAsTriStrip(FILE *);
};

//--------------------------------------------------------------------
// eine Face-Definition
//--------------------------------------------------------------------
class nFace : public nNode {
public:
    enum nFaceType {
        N_FACE,             // Face ist eine Standard-Wavefront-Definition
        N_TRISTRIP,         // Face ist eine Tristrip-Definition
    };

    nFaceType type;         
    float nx,ny,nz;         // die Flaechen-Normalen, werden initialisiert 
                            // durch ein GenFaceNormals() (nicht bei Tristrips!)    
    nWavefront *wf;
    nIList points;
    nFace(nWavefront *);
    ~nFace();
    void SetType(nFaceType t);
    void AddPoint(nV *, nVN *, nVT *, nRgba *);
    long NumPoints(void);
    long SharedPoints(nFace *);
    bool Print(FILE *);
    bool PrintAsFN(FILE *);
};

//--------------------------------------------------------------------
//  WingedEdge (we)
//  Zeigt sowohl auf die beiden angrenzenden Flaechen, als auch
//  auf die Eckpunkt-Vertices.
//--------------------------------------------------------------------
class nWingedEdge : public nNode {
public:
    nFace *f0, *f1;
    nV *v0, *v1;
    nWavefront *wf;
    nWingedEdge(nWavefront *, nIList *, nV *, nV *, nFace *, nFace *);
    ~nWingedEdge();
    bool Print(FILE *fp);
};

//--------------------------------------------------------------------
// ein komplettes Wavefront-Objekt
//--------------------------------------------------------------------
class nWavefront {
public:
    nIList v_list;
    nIList vn_list;
    nIList vt_list;
    nIList rgba_list;
    nIList face_list;
    nIList we_list;

    nWavefront();
    ~nWavefront();
    
    nV    *AddV(float, float, float);
    nVN   *AddVN(float, float, float);
    nVT   *AddVT(float, float);
    nRgba *AddRgba(float, float, float, float);
    nFace *AddFace(void);
    nWingedEdge *AddWingedEdge(nV *, nV *, nFace *, nFace *);

    void GenFaceNormals(void);
    
    nPoint *GetFirstPoint(void);
    nPoint *GetNextPoint(nPoint *);
    
    nPoint *NextPointV(nV *, nPoint *);
    nPoint *NextPointVN(nVN *, nPoint *);
    nPoint *NextPointVT(nVT *, nPoint *);
    nPoint *NextPointRgba(nRgba *, nPoint *);
    
    long NumV(void);
    long NumVN(void);
    long NumVT(void);
    long NumRgba(void);
    long NumFaces(void);        // kann auch NumTriStrips sein...
    long NumWingedEdges(void);
    
    bool Load(FILE *);
    bool SaveV(FILE *);
    bool SaveVN(FILE *);
    bool SaveVT(FILE *);
    bool SaveRgba(FILE *);
    bool SaveFaces(FILE *);
    bool SaveFacesAsFN(FILE *);
    bool SaveWingedEdges(FILE *);
    bool Save(FILE *);
};
//--------------------------------------------------------------------
#endif
