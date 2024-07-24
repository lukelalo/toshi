#ifndef N_BBOX_H
#define N_BBOX_H
//-------------------------------------------------------------------
/**
    @class bbox3
    @brief (non-oriented) bounding box
*/
//-------------------------------------------------------------------
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

//-------------------------------------------------------------------
//  bbox3
//-------------------------------------------------------------------
class bbox3 {
public:
    vector3 vmin;
    vector3 vmax;

    enum {
        CLIP_LEFT   = (1<<0),
        CLIP_RIGHT  = (1<<1),
        CLIP_BOTTOM = (1<<2),
        CLIP_TOP    = (1<<3),
        CLIP_NEAR   = (1<<4),
        CLIP_FAR    = (1<<5),
    };

    enum {
        OUTSIDE     = 0,
        ISEQUAL     = (1<<0),
        ISCONTAINED = (1<<1),
        CONTAINS    = (1<<2),
        CLIPS       = (1<<3),
    };

    //--- constructors ----------------------------------------------
    bbox3() {};
    bbox3(const vector3& _vmin, const vector3& _vmax) : vmin(_vmin), vmax(_vmax) {};
    bbox3(const bbox3& bb) : vmin(bb.vmin), vmax(bb.vmax) {};
    
    //--- initialize from vector3 cloud -----------------------------
    bbox3(vector3 *varray, int num) {
        vmin = varray[0];
        vmax = varray[0];
        int i;
        for (i=0; i<num; i++) {
            if      (varray[i].x<vmin.x) vmin.x=varray[i].x;
            else if (varray[i].x>vmax.x) vmax.x=varray[i].x;
            if      (varray[i].y<vmin.y) vmin.y=varray[i].y;
            else if (varray[i].y>vmax.y) vmax.y=varray[i].y;
            if      (varray[i].z<vmin.z) vmin.z=varray[i].z;
            else if (varray[i].z>vmax.z) vmax.z=varray[i].z;
        }
    };

    //--- setting elements ------------------------------------------
    void set(const vector3& _vmin, const vector3& _vmax) {
        vmin = _vmin;
        vmax = _vmax;
    };
    void set(vector3 *varray, int num) {
        vmin = varray[0];
        vmax = varray[0];
        int i;
        for (i=0; i<num; i++) {
            if      (varray[i].x<vmin.x) vmin.x=varray[i].x;
            else if (varray[i].x>vmax.x) vmax.x=varray[i].x;
            if      (varray[i].y<vmin.y) vmin.y=varray[i].y;
            else if (varray[i].y>vmax.y) vmax.y=varray[i].y;
            if      (varray[i].z<vmin.z) vmin.z=varray[i].z;
            else if (varray[i].z>vmax.z) vmax.z=varray[i].z;
        }
    };

    //--- invalidate bounding box to prepare for growing ------------
    void begin_grow(void) {
        vmin.set(+1000000.0f,+1000000.0f,+1000000.0f);
        vmax.set(-1000000.0f,-1000000.0f,-1000000.0f);
    };
    void grow(const vector3& v) {
        if (v.x<vmin.x) vmin.x=v.x;
        if (v.x>vmax.x) vmax.x=v.x;
        if (v.y<vmin.y) vmin.y=v.y;
        if (v.y>vmax.y) vmax.y=v.y;
        if (v.z<vmin.z) vmin.z=v.z;
        if (v.z>vmax.z) vmax.z=v.z;
    };
    void grow(vector3 *varray, int num) {
        int i;
        for (i=0; i<num; i++) {
            grow(varray[i]);
        }
    };

    void grow(const bbox3& bb) {
        if (bb.vmin.x<vmin.x) vmin.x=bb.vmin.x;
        if (bb.vmin.y<vmin.y) vmin.y=bb.vmin.y;
        if (bb.vmin.z<vmin.z) vmin.z=bb.vmin.z;
        if (bb.vmax.x>vmax.x) vmax.x=bb.vmax.x;
        if (bb.vmax.y>vmax.y) vmax.y=bb.vmax.y;
        if (bb.vmax.z>vmax.z) vmax.z=bb.vmax.z;
    };

    // get point of intersection of 3d line with planes
    // on const x,y,z
    bool isect_const_x(const float x, const line3& l, vector3& out) const {
        if (l.m.x != 0.0f) {
            float t = (x - l.b.x) / l.m.x;
            if ((t>=0.0f) && (t<=1.0f)) {
                // point of intersection...
                out = l.ipol(t);
                return true;
            }
        }
        return false;
    }
    bool isect_const_y(const float y, const line3& l, vector3& out) const {
        if (l.m.y != 0.0f) {
            float t = (y - l.b.y) / l.m.y;
            if ((t>=0.0f) && (t<=1.0f)) {
                // point of intersection...
                out = l.ipol(t);
                return true;
            }
        }
        return false;
    }
    bool isect_const_z(const float z, const line3& l, vector3& out) const {
        if (l.m.z != 0.0f) {
            float t = (z - l.b.z) / l.m.z;
            if ((t>=0.0f) && (t<=1.0f)) {
                // point of intersection...
                out = l.ipol(t);
                return true;
            }
        }
        return false;
    }

    // point in polygon check for sides with constant x,y and z
    bool pip_const_x(const vector3& p) const {
        if ((p.y>=vmin.y)&&(p.y<=vmax.y)&&(p.z>=vmin.z)&&(p.z<=vmax.z)) return true;
        else return false;
    }
    bool pip_const_y(const vector3& p) const {
        if ((p.x>=vmin.x)&&(p.x<=vmax.x)&&(p.z>=vmin.z)&&(p.z<=vmax.z)) return true;
        else return false;
    }
    bool pip_const_z(const vector3& p) const {
        if ((p.x>=vmin.x)&&(p.x<=vmax.x)&&(p.y>=vmin.y)&&(p.y<=vmax.y)) return true;
        else return false;
    }

