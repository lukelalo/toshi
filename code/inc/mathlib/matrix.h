#ifndef N_MATRIX_H
#define N_MATRIX_H
//-------------------------------------------------------------------
/**
    @class matrix33
    @brief 3x3 matrix
*/
/**
    @class matrix44
    @brief 4x4 matrix
*/
//-------------------------------------------------------------------

#include <string.h>

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_QUATERNION_H
#include "mathlib/quaternion.h"
#endif

#ifndef N_EULERANGLES_H
#include "math/eulerangles.h"
#endif


static float m33_ident[9] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
};

static float m44_ident[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

#define M11 m[0][0]
#define M12 m[0][1]
#define M13 m[0][2]
#define M14 m[0][3]
#define M21 m[1][0]
#define M22 m[1][1]
#define M23 m[1][2]
#define M24 m[1][3]
#define M31 m[2][0]
#define M32 m[2][1]
#define M33 m[2][2]
#define M34 m[2][3]
#define M41 m[3][0]
#define M42 m[3][1]
#define M43 m[3][2]
#define M44 m[3][3]

class matrix33;
class matrix44;
static matrix33 operator * (const matrix33& m0, const matrix33& m1);

//-------------------------------------------------------------------
class matrix33 {
public:
    float m[3][3];

    //-- constructors -----------------------------------------------
    matrix33() {
        memcpy(&(m[0][0]),m33_ident,sizeof(m33_ident));
    };
    matrix33(const vector3& v0, const vector3& v1, const vector3& v2) {
        M11=v0.x; M12=v0.y; M13=v0.z;
        M21=v1.x; M22=v1.y; M23=v1.z;
        M31=v2.x; M32=v2.y; M33=v2.z;
    };
    matrix33(const matrix33& m1) {
        memcpy(m, &(m1.m[0][0]), 9*sizeof(float));
    };
    matrix33(float *p) {
        memcpy(m, p, 9*sizeof(float));
    };
    matrix33(float _m11, float _m12, float _m13,
             float _m21, float _m22, float _m23,
             float _m31, float _m32, float _m33)
    {
        M11=_m11; M12=_m12; M13=_m13;
        M21=_m21; M22=_m22; M23=_m23;
        M31=_m31; M32=_m32; M33=_m33;
    };
    matrix33(const quaternion& q) {
        float xx = q.x*q.x; float yy = q.y*q.y; float zz = q.z*q.z;
        float xy = q.x*q.y; float xz = q.x*q.z; float yz = q.y*q.z;
        float wx = q.w*q.x; float wy = q.w*q.y; float wz = q.w*q.z;

        m[0][0] = 1.0f - 2.0f * (yy + zz);
        m[1][0] =        2.0f * (xy - wz);
        m[2][0] =        2.0f * (xz + wy);

        m[0][1] =        2.0f * (xy + wz);
        m[1][1] = 1.0f - 2.0f * (xx + zz);
        m[2][1] =        2.0f * (yz - wx);

        m[0][2] =        2.0f * (xz - wy);
        m[1][2] =        2.0f * (yz + wx);
        m[2][2] = 1.0f - 2.0f * (xx + yy);
    };

    /// get quaternion representation
    quaternion get_quaternion(void) const {
        float qa[4];
        float tr = m[0][0] + m[1][1] + m[2][2];
        if (tr > 0.0f) {
            float s = n_sqrt (tr + 1.0f);
            qa[3] = s * 0.5f;
            s = 0.5f / s;
            qa[0] = (m[1][2] - m[2][1]) * s;
            qa[1] = (m[2][0] - m[0][2]) * s;
            qa[2] = (m[0][1] - m[1][0]) * s;
        } else {
            int i, j, k, nxt[3] = {1,2,0};
            i = 0;
            if (m[1][1] > m[0][0]) i=1;
            if (m[2][2] > m[i][i]) i=2;
            j = nxt[i];
            k = nxt[j];
            float s = n_sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);
            qa[i] = s * 0.5f;
            s = 0.5f / s;
            qa[3] = (m[j][k] - m[k][j])* s;
            qa[j] = (m[i][j] + m[j][i]) * s;
            qa[k] = (m[i][k] + m[k][i]) * s;
        }
        quaternion q(qa[0],qa[1],qa[2],qa[3]);
        return q;
    };

    vector3 to_euler() const {
    
        vector3 ea;
        int i,j,k,h,n,s,f;
        EulGetOrd(EulOrdXYZs,i,j,k,h,n,s,f);
        if (s==EulRepYes) {
            double sy = (float) n_sqrt(M12*M12 + M13*M13);
            if (sy > 16*FLT_EPSILON) {
                ea.x = (float) atan2(M12, M13);
                ea.y = (float) atan2((float)sy, M11);
                ea.z = (float) atan2(M21, -M31);
            } else {
                ea.x = (float) atan2(-M23, M22);
                ea.y = (float) atan2((float)sy, M11);
                ea.z = 0;
            }
        } else {
            double cy = n_sqrt(M11*M11 + M21*M21);
            if (cy > 16*FLT_EPSILON) {
                ea.x = (float) atan2(M32, M33);
                ea.y = (float) atan2(-M31, (float)cy);
                ea.z = (float) atan2(M21, M11);
            } else {
                ea.x = (float) atan2(-M23, M22);
                ea.y = (float) atan2(-M31, (float)cy);
                ea.z = 0;
            }
        }
        if (n==EulParOdd) {ea.x = -ea.x; ea.y = - ea.y; ea.z = -ea.z;}
        if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}

        return ea;
    };

    void from_euler(const vector3& ea) {
        vector3 tea = ea;
        double ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
        int i,j,k,h,n,s,f;
        EulGetOrd(EulOrdXYZs,i,j,k,h,n,s,f);
        if (f==EulFrmR) {float t = ea.x; tea.x = ea.z; tea.z = t;}
        if (n==EulParOdd) {tea.x = -ea.x; tea.y = -ea.y; tea.z = -ea.z;}
        ti = tea.x;   tj = tea.y;   th = tea.z;
        ci = n_cos(ti); cj = n_cos(tj); ch = n_cos(th);
        si = n_sin(ti); sj = n_sin(tj); sh = n_sin(th);
        cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
        if (s==EulRepYes) {
            M11 = (float)(cj);     M12 = (float)(sj*si);     M13 = (float)(sj*ci);
            M21 = (float)(sj*sh);  M22 = (float)(-cj*ss+cc); M23 = (float)(-cj*cs-sc);
            M31 = (float)(-sj*ch); M23 = (float)( cj*sc+cs); M33 = (float)( cj*cc-ss);
        } else {
            M11 = (float)(cj*ch); M12 = (float)(sj*sc-cs); M13 = (float)(sj*cc+ss);
            M21 = (float)(cj*sh); M22 = (float)(sj*ss+cc); M23 = (float)(sj*cs-sc);
            M31 = (float)(-sj);   M32 = (float)(cj*si);    M33 = (float)(cj*ci);
        }
    };

    /**
        @brief free lookat function 

    @param from Position of viewpoint.
    @param to   Position to look at.
    @param up   Up vector.
    */
    void lookat(const vector3& from, const vector3& to, const vector3& up) {

        vector3 z(from - to);
        z.norm();
        vector3 y(up);
        vector3 x(y * z);   // x = y cross z
        y = z * x;      // y = z cross x
        x.norm();
        y.norm();

        M11=x.x;  M12=x.y;  M13=x.z;  
        M21=y.x;  M22=y.y;  M23=y.z;  
        M31=z.x;  M32=z.y;  M33=z.z;  
    };

    /**
        @brief restricted lookat function

    @param from Position of viewpoint.
    @param to   Position to look at.
    @param up   Up vector.
    */
    void billboard(const vector3& from, const vector3& to, const vector3& up)
    {
        vector3 z(from - to);
        z.norm();
        vector3 y(up);
        vector3 x(y * z);
        z = x * y;       
        x.norm();
        y.norm();
        z.norm();

        M11=x.x;  M12=x.y;  M13=x.z;  
        M21=y.x;  M22=y.y;  M23=y.z;  
        M31=z.x;  M32=z.y;  M33=z.z;  
    };

    //-- misc -------------------------------------------------------
    void set(float m11, float m12, float m13,
             float m21, float m22, float m23,
             float m31, float m32, float m33) 
    {
        M11=m11; M12=m12; M13=m13;
        M21=m21; M22=m22; M23=m23;
        M31=m31; M32=m32; M33=m33;
    };
    void set(const vector3& v0, const vector3& v1, const vector3& v2) {
        M11=v0.x; M12=v0.y; M13=v0.z;
        M21=v1.x; M22=v1.y; M23=v1.z;
        M31=v2.x; M32=v2.y; M33=v2.z;
    };
    void set(const matrix33& m1) {
        memcpy(m, &(m1.m), 9*sizeof(float));
    };
    void set(const matrix44& m1);
    void set(float *p) {
        memcpy(m, p, 9*sizeof(float));
    };
    void ident(void) {
        memcpy(&(m[0][0]),m33_ident,sizeof(m33_ident));
    };
    void transpose(void) {
        #define n_swap(x,y) { float t=x; x=y; y=t; }
        n_swap(m[0][1],m[1][0]);
        n_swap(m[0][2],m[2][0]);
        n_swap(m[1][2],m[2][1]);
    };
    bool orthonorm(float limit) {
        if (((M11*M21+M12*M22+M13*M23)<limit) &&
            ((M11*M31+M12*M32+M13*M33)<limit) &&
            ((M31*M21+M32*M22+M33*M23)<limit) &&
            ((M11*M11+M12*M12+M13*M13)>(1.0-limit)) &&
            ((M11*M11+M12*M12+M13*M13)<(1.0+limit)) &&
            ((M21*M21+M22*M22+M23*M23)>(1.0-limit)) &&
            ((M21*M21+M22*M22+M23*M23)<(1.0+limit)) &&
            ((M31*M31+M32*M32+M33*M33)>(1.0-limit)) &&
            ((M31*M31+M32*M32+M33*M33)<(1.0+limit)))
            return true;
        else
            return false;
    };
    void scale(const float x, const float y, const float z) {
        int i;
        for (i=0; i<3; i++) {
            m[i][0] *= x;
            m[i][1] *= y;
            m[i][2] *= z;
        }
    };

    /// rotate about global x
    void rotate_x(const float a)
    {
        float c = n_cos(a);
        float s = n_sin(a);
        int i;
        for (i=0; i<3; i++)
        {
            float mi1 = m[i][1];
            float mi2 = m[i][2];
            m[i][1] = mi1*c + mi2*-s;
            m[i][2] = mi1*s + mi2*c;
        }
    }

    /// rotates matrix about global y
    void rotate_y(const float a)
    {
        float c = n_cos(a);
        float s = n_sin(a);
        int i;
        for (i=0; i<3; i++)
        {
            float mi0 = m[i][0];
            float mi2 = m[i][2];
            m[i][0] = mi0*c + mi2*s;
            m[i][2] = mi0*-s + mi2*c;
        }
    }

    /// rotate about global z
    void rotate_z(const float a)
    {
        float c = n_cos(a);
        float s = n_sin(a);
        int i;
        for (i=0; i<3; i++)
        {
            float mi0 = m[i][0];
            float mi1 = m[i][1];
            m[i][0] = mi0*c + mi1*-s;
            m[i][1] = mi0*s + mi1*c;
        }
    }
    
    /// rotate about local x (not very fast)
    void rotate_local_x(const float a)
    {
        matrix33 rotM;  // initialized as identity matrix
        rotM.M22 = (float) n_cos(a); rotM.M23 = -(float) n_sin(a);
        rotM.M32 = (float) n_sin(a); rotM.M33 =  (float) n_cos(a);

        (*this) = rotM * (*this); 
    }

    /// rotate about local y (not very fast)
    void rotate_local_y(const float a)
    {
        matrix33 rotM;  // initialized as identity matrix
        rotM.M11 = (float) n_cos(a);  rotM.M13 = (float) n_sin(a);
        rotM.M31 = -(float) n_sin(a); rotM.M33 = (float) n_cos(a);

        (*this) = rotM * (*this); 
    }

    /// rotate about local z (not very fast)
    void rotate_local_z(const float a)
    {
        matrix33 rotM;  // initialized as identity matrix
        rotM.M11 = (float) n_cos(a); rotM.M12 = -(float) n_sin(a);
        rotM.M21 = (float) n_sin(a); rotM.M22 =  (float) n_cos(a);

        (*this) = rotM * (*this); 
    }

    /// rotate about any axis
    void rotate(const vector3& vec, float a)
    {
        vector3 v(vec);
        v.norm();
        float sa = (float) n_sin(a);
        float ca = (float) n_cos(a);

        matrix33 rotM;
        rotM.M11 = ca + (1.0f - ca) * v.x * v.x;
        rotM.M12 = (1.0f - ca) * v.x * v.y - sa * v.z;
        rotM.M13 = (1.0f - ca) * v.z * v.x + sa * v.y;
        rotM.M21 = (1.0f - ca) * v.x * v.y + sa * v.z;
        rotM.M22 = ca + (1.0f - ca) * v.y * v.y;
        rotM.M23 = (1.0f - ca) * v.y * v.z - sa * v.x;
        rotM.M31 = (1.0f - ca) * v.z * v.x - sa * v.y;
        rotM.M32 = (1.0f - ca) * v.y * v.z + sa * v.x;
        rotM.M33 = ca + (1.0f - ca) * v.z * v.z;
        
        (*this) = (*this) * rotM;
    }

    /// get x component as vector3
    vector3 x_component(void) const
    {
        vector3 v(M11,M12,M13);
        return v;
    };
    /// get y component as vector3
    vector3 y_component(void) const
    {
        vector3 v(M21,M22,M23);
        return v;
    };
    /// get z component as vector3
    vector3 z_component(void) const 
    {
        vector3 v(M31,M32,M33);
        return v;
    };

    // get scale as vector3
    vector3 get_scale(void) const 
    {
        vector3 v1(M11,M21,M31);
        vector3 v2(M12,M22,M32);
        vector3 v3(M13,M23,M33);
        vector3 v(v1.len(),v2.len(),v3.len());
        return v;
    };

    // compute matrix determinant
    float matrix33::det()
    {
        return ( M11 * ( M22 * M33 - M23 * M32 ) -
                 M12 * ( M21 * M33 - M23 * M31 ) +
                 M13 * ( M21 * M32 - M22 * M31 ) );
    };
    // matrix33 inversion
    void matrix33::invert()
    {
        float d;
        matrix33 H;
        
        H.M11 =   M22 * M33 - M23 * M32;
        H.M12 = -(M21 * M33 - M23 * M31);
        H.M13 =   M21 * M32 - M22 * M31;
        H.M21 = -(M12 * M33 - M13 * M32);
        H.M22 =   M11 * M33 - M13 * M31;
        H.M23 = -(M11 * M32 - M12 * M31);
        H.M31 =   M12 * M23 - M13 * M22;
        H.M32 = -(M11 * M23 - M13 * M21);
        H.M33 =   M11 * M22 - M12 * M21;

        H.transpose();

        d = det();
        if(d != 0) {

            d = 1 / d;
            M11 = H.M11 * d;
            M12 = H.M12 * d;
            M13 = H.M13 * d;
            M21 = H.M21 * d;
            M22 = H.M22 * d;
            M23 = H.M23 * d;
            M31 = H.M31 * d;
            M32 = H.M32 * d;
            M33 = H.M33 * d;
            }
    };



    //-- operators --------------------------------------------------
    matrix33& operator *= (const matrix33& m1) {
        int i;
        for (i=0; i<3; i++) {
            float mi0 = m[i][0];
            float mi1 = m[i][1];
            float mi2 = m[i][2];
            m[i][0] = mi0*m1.m[0][0] + mi1*m1.m[1][0] + mi2*m1.m[2][0];
            m[i][1] = mi0*m1.m[0][1] + mi1*m1.m[1][1] + mi2*m1.m[2][1];
            m[i][2] = mi0*m1.m[0][2] + mi1*m1.m[1][2] + mi2*m1.m[2][2];
        };
        return *this;
    };
    /// comparision with threshold
    bool isequal(const matrix33& other, float t) const {
        // first row
        if ((fabs(other.m[0][0] - m[0][0]) > t) ||
            (fabs(other.m[0][1] - m[0][1]) > t) ||
            (fabs(other.m[0][2] - m[0][2]) > t)) {

            return false;
        }
            
        // second row
        if ((fabs(other.m[1][0] - m[1][0]) > t) ||
            (fabs(other.m[1][1] - m[1][1]) > t) ||
            (fabs(other.m[1][2] - m[1][2]) > t)) {

            return false;
        }

        // third row
        if ((fabs(other.m[2][0] - m[2][0]) > t) ||
            (fabs(other.m[2][1] - m[2][1]) > t) ||
            (fabs(other.m[2][2] - m[2][2]) > t)) {

            return false;
        }
            
        return true;
    };
    /// multiply source vector with matrix and store in destination vector
    /// this eliminates the construction of a temp vector3 object
    void mult(const vector3& src, vector3& dst)
    {
        dst.x = M11*src.x + M21*src.y + M31*src.z;
        dst.y = M12*src.x + M22*src.y + M32*src.z;
        dst.z = M13*src.x + M23*src.y + M33*src.z;
    }
};

