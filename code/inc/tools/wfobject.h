#ifndef N_WFOBJECT_H
#define N_WFOBJECT_H
//-------------------------------------------------------------------
/**
    @class wfObject
    
    @brief new style wavefront object wrapper (uses STL)

    (C) 2000 RadonLabs GmbH -- A.Weissflog
*/
//-------------------------------------------------------------------

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#include <stdlib.h>
#include <assert.h>
#include <string.h>

//-------------------------------------------------------------------
//  A 3d coordinate with optional associated weight and joint index
//-------------------------------------------------------------------
class wfCoord 
{
public:
    vector3 v;      // the coordinate
    int ji[4];      // up to 4 joint indices (-1 if not initialized)
    char* jn[4];    // up to 4 joint names (0 if not initialized)
    float w[4];     // up to 4 weights

    wfCoord() 
    {
        int i;
        for (i=0; i<4; i++) 
        {
            ji[i] = -1;
            jn[i] = 0;
            w[i] = 0.0f;
        }
    }
    wfCoord(float x, float y, float z)
        : v(x,y,z)
    {
        int i;
        for (i=0; i<4; i++) 
        {
            ji[i] = -1;
            jn[i] = 0;
            w[i] = 0.0f;
        }
    };

    ~wfCoord()
    {
        for (int i = 0; i < 4; i++)
        {
            if (jn[i])
            {
                free(jn[i]);
            }
        }
    }

    void setJointIndexWeight(int _i, int _ji, float _w) 
    {
        assert((_i>=0) && (_i<4));
        ji[_i] = _ji;
        w[_i]  = _w;
    };

    void setJointNameWeight(int _i, const char* _jn, float _w) 
    {
        assert((_i>=0) && (_i<4));
        assert(_jn);
        jn[_i] = strdup(_jn);
        w[_i]  = _w;
    };

    void insertJointIndexWeight(int _ji, float _w)
    {
        // add weight if greater as any existing weight
        int i;
        for (i = 0; i < 4; i++)
        {
            if (_w > w[i])
            {
                // shift following values to back
                int j;
                for (j = 3; j > i; j--)
                {
                    w[j]  = w[j-1];
                    ji[j] = ji[j-1];
                }
                w[i]  = _w;
                ji[i] = _ji;
                break;
            }
        }
    }


    void insertJointNameWeight(const char* _jn, float _w)
    {
        // adds weight if greater as any existing weight
        assert(_jn);
        int i;
        for (i = 0; i < 4; i++)
        {
            if (_w > w[i])
            {
                // shift following values to back
                int j;
                for (j = 3; j > i; j--)
                {
                    w[j]  = w[j-1];
                    if (jn[j])
                    {
                        free(jn[j]);
                        jn[j] = 0;
                    }
                    jn[j] = jn[j-1];
                    jn[j-1] = 0;
                }
                w[i]   = _w;
                jn[i] = strdup(_jn);
                break;
            }
        }
    }

    void normalizeJointWeights()
    {
        // count number of valid weights
        int i;
        float wsum = 0.0f;
        for (i = 0; i < 4; i++)
        {
            wsum += w[i];
        }
        if (wsum > 0.0f)
        {
            for (i = 0; i < 4; i++)
            {
                w[i] /= wsum;
            }
        }
    }

