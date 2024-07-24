#define N_IMPLEMENTS nFlatTerrainNode
//-------------------------------------------------------------------
//  nflatterrain_map.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nbmpfile.h"
#include "terrain/nflatterrainnode.h"

//-------------------------------------------------------------------
/**
    @brief Queues the welding of this terrain's south|east borders with the 
    arguments' north|west borders.
    
    If an argument is null, it is silently ignored.  This allows update
    of either or both with the same call.
  
    This defers the welding until the heightmap has been fully loaded
    and the 'first_run' updates are done to avoid incomplete terrains
    from being traversed

    17-Aug-00   WhiteGold created
    22-Dec-00   WhiteGold updated
*/
//-------------------------------------------------------------------
bool nFlatTerrainNode::Weld( nRoot *south, nRoot *east)
{
	this->e_weld = east;
	this->s_weld = south;

	return( true );
}

//-------------------------------------------------------------------
/**
    Welds this terrain's south|east borders with the arguments'
    north|west borders.  If an argument is null, it is silently
    ignored.  This allows update of either or both with the same
    call.

    17-Aug-00   WhiteGold created
    22-Dec-00	WhiteGold moved to WeldAux to defer welding until
  		  the terrain's are fully realized
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::WeldAux()
{
	if (this->s_weld)
	{
		nFlatTerrainNode *s = (nFlatTerrainNode *)this->s_weld;
		
		if ( !s->first_run && !s->map_dirty )
		{
			// neighbors - E, N, W, S
			this->neighbors[3] = &s->root_corner_data;
			s->neighbors[1] = &this->root_corner_data;
			this->root_node->AlignSouthBorders( this->root_corner_data, s->root_corner_data);
			
			this->s_weld = NULL;
		}
	}

	if (e_weld)
	{
		nFlatTerrainNode *e = (nFlatTerrainNode *)this->e_weld;
		
		if ( !e->first_run && !e->map_dirty )
		{
			// neighbors - E, N, W, S
			this->neighbors[0] = &e->root_corner_data;
			e->neighbors[2] = &this->root_corner_data;
			this->root_node->AlignEastBorders( this->root_corner_data, e->root_corner_data);

			this->e_weld = NULL;
		}
	}
}

//-------------------------------------------------------------------
/**
    @brief Find the height (interpolate if necessary) for the terrain at the
    given x/z.

    This is not the height value that you are expecting as it is in
    terrain coordinates.  You can get the height value in regular
    coordinates with the following:

    @code
    float heightmapMultiplier = (256*256) / terrain->GetRadius();
    vector3 terrain_pt = (inverseMatrix * p) * heightmapMultiplier;
    return terrain->GetHeight(terrain_pt.x, terrain_pt.z) / heightmapMultiplier;
    @endcode

    where <code>inverseMatrix</code> is the inverse of the transform
    matrix and <code>p</code> is the position at which you want to
    determine the height.

    17-Aug-00   WhiteGold created
*/
//-------------------------------------------------------------------
float nFlatTerrainNode::GetHeight( float x, float z)
{
	return( this->root_node->GetHeight( this->root_corner_data, x, z )); 
}