//-------------------------------------------------------------------
static inline matrix33 operator * (const matrix33& m0, const matrix33& m1) {
    matrix33 m2(
        m0.m[0][0]*m1.m[0][0] + m0.m[0][1]*m1.m[1][0] + m0.m[0][2]*m1.m[2][0],
        m0.m[0][0]*m1.m[0][1] + m0.m[0][1]*m1.m[1][1] + m0.m[0][2]*m1.m[2][1],
        m0.m[0][0]*m1.m[0][2] + m0.m[0][1]*m1.m[1][2] + m0.m[0][2]*m1.m[2][2],

        m0.m[1][0]*m1.m[0][0] + m0.m[1][1]*m1.m[1][0] + m0.m[1][2]*m1.m[2][0],
        m0.m[1][0]*m1.m[0][1] + m0.m[1][1]*m1.m[1][1] + m0.m[1][2]*m1.m[2][1],
        m0.m[1][0]*m1.m[0][2] + m0.m[1][1]*m1.m[1][2] + m0.m[1][2]*m1.m[2][2],

        m0.m[2][0]*m1.m[0][0] + m0.m[2][1]*m1.m[1][0] + m0.m[2][2]*m1.m[2][0],
        m0.m[2][0]*m1.m[0][1] + m0.m[2][1]*m1.m[1][1] + m0.m[2][2]*m1.m[2][1],
        m0.m[2][0]*m1.m[0][2] + m0.m[2][1]*m1.m[1][2] + m0.m[2][2]*m1.m[2][2]
    );
    return m2;
};