    void save(FILE *fp) 
    {
        assert(fp);
        // save standard v statement
        fprintf(fp, "v %f %f %f\n",v.x,v.y,v.z);

        // save joint indices or joint names?
        if (jn[0])
        {
            // save joint names
            int num_jw = 0;
            int i;
            for (i=0; i<4; i++) 
            {
                if (jn[i] != 0) num_jw++;
            }
            if (num_jw > 0) 
            {
                switch (num_jw) 
                {
                    case 1:
                       fprintf(fp, "jwn1 %s %f\n",jn[0],w[0]);
                        break;
                    case 2:
                        fprintf(fp, "jwn2 %s %f %s %f\n",jn[0],w[0],jn[1],w[1]);
                        break;
                    case 3:
                        fprintf(fp, "jwn3 %s %f %s %f %s %f\n",jn[0],w[0],jn[1],w[1],jn[2],w[2]);
                        break;
                    default:
                        fprintf(fp, "jwn4 %s %f %s %f %s %f %s %f\n",jn[0],w[0],jn[1],w[1],jn[2],w[2],jn[3],w[3]);
                        break;
                }
            }
        } 
        else if (ji[0] != -1) 
        {
            // save joint indices
            int num_jw = 0;
            int i;
            for (i=0; i<4; i++) 
            {
                if (ji[i] != -1) num_jw++;
            }
            if (num_jw > 0) 
            {
                switch (num_jw) 
                {
                    case 1:
                       fprintf(fp, "jw1 %d %f\n",ji[0],w[0]);
                        break;
                    case 2:
                        fprintf(fp, "jw2 %d %f %d %f\n",ji[0],w[0],ji[1],w[1]);
                        break;
                    case 3:
                        fprintf(fp, "jw3 %d %f %d %f %d %f\n",ji[0],w[0],ji[1],w[1],ji[2],w[2]);
                        break;
                    default:
                        fprintf(fp, "jw4 %d %f %d %f %d %f %d %f\n",ji[0],w[0],ji[1],w[1],ji[2],w[2],ji[3],w[3]);
                        break;
                }
            }
        }            
    };
};

//-------------------------------------------------------------------
//  A face point, indexing into the various arrays.
//-------------------------------------------------------------------
class wfPoint 
{
public:
    int v_index;
    int vt_index;
    int vt1_index;
    int vt2_index;
    int vt3_index;
   int vn_index;
    int c_index;
    
    wfPoint()
        : v_index(-1),
          vt_index(-1),
          vt1_index(-1),
          vt2_index(-1),
          vt3_index(-1),
          vn_index(-1),
          c_index(-1)
    { };
    wfPoint(int vi, int vti, int vni, int ci)
        : v_index(vi),
          vt_index(vti),
          vt1_index(-1),
          vt2_index(-1),
          vt3_index(-1),
          vn_index(vni),
          c_index(ci)
    { };
    wfPoint(int vi, int vni, int ci, int vt, int vt1, int vt2, int vt3)
        : v_index(vi),
          vn_index(vni),
          c_index(ci),
          vt_index(vt),
          vt1_index(vt1),
          vt2_index(vt2),
          vt3_index(vt3)
    { };
    
    void save(FILE *fp) 
    {
        assert(fp);
        assert(v_index!=-1);
        fprintf(fp,"%d",v_index+1);
        if (vt_index>-1)                        fprintf(fp,"/%d",vt_index+1);
        else if ((vn_index>-1) || (c_index>-1)) fprintf(fp,"/");
        if (vn_index>-1)                        fprintf(fp,"/%d",vn_index+1);
        else if (c_index>-1)                    fprintf(fp,"/");
        if (c_index>-1)                         fprintf(fp,"/%d",c_index+1);
    }
    void save2(FILE *fp) 
    {
        assert(fp);
        assert(v_index!=-1);
        fprintf(fp,"%d/%d/%d/%d/%d/%d/%d",v_index+1,vn_index+1,c_index+1,
            vt_index+1,vt1_index+1,vt2_index+1,vt3_index+1);
    }
};

//-------------------------------------------------------------------
//  A face definition.
//-------------------------------------------------------------------
class wfFace 
{
public:    
    nArray<wfPoint> points;
    int validUVsets;

    wfFace() : 
        points(4,4),
        validUVsets(1)
    {}

    // save face definition to file
    void save(FILE *fp) 
    {
        nArray<wfPoint>::iterator cur;
        if (validUVsets > 1) 
        {
            fprintf(fp,"fx");
            for (cur = points.Begin(); cur != points.End(); cur++) 
            {
                fprintf(fp, " ");
                cur->save2(fp);
            }
        } 
        else 
        {
            fprintf(fp,"f");
            for (cur=points.Begin(); cur!=points.End(); cur++) 
            {
                fprintf(fp," ");
                cur->save(fp);
            }
        }
        fprintf(fp,"\n");
    }
};

//-------------------------------------------------------------------
//  A winged edge, made of 4 indices into vertex array.
//  The first two indices (v0 and v1) are the actual edge 
//  vertices, the next two vertices are used to describe the
//  2 triangles which are connected by the edge.
//-------------------------------------------------------------------
class wfWingedEdge 
{
public:
    wfWingedEdge() :
      v0(0), v1(0), vp0(0), vp1(0)
    {
        // empty
    }

