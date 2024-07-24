#ifndef N_VECTOR_H
#define N_VECTOR_H
//-------------------------------------------------------------------
/**
    @class vector2
    @brief 2-dimensional float vector class with operators.
*/
/**
    @class vector3
    @brief 3-dimensional float vector class with operators.
*/
/**
    @class vector4
    @brief 4-dimensional float vector class with operators.
*/
//-------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "math/nmath.h"

//-------------------------------------------------------------------
//  vector2
//-------------------------------------------------------------------
class vector2 {
public:
    float x,y;

    //-- constructors -----------------------------------------------
    vector2() : x(0.0f), y(0.0f) {};
    vector2(const float _x, const float _y) : x(_x), y(_y) {};  
    vector2(const vector2& vec) : x(vec.x), y(vec.y) {};
    vector2(float *p) : x(p[0]), y(p[1]) {};

    //-- setting elements -------------------------------------------
    void set(const float _x, const float _y) {
        x = _x;
        y = _y;
    };

    //-- misc -------------------------------------------------------
    float len(void) const {
        return (float) n_sqrt(x*x+y*y);
    };
    float len_squared(void) const {
        return x*x+y*y;
    };
    void norm(void) {
        float l = len();
        if (l > TINY) {
            x /= l;
            y /= l;
        } else {
            x = 0.0f;
            y = 1.0f;
        }
    };

    //-- math operators ---------------------------------------------
    vector2& operator+=(const vector2& v0) {
        x += v0.x;
        y += v0.y;
        return *this;
    };
    vector2& operator-=(const vector2& v0) {
        x -= v0.x;
        y -= v0.y;
        return *this;
    };
    vector2& operator*=(const float s) {
        x *= s;
        y *= s;
        return *this;
    };
    vector2& operator/=(const float s) {
        x /= s;
        y /= s;
        return *this;
    };

    //--- fuzzy compare operators -----------------------------------
    bool isequal(const vector2& v, const float tol) const
    {
        float dx = n_abs(v.x-x);
        float dy = n_abs(v.y-y);
        if (dx > tol)      return false;
        else if (dy > tol) return false;
        return true;
    };
};

static inline vector2 operator+(const vector2& v0, const vector2& v1) {
    return vector2(v0.x+v1.x, v0.y+v1.y); 
};
static inline vector2 operator-(const vector2& v0, const vector2& v1) {
    return vector2(v0.x-v1.x, v0.y-v1.y);
};
static inline vector2 operator*(const vector2& v0, const float s) {
    return vector2(v0.x*s, v0.y*s);
};
static inline vector2 operator/(const vector2& v0, const float s) {
    return vector2(v0.x/s, v0.y/s);
};
static inline vector2 operator-(const vector2& v) {
    return vector2(-v.x,-v.y);
};
//--- dot product ---------------------------------------------------
static inline float operator %(const vector2& v0, const vector2& v1) {
    return v0.x*v1.x + v0.y*v1.y;
};

//-------------------------------------------------------------------
//  vector3
//-------------------------------------------------------------------
class vector3 {
public:
    float x,y,z;

    //-- constructors -----------------------------------------------
    vector3() : x(0.0f), y(0.0f), z(0.0f) {};
    vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}; 
    vector3(const vector3& vec) : x(vec.x), y(vec.y), z(vec.z) {};
    vector3(float *p) : x(p[0]), y(p[1]), z(p[2]) {};

    //-- setting elements -------------------------------------------
    void set(float _x, float _y, float _z) {
        x=_x; y=_y; z=_z;
    };
    void set(const vector3& v) 
    {
        x = v.x; y = v.y; z = v.z;
    }

    //-- misc -------------------------------------------------------
    float len(void) const {
        return (float) n_sqrt(x*x+y*y+z*z);
    };
    float len_squared(void) const {
        return x*x+y*y+z*z;
    };
    void norm(void) {
        float l = len();
        if (l > TINY) {
            x /= l;
            y /= l;
            z /= l;
        } else {
            x = 0.0f;
            y = 1.0f;
            z = 0.0f;
        };
    };

    //-- math operators ---------------------------------------------
    vector3& operator +=(const vector3& v0) {
        x += v0.x;
        y += v0.y;
        z += v0.z;
        return *this;
    };
    vector3& operator -=(const vector3& v0) {
        x -= v0.x;
        y -= v0.y;
        z -= v0.z;
        return *this;
    };
    vector3& operator *=(float s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    };
    vector3& operator /=(float s) {
        float one_over_s = 1.0f/s;
        x *= one_over_s;
        y *= one_over_s;
        z *= one_over_s;
        return *this;
    };

    //--- fuzzy compare operators -----------------------------------
    bool isequal(const vector3& v, float tol) const
    {
        if (fabs(v.x-x) > tol)      return false;
        else if (fabs(v.y-y) > tol) return false;
        else if (fabs(v.z-z) > tol) return false;
        return true;
    };
};