//-------------------------------------------------------------------
static inline vector3 operator * (const matrix33& m, const vector3& v)
{
    return vector3(
        m.M11*v.x + m.M21*v.y + m.M31*v.z,
        m.M12*v.x + m.M22*v.y + m.M32*v.z,
        m.M13*v.x + m.M23*v.y + m.M33*v.z);
};

//-------------------------------------------------------------------
class matrix44 {
public:
    float m[4][4];

    //-- constructors -----------------------------------------------
    matrix44() {
        memcpy(&(m[0][0]),m44_ident,sizeof(m44_ident));
    };
    matrix44(const vector4& v0, const vector4& v1, const vector4& v2, const vector4& v3) {
        M11=v0.x; M12=v0.y; M13=v0.z, M14=v0.w;
        M21=v1.x; M22=v1.y; M23=v1.z; M24=v1.w;
        M31=v2.x; M32=v2.y; M33=v2.z; M34=v2.w;
        M41=v3.x; M42=v3.y; M43=v3.z; M44=v3.w;
    };
    matrix44(const matrix44& m1) {
        memcpy(m, &(m1.m[0][0]), 16*sizeof(float));
    };
    matrix44(const matrix33& m1) {
        set( m1 );
        M41 = M42 = M43 = 0.0f;  M44=1.0f; 
    };
    matrix44(float *p) {
        memcpy(m, p, 16*sizeof(float));
    };
    matrix44(float _m11, float _m12, float _m13, float _m14,
             float _m21, float _m22, float _m23, float _m24,
             float _m31, float _m32, float _m33, float _m34,
             float _m41, float _m42, float _m43, float _m44)
    {
        M11=_m11; M12=_m12; M13=_m13; M14=_m14;
        M21=_m21; M22=_m22; M23=_m23; M24=_m24;
        M31=_m31; M32=_m32; M33=_m33; M34=_m34;
        M41=_m41; M42=_m42; M43=_m43; M44=_m44;
    };