    /// constructors
    wfWingedEdge(int _v0, int _v1, int _vp0, int _vp1) :
      v0(_v0), v1(_v1), vp0(_vp0), vp1(_vp1)
    {
        // empty
    }

    /// set contents
    void set(int _v0, int _v1, int _vp0, int _vp1)
    {
        v0  = _v0;
        v1  = _v1;
        vp0 = _vp0;
        vp1 = _vp1;
    }
            
    /// save winged edge to file
    void save(FILE* fp)
    {
        fprintf(fp, "we %d %d %d %d\n", v0, v1, vp0, vp1);
    }

    int v0, v1, vp0, vp1;
};

//-------------------------------------------------------------------
class wfObject {
public:
    nArray<wfCoord> v_array;        // vertex coordinates
    nArray<vector3> vn_array;       // vertex normals
    nArray<vector4> c_array;        // vertex colors
    nArray<vector2> vt_array;       // uv coordinates
    nArray<vector2> vt1_array;      // uv coordinates layer 1
    nArray<vector2> vt2_array;      // uv coordinates layer 2
    nArray<vector2> vt3_array;      // uv coordinates layer 3
    nArray<vector3> tangent_array;  // pervertex tangent-vector for pixel lighting

    nArray<wfFace>  f_array;        // face definitions

    nArray<wfWingedEdge> we_array;  // optional winged edges
    
    //--- constructor ---
    wfObject() :
        v_array(5000, 5000),
        vn_array(5000, 5000),
        c_array(5000, 5000),
        vt_array(5000, 5000),
        vt1_array(5000, 5000),
        vt2_array(5000, 5000),
        vt3_array(5000, 5000),
        tangent_array(5000, 5000),
        f_array(5000, 5000),
        we_array(5000, 5000)
    {
        // empty
    }