static inline vector3 operator +(const vector3& v0, const vector3& v1) {
    return vector3(v0.x+v1.x, v0.y+v1.y, v0.z+v1.z);
};
static inline vector3 operator -(const vector3& v0, const vector3& v1) {
    return vector3(v0.x-v1.x, v0.y-v1.y, v0.z-v1.z);
};
static inline vector3 operator *(const vector3& v0, const float s) {
    return vector3(v0.x*s, v0.y*s, v0.z*s);
};
static inline vector3 operator /(const vector3& v0, const float s) {
    float one_over_s = 1.0f/s;
    return vector3(v0.x*one_over_s, v0.y*one_over_s, v0.z*one_over_s);
};
static inline vector3 operator-(const vector3& v) {
    return vector3(-v.x,-v.y,-v.z);
};

//--- dot product ---------------------------------------------------
static inline float operator %(const vector3& v0, const vector3& v1) {
    return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z;
};
//--- cross product -------------------------------------------------
static inline vector3 operator *(const vector3& v0, const vector3& v1) {
    return vector3(v0.y*v1.z - v0.z*v1.y,
                   v0.z*v1.x - v0.x*v1.z,
                   v0.x*v1.y - v0.y*v1.x);
};

//--- rotating a vector3 around another vector3 ---------------------
// taken from the now obsolete and removed nVector3 class
static inline void n_vec_rot_round_vec( vector3 *vec, float *m )
{
    vector3 v;
    v = (*vec);

    vec->x = m[0] * v.x + m[1] * v.y + m[2] * v.z;
    vec->y = m[3] * v.x + m[4] * v.y + m[5] * v.z;
    vec->z = m[6] * v.x + m[7] * v.y + m[8] * v.z;
};

//-------------------------------------------------------------------
class vector4 {
public:
    float x,y,z,w;

    //-- constructors -----------------------------------------------
    vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {};
    vector4(const float _x, const float _y, const float _z, const float _w) : x(_x), y(_y), z(_z), w(_w) {}; 
    vector4(const vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {};
    vector4(float *p) : x(p[0]), y(p[1]), z(p[2]), w(p[3]) {};

    //-- setting elements -------------------------------------------
    void set(const float _x, const float _y, const float _z, const float _w) {
        x=_x; y=_y; z=_z; w=_w;
    };

    //-- misc -------------------------------------------------------
    float len(void) const {
        return (float) n_sqrt(x*x+y*y+z*z+w*w);
    };
    void norm(void) {
        float l = len();
        if (l > 0.0f) {
            x /= l;
            y /= l;
            z /= l;
            w /= l;
        };
    };

    //-- math operators ---------------------------------------------
    vector4& operator+=(const vector4& v) {
        x+=v.x; y+=v.y; z+=v.z; w+=v.w;
        return *this;
    };
    vector4& operator-=(const vector4& v) {
        x-=v.x; y-=v.y; z-=v.z; w-=v.w;
        return *this;
    };
    vector4& operator*=(const float s) {
        x*=s; y*=s; z*=s; w*=s;
        return *this;
    };
    vector4& operator/=(const float s) {
        float one_over_s = 1.0f/s;
        x*=one_over_s; y*=one_over_s; z*=one_over_s; w*=one_over_s;
        return *this;
    };

    //--- fuzzy compare operators -----------------------------------
    bool isequal(const vector4& v, float tol) const
    {
        if (fabs(v.x-x) > tol)      return false;
        else if (fabs(v.y-y) > tol) return false;
        else if (fabs(v.z-z) > tol) return false;
        else if (fabs(v.w-w) > tol) return false;
        return true;
    };
};

static inline vector4 operator+(const vector4& v0, const vector4& v1) {
    return vector4(v0.x+v1.x, v0.y+v1.y, v0.z+v1.z, v0.w+v1.w);
};
static inline vector4 operator-(const vector4& v0, const vector4& v1) {
    return vector4(v0.x-v1.x, v0.y-v1.y, v0.z-v1.z, v0.w-v1.w);
};
static inline vector4 operator*(const vector4& v0, const float& s) {
    return vector4(v0.x*s, v0.y*s, v0.z*s, v0.w*s);
};
static inline vector4 operator/(const vector4& v0, const float& s) {
    float one_over_s = 1.0f/s;
    return vector4(v0.x*one_over_s, v0.y*one_over_s, v0.z*one_over_s, v0.w*one_over_s);
};
static inline vector4 operator-(const vector4& v)
{
    return vector4(v.x,v.y,v.z,v.w);
};

//-------------------------------------------------------------------
//  TODO: vector4, vector3 operators!
//-------------------------------------------------------------------

//-------------------------------------------------------------------
#endif