    matrix44(const quaternion& q) {
        float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
        x2 = q.x + q.x; y2 = q.y + q.y; z2 = q.z + q.z;
        xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
        yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
        wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;

        m[0][0] = 1.0f - (yy + zz);
        m[1][0] = xy - wz;
        m[2][0] = xz + wy;

        m[0][1] = xy + wz;
        m[1][1] = 1.0f - (xx + zz);
        m[2][1] = yz - wx;

        m[0][2] = xz - wy;
        m[1][2] = yz + wx;
        m[2][2] = 1.0f - (xx + yy);

        m[3][0] = m[3][1] = m[3][2] = 0.0f;
        m[0][3] = m[1][3] = m[2][3] = 0.0f;
        m[3][3] = 1.0f;
    };

    matrix44( const vector3& from, const vector3& to, const vector3& up ) {
        vector3 z(from - to);
        z.norm();
        vector3 y(up);
        vector3 x(y * z);   // x = y cross z
        y = z * x;      // y = z cross x
        x.norm();
        y.norm();

        M11=x.x;  M12=x.y;  M13=x.z;  M14=0.0f;
        M21=y.x;  M22=y.y;  M23=y.z;  M24=0.0f;
        M31=z.x;  M32=z.y;  M33=z.z;  M34=0.0f;    
        M41 = from.x ; M42 = from.y ; M43 = from.z ; M44=1.0f; 
    }