    //--- check if box intersects or contains line ------------------
    bool intersect(const line3& line) const {
        // For each side of box, check if point of intersection
        // lies within this sides 2d rectangle. If at least one
        // intersection occurs, the line intersects the box
        // (usually, there will be 2 intersections).
        // The line parameter t for the intersection is computed
        // by resolving the formula:
        //  p = line.b + line.m*t
        // after t:
        //  t = (p - line.b) / line.m
        // if line.m is zero, the line is parallel to the plane in
        // question.
        // MAY BE EXTENDED TO RETURN CLOSEST POINT OF INTERSECTION!
        
        // check if at least one of the 2 points is included in the volume
        vector3 s(line.start());
        vector3 e(line.end());
        if (((s.x>=vmin.x) && (s.y>=vmin.y) && (s.z>=vmin.z) &&
             (s.x<=vmax.x) && (s.y<=vmax.y) && (s.z<=vmax.z)) ||
             ((e.x>=vmin.x) && (e.y>=vmin.y) && (e.z>=vmin.z) &&
             (e.x<=vmax.x) && (e.y<=vmax.y) && (e.z<=vmax.z)))
        {
            return true;
        } else {
            // otherwise do intersection check
            int i;
            vector3 ipos;
            for (i=0; i<6; i++) {
                switch (i) {
                    // left side, vmin.x is constant
                    case 0:
                        if (isect_const_x(vmin.x,line,ipos) && pip_const_x(ipos)) return true;
                        break;
                    case 1:
                        if (isect_const_x(vmax.x,line,ipos) && pip_const_x(ipos)) return true;
                        break;
                    case 2:
                        if (isect_const_y(vmin.y,line,ipos) && pip_const_y(ipos)) return true;
                        break;
                    case 3:
                        if (isect_const_y(vmax.y,line,ipos) && pip_const_y(ipos)) return true;
                        break;
                    case 4:
                        if (isect_const_z(vmin.z,line,ipos) && pip_const_z(ipos)) return true;
                        break;
                    case 5:
                        if (isect_const_z(vmax.z,line,ipos) && pip_const_z(ipos)) return true;
                        break;
                }
            }
        }
        return false;
    }

    /**
        @brief Gets closest intersection with AABB.
        If the line starts inside the box,  start point is returned in ipos.
        @param line the pick ray
        @param ipos closest point of intersection if successful, trash otherwise
        @return true if an intersection occurs
    */
    bool intersect(const line3& line, vector3& ipos) const
    {
        // Handle special case for start point inside box
        if (line.b.x >= vmin.x && line.b.y >= vmin.y && line.b.z >= vmin.z &&
            line.b.x <= vmax.x && line.b.y <= vmax.y && line.b.z <= vmax.z)
        {
            ipos = line.b;
            return true;
        }

        // Order planes to check, closest three only
        int plane[3];
        if (line.m.x > 0) plane[0] = 0;
        else              plane[0] = 1;
        if (line.m.y > 0) plane[1] = 2;
        else              plane[1] = 3;
        if (line.m.z > 0) plane[2] = 4;
        else              plane[2] = 5;
        
        for (int i = 0; i < 3; ++i)
        {
            switch (plane[i]) {
                case 0:
                    if (isect_const_x(vmin.x,line,ipos) && pip_const_x(ipos)) return true;
                    break;
                case 1:
                    if (isect_const_x(vmax.x,line,ipos) && pip_const_x(ipos)) return true;
                    break;
                case 2:
                    if (isect_const_y(vmin.y,line,ipos) && pip_const_y(ipos)) return true;
                    break;
                case 3:
                    if (isect_const_y(vmax.y,line,ipos) && pip_const_y(ipos)) return true;
                    break;
                case 4:
                    if (isect_const_z(vmin.z,line,ipos) && pip_const_z(ipos)) return true;
                    break;
                case 5:
                    if (isect_const_z(vmax.z,line,ipos) && pip_const_z(ipos)) return true;
                    break;
            }
        }

        return false;
    }

    //--- check if box intersects, contains or is contained in other box
    //--- by doing 3 projection tests for each dimension, if all 3 test 
    //--- return true, then the 2 boxes intersect
    int line_test(float v0, float v1, float w0, float w1)
    {
        // quick rejection test
        if ((v1<w0) || (v0>w1)) return OUTSIDE;
        else if ((v0==w0) && (v1==w1)) return ISEQUAL;
        else if ((v0>=w0) && (v1<=w1)) return ISCONTAINED;
        else if ((v0<=w0) && (v1>=w1)) return CONTAINS;
        else return CLIPS;
    }

    int intersect(bbox3 box) {
        int and_code = 0xffff;
        int or_code  = 0;
        int cx,cy,cz;
        cx = line_test(vmin.x,vmax.x,box.vmin.x,box.vmax.x);
        and_code&=cx; or_code|=cx;
        cy = line_test(vmin.y,vmax.y,box.vmin.y,box.vmax.y);
        and_code&=cy; or_code|=cy;
        cz = line_test(vmin.z,vmax.z,box.vmin.z,box.vmax.z);
        and_code&=cz; or_code|=cz;
        if (or_code == 0) return OUTSIDE;
        else if (and_code != 0) {
            return and_code;
        } else {
            // only if all test produced a non-outside result,
            // an intersection has occured
            if (cx && cy && cz) return CLIPS;
            else                return OUTSIDE;
        }
    }
};

//-------------------------------------------------------------------
#endif
