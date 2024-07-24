#ifndef N_PLANE_H
#define N_PLANE_H
//-------------------------------------------------------------------
/**
    @class plane
    @brief a plane in 3d space
*/
//-------------------------------------------------------------------
#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

//-------------------------------------------------------------------
class plane {
public:
    float a,b,c,d;

    plane() : a(0.0f), b(0.0f), c(0.0f), d(1.0f) {};
    plane(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) {};
    plane(const plane& p) : a(p.a), b(p.b), c(p.c), d(p.d) {};

    void set(float _a, float _b, float _c, float _d) {
        a = _a;
        b = _b;
        c = _c;
        d = _d;
    };

    //--- construct plane from 3 vectors ----------------------------
    void set(const vector3& v0, const vector3& v1, const vector3& v2) {
        vector3 cross((v2-v0) * (v1-v0));
        cross.norm();
        a = cross.x;
        b = cross.y;
        c = cross.z;
        d = -(a*v0.x + b*v0.y + c*v0.z);
    };
    plane(const vector3& v0, const vector3& v1, const vector3& v2) {
        set(v0,v1,v2);
    };

    //--- compute distance of point to plane ------------------------
    //  distance is 0.0 if point is on plane
    //
    float distance(const vector3& v) {
        return a*v.x + b*v.y + c*v.z + d;
    };

    //--- get plane normal ------------------------------------------
    vector3 normal(void) const {
        return vector3(a,b,c);
    };
    
    //--- get intersecting t of line with one-sided plane -----------
    //  return false if line is parallel to plane
    //
    bool intersect(const line3& l, float& t) {
        float f0 = a * l.b.x + b * l.b.y + c * l.b.z + d;
        float f1 = a * -l.m.x + b * -l.m.y + c * -l.m.z;
        if ((f1 < -0.0001f) || (f1 > 0.0001f))
        {
            t = f0 / f1;
            return true;
        }
        else
        {
            return false;
        }
    }

    //--- get intersection plane/plane intersection -----------------
    //  return false is planes are parallel
    bool intersect(plane& p, line3& l) {
        vector3 n0 = normal();
        vector3 n1 = p.normal();
        float n00 = n0 % n0;
        float n01 = n0 % n1;
        float n11 = n1 % n1;
        float det = n00*n11 - n01*n01;
        const float tol = 1e-06f;
        if (fabs(det) < tol) return false;
        else {
            float inv_det = 1.0f/det;
            float c0 = (n11*d   - n01*p.d) * inv_det;
            float c1 = (n00*p.d - n01*d)   * inv_det;
            l.m = n0 * n1;
            l.b = n0*c0 + n1*c1;
            return true;
        }
    };
};
//-------------------------------------------------------------------
#endif
