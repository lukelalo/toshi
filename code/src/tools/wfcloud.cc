//------------------------------------------------------------------------------
//  wfcloud.cc
//
//  Does the offline processing of cloud data. This includes:
//
//  - computing the amount of light reaching a cloud particle from
//    several predefined directions
//
//  Input file format:
//  For each cloud "ball" there is one v and one vt statement. v is the
//  center of a ball, and the u texture coordinate defines the radius.
//
//  Output file format:
//  Additionally to the original data one vt1, vt2, vt3 statement
//  each statement will be generated with the following contents:
//
//  vt1.x   -> light coefficient for sun at top (0,+1,0)
//  vt1.y   -> light coefficient for sun at bottom (0,-1,0)
//  vt2.x   -> light coefficient for sun at south (0,0,+1)
//  vt2.y   -> light coefficient for sun at north (0,0,-1)
//  vt3.x   -> light coefficient for sun at east (+1,0,0)
//  vt3.y   -> light coefficient for sun at west (-1,0,0)
//
//  Reference:
//  A Simple Efficient Method for Realistic Animation of Clouds
//  Yoshinori Dobashi, Kazufumi Kaneda, 
//  Hideo Yamashita, Tsuyoshi Okita, Tomoyuki Nishita
//
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "math/nmath.h"
#include "tools/wfobject.h"
#include "tools/wftools.h"
#include "mathlib/matrix.h"

enum DIRECTION
{
    TOP    = 0,     // +y -> -y
    BOTTOM = 1,     // -y -> +y
    SOUTH  = 2,     // +z -> -z
    NORTH  = 3,     // -z -> +z
    EAST   = 4,     // +x -> -x
    WEST   = 5,     // -x -> +x
};

//------------------------------------------------------------------------------
/**
    The field function.

    @param  r       normalized distance to center of metaball
    @result         the field's inverse strength at radius r (between 1.0 and 0.0)
*/
double
field(double r, double R)
{
    double f;
    if (r > R)
    {
        f = 0.0;
    }
    else
    {
        n_assert(R > 0.0);
        double a  = r / R;
        double a2 = a * a;
        double a4 = a2 * a2;
        double a6 = a4 * a2;
        double hr = (-(4.0f / 9.0f) * a6) + ((17.0f / 9.0f) * a4) - ((22.0f / 9.0f) * a2) + 1.0f;
        f = hr * R;
    }
//    printf("field(%f, %f) = %f\n", r, R, f);
    return f;
}

//------------------------------------------------------------------------------
/**
    Compute the light reaching a point inside the cloud from one of
    6 preset sun light directions.
*/
double
getLightAt(const vector3& pos, float /*radius*/, wfObject& src, DIRECTION dir)
{
    // build a "sun" matrix from dir
    matrix44 sun;
    switch (dir)
    {
        case TOP:
            sun.rotate_x(n_deg2rad(+90.0f));
            break;
        case BOTTOM:
            sun.rotate_x(n_deg2rad(-90.0f));
            break;
        case SOUTH:
            // no rotate
            break;
        case NORTH:
            sun.rotate_y(n_deg2rad(180.0f));
            break;
        case EAST:
            sun.rotate_y(n_deg2rad(-90.0f));
            break;
        case WEST:
            sun.rotate_y(n_deg2rad(+90.0f));
            break;
    }

    // transform pos into sun space
    vector3 tPos = sun * pos;

    // start with a light value of 0 (that means ALL light reaches the position)
    // and add the field function of each cloud ball in front of
    // the position
    double light = 0.0;
    int num = src.v_array.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        // get (transformed) position and radius of current cloud ball
        const vector3& curPos = sun * src.v_array.At(i).v;
        float curRadius = src.vt_array.At(i).x;
        n_assert(curRadius > 0.0f);

        if (curPos.z > (tPos.z + 0.1f))
        {
            // get the radius in current ball's energy field
            vector3 dist = curPos - tPos;
            dist.z = 0.0f;
            double r = dist.len();
            double f = field(r, curRadius);
            light += f;
        }
    }

    return light;
}

//------------------------------------------------------------------------------
/**
*/
void
doit(wfObject& src, wfObject& dst)
{
    // copy v and vt array to dst
    dst.v_array = src.v_array;
    dst.vt_array = src.vt_array;
    
    // for each cloud ball...
    n_assert(src.v_array.Size() == src.vt_array.Size());
    int num = src.v_array.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        // get position and radius of current cloud ball
        const vector3& pos = src.v_array.At(i).v;
        float radius = src.vt_array.At(i).x;

//        printf("\nball at (%f, %f, %f):\n\n", pos.x, pos.y, pos.z);

        // get light reaching the cloud ball from preset sun light directions
        int j;
        float light[6];
        for (j = 0; j < 6; j++)
        {
            light[j] = (float) getLightAt(pos, radius, src, (DIRECTION) j);
//            printf("dir %d -> light %f\n", j, light[j]);
        }

        vector2 vt1(light[0], light[1]);
        vector2 vt2(light[2], light[3]);
        vector2 vt3(light[4], light[5]);
        dst.vt1_array.PushBack(vt1);
        dst.vt2_array.PushBack(vt2);
        dst.vt3_array.PushBack(vt3);
    }
}

//------------------------------------------------------------------------------
/**
    The main function
*/
int
main(int argc, char* argv[])
{
    bool help;
    long retval = 0;

    fprintf(stderr, "-> wfcloud\n");

    // get arguments
    help = wf_getboolarg(argc, argv, "-help");
    if (help)
    {
        fprintf(stderr, "wfcloud [-help]\n"
                "(C) 2001 RadonLabs GmbH\n"
                "Compute lighting parameters for cloud renderer.\n");
        return 0;
    }

    wfObject src;
    wfObject dst;

    FILE* in;
    FILE* out;
    if (!wf_openfiles(argc, argv, in, out))
    {
        fprintf(stderr, "file open failed!\n");
        retval = 10;
        goto ende;
    }

    // load source data
    fprintf(stderr, "loading...\n");
    if (!src.load(in))
    {
        fprintf(stderr,"load failed!\n");
        retval = 10;
        goto ende;
    }

    // do the cloud stuff
    doit(src, dst);

    // write result
    fprintf(stderr, "saving...\n");
    dst.save(out);

    wf_closefiles(in, out);

ende:
    fprintf(stderr, "<- wfcloud\n");
    return retval;
}

//----------------------------------------------------------====================
        