    /**
        @brief convert orientation of 4x4 matrix into quaterion, 
        4x4 matrix may not be scaled!
    */
    quaternion get_quaternion(void) const {
        float qa[4];
        float tr = m[0][0] + m[1][1] + m[2][2];
        if (tr > 0.0f) {
            float s = n_sqrt (tr + 1.0f);
            qa[3] = s * 0.5f;
            s = 0.5f / s;
            qa[0] = (m[1][2] - m[2][1]) * s;
            qa[1] = (m[2][0] - m[0][2]) * s;
            qa[2] = (m[0][1] - m[1][0]) * s;
        } else {
            int i, j, k, nxt[3] = {1,2,0};
            i = 0;
            if (m[1][1] > m[0][0]) i=1;
            if (m[2][2] > m[i][i]) i=2;
            j = nxt[i];
            k = nxt[j];
            float s = n_sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);
            qa[i] = s * 0.5f;
            s = 0.5f / s;
            qa[3] = (m[j][k] - m[k][j])* s;
            qa[j] = (m[i][j] + m[j][i]) * s;
            qa[k] = (m[i][k] + m[k][i]) * s;
        }
        quaternion q(qa[0],qa[1],qa[2],qa[3]);
        return q;
    };

    //-- misc -------------------------------------------------------
    void set(const vector4& v0, const vector4& v1, const vector4& v2, const vector4& v3) {
        M11=v0.x; M12=v0.y; M13=v0.z, M14=v0.w;
        M21=v1.x; M22=v1.y; M23=v1.z; M24=v1.w;
        M31=v2.x; M32=v2.y; M33=v2.z; M34=v2.w;
        M41=v3.x; M42=v3.y; M43=v3.z; M44=v3.w;
    };
    void set(const matrix44& m1) {
        memcpy(m, &(m1.m[0][0]), 16*sizeof(float));
    };
    void set(const matrix33& m1) { //Add by Clockwise - Zeev Hadar
        for ( int i=0; i<3; i++ )   {
            memcpy(m[i], m1.m[i], 3*sizeof(float));
            m[i][3] = 0;
        }
    };
    void set(float *p) {
        memcpy(m, p, 16*sizeof(float));
    };
    void set(float _m11, float _m12, float _m13, float _m14,
             float _m21, float _m22, float _m23, float _m24,
             float _m31, float _m32, float _m33, float _m34,
             float _m41, float _m42, float _m43, float _m44)
    {
        M11=_m11; M12=_m12; M13=_m13; M14=_m14;
        M21=_m21; M22=_m22; M23=_m23; M24=_m24;
        M31=_m31; M32=_m32; M33=_m33; M34=_m34;
        M41=_m41; M42=_m42; M43=_m43; M44=_m44;
    };
    void set(const quaternion& q) {
        float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
        x2 = q.x + q.x; y2 = q.y + q.y; z2 = q.z + q.z;
        xx = q.x * x2;   xy = q.x * y2;   xz = q.x * z2;
        yy = q.y * y2;   yz = q.y * z2;   zz = q.z * z2;
        wx = q.w * x2;   wy = q.w * y2;   wz = q.w * z2;

        m[0][0] = 1.0f - (yy + zz);
        m[1][0] = xy - wz;
        m[2][0] = xz + wy;

        m[0][1] = xy + wz;
        m[1][1] = 1.0f - (xx + zz);
        m[2][1] = yz - wx;

        m[0][2] = xz - wy;
        m[1][2] = yz + wx;
        m[2][2] = 1.0f - (xx + yy);

        m[3][0] = m[3][1] = m[3][2] = 0.0f;
        m[0][3] = m[1][3] = m[2][3] = 0.0f;
        m[3][3] = 1.0f;
    };

    void ident(void) {
        memcpy(&(m[0][0]),m44_ident,sizeof(m44_ident));
    };
    void transpose(void) {
        #define n_swap(x,y) { float t=x; x=y; y=t; }
        n_swap(M12,M21);
        n_swap(M13,M31);
        n_swap(M14,M41);
        n_swap(M23,M32);
        n_swap(M24,M42);
        n_swap(M34,M43);
    };
    float det(void) {
        return
            (M11*M22 - M12*M21)*(M33*M44 - M34*M43)
           -(M11*M23 - M13*M21)*(M32*M44 - M34*M42)
           +(M11*M24 - M14*M21)*(M32*M43 - M33*M42)
           +(M12*M23 - M13*M22)*(M31*M44 - M34*M41)
           -(M12*M24 - M14*M22)*(M31*M43 - M33*M41)
           +(M13*M24 - M14*M23)*(M31*M42 - M32*M41);
    };
    void invert(void) {
        float s = det();
        if (s == 0.0) return;
        s = 1/s;
        this->set(
            s*(M22*(M33*M44 - M34*M43) + M23*(M34*M42 - M32*M44) + M24*(M32*M43 - M33*M42)),
            s*(M32*(M13*M44 - M14*M43) + M33*(M14*M42 - M12*M44) + M34*(M12*M43 - M13*M42)),
            s*(M42*(M13*M24 - M14*M23) + M43*(M14*M22 - M12*M24) + M44*(M12*M23 - M13*M22)),
            s*(M12*(M24*M33 - M23*M34) + M13*(M22*M34 - M24*M32) + M14*(M23*M32 - M22*M33)),
            s*(M23*(M31*M44 - M34*M41) + M24*(M33*M41 - M31*M43) + M21*(M34*M43 - M33*M44)),
            s*(M33*(M11*M44 - M14*M41) + M34*(M13*M41 - M11*M43) + M31*(M14*M43 - M13*M44)),
            s*(M43*(M11*M24 - M14*M21) + M44*(M13*M21 - M11*M23) + M41*(M14*M23 - M13*M24)),
            s*(M13*(M24*M31 - M21*M34) + M14*(M21*M33 - M23*M31) + M11*(M23*M34 - M24*M33)),
            s*(M24*(M31*M42 - M32*M41) + M21*(M32*M44 - M34*M42) + M22*(M34*M41 - M31*M44)),
            s*(M34*(M11*M42 - M12*M41) + M31*(M12*M44 - M14*M42) + M32*(M14*M41 - M11*M44)),
            s*(M44*(M11*M22 - M12*M21) + M41*(M12*M24 - M14*M22) + M42*(M14*M21 - M11*M24)),
            s*(M14*(M22*M31 - M21*M32) + M11*(M24*M32 - M22*M34) + M12*(M21*M34 - M24*M31)),
            s*(M21*(M33*M42 - M32*M43) + M22*(M31*M43 - M33*M41) + M23*(M32*M41 - M31*M42)),
            s*(M31*(M13*M42 - M12*M43) + M32*(M11*M43 - M13*M41) + M33*(M12*M41 - M11*M42)),
            s*(M41*(M13*M22 - M12*M23) + M42*(M11*M23 - M13*M21) + M43*(M12*M21 - M11*M22)),
            s*(M11*(M22*M33 - M23*M32) + M12*(M23*M31 - M21*M33) + M13*(M21*M32 - M22*M31)));
    };

    /**
        @brief inverts a 4x4 matrix consisting of a 3x3 rotation matrix and
        a translation (eg. everything that has [0,0,0,1] as the rightmost
    column) MUCH cheaper then a real 4x4 inversion
    */
    void invert_simple(void) {

        float s = det();
        if (s == 0.0f) return;
        s = 1.0f/s;
        this->set(
            s * ((M22 * M33) - (M23 * M32)),
            s * ((M32 * M13) - (M33 * M12)),
            s * ((M12 * M23) - (M13 * M22)),
            0.0f,
            s * ((M23 * M31) - (M21 * M33)),
            s * ((M33 * M11) - (M31 * M13)),
            s * ((M13 * M21) - (M11 * M23)),
            0.0f,
            s * ((M21 * M32) - (M22 * M31)),
            s * ((M31 * M12) - (M32 * M11)),
            s * ((M11 * M22) - (M12 * M21)),
            0.0f,
            s * (M21*(M33*M42 - M32*M43) + M22*(M31*M43 - M33*M41) + M23*(M32*M41 - M31*M42)),
            s * (M31*(M13*M42 - M12*M43) + M32*(M11*M43 - M13*M41) + M33*(M12*M41 - M11*M42)),
            s * (M41*(M13*M22 - M12*M23) + M42*(M11*M23 - M13*M21) + M43*(M12*M21 - M11*M22)),
            1.0f);
    };

    /// optimized multiplication, assumes that M14==M24==M34==0 AND M44==1
    void mult_simple(const matrix44& m1) {
        int i;
        for (i=0; i<4; i++) {
            float mi0 = m[i][0];
            float mi1 = m[i][1];
            float mi2 = m[i][2];
            m[i][0] = mi0*m1.m[0][0] + mi1*m1.m[1][0] + mi2*m1.m[2][0];
            m[i][1] = mi0*m1.m[0][1] + mi1*m1.m[1][1] + mi2*m1.m[2][1];
            m[i][2] = mi0*m1.m[0][2] + mi1*m1.m[1][2] + mi2*m1.m[2][2];
        }
        m[3][0] += m1.m[3][0];
        m[3][1] += m1.m[3][1];
        m[3][2] += m1.m[3][2];
        m[0][3] = 0.0f;
        m[1][3] = 0.0f;
        m[2][3] = 0.0f;
        m[3][3] = 1.0f;
    };
    
    /// Transforms a vector by the matrix, projecting the result back into w=1.
    vector3 transform_coord(const vector3 v) const
    {
        float d = 1.0f / (M14*v.x + M24*v.y + M34*v.z + M44);
        return vector3(
            (M11*v.x + M21*v.y + M31*v.z + M41) * d,
            (M12*v.x + M22*v.y + M32*v.z + M42) * d,
            (M13*v.x + M23*v.y + M33*v.z + M43) * d);
    };           

    /// return the x component as 3d vector
    vector3 x_component(void) const
    {
        vector3 v(M11,M12,M13);
        return v;
    };
    /// return the y component as 3d vector
    vector3 y_component(void) const
    {
        vector3 v(M21,M22,M23);
        return v;
    };
    /// return the z component as 3d vector
    vector3 z_component(void) const 
    {
        vector3 v(M31,M32,M33);
        return v;
    };
    /// return the position component as 3d vector
    vector3 pos_component(void) const 
    {
        vector3 v(M41,M42,M43);
        return v;
    };
    /// return the position component as 3d vector
    vector3 get_scale(void) const 
    {
        vector3 v1(M11,M12,M13);
        vector3 v2(M21,M22,M23);
        vector3 v3(M31,M32,M33);
        vector3 v(v1.len(),v2.len(),v3.len());
        return v;
    };

    /// Rotate about the global X axis.
    void rotate_x(const float a) {
        float c = n_cos(a);
        float s = n_sin(a);
        int i;
        for (i=0; i<4; i++) {
            float mi1 = m[i][1];
            float mi2 = m[i][2];
            m[i][1] = mi1*c + mi2*-s;
            m[i][2] = mi1*s + mi2*c;
        }
    };
    /// Rotate about the global Y axis.
    void rotate_y(const float a) {
        float c = n_cos(a);
        float s = n_sin(a);
        int i;
        for (i=0; i<4; i++) {
            float mi0 = m[i][0];
            float mi2 = m[i][2];
            m[i][0] = mi0*c + mi2*s;
            m[i][2] = mi0*-s + mi2*c;
        }
    };
    /// Rotate about the global Z axis.
    void rotate_z(const float a) {
        float c = n_cos(a);
        float s = n_sin(a);
        int i;
        for (i=0; i<4; i++) {
            float mi0 = m[i][0];
            float mi1 = m[i][1];
            m[i][0] = mi0*c + mi1*-s;
            m[i][1] = mi0*s + mi1*c;
        }
    };
    void set_translation(const vector3& t) {
        M41 = t.x;
        M42 = t.y;
        M43 = t.z;
    };

    void set_translation(const float x, const float y, const float z) {
        M41 = x;
        M42 = y;
        M43 = z;
    };

    void translate(const vector3& t) {
        M41 += t.x;
        M42 += t.y;
        M43 += t.z;
    };
    void translate(const float x, const float y, const float z) {
        M41 += x;
        M42 += y;
        M43 += z;
    };
    void scale(const vector3& s) {
        int i;
        for (i=0; i<4; i++) {
            m[i][0] *= s.x;
            m[i][1] *= s.y;
            m[i][2] *= s.z;
        }
    };
    void scale(const float x, const float y, const float z) {
        int i;
        for (i=0; i<4; i++) {
            m[i][0] *= x;
            m[i][1] *= y;
            m[i][2] *= z;
        }
    };

    /**
        @brief free lookat function

    @param to Position to look at.
    @param up Up vector.
    */
    void lookat(const vector3& to, const vector3& up) {

        vector3 from(M41,M42,M43);
        vector3 z(from - to);
        z.norm();
        vector3 y(up);
        vector3 x(y * z);   // x = y cross z
        y = z * x;      // y = z cross x
        x.norm();
        y.norm();

        M11=x.x;  M12=x.y;  M13=x.z;  M14=0.0f;
        M21=y.x;  M22=y.y;  M23=y.z;  M24=0.0f;
        M31=z.x;  M32=z.y;  M33=z.z;  M34=0.0f;
    };

    /**
        @brief restricted lookat function

    @param to Position to look at.
    @param up Up vector.
    */
    void billboard(const vector3& to, const vector3& up)
    {
        vector3 from(M41, M42, M43);
        vector3 z(from - to);
        z.norm();
        vector3 y(up);
        vector3 x(y * z);
        z = x * y;       
        x.norm();
        y.norm();
        z.norm();

        M11=x.x;  M12=x.y;  M13=x.z;  M14=0.0f;
        M21=y.x;  M22=y.y;  M23=y.z;  M24=0.0f;
        M31=z.x;  M32=z.y;  M33=z.z;  M34=0.0f;
    };

    //-- operators --------------------------------------------------
    matrix44& operator *= (const matrix44& m1) {
        int i;
        for (i=0; i<4; i++) {
            float mi0 = m[i][0];
            float mi1 = m[i][1];
            float mi2 = m[i][2];
            float mi3 = m[i][3];
            m[i][0] = mi0*m1.m[0][0] + mi1*m1.m[1][0] + mi2*m1.m[2][0] + mi3*m1.m[3][0];
            m[i][1] = mi0*m1.m[0][1] + mi1*m1.m[1][1] + mi2*m1.m[2][1] + mi3*m1.m[3][1];
            m[i][2] = mi0*m1.m[0][2] + mi1*m1.m[1][2] + mi2*m1.m[2][2] + mi3*m1.m[3][2];
            m[i][3] = mi0*m1.m[0][3] + mi1*m1.m[1][3] + mi2*m1.m[2][3] + mi3*m1.m[3][3];
        };
        return *this;
    };
};

