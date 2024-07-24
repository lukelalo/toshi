#ifndef N_WFTOOLBOX_H
#define N_WFTOOLBOX_H
//-------------------------------------------------------------------
/**
    @class wfToolbox

    @brief define common operations on wfObject
*/
//-------------------------------------------------------------------

#ifndef N_WFOBJECT_H
#include "tools/wfobject.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_TRIANGLE_H
#include "mathlib/triangle.h"
#endif

//-------------------------------------------------------------------
class wfToolbox {

    // needed by smooth(): contains for each vertex a list
    // of face indices which reference this vertex
    nArray< nArray<int> > connect_array;

    // needed by smooth(): the face normals for each vertex
    nArray<vector3> face_normals;

public:
//-------------------------------------------------------------------
//  clean()
//  Remove any redundant data from src object and write to
//  target object, should also fix most modeling artefacts.
//  For each vertex component, a mapping table is defined
//  which maps source vertices to destination vertices.
//  24-Oct-00   floh    created
//  07-Mar-01   floh    + if some face points don't have texture
//                        coordinates assigned, they will get
//                        the first texture coordinate
//-------------------------------------------------------------------
void clean(wfObject& src,   // source object (complete)
           wfObject& dst,   // dest object (empty)
           float vtol,      // tolerance for coordinates
           float vntol,     // tolerance for normals
           float vttol,     // tolerance for texture coordinates
           float ctol)      // tolerance for colors
{
    // check v components for redundancies
    nArray<int> v_maptable(5000, 5000);
    nArray<wfCoord>::iterator v0;
    for (v0=src.v_array.Begin(); v0!=src.v_array.End(); v0++) {

        // see if the component already exists in the target
        nArray<wfCoord>::iterator v1;
        int i1;
        bool exists = false;
        for (i1=0,v1=dst.v_array.Begin(); v1!=dst.v_array.End(); i1++,v1++) {
            if (v1->v.isequal(v0->v,vtol)) {
                // v1 is a copy of v0, break!
                exists = true;
                break;
            }
        }

        // push the vertex index on the mapping table, if it was
        // not redundant, the last loop fell through, otherwise,
        // it broke on the redundant vertex
        v_maptable.PushBack(i1);

        // if it was a new unique vertex, we will have to push
        // it into the dest component array
        if (!exists) {
            dst.v_array.PushBack(*v0);
        }
    }
    fprintf(stderr,"-> v's reduced from %d to %d\n",src.v_array.Size(),dst.v_array.Size());

    // check vn components for redundancies
    nArray<int> vn_maptable(5000, 5000);
    nArray<vector3>::iterator vn0;
    for (vn0=src.vn_array.Begin(); vn0!=src.vn_array.End(); vn0++) {
        nArray<vector3>::iterator vn1;
        int i1;
        bool exists = false;
        for (i1=0,vn1=dst.vn_array.Begin(); vn1!=dst.vn_array.End(); i1++,vn1++) {
            if (vn1->isequal(*vn0,vntol)) {
                // v1 is a copy of v0, break!
                exists = true;
                break;
            }
        }
        vn_maptable.PushBack(i1);
        if (!exists) {
            dst.vn_array.PushBack(*vn0);
        }
    }
    fprintf(stderr,"-> vn's reduced from %d to %d\n",src.vn_array.Size(),dst.vn_array.Size());

    // check vt components for redundancies
    nArray<int> vt_maptable(5000, 5000);
    nArray<vector2>::iterator vt0;
    for (vt0=src.vt_array.Begin(); vt0!=src.vt_array.End(); vt0++) {
        nArray<vector2>::iterator vt1;
        int i1;
        bool exists = false;
        for (i1=0,vt1=dst.vt_array.Begin(); vt1!=dst.vt_array.End(); i1++,vt1++) {
            if (vt1->isequal(*vt0,vttol)) {
                // v1 is a copy of v0, break!
                exists = true;
                break;
            }
        }
        vt_maptable.PushBack(i1);
        if (!exists) {
            dst.vt_array.PushBack(*vt0);
        }
    }
    fprintf(stderr,"-> vt's reduced from %d to %d\n",src.vt_array.Size(),dst.vt_array.Size());

    // check vt1 components for redundancies
    nArray<int> vt1_maptable(5000, 5000);
    nArray<vector2>::iterator vt10;
    for (vt10=src.vt1_array.Begin(); vt10!=src.vt1_array.End(); vt10++) {
        nArray<vector2>::iterator vt11;
        int i1;
        bool exists = false;
        for (i1=0,vt11=dst.vt1_array.Begin(); vt11!=dst.vt1_array.End(); i1++,vt11++) {
            if (vt11->isequal(*vt10,vttol)) {
                // v1 is a copy of v0, break!
                exists = true;
                break;
            }
        }
        vt1_maptable.PushBack(i1);
        if (!exists) {
            dst.vt1_array.PushBack(*vt10);
        }
    }
    fprintf(stderr,"-> vt1's reduced from %d to %d\n",src.vt1_array.Size(),dst.vt1_array.Size());

    // check vt2 components for redundancies
    nArray<int> vt2_maptable(5000, 5000);
    nArray<vector2>::iterator vt20;
    for (vt20=src.vt2_array.Begin(); vt20!=src.vt2_array.End(); vt20++) {
        nArray<vector2>::iterator vt21;
        int i1;
        bool exists = false;
        for (i1=0,vt21=dst.vt2_array.Begin(); vt21!=dst.vt2_array.End(); i1++,vt21++) {
            if (vt21->isequal(*vt20,vttol)) {
                // v1 is a copy of v0, break!
                exists = true;
                break;
            }
        }
        vt2_maptable.PushBack(i1);
        if (!exists) {
            dst.vt2_array.PushBack(*vt20);
        }
    }
    fprintf(stderr,"-> vt2's reduced from %d to %d\n",src.vt2_array.Size(),dst.vt2_array.Size());

    // check vt3 components for redundancies
    nArray<int> vt3_maptable(5000, 5000);
    nArray<vector2>::iterator vt30;
    for (vt30=src.vt3_array.Begin(); vt30!=src.vt3_array.End(); vt30++) {
        nArray<vector2>::iterator vt31;
        int i1;
        bool exists = false;
        for (i1=0,vt31=dst.vt3_array.Begin(); vt31!=dst.vt3_array.End(); i1++,vt31++) {
            if (vt31->isequal(*vt30,vttol)) {
                // v1 is a copy of v0, break!
                exists = true;
                break;
            }
        }
        vt3_maptable.PushBack(i1);
        if (!exists) {
            dst.vt3_array.PushBack(*vt30);
        }
    }
    fprintf(stderr,"-> vt3's reduced from %d to %d\n",src.vt3_array.Size(),dst.vt3_array.Size());

    // check rgba components for redundancies
    nArray<int> c_maptable(5000, 5000);
    nArray<vector4>::iterator c0;
    for (c0=src.c_array.Begin(); c0!=src.c_array.End(); c0++) {
        nArray<vector4>::iterator c1;
        int i1;
        bool exists = false;
        for (i1=0,c1=dst.c_array.Begin(); c1!=dst.c_array.End(); i1++,c1++) {
            if (c1->isequal(*c0,ctol)) {
                // v1 is a copy of v0, break!
                exists = true;
                break;
            }
        }
        c_maptable.PushBack(i1);
        if (!exists) {
            dst.c_array.PushBack(*c0);
        }
    }
    fprintf(stderr,"-> c's reduced from %d to %d\n",src.c_array.Size(),dst.c_array.Size());

    // handle faces: for each source face, generate a target face
    // with the src->dst mapped indices, do not add identical
    // indices. If the dest face has less then 3 points left,
    // ignore the whole face
    // if any face points don't have texture coordinates assigned,
    // but the number of texture coordinates is greater 0,
    // assign the first texture coordinate to the point
    nArray<wfFace>::iterator f_src;
    for (f_src=src.f_array.Begin(); f_src!=src.f_array.End(); f_src++) {
        wfFace f_dst;

        nArray<wfPoint>::iterator p_src;
        for (p_src=f_src->points.Begin(); p_src!=f_src->points.End(); p_src++) {

            // fill p1 with remapped indices
            wfPoint p_dst;
            if (p_src->v_index != -1)
            {
                p_dst.v_index = v_maptable.At(p_src->v_index);
            }

			// uv index 0
            if (p_src->vt_index != -1)
            {
                p_dst.vt_index = vt_maptable.At(p_src->vt_index);
            }
            else
            {
                // FIX: if no texture assigned, but texture coordinates
                // exist, map to first texture coordinate
                if (src.vt_array.Size() > 0)
                {
                    p_dst.vt_index = 0;
                }
            }

			// uv index 1
            if (p_src->vt1_index != -1)
            {
                p_dst.vt1_index = vt1_maptable.At(p_src->vt1_index);
            }
            else
            {
                // FIX: if no texture assigned, but texture coordinates
                // exist, map to first texture coordinate
                if (src.vt1_array.Size() > 0)
                {
                    p_dst.vt1_index = 0;
                }
            }

			// uv index 2
            if (p_src->vt2_index != -1)
            {
                p_dst.vt2_index = vt2_maptable.At(p_src->vt2_index);
            }
            else
            {
                // FIX: if no texture assigned, but texture coordinates
                // exist, map to first texture coordinate
                if (src.vt2_array.Size() > 0)
                {
                    p_dst.vt2_index = 0;
                }
            }

			// uv index 3
            if (p_src->vt3_index != -1)
            {
                p_dst.vt3_index = vt3_maptable.At(p_src->vt3_index);
            }
            else
            {
                // FIX: if no texture assigned, but texture coordinates
                // exist, map to first texture coordinate
                if (src.vt3_array.Size() > 0)
                {
                    p_dst.vt3_index = 0;
                }
            }
            if (p_src->vn_index != -1)
            {
                p_dst.vn_index = vn_maptable.At(p_src->vn_index);
            }
            if (p_src->c_index != -1)
            {
                p_dst.c_index = c_maptable.At(p_src->c_index);
            }

            // see if f1 already contains a point with identical
            // v index, if so, drop p1, otherwise add to face
            nArray<wfPoint>::iterator p;
            bool ignore = false;
            for (p=f_dst.points.Begin(); p!=f_dst.points.End(); p++) {
                if (p_dst.v_index == p->v_index) {
                    ignore = true;
                }
            }
            if (!ignore) {
                f_dst.points.PushBack(p_dst);
            }
        }
		f_dst.validUVsets = f_src->validUVsets;

        // if number of points in dest surface > 2, add, otherwise ignore
        if (f_dst.points.Size() > 2) {
            dst.f_array.PushBack(f_dst);
        }
    }
    fprintf(stderr, "-> f's reduced from %d to %d\n",src.f_array.Size(),dst.f_array.Size());
};

//-------------------------------------------------------------------
//  flatten()
//  Flatten the source object into the dest object, so that
//  each complete vertex can be adressed through a single index.
//  24-Oct-00   floh    created
//-------------------------------------------------------------------
public:
void flatten(wfObject& src, wfObject& dst) {

    // for each face in the src object...
    nArray<wfFace>::iterator f_src;
    int points_reused = 0;
    for (f_src=src.f_array.Begin(); f_src!=src.f_array.End(); f_src++) 
    {
        // the destination face
        wfFace f_dst;

        // for each point in the src face...
        nArray<wfPoint>::iterator p_src;
        for (p_src=f_src->points.Begin(); p_src!=f_src->points.End(); p_src++) 
        {
            // see if there already exists an identical vertex in the dst object
            nArray<wfCoord>::iterator v_dst   = dst.v_array.Begin();
            nArray<vector3>::iterator vn_dst  = dst.vn_array.Begin();
            nArray<vector2>::iterator vt_dst  = dst.vt_array.Begin();
            nArray<vector2>::iterator vt1_dst = dst.vt1_array.Begin();
            nArray<vector2>::iterator vt2_dst = dst.vt2_array.Begin();
            nArray<vector2>::iterator vt3_dst = dst.vt3_array.Begin();
            nArray<vector4>::iterator c_dst   = dst.c_array.Begin();
            int dst_index = 0;

            // get the original vertex components block
            wfCoord v_src;
            vector3 vn_src;
            vector2 vt_src;
            vector2 vt1_src;
            vector2 vt2_src;
            vector2 vt3_src;
            vector4 c_src;
            if (p_src->v_index != -1)   v_src   = src.v_array.At(p_src->v_index);
            if (p_src->vn_index != -1)  vn_src  = src.vn_array.At(p_src->vn_index);
            if (p_src->vt_index != -1)  vt_src  = src.vt_array.At(p_src->vt_index);
            if (p_src->vt1_index != -1) vt1_src = src.vt1_array.At(p_src->vt1_index);
            if (p_src->vt2_index != -1) vt2_src = src.vt2_array.At(p_src->vt2_index);
            if (p_src->vt3_index != -1) vt3_src = src.vt3_array.At(p_src->vt3_index);
            if (p_src->c_index != -1)   c_src   = src.c_array.At(p_src->c_index);

            bool all_equal = false;
            for (; v_dst!=dst.v_array.End(); v_dst++) 
            {
                bool v_equal  = true;
                bool vn_equal = true;
                bool vt_equal = true;
                bool vt1_equal = true;
                bool vt2_equal = true;
                bool vt3_equal = true;
                bool c_equal  = true;
                if (p_src->v_index != -1) 
                {
                    if (!v_src.v.isequal(v_dst->v,0.0001f)) 
                    {
                        v_equal = false;
                    }
                }
                if (p_src->vn_index != -1) 
                {
                    if (!vn_src.isequal(*vn_dst,0.0001f)) 
                    {
                        vn_equal = false;
                    }
                }

                if (p_src->vt_index != -1) 
                {
                    if (!vt_src.isequal(*vt_dst,0.0001f)) 
                    {
                        vt_equal = false;
                    }
                }
                if (p_src->vt1_index != -1) 
                {
                    if (!vt1_src.isequal(*vt1_dst,0.0001f)) 
                    {
                        vt1_equal = false;
                    }
                }
                if (p_src->vt2_index != -1) 
                {
                    if (!vt2_src.isequal(*vt2_dst,0.0001f)) 
                    {
                        vt2_equal = false;
                    }
                }
                if (p_src->vt3_index != -1) 
                {
                    if (!vt3_src.isequal(*vt3_dst,0.0001f)) 
                    {
                        vt3_equal = false;
                    }
                }

                if (p_src->c_index != -1) 
                {
                    if (!c_src.isequal(*c_dst,0.0001f)) 
                    {
                        c_equal = false;
                    }
                }

                if (v_equal && vn_equal && vt_equal && vt1_equal && vt2_equal && vt3_equal && c_equal) 
                {
                    all_equal = true;
                    points_reused++;
                    break;
                }

                if (vn_dst) vn_dst++;
                if (vt_dst) vt_dst++;
                if (vt1_dst) vt1_dst++;
                if (vt2_dst) vt2_dst++;
                if (vt3_dst) vt3_dst++;
                if (c_dst)  c_dst++;
                dst_index++;
            }

            // if the vertex does not already exist, push a new one to the
            // destination object
            if (!all_equal) 
            {
                if (p_src->v_index != -1)  dst.v_array.PushBack(v_src);
                if (p_src->vn_index != -1) dst.vn_array.PushBack(vn_src);
                if (p_src->vt_index != -1) dst.vt_array.PushBack(vt_src);
                if (p_src->vt1_index != -1) dst.vt1_array.PushBack(vt1_src);
                if (p_src->vt2_index != -1) dst.vt2_array.PushBack(vt2_src);
                if (p_src->vt3_index != -1) dst.vt3_array.PushBack(vt3_src);
                if (p_src->c_index != -1)  dst.c_array.PushBack(c_src);
            }

            // create a new point and append to the destination face
            wfPoint p_dst;
            if (p_src->v_index != -1)  p_dst.v_index  = dst_index;
            if (p_src->vn_index != -1) p_dst.vn_index = dst_index;
            if (p_src->vt_index != -1) p_dst.vt_index = dst_index;
            if (p_src->vt1_index != -1) p_dst.vt1_index = dst_index;
            if (p_src->vt2_index != -1) p_dst.vt2_index = dst_index;
            if (p_src->vt3_index != -1) p_dst.vt3_index = dst_index;
            if (p_src->c_index != -1)  p_dst.c_index  = dst_index;

            f_dst.points.PushBack(p_dst);

        } // for each point in src_face

		f_dst.validUVsets = f_src->validUVsets;
        // append new face to target object
        dst.f_array.PushBack(f_dst);
    }

    // fix indices in winged edges
    nArray<wfWingedEdge>::iterator we_src;
    for (we_src = src.we_array.Begin(); we_src != src.we_array.End(); we_src++)
    {
        // get the referenced coords in the src
        vector3& v0  = src.v_array.At(we_src->v0).v;
        vector3& v1  = src.v_array.At(we_src->v1).v;
        vector3& vp0 = src.v_array.At(we_src->vp0).v;
        vector3& vp1 = src.v_array.At(we_src->vp1).v;

        // find corresponding coords in the dest
        int dst_v0i  = -1;
        int dst_v1i  = -1;
        int dst_vp0i = -1;
        int dst_vp1i = -1;
        int i;
        for (i = 0; i < dst.v_array.Size(); i++)
        {
            if (v0.isequal(dst.v_array.At(i).v, 0.0001f))
            {
                dst_v0i = i;
                break;
            }
        }
        for (i = 0; i < dst.v_array.Size(); i++)
        {
            if (v1.isequal(dst.v_array.At(i).v, 0.0001f))
            {
                dst_v1i = i;
                break;
            }
        }
        for (i = 0; i < dst.v_array.Size(); i++)
        {
            if (vp0.isequal(dst.v_array.At(i).v, 0.0001f))
            {
                dst_vp0i = i;
                break;
            }
        }
        for (i = 0; i < dst.v_array.Size(); i++)
        {
            if (vp1.isequal(dst.v_array.At(i).v, 0.0001f))
            {
                dst_vp1i = i;
                break;
            }
        }

        // has a match been found for each index???
        if ((dst_v0i != -1) && (dst_v1i != -1) && (dst_vp0i != -1) && (dst_vp1i != -1))
        {
            wfWingedEdge we(dst_v0i, dst_v1i, dst_vp0i, dst_vp1i);
            dst.we_array.PushBack(we);
        }
    }

    fprintf(stderr,"points reused: %d\n",points_reused);
};

//-------------------------------------------------------------------
//  transform()
//  Transform the wavefront object in place (rotation order is x->y->z).
//  Angles around x,y,z must be given in rad.
//  24-Oct-00   floh    created
//-------------------------------------------------------------------
public:
void transform(wfObject& src, const vector3& t, const vector3& r, const vector3& s) 
{
    // build a 4x4 transformation matrix
    matrix44 m;
    m.scale(s);
    m.rotate_x(r.x);
    m.rotate_y(r.y);
    m.rotate_z(r.z);
    m.translate(t.x,t.y,t.z);

    // transform coords
    nArray<wfCoord>::iterator v;
    for (v=src.v_array.Begin(); v!=src.v_array.End(); v++) 
    {
        vector3 v1 = m * v->v;
        v->v = v1;
    }

    // build a normals transformation matrix (no translation)
    m.ident();
    m.scale(s);
    m.rotate_x(r.x);
    m.rotate_y(r.y);
    m.rotate_z(r.z);
    nArray<vector3>::iterator vn;
    for (vn=src.vn_array.Begin(); vn!=src.vn_array.End(); vn++) 
    {
        vector3 v1 = m * (*vn);
        v1.norm();
        *vn = v1;
    };

    // that's it...
};

//-------------------------------------------------------------------
//  triangulate()
//  Copy the src to the dst object but triangulate surfaces.
//-------------------------------------------------------------------
public:
void triangulate(wfObject& src, wfObject&dst)
{
    // copy source vertex data to destination
    dst.v_array  = src.v_array;
    dst.vn_array = src.vn_array;
    dst.vt_array = src.vt_array;
    dst.vt1_array = src.vt1_array;
    dst.vt2_array = src.vt2_array;
    dst.vt3_array = src.vt3_array;
    dst.c_array  = src.c_array;

    // for each source face...
    nArray<wfFace>::iterator f_src;
    for (f_src=src.f_array.Begin(); f_src<src.f_array.End(); f_src++) {

        // ignore it if it has less then 3 points (this normally should
        // not happen)
        if (f_src->points.Size() > 2) 
        {
            // get the first 3 points, and loop through the rest...
            nArray<wfPoint>::iterator p0_src = f_src->points.Begin();
            nArray<wfPoint>::iterator p1_src = p0_src+1;
            nArray<wfPoint>::iterator p2_src = p1_src+1;
            for (;p2_src!=f_src->points.End();) 
            {
                // a new destination face (triangle!)
                wfFace f_dst;
                f_dst.points.PushBack(*p0_src);
                f_dst.points.PushBack(*p1_src);
                f_dst.points.PushBack(*p2_src);

                // add the face to the dst object
				f_dst.validUVsets = f_src->validUVsets;
                dst.f_array.PushBack(f_dst);

                // advance to next triangle within source face
                p1_src = p2_src;
                p2_src++;
            }
        }
    }
    fprintf(stderr, "face count: before=%d after triang=%d\n", src.f_array.Size(), dst.f_array.Size());
};

//--------------------------------------------------------------------
//  build_face_normals()
//  Create the face normals array.
//  Needed by smooth().
//  26-Oct-00   floh    created
//--------------------------------------------------------------------
private:
void build_face_normals(wfObject& src)
{
    face_normals.Clear();

    // for each face...
    nArray<wfFace>::iterator f_src;
    vector3 normal;
        vector3 d[3];
        vector3 n[3];
        float l[3];
        int i;
    for (f_src=src.f_array.Begin(); f_src!=src.f_array.End(); f_src++) 
    {
        assert(f_src->points.Size() >= 3);
        const vector3& v0 = src.v_array.At(f_src->points.At(0).v_index).v;
        const vector3& v1 = src.v_array.At(f_src->points.At(1).v_index).v;
        const vector3& v2 = src.v_array.At(f_src->points.At(2).v_index).v;

        // we're trying all 3 combinations and pick the most
        // accurate result
        d[0] = v1 - v0;
        d[1] = v2 - v0;
        d[2] = v2 - v1;
        n[0] = d[0] * d[1];
        n[1] = d[1] * d[2];
        n[2] = d[0] * d[2];

        for (i=0; i<3; i++) 
        {
            l[i] = n[i].len();
        };

        // HINT: we don't handle illegal faces here, since they
        // should already be taken care of by doing a wfclean on
        // the object before!
        if ((l[0]>l[1]) && (l[0]>l[2])) normal=n[0];
        else if (l[1] > l[2])           normal=n[1];
        else                            normal=n[2];
        normal.norm();
        face_normals.PushBack(normal);
    }
}

//--------------------------------------------------------------------
//  build_connect_array()
//  Fill out the connect array, for each vertex, find all faces
//  which reference this vertex, if faces are coplanar, add only
//  one of them.
//  Needed by smooth().
//  25-Oct-00   floh    created
//-------------------------------------------------------------------
private:
void build_connect_array(wfObject& src)
{
    build_face_normals(src);
    connect_array.Clear();

    // for each vertex, find all faces which reference this vertex
    int i;
    int vnum = src.v_array.Size();
    for (i=0; i<vnum; i++) 
    {
        nArray<wfFace>::iterator f_src;
        nArray<int> f_array(5000, 5000);
        int fnum;
        for (fnum=0,f_src=src.f_array.Begin(); f_src!=src.f_array.End(); f_src++,fnum++) 
        {
            nArray<wfPoint>::iterator p_src;
            for (p_src=f_src->points.Begin(); p_src!=f_src->points.End(); p_src++) 
            {
                if (i==p_src->v_index) {
                    // drop any faces which are coplanar with an already
                    // collected face!
                    nArray<int>::iterator fi;
                    bool drop_face = false;
                    for (fi=f_array.Begin(); fi!=f_array.End(); fi++) 
                    {
                        vector3 v0 = face_normals.At(fnum);
                        vector3 v1 = face_normals.At(*fi);
                        if (v0.isequal(v1,0.001f)) 
                        {
                            drop_face = true;
                            break;
                        }
                    }
                    if (!drop_face) f_array.PushBack(fnum);
                }
            }
        }
        connect_array.PushBack(f_array);
    }
}

//--------------------------------------------------------------------
//  smooth()
//  Generate the face normals as defined by the threshold angle.
//  Copies all data from old object to new object, generates lots
//  of redundant data, so be sure to do a wfclean afterwards!
//
//  YOU SHOULD GENERALLY DO A clean() BEFORE AND AFTER CALLING
//  SMOOTH, since smooth() relies on a clean() vertex pool
//  do get the face connections right, and also, smooth generates
//  myriads of redundant face normals, so be sure to do another
//  cleanup pass before calling anything else (esp. flatten()).
//
//  26-Oct-00   floh    created
//--------------------------------------------------------------------
public:
void smooth(wfObject& src, wfObject& dst, float thresh_degree)
{
    float cos_ang = (float) cos((thresh_degree * PI) / 180.0f);

    // build the connection and face normals database
    build_connect_array(src);

    // fist copy the src data into the destination object in the
    // following way:
    // - the v, vt and rgba components are copied unchanged
    // - for each face, 3 normals are generaded, and the face
    //   point indices are fixed accordingly
    dst.v_array  = src.v_array;
    dst.vt_array = src.vt_array;
    dst.vt1_array = src.vt1_array;
    dst.vt2_array = src.vt2_array;
    dst.vt3_array = src.vt3_array;
    dst.c_array  = src.c_array;
    dst.f_array  = src.f_array;     // fix normal indices later...

    // for each face point...
    nArray<wfFace>::iterator f_dst;
    int f_index;
    for (f_index=0, f_dst=dst.f_array.Begin(); f_dst!=dst.f_array.End(); f_dst++, f_index++) 
    {
        nArray<wfPoint>::iterator p_dst;
        for (p_dst=f_dst->points.Begin(); p_dst!=f_dst->points.End(); p_dst++) 
        {
            // get the list of all faces which use this vertex coordinate
            nArray<int> f_list = connect_array.At(p_dst->v_index);

            // generate a new averaged vertex normal, all face normals
            // within the threshold angle are added together and
            // renormalized
            nArray<int>::iterator fi;

            // get the base vector (the current point's face's normal)
            vector3 v0 = face_normals.At(f_index);

            // initialize the average vector with the face normal
            // of the point's owner face
            vector3 avg = v0;
            for (fi=f_list.Begin(); fi!=f_list.End(); fi++) 
            {
                // don't add redundant normals!
                vector3 v1 = face_normals.At(*fi);
                if (!v0.isequal(v1,0.001f)) 
                {
                    float dot  = v1 % v0;
                    if (dot > cos_ang) 
                    {
                        avg += v1;
                    }
                }
            }

            // normalize average normal, write to target object and
            // fix normal index in target point
            avg.norm();
            dst.vn_array.PushBack(avg);
            p_dst->vn_index = dst.vn_array.Size()-1;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Generate a new texture coordinate set by doing planar mapping as follows:

    u = x*uParam.x + y*uParam.y + z*uParam.z + uParam.w
    v = x*vParam.x + y*vParam.y + z*vParam.z + vParam.w

    NOTE:
    The src object must be FLATTENED!

    @param  src         the src wfObject (FLATTENED!)
    @param  dst         empty destination wfObject
    @param  texLayer    the texture set index to generate (0..3)
    @param  uParams     the 4D u "plane equation coefficients"
    @param  vParams     same for v dimension
*/
public:
void
mapPlanar(wfObject& src, wfObject& dst, int texLayer, const vector4& uParams, const vector4& vParams)
{
    assert((texLayer >= 0) && (texLayer < 4));

    // copy over stuff
    dst.v_array  = src.v_array;
    dst.vn_array = src.vn_array;
    dst.c_array  = src.c_array;
    dst.f_array  = src.f_array;

    if (0 != texLayer)
        dst.vt_array = src.vt_array;
    if (1 != texLayer)
        dst.vt1_array = src.vt1_array;
    if (2 != texLayer)
        dst.vt2_array = src.vt2_array;
    if (3 != texLayer)
        dst.vt3_array = src.vt3_array;

    // iterate through vertices and generate a corresponding uv coordinate
    nArray<wfCoord>::iterator curCoord;
    for (curCoord = src.v_array.Begin(); curCoord != src.v_array.End(); curCoord++)
    {
        vector3 coord = curCoord->v;
        vector2 uv;

        uv.x = coord.x * uParams.x + coord.y * uParams.y + coord.z * uParams.z + uParams.w;
        uv.y = coord.x * vParams.x + coord.y * vParams.y + coord.z * vParams.z + vParams.w;

        switch (texLayer)
        {
            case 0:
                dst.vt_array.PushBack(uv);
                break;

            case 1:
                dst.vt1_array.PushBack(uv);
                break;

            case 2:
                dst.vt2_array.PushBack(uv);
                break;

            case 3:
                dst.vt3_array.PushBack(uv);
                break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Generate winged edges. Object must be triangulated and cleaned!
*/
public:
void
genWingedEdges(wfObject& src)
{
    // clear existing winged edge array if exists
    src.we_array.Clear();

    // for each face, build 3 unique winged edge objects
    nArray<wfFace>::iterator f;
    for (f = src.f_array.Begin(); f != src.f_array.End(); f++)
    {
        int p0i = f->points.At(0).v_index;
        int p1i = f->points.At(1).v_index;
        int p2i = f->points.At(2).v_index;

        wfWingedEdge we[3];
        we[0].set(p0i, p1i, p2i, -1);
        we[1].set(p1i, p2i, p0i, -1);
        we[2].set(p2i, p0i, p1i, -1);

        int i;
        for (i = 0; i < 3; i++)
        {
            nArray<wfWingedEdge>::iterator cur_we;
            bool exists = false;
            bool merged = false;
            for (cur_we = src.we_array.Begin(); cur_we != src.we_array.End(); cur_we++)
            {
                // see if edge shared
                vector3& cur_v0 = src.v_array.At(cur_we->v0).v;
                vector3& cur_v1 = src.v_array.At(cur_we->v1).v;
                vector3& v0     = src.v_array.At(we[i].v0).v;
                vector3& v1     = src.v_array.At(we[i].v1).v;
                if ((v0.isequal(cur_v0, 0.0001f) && v1.isequal(cur_v1, 0.0001f)) ||
                    (v0.isequal(cur_v1, 0.0001f) && v1.isequal(cur_v0, 0.0001f)))
                {
                    // found a shared edge, now see if our new edge is somehow
                    // identical with with the existing edge (this shouldn't happen)
                    vector3& cur_vp0 = src.v_array.At(cur_we->vp0).v;
                    vector3& vp0     = src.v_array.At(we[i].vp0).v;
                    if (vp0.isequal(cur_vp0, 0.0001f))
                    {
                        exists = true;
                        break;
                    }

                    if (cur_we->vp1 != -1)
                    {
                        vector3& cur_vp1 = src.v_array.At(cur_we->vp1).v;
                        if (vp0.isequal(cur_vp1, 0.0001f))
                        {
                            exists = true;
                            break;
                        }
                    }
                    else
                    {
                        // when arrived here, merge the new winged edge with 
                        // the existing winged edge (this gets rid of the -1
                        // in the vp1 member)
                        cur_we->vp1 = we[i].vp0;
                        merged = true;
                        break;
                    }
                }
            }

            // if the is a new winged edge, add it to the winged edge array
            if (!(exists || merged))
            {
                src.we_array.PushBack(we[i]);
            }
        }
    }

    // Handle incomplete winged edges (edges which have a -1 vp1 member left),
    // this can be either real object edges (if the mesh is open), or some
    // modeling artefact. We will just mirror the vp0 member into the vp1
    // member, this should give the desired results (at least for silhouette
    // finding)

    // final validity check on edges, remove coplanar edges, and fix 
    // "real" object edges if object surface is not closed
    nArray<wfWingedEdge>::iterator cur_we;
    for (cur_we = src.we_array.Begin(); cur_we != src.we_array.End();)
    {
        // a real edge?
        if (-1 == cur_we->vp1)
        {
            cur_we->vp1 = cur_we->vp0;
        }
            cur_we++;
    }
}    

//--- end of wfToolbox ----------------------------------------------
};

//-------------------------------------------------------------------
#endif
