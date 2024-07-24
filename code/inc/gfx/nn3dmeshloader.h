#ifndef N_N3DMESHLOADER_H
#define N_N3DMESHLOADER_H
//------------------------------------------------------------------------------
/**
    @class nN3dMeshLoader

    @brief Create vertex buffer, index buffer and shadow caster from n3d file.
    The n3d file format is a slightly extended Wavefront obj fileformat.

    There is also a binary format for loading vertex data which is
    implemented as nNvxMeshLoader.

    The following things are supported in an n3d file:

     - Vertex position
     - Vertex normals
     - Vertex colors
     - Up to 4 texture UV coordinates per vertex
     - Up to 4 joint weights per vertex
     - Winged edge
     - Face list

    The Nebula Device provides a set of tools for working with meshes and
    doing various sorts of post-processing.  This can be found in the
    following files:

     - nebula/code/inc/tools/wfobject.h
     - nebula/code/inc/tools/wftoolbox.h
     - nebula/code/src/tools/ *.cc

    The file format looks like:

<blockquote><pre>
command data data ...
</pre></blockquote>

    The commands correspond to the list of supported features above:

<blockquote>
<table border="0">
  <tr>
    <td><b>Command</b></td><td><b>Meaning</b></td><td><b>Optional</b></td>
  </tr>
  <tr>
    <td>v</td><td>vertex position</td><td>no</td>
  </tr>
  <tr>
    <td>vn</td><td>vertex normal</td><td>yes</td>
  </tr>
  <tr>
    <td>rgba</td><td>vertex color</td><td>yes</td>
  </tr>
  <tr>
    <td>vt</td><td>texture coordinate</td><td>yes</td>
  </tr>
  <tr>
    <td>vt1</td><td>texture coordinate layer 1</td><td>yes</td>
  </tr>
  <tr>
    <td>vt2</td><td>texture coordinate layer 2</td><td>yes</td>
  </tr>
  <tr>
    <td>vt3</td><td>texture coordinate layer 3</td><td>yes</td>
  </tr>
  <tr>
    <td>jw1</td><td>1 joint weight</td><td>yes</td>
  </tr>
  <tr>
    <td>jw2</td><td>2 joint weights</td><td>yes</td>
  </tr>
  <tr>
    <td>jw3</td><td>3 joint weights</td><td>yes</td>
  </tr>
  <tr>
    <td>jw4</td><td>4 joint weights</td><td>yes</td>
  </tr>
  <tr>
    <td>we</td><td>winged edge</td><td>yes</td>
  </tr>
  <tr>
    <td>f</td><td>face information</td><td>no</td>
  </tr>
</table>
</blockquote>

    <h4>vertex position</h4>

    <blockquote><code>
    v <i>float float float</i>
    </code></blockquote>

    <h4>vertex normal</h4>

    <blockquote><code>
    vn <i>float float float</i>
    </code></blockquote>

    <h4>vertex color</h4>

    <blockquote><code>
    rgba <i>float float float float</i>
    </code></blockquote>

    <h4>texture coordinates</h4>

    <blockquote><code>
    vt <i>float float</i> <br>
    vt1 <i>float float</i> <br>
    vt2 <i>float float</i> <br>
    vt3 <i>float float</i> <br>
    </code></blockquote>

    <h4>joint weights</h4>

    <blockquote><code>
    jw1 <i>int float</i> <br>
    jw2 <i>int float int float</i> <br>
    jw3 <i>int float int float int float</i> <br>
    jw4 <i>int float int float int float int float</i> <br>
    </code></blockquote>

    Joint weights are specified in pairs of ints and floats, with the first
    value being an integer representing a joint index, and the second being
    a float that represents the weight of that joint.

    <h4>winged edge</h4>

    <blockquote><code>
    we <i>int int int int</i>
    </code></blockquote>

    A winged edge is a pair of faces, connected by an edge.  The first
    two integers are the indices of the vertices that define the common
    edge.  The last two integers are the vertices that respectively
    define each of the joined faces.  This info is used for casting shadows.

    Winged edge information can be created by the <code>wfwedge</code>
    tool from the Nebula Device distribution.

    <h4>face definition</h4>

    <blockquote><code>
    f <i>int int int</i>

    *or*

    f <i>int/int int/int int/int</i>

    *or*

    f <i>int/int/int int/int/int int/int/int</i>

    *or*

    f <i>int/int/int/int int/int/int/int int/int/int/int</i>
    </code></blockquote>

    Face information is defined by integers which are indexes into the
    other data lists.  However, each face can have additional data
    attached to it, beyond just the vertex information.  Each form
    above simply adds further information, in the following order:

     - vertex position index
     - texture coordinate index
     - vertex normal index
     - vertex color index

    Note: Although the wf tools handle different indices for each 
    vertex element, the mesh rendering code expects the indices to 
    be the same eg. <code>f 1/1/1 2/2/2 3/3/3</code>. This can be 
    done by running the mesh file through <code>wfflatten</code>.

    For example, <code>1/2/3</code> would define a portion of a face
    with the vertex position index 1, the texture coordinate index 2,
    and the vertex normal 3.  The indexes are into the lists of data,
    as they were described previous within the data file.

    An easy example (from <code>nebula/doc/tutorial/object.n3d</code>)
    defining a triangular prism would be:

<blockquote><pre>
v -0.500000 -1.000000 -0.800000
v 1.000000 -1.000000 0.000000
v -0.500000 -1.000000 0.800000
v -0.500000 -1.000000 -0.800000
v -0.500000 -1.000000 0.800000
v 0.000000 1.000000 0.000000
v -0.500000 -1.000000 0.800000
v 1.000000 -1.000000 0.000000
v 0.000000 1.000000 0.000000
v 1.000000 -1.000000 0.000000
v -0.500000 -1.000000 -0.800000
v 0.000000 1.000000 0.000000
vn 0.000000 -1.000000 0.000000
vn 0.000000 -1.000000 0.000000
vn 0.000000 -1.000000 0.000000
vn -0.970142 0.242536 0.000000
vn -0.970142 0.242536 0.000000
vn -0.970142 0.242536 0.000000
vn 0.458079 0.229039 0.858898
vn 0.458079 0.229039 0.858898
vn 0.458079 0.229039 0.858898
vn 0.458079 0.229039 -0.858898
vn 0.458079 0.229039 -0.858898
vn 0.458079 0.229039 -0.858898
vt 0.915225 0.000000
vt 0.915225 1.000000
vt 0.084775 0.442906
vt 0.162732 0.261759
vt 0.837268 0.000000
vt 0.837268 1.000000
vt 0.226727 0.210004
vt 0.981732 0.145006
vt 0.683933 1.066420
vt 0.150715 0.000000
vt 0.849285 0.300000
vt 0.150715 1.000000
f 1/1/1 2/2/2 3/3/3
f 4/4/4 5/5/5 6/6/6
f 7/7/7 8/8/8 9/9/9
f 10/10/10 11/11/11 12/12/12
</pre></blockquote>

<h4>Constraints</h4>

    If there are extra bits of per-vertex information present, then
    that must be present for all vertices.

    Files must be flattened prior to being rendered by the
    <code>wfflatten</code> tool or the equivalent.


    (C) 2001 RadonLabs GmbH
*/
#ifndef N_MESHLOADER_H
#include "gfx/nmeshloader.h"
#endif

//------------------------------------------------------------------------------
class nN3dMeshLoader : public nMeshLoader
{
public:
    /// constructor
    nN3dMeshLoader(nKernelServer* ks);
    /// load a mesh file
    bool Load(const char* fileName);

private:
    /// fill index buffer with triangle list
    void createTriList(ushort* indices, int num_indices);
};

//------------------------------------------------------------------------------
/**
*/
inline
nN3dMeshLoader::nN3dMeshLoader(nKernelServer* ks) :
    nMeshLoader(ks)
{
    // empty
}

//------------------------------------------------------------------------------
#endif