    //--- load from wavefront file ----------------------------------
    bool load(FILE *fp) 
    {
        assert(fp);
        char line[1024];
        int act_line  = 0;
        int act_jw = 0;
        while (fgets(line,sizeof(line),fp)) 
        {
            act_line++;
            char *kw = strtok(line, N_WHITESPACE);
            if (kw) 
            {
                if (strcmp(kw,"v")==0) 
                {
                    char *xs = strtok(NULL,N_WHITESPACE);
                    char *ys = strtok(NULL,N_WHITESPACE);
                    char *zs = strtok(NULL,N_WHITESPACE);
                    if (xs && ys && zs) 
                    {
                        float x = (float) atof(xs);
                        float y = (float) atof(ys);
                        float z = (float) atof(zs);
                        wfCoord v(x,y,z);
                        v_array.PushBack(v);
                    } else {
                        fprintf(stderr,"line %d: broken 'v' statement\n",act_line);
                        fclose(fp); return false;
                    }
                } 
                else if (strcmp(kw,"vn")==0) 
                {
                    char *xs = strtok(NULL,N_WHITESPACE);
                    char *ys = strtok(NULL,N_WHITESPACE);
                    char *zs = strtok(NULL,N_WHITESPACE);
                    if (xs && ys && zs) 
                    {
                        float x = (float) atof(xs);
                        float y = (float) atof(ys);
                        float z = (float) atof(zs);
                        vector3 v(x,y,z);
                        vn_array.PushBack(v);
                    } 
                    else 
                    {
                        fprintf(stderr,"line %d: broken 'vn' statement\n",act_line);
                        fclose(fp); return false;
                    }
                } 
                else if (strcmp(kw, "tangent") == 0)
                {
                    char* xs = strtok(NULL, N_WHITESPACE);
                    char* ys = strtok(NULL, N_WHITESPACE);
                    char* zs = strtok(NULL, N_WHITESPACE);
                    if (xs && ys && zs)
                    {
                        float x = (float) atof(xs);
                        float y = (float) atof(ys);
                        float z = (float) atof(zs);
                        vector3 v(x, y, z);
                        tangent_array.PushBack(v);
                    }
                    else
                    {
                        fprintf(stderr,"line %d: broken 'tangent' statement\n",act_line);
                        fclose(fp); return false;
                    }
                } 
                else if (strcmp(kw,"vt")==0) 
                {
                    char *xs = strtok(NULL,N_WHITESPACE);
                    char *ys = strtok(NULL,N_WHITESPACE);
                    if (xs && ys) 
                    {
                        float x = (float) atof(xs);
                        float y = (float) atof(ys);
                        vector2 v(x,y);
                        vt_array.PushBack(v);
                    } 
                    else 
                    {
                        fprintf(stderr,"line %d: broken 'vt' statement\n",act_line);
                        fclose(fp); return false;
                    }
                } 
                else if (strcmp(kw,"vt1")==0) 
                {
                    char *xs = strtok(NULL,N_WHITESPACE);
                    char *ys = strtok(NULL,N_WHITESPACE);
                    if (xs && ys) 
                    {
                        float x = (float) atof(xs);
                        float y = (float) atof(ys);
                        vector2 v(x,y);
                        vt1_array.PushBack(v);
                    } 
                    else 
                    {
                        fprintf(stderr,"line %d: broken 'vt1' statement\n",act_line);
                        fclose(fp); return false;
                    }
                } 
                else if (strcmp(kw,"vt2")==0) 
                {
                    char *xs = strtok(NULL,N_WHITESPACE);
                    char *ys = strtok(NULL,N_WHITESPACE);
                    if (xs && ys) 
                    {
                        float x = (float) atof(xs);
                        float y = (float) atof(ys);
                        vector2 v(x,y);
                        vt2_array.PushBack(v);
                    } 
                    else 
                    {
                        fprintf(stderr,"line %d: broken 'vt2' statement\n",act_line);
                        fclose(fp); return false;
                    }
                } 
                else if (strcmp(kw,"vt3")==0) 
                {
                    char *xs = strtok(NULL,N_WHITESPACE);
                    char *ys = strtok(NULL,N_WHITESPACE);
                    if (xs && ys) 
                    {
                        float x = (float) atof(xs);
                        float y = (float) atof(ys);
                        vector2 v(x,y);
                        vt3_array.PushBack(v);
                    } 
                    else 
                    {
                        fprintf(stderr,"line %d: broken 'vt3' statement\n",act_line);
                        fclose(fp); return false;
                    }
                } 
                else if (strcmp(kw,"rgba")==0) 
                {
                    char *rs = strtok(NULL, N_WHITESPACE);
                    char *gs = strtok(NULL, N_WHITESPACE);
                    char *bs = strtok(NULL, N_WHITESPACE);
                    char *as = strtok(NULL, N_WHITESPACE);
                    if (rs && gs && bs && as) 
                    {
                        float r = (float) atof(rs);
                        float g = (float) atof(gs);
                        float b = (float) atof(bs);
                        float a = (float) atof(as);
                        vector4 v(r,g,b,a);
                        c_array.PushBack(v);
                    } 
                    else 
                    {
                        fprintf(stderr,"line %d: broken 'rgba' statement\n",act_line);
                        fclose(fp); return false;
                    }
                } 
                else if (strcmp(kw,"jw1")==0) 
                {
                    char *j0s = strtok(NULL, N_WHITESPACE);
                    char *w0s = strtok(NULL, N_WHITESPACE);
                    if (j0s && w0s) 
                    {
                        int j0  = atoi(j0s);
                        float w0 = (float) atof(w0s);
                        assert(act_jw < int(this->v_array.Size()));
                        wfCoord& v = this->v_array.At(act_jw++);
                        v.setJointIndexWeight(0,j0,w0);
                    }
                } 
                else if (strcmp(kw,"jw2")==0) 
                {
                    char *j0s = strtok(NULL, N_WHITESPACE);
                    char *w0s = strtok(NULL, N_WHITESPACE);
                    char *j1s = strtok(NULL, N_WHITESPACE);
                    char *w1s = strtok(NULL, N_WHITESPACE);
                    if (j0s && w0s && j1s && w1s) 
                    {
                        int j0   = atoi(j0s);
                        float w0 = (float) atof(w0s);
                        int j1   = atoi(j1s);
                        float w1 = (float) atof(w1s);
                        assert(act_jw < int(this->v_array.Size()));
                        wfCoord& v = this->v_array.At(act_jw++);
                        v.setJointIndexWeight(0,j0,w0);
                        v.setJointIndexWeight(1,j1,w1);
                    }
                } 
                else if (strcmp(kw,"jw3")==0) 
                {
                    char *j0s = strtok(NULL, N_WHITESPACE);
                    char *w0s = strtok(NULL, N_WHITESPACE);
                    char *j1s = strtok(NULL, N_WHITESPACE);
                    char *w1s = strtok(NULL, N_WHITESPACE);
                    char *j2s = strtok(NULL, N_WHITESPACE);
                    char *w2s = strtok(NULL, N_WHITESPACE);
                    if (j0s && w0s && j1s && w1s && j2s && w2s) 
                    {
                        int j0   = atoi(j0s);
                        float w0 = (float) atof(w0s);
                        int j1   = atoi(j1s);
                        float w1 = (float) atof(w1s);
                        int j2   = atoi(j2s);
                        float w2 = (float) atof(w2s);
                        assert(act_jw < int(this->v_array.Size()));
                        wfCoord& v = this->v_array.At(act_jw++);
                        v.setJointIndexWeight(0,j0,w0);
                        v.setJointIndexWeight(1,j1,w1);
                        v.setJointIndexWeight(2,j2,w2);
                    }
                } 
                else if (strcmp(kw,"jw4")==0) 
                {
                    char *j0s = strtok(NULL, N_WHITESPACE);
                    char *w0s = strtok(NULL, N_WHITESPACE);
                    char *j1s = strtok(NULL, N_WHITESPACE);
                    char *w1s = strtok(NULL, N_WHITESPACE);
                    char *j2s = strtok(NULL, N_WHITESPACE);
                    char *w2s = strtok(NULL, N_WHITESPACE);
                    char *j3s = strtok(NULL, N_WHITESPACE);
                    char *w3s = strtok(NULL, N_WHITESPACE);
                    if (j0s && w0s && j1s && w1s && j2s && w2s && j3s && w3s) 
                    {
                        int j0   = atoi(j0s);
                        float w0 = (float) atof(w0s);
                        int j1   = atoi(j1s);
                        float w1 = (float) atof(w1s);
                        int j2   = atoi(j2s);
                        float w2 = (float) atof(w2s);
                        int j3   = atoi(j3s);
                        float w3 = (float) atof(w3s);
                        assert(act_jw < int(this->v_array.Size()));
                        wfCoord& v = this->v_array.At(act_jw++);
                        v.setJointIndexWeight(0,j0,w0);
                        v.setJointIndexWeight(1,j1,w1);
                        v.setJointIndexWeight(2,j2,w2);
                        v.setJointIndexWeight(3,j3,w3);
                    }
                } 
                else if (strcmp(kw,"f")==0) 
                {
                    char *p_str;
                    wfFace f;
                    while ((p_str = strtok(NULL,N_WHITESPACE))) 
                    {
                        char *slash;
                        int act_i  = 0;
                        int i[4];
                        memset(i,-1,sizeof(i));
                        do 
                        {
                            slash = strchr(p_str,'/');
                            if (slash) *slash=0;
                            if (*p_str) i[act_i] = atoi(p_str)-1;
                            act_i++;
                            p_str = slash+1;
                        } while (slash && (act_i<4));
                        wfPoint p(i[0],i[1],i[2],i[3]);
                        f.points.PushBack(p);
                        
                    }
                    f_array.PushBack(f);
                    }
                else if (strcmp(kw, "fx") == 0) 
                {
                    char *token;
                    wfFace face;
                    while ((token = strtok(NULL, N_WHITESPACE))) 
                    {
                        char *slash;
                        int value[7];
                        for (int i = 6; i > 0; i--) 
                        {
                            slash = strrchr(token, '/');
                            //n_assert(slash);
                            *slash = 0;
                            slash++;
                            value[i] = atoi(slash) - 1;
                        }
                        value[0] = atoi(token) - 1;
                        wfPoint point(value[0], value[1], value[2], value[3], value[4],
                            value[5], value[6]);
                        face.points.PushBack(point);
                    }
                    face.validUVsets = 4;
                    f_array.PushBack(face);
                } 
                else if (strcmp(kw, "we") == 0) 
                {
                    char* v0s  = strtok(NULL, N_WHITESPACE);
                    char* v1s  = strtok(NULL, N_WHITESPACE);
                    char* vp0s = strtok(NULL, N_WHITESPACE);
                    char* vp1s = strtok(NULL, N_WHITESPACE);
                    if (v0s && v1s && vp0s && vp1s)
                    {
                        int v0  = atoi(v0s);
                        int v1  = atoi(v1s);
                        int vp0 = atoi(vp0s);
                        int vp1 = atoi(vp1s);
                        wfWingedEdge we(v0, v1, vp0, vp1);
                        we_array.PushBack(we);
                    }
                    else
                    {
                        fprintf(stderr,"line %d: broken 'we' statement\n", act_line);
                        fclose(fp); 
                        return false;
                    }
                }
            }
        }
        return true;
    };