//-------------------------------------------------------------------
static inline matrix44 operator * (const matrix44& m0, const matrix44& m1) {
    matrix44 m2(
        m0.m[0][0]*m1.m[0][0] + m0.m[0][1]*m1.m[1][0] + m0.m[0][2]*m1.m[2][0] + m0.m[0][3]*m1.m[3][0],
        m0.m[0][0]*m1.m[0][1] + m0.m[0][1]*m1.m[1][1] + m0.m[0][2]*m1.m[2][1] + m0.m[0][3]*m1.m[3][1],
        m0.m[0][0]*m1.m[0][2] + m0.m[0][1]*m1.m[1][2] + m0.m[0][2]*m1.m[2][2] + m0.m[0][3]*m1.m[3][2],
        m0.m[0][0]*m1.m[0][3] + m0.m[0][1]*m1.m[1][3] + m0.m[0][2]*m1.m[2][3] + m0.m[0][3]*m1.m[3][3],

        m0.m[1][0]*m1.m[0][0] + m0.m[1][1]*m1.m[1][0] + m0.m[1][2]*m1.m[2][0] + m0.m[1][3]*m1.m[3][0],
        m0.m[1][0]*m1.m[0][1] + m0.m[1][1]*m1.m[1][1] + m0.m[1][2]*m1.m[2][1] + m0.m[1][3]*m1.m[3][1],
        m0.m[1][0]*m1.m[0][2] + m0.m[1][1]*m1.m[1][2] + m0.m[1][2]*m1.m[2][2] + m0.m[1][3]*m1.m[3][2],
        m0.m[1][0]*m1.m[0][3] + m0.m[1][1]*m1.m[1][3] + m0.m[1][2]*m1.m[2][3] + m0.m[1][3]*m1.m[3][3],

        m0.m[2][0]*m1.m[0][0] + m0.m[2][1]*m1.m[1][0] + m0.m[2][2]*m1.m[2][0] + m0.m[2][3]*m1.m[3][0],
        m0.m[2][0]*m1.m[0][1] + m0.m[2][1]*m1.m[1][1] + m0.m[2][2]*m1.m[2][1] + m0.m[2][3]*m1.m[3][1],
        m0.m[2][0]*m1.m[0][2] + m0.m[2][1]*m1.m[1][2] + m0.m[2][2]*m1.m[2][2] + m0.m[2][3]*m1.m[3][2],
        m0.m[2][0]*m1.m[0][3] + m0.m[2][1]*m1.m[1][3] + m0.m[2][2]*m1.m[2][3] + m0.m[2][3]*m1.m[3][3],

        m0.m[3][0]*m1.m[0][0] + m0.m[3][1]*m1.m[1][0] + m0.m[3][2]*m1.m[2][0] + m0.m[3][3]*m1.m[3][0],
        m0.m[3][0]*m1.m[0][1] + m0.m[3][1]*m1.m[1][1] + m0.m[3][2]*m1.m[2][1] + m0.m[3][3]*m1.m[3][1],
        m0.m[3][0]*m1.m[0][2] + m0.m[3][1]*m1.m[1][2] + m0.m[3][2]*m1.m[2][2] + m0.m[3][3]*m1.m[3][2],
        m0.m[3][0]*m1.m[0][3] + m0.m[3][1]*m1.m[1][3] + m0.m[3][2]*m1.m[2][3] + m0.m[3][3]*m1.m[3][3]);
    return m2;
};
//-------------------------------------------------------------------
static inline vector3 operator * (const matrix44& m, const vector3& v)
{
    return vector3(
        m.M11*v.x + m.M21*v.y + m.M31*v.z + m.M41,
        m.M12*v.x + m.M22*v.y + m.M32*v.z + m.M42,
        m.M13*v.x + m.M23*v.y + m.M33*v.z + m.M43);
};
//-------------------------------------------------------------------
static inline vector4 operator * (const matrix44& m, const vector4& v)
{
    return vector4(
        m.M11*v.x + m.M21*v.y + m.M31*v.z + m.M41*v.w,
        m.M12*v.x + m.M22*v.y + m.M32*v.z + m.M42*v.w,
        m.M13*v.x + m.M23*v.y + m.M33*v.z + m.M43*v.w,
        m.M14*v.x + m.M24*v.y + m.M34*v.z + m.M44*v.w);
};
//-------------------------------------------------------------------
inline void matrix33::set(const matrix44& m1) {
    for ( int i=0; i<3; i++ )
    {
        memcpy(m[i], &(m1.m[i][0]), 3*sizeof(float));
    }
};
//-------------------------------------------------------------------
#endif