//-------------------------------------------------------------------
/**
    Kill the entire quadtree and the quadpool.

    27-Apr-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::free_tree(void)
{
    // delete old tree
    if (this->root_node) {
        this->root_node->Release(this);
        this->root_node = NULL;
    }

    // delete quad pool
    if (this->quad_pool) {
        delete[] this->quad_pool;
        this->quad_pool = NULL;
        this->free_quads = NULL;
    }
}

//-------------------------------------------------------------------
/**
    Do the job of (re-)allocating the quadpool and quadtree.

    27-Apr-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::allocate_tree(int num_quads)
{
    // kill previous stuff
    this->free_tree();
    
    // allocate and initialize new quad pool
    this->quad_pool = new flatquadsquare[num_quads];
    int i;
    for (i=0; i<num_quads-1; i++) {
        this->quad_pool[i].next = &(this->quad_pool[i+1]);
    }
    this->free_quads = this->quad_pool;

    // allocate new root tree
    this->root_node = &(this->quad_pool[this->AllocQuadSquare(&(this->root_corner_data))]);

    n_printf("%d quads allocated, sizeof(quad)=%d, used mem=%d\n",
             num_quads, sizeof(flatquadsquare), num_quads*sizeof(flatquadsquare));
}

//-------------------------------------------------------------------
/**
    Generate a HeightMapInfo object with data from bmp file.

    05-Apr-00   floh    created
*/
//-------------------------------------------------------------------
bool nFlatTerrainNode::fill_heightmap_from_bmp(const char *abs_path, FlatHeightMapInfo& hm, int& w, int& h)
{
    n_assert(abs_path);
    bool retval = false;
    nBmpFile bmp_file;
    if (bmp_file.Open(abs_path,"rb")) {
        // get width and height
        w = bmp_file.GetWidth();
        h = bmp_file.GetHeight();
        n_assert(w == h);

        // initialize HeightMapInfo data
        int i;
        int level;
        for (level=17, i=w; i; i>>=1, level--);
        hm.Set(0,0,w,h,level);

        // allocate a line buffer for pixelformat conversion
        uchar *line_buf = (uchar *) n_malloc(w*3+4);

        // pixelformat of file
        nPixelFormat *file_pf = bmp_file.GetPixelFormat();

        // pixelformat of line buffer
        nPixelFormat line_pf(24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);

        // load file line by line, pixelformat convert, use red component for height
        int act_y;
        uchar *l;
        line_pf.BeginConv(file_pf);
        while ((l = bmp_file.ReadLine(act_y))) {
            int x;
            line_pf.Conv(l,line_buf,w);
            
            // write line to HeightMapInfo
            for (x=0; x<w; x++) {
                uchar r  = line_buf[x*3];
                float rf = float(r) * (65536.0f/256.0f);
                hm.SetHeight(x,h-1-act_y,rf);
            }
        }
        line_pf.EndConv();
        n_free(line_buf);
        bmp_file.Close();
        retval = true;
    }
    return retval;
}


//-------------------------------------------------------------------
/**
    Load img heightfield and initialize quad tree from it.

    24-Mar-00   floh    created
    05-Apr-00   floh    shift by clip_height...
*/
//-------------------------------------------------------------------
bool nFlatTerrainNode::load_height_map(const char *abs_path)
{
    FlatHeightMapInfo hm;
    int w,h;
    if (this->fill_heightmap_from_bmp(abs_path,hm,w,h)) {
        
        // allocate new root node 
        int num_quads = 2*(w*h);
        this->allocate_tree(num_quads);

        // add height map to quad tree
        this->root_node->AddHeightMap(this,this->root_corner_data, hm);

        // fix the root vertices...
        int half  = 1<<this->root_corner_data.level;
        int whole = half<<1;
        int x0 = this->root_corner_data.xorg;
        int x2 = this->root_corner_data.xorg + whole;
        int z0 = this->root_corner_data.zorg;
        int z2 = this->root_corner_data.zorg + whole;

        // Properly set the corner verts from the heightmap data
		vector3 v((float)x2, hm.Sample(x2,z0), (float)z0);
		this->root_vertices[0].y = ushort(v.y);
		v.set((float)x0, hm.Sample(x0,z0), (float)z0);
		this->root_vertices[1].y = ushort(v.y);
		v.set((float)x0,hm.Sample(x0,z2), (float)z2);
		this->root_vertices[2].y = ushort(v.y);
		v.set((float)x2,hm.Sample(x2,z2), (float)z2);
		this->root_vertices[3].y = ushort(v.y);

		// Let's pretend the normals are right, which they're not,
		// but setting them to the alias edge's normal is bad too, 
		// so how about we set it at 0,1,0 - straight up? Better
		// than randomness.
		v.set(0.0f, 1.0f, 0.0f);
        this->root_vertices[0].n.pack( v );
        this->root_vertices[1].n.pack( v );
        this->root_vertices[2].n.pack( v );
        this->root_vertices[3].n.pack( v );


        x0>>=8; x2>>=8;
        z0>>=8; z2>>=8;

		this->root_vertices[0].u = x2;
		this->root_vertices[0].v = z0;

		this->root_vertices[1].u = x0;
		this->root_vertices[1].v = z0;

		this->root_vertices[2].u = x0;
		this->root_vertices[2].v = z2;

		this->root_vertices[3].u = x2;
		this->root_vertices[3].v = z2;

		// reduce detail in flat areas (FIXME: DETAILLEVEL)
		this->precull_nodes = this->root_node->CountNodes();
		n_printf("performing static culling with error '%f'\n",this->static_error);
    	n_printf("precull nodes     = %d\n", this->precull_nodes);
		this->root_node->StaticCullData(this,this->root_corner_data,this->static_error);
		this->postcull_nodes = this->root_node->CountNodes();
		n_printf("postcull nodes = %d\n", this->postcull_nodes);
    } else {
        n_printf("nTerrain::load_height_field(): could not open '%s'\n",abs_path);
        return false;
    }
    return true;
}