    //--- save v/vn/vt/rgba streams to wavefront file ---------------
    void save_v(FILE *fp) 
    {
        assert(fp);
        nArray<wfCoord>::iterator cur;
        for (cur=this->v_array.Begin(); cur!=this->v_array.End(); cur++) 
        {
            cur->save(fp);
        }
    };
    void save_vn(FILE *fp) 
    {
        assert(fp);
        nArray<vector3>::iterator cur;
        for (cur=this->vn_array.Begin(); cur!=this->vn_array.End(); cur++) 
        {
            fprintf(fp,"vn %f %f %f\n",cur->x,cur->y,cur->z);
        }
    };
    void save_tangent(FILE* fp)
    {
        n_assert(fp);
        nArray<vector3>::iterator cur;
        for (cur = this->tangent_array.Begin(); cur != this->tangent_array.End(); cur++)
        {
            fprintf(fp, "tangent %f %f %f\n", cur->x, cur->y, cur->z);
        }
    };
    void save_vt(FILE *fp) 
    {
        assert(fp);
        nArray<vector2>::iterator cur;
        for (cur=this->vt_array.Begin(); cur!=this->vt_array.End(); cur++) 
        {
            fprintf(fp,"vt %f %f\n",cur->x,cur->y);
        }
    };
    void save_vt1(FILE *fp) 
    {
        assert(fp);
        nArray<vector2>::iterator cur;
        for (cur=this->vt1_array.Begin(); cur!=this->vt1_array.End(); cur++) 
        {
            fprintf(fp,"vt1 %f %f\n",cur->x,cur->y);
        }
    };
    void save_vt2(FILE *fp) 
    {
        assert(fp);
        nArray<vector2>::iterator cur;
        for (cur=this->vt2_array.Begin(); cur!=this->vt2_array.End(); cur++) 
        {
            fprintf(fp,"vt2 %f %f\n",cur->x,cur->y);
        }
    };
    void save_vt3(FILE *fp) 
    {
        assert(fp);
        nArray<vector2>::iterator cur;
        for (cur=this->vt3_array.Begin(); cur!=this->vt3_array.End(); cur++) 
        {
            fprintf(fp,"vt3 %f %f\n",cur->x,cur->y);
        }
    };
    void save_rgba(FILE *fp) 
    {
        assert(fp);
        nArray<vector4>::iterator cur;
        for (cur=this->c_array.Begin(); cur!=this->c_array.End(); cur++) 
        {
            fprintf(fp,"rgba %f %f %f %f\n",cur->x,cur->y,cur->z,cur->w);
        }
    };
    void save_we(FILE* fp)
    {
        assert(fp);
        nArray<wfWingedEdge>::iterator cur;
        for (cur = this->we_array.Begin(); cur != this->we_array.End(); cur++)
        {
            fprintf(fp, "we %i %i %i %i\n", cur->v0, cur->v1, cur->vp0, cur->vp1);
        }
    }

    //--- save face definitions -------------------------------------
    void save_faces(FILE *fp) 
    {
        assert(fp);
        nArray<wfFace>::iterator cur;
        for (cur=this->f_array.Begin(); cur!=this->f_array.End(); cur++) 
        {
            cur->save(fp);
        }
    };

    //--- save entire object to wavefront file ----------------------
    void save(FILE *fp) 
    {
        assert(fp);
        this->save_v(fp);
        this->save_vt(fp);
        this->save_vt1(fp);
        this->save_vt2(fp);
        this->save_vt3(fp);
        this->save_vn(fp);
        this->save_rgba(fp);
        this->save_we(fp);
        this->save_tangent(fp);
        this->save_faces(fp);
    };
};
//-------------------------------------------------------------------
#endif