//-------------------------------------------------------------------
/**
    Load quad tree from tree file (saved by SaveTree()).

    26-Apr-00   floh    created
*/
//-------------------------------------------------------------------
bool nFlatTerrainNode::load_tree_file(const char *abs_path)
{
    // open tree file
    nFile* file = ks->GetFileServer2()->NewFileObject();
    if (file->Open(abs_path, "rb")) 
    {

		// Read Root Verts:
		// NOTE: FILE FORMAT CHANGE:  The first 4 floats in the file will be
		//                            the root corner Y values.  This will allow
		//                            the corners to be correct once saved and 
		//                            loaded as a tree
		for (int i=0; i<5; i++) {
			ushort y;
                       file->Read(&y, sizeof(y));
			this->root_vertices[i].y = y;
		}

        // read number of quads in tree
        int num_quads;
        file->Read(&num_quads,sizeof(num_quads));
        
        // allocate quadtree stuff
        num_quads = 2*num_quads;
        this->allocate_tree(num_quads);

        // skip header byte of root quad
        file->Seek(1, nFile::CURRENT);
        this->root_node->Load(this,this->root_corner_data,file);

        // fix the root vertices... This time with feeling
        int half  = 1<<this->root_corner_data.level;
        int whole = half<<1;
        int x0 = this->root_corner_data.xorg;
        int x2 = this->root_corner_data.xorg + whole;
        int z0 = this->root_corner_data.zorg;
        int z2 = this->root_corner_data.zorg + whole;
 
        x0>>=8; x2>>=8;
        z0>>=8; z2>>=8;

		this->root_vertices[0].u = x2;
		this->root_vertices[0].v = z0;

		this->root_vertices[1].u = x0;
		this->root_vertices[1].v = z0;

		this->root_vertices[2].u = x0;
		this->root_vertices[2].v = z2;

		this->root_vertices[3].u = x2;
		this->root_vertices[3].v = z2;

        file->Close();
        n_delete file;
        return true;
    } else {
        n_delete file;
        n_error("Could not load tree file '%s'!\n",abs_path);
        return false;
    }
}

//-------------------------------------------------------------------
/**
    Load height map either from img file or from nqs binary
    file. Set map_dirty to false when done.

    03-May-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::load_map(void)
{
    n_assert(this->map_dirty);
    const char *p = this->image_path.GetAbsPath();
    if (p) {
        // load terrain data from height map
        if (!this->load_height_map(p)) {
            n_error("nFlatTerrainNode: Could not load height map '%s'\n",
                      this->image_path.GetAbsPath());
        }
    } else {
        // load terrain data from tree file
        p = this->tree_path.GetAbsPath();
        if (!this->load_tree_file(p)) {
            n_error("nFlatTerrainNode: Could not load tree file '%s'\n",
                      this->tree_path.GetAbsPath());
        }
    }

    this->map_dirty = false;
    this->first_run = true;
}

//-------------------------------------------------------------------
/**
    @brief Save quadtree into private fileformat.

    This allows the terrain to be loaded rapidly and consume
    far less memory in the future.

    26-Apr-00   floh    created
*/
//-------------------------------------------------------------------
bool nFlatTerrainNode::SaveTree(const char *fname)
{
    // make sure everything is loaded...
    if (this->map_dirty) this->load_map();
    
    // save tree
    nFile* file = ks->GetFileServer2()->NewFileObject();
    if (file->Open(fname, "wb"))
    {
		// Save Root Verts OR LOSE THEM FOREVER
		// NOTE: FILE FORMAT CHANGE:  The first 4 floats in the file will be
		//                            the root corner Y values.  This will allow
		//                            the corners to be correct once saved and 
		//                            loaded as a tree.  The rest of the file is
		//                            the same.
		for (int i=0; i<5; i++) {
			ushort y = (ushort) this->root_vertices[i].y;
                       file->Write(&y,sizeof(y));
		}
        this->root_node->Save(this,this->root_corner_data,file);
        file->Close();
        return true;    
    }
    return false;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

