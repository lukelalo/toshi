#define N_IMPLEMENTS nTerrainNode
//-------------------------------------------------------------------
//  nterrain_map.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nbmpfile.h"
#include "terrain/nterrainnode.h"

//-------------------------------------------------------------------
//  free_tree()
//  Kill the entire quadtree and the quadpool.
//  27-Apr-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::free_tree(void)
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
//  allocate_tree()
//  Do the job of (re-)allocating the quadpool and quadtree.
//  27-Apr-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::allocate_tree(int num_quads)
{
    // kill previous stuff
    this->free_tree();
    
    // allocate and initialize new quad pool
    this->quad_pool = new quadsquare[num_quads];
    int i;
    for (i=0; i<num_quads-1; i++) {
        this->quad_pool[i].next = &(this->quad_pool[i+1]);
    }
    this->free_quads = this->quad_pool;

    // allocate new root tree
    this->root_node = &(this->quad_pool[this->AllocQuadSquare(&(this->root_corner_data))]);

    n_printf("%d quads allocated, sizeof(quad)=%d, used mem=%d\n",
             num_quads, sizeof(quadsquare), num_quads*sizeof(quadsquare));
}

//-------------------------------------------------------------------
//  fill_heightmap_from_bmp()
//  Generate a HeightMapInfo object with data from bmp file.
//  05-Apr-00   floh    created
//-------------------------------------------------------------------
bool nTerrainNode::fill_heightmap_from_bmp(const char *abs_path, HeightMapInfo& hm, int& w, int& h)
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
                ushort highByte = line_buf[x*3 + 1];
                ushort lowByte  = line_buf[x*3 + 2];
                ushort radius16 = (highByte<<8) | lowByte;
                float rf = float(radius16);
                hm.SetHeight(x,act_y,rf);
            }
        }
        line_pf.EndConv();
        n_free(line_buf);
        bmp_file.Close();
        retval = true;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Fill a HeightMapInfo object directly from a float array. Height values
    must be between 0.0f and 1.0f.

    @param  width           width of array
    @param  height          height of array (must be equal to w)
    @param  floatPointer    pointer to float values
    @param  heightMapInfo   reference of HeightMapInfo object to be filled
*/
void 
nTerrainNode::fillHeightMapFromFloatArray(int width, int height, float* floatPointer, HeightMapInfo& heightMapInfo)
{
    n_assert(floatPointer);
    n_assert(width == height);

    // initialize HeightMapInfo data
    int i;
    int level;
    for (level = 17, i = width; i; i >>= 1, level--);
    heightMapInfo.Set(0, 0, width, height, level);

    int y;
    for (y = 0; y < height; y++)
    {
        int x;
        for (x = 0; x < width; x++)
        {
            float val = floatPointer[y * width + x] * 65535.0f;
            heightMapInfo.SetHeight(x, y, val);
        }
    }
}

//-------------------------------------------------------------------
//  load_height_map()
//  Load bmp heightfield and initialize quad tree from it.
//  24-Mar-00   floh    created
//  05-Apr-00   floh    shift by clip_height...
//-------------------------------------------------------------------
bool nTerrainNode::load_height_map(const char *abs_path)
{
    HeightMapInfo hm;
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
        this->root_vertices[0].y    = this->root_node->vertex[1].y;
        this->root_vertices[0].n    = this->root_node->vertex[1].n;        
        this->root_vertices[0].cart = this->root_node->GetCartesian(x2,z0,this->root_vertices[0].y);
        this->root_vertices[1].y    = this->root_node->vertex[3].y;
        this->root_vertices[1].n    = this->root_node->vertex[3].n;        
        this->root_vertices[1].cart = this->root_node->GetCartesian(x0,z0,this->root_vertices[1].y);
        this->root_vertices[2].y    = this->root_node->vertex[3].y;
        this->root_vertices[2].n    = this->root_node->vertex[3].n;        
        this->root_vertices[2].cart = this->root_node->GetCartesian(x0,z2,this->root_vertices[2].y);
        this->root_vertices[3].y    = this->root_node->vertex[1].y;
        this->root_vertices[3].n    = this->root_node->vertex[1].n;        
        this->root_vertices[3].cart = this->root_node->GetCartesian(x2,z2,this->root_vertices[3].y);

        // align borders...
        this->root_node->AlignBorders(this->root_corner_data);

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

//------------------------------------------------------------------------------
/**
    Load height values from a 2d array of float values. Sets map_dirty to false
    when done.

    @param  width           width of array
    @param  height          height of array
    @param  floatPointer    pointer to start of array
*/
void
nTerrainNode::SetFloatArrayHeightMap(int width, int height, float* floatPointer)
{
    HeightMapInfo hm;
    this->fillHeightMapFromFloatArray(width, height, floatPointer, hm);
        
    // allocate new root node 
    int num_quads = 2 * (width * height);
    this->allocate_tree(num_quads);

    // add height map to quad tree
    this->root_node->AddHeightMap(this, this->root_corner_data, hm);

    // fix the root vertices...
    int half  = 1<<this->root_corner_data.level;
    int whole = half<<1;
    int x0 = this->root_corner_data.xorg;
    int x2 = this->root_corner_data.xorg + whole;
    int z0 = this->root_corner_data.zorg;
    int z2 = this->root_corner_data.zorg + whole;
    this->root_vertices[0].y    = this->root_node->vertex[1].y;
    this->root_vertices[0].n    = this->root_node->vertex[1].n;        
    this->root_vertices[0].cart = this->root_node->GetCartesian(x2,z0,this->root_vertices[0].y);
    this->root_vertices[1].y    = this->root_node->vertex[3].y;
    this->root_vertices[1].n    = this->root_node->vertex[3].n;        
    this->root_vertices[1].cart = this->root_node->GetCartesian(x0,z0,this->root_vertices[1].y);
    this->root_vertices[2].y    = this->root_node->vertex[3].y;
    this->root_vertices[2].n    = this->root_node->vertex[3].n;        
    this->root_vertices[2].cart = this->root_node->GetCartesian(x0,z2,this->root_vertices[2].y);
    this->root_vertices[3].y    = this->root_node->vertex[1].y;
    this->root_vertices[3].n    = this->root_node->vertex[1].n;        
    this->root_vertices[3].cart = this->root_node->GetCartesian(x2,z2,this->root_vertices[3].y);

    // align borders...
    this->root_node->AlignBorders(this->root_corner_data);

    // reduce detail in flat areas (FIXME: DETAILLEVEL)
    this->precull_nodes = this->root_node->CountNodes();
    n_printf("performing static culling with error '%f'\n",this->static_error);
    n_printf("precull nodes     = %d\n", this->precull_nodes);
    this->root_node->StaticCullData(this,this->root_corner_data,this->static_error);
    this->postcull_nodes = this->root_node->CountNodes();
    n_printf("postcull nodes = %d\n", this->postcull_nodes);

    this->map_dirty = false;
    this->first_run = true;
}

//-------------------------------------------------------------------
//  load_tree_file()
//  Load quad tree from tree file (saved by SaveTree()).
//  26-Apr-00   floh    created
//-------------------------------------------------------------------
bool nTerrainNode::load_tree_file(const char *abs_path)
{
    // open tree file
    nFile* file = ks->GetFileServer2()->NewFileObject();
    if (file->Open(abs_path, "rb"))
    {

        // read number of quads in tree
        int num_quads;
        file->Read(&num_quads,sizeof(num_quads));
        
        // allocate quadtree stuff
        num_quads = 2*num_quads;
        this->allocate_tree(num_quads);

        // skip header byte of root quad
        file->Seek(1, nFile::CURRENT);
        this->root_node->Load(this,this->root_corner_data,file);

        // fix the root vertices...
        int half  = 1<<this->root_corner_data.level;
        int whole = half<<1;
        int x0 = this->root_corner_data.xorg;
        int x2 = this->root_corner_data.xorg + whole;
        int z0 = this->root_corner_data.zorg;
        int z2 = this->root_corner_data.zorg + whole;
        this->root_vertices[0].y    = this->root_node->vertex[1].y;
        this->root_vertices[0].n    = this->root_node->vertex[1].n;        
        this->root_vertices[0].cart = this->root_node->GetCartesian(x2,z0,this->root_vertices[0].y);
        this->root_vertices[1].y    = this->root_node->vertex[3].y;
        this->root_vertices[1].n    = this->root_node->vertex[3].n;        
        this->root_vertices[1].cart = this->root_node->GetCartesian(x0,z0,this->root_vertices[1].y);
        this->root_vertices[2].y    = this->root_node->vertex[3].y;
        this->root_vertices[2].n    = this->root_node->vertex[3].n;        
        this->root_vertices[2].cart = this->root_node->GetCartesian(x0,z2,this->root_vertices[2].y);
        this->root_vertices[3].y    = this->root_node->vertex[1].y;
        this->root_vertices[3].n    = this->root_node->vertex[1].n;        
        this->root_vertices[3].cart = this->root_node->GetCartesian(x2,z2,this->root_vertices[3].y);

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
//  load_map()
//  Load height map either from bmp file or from nqs binary
//  file. Set map_dirty to false when done.
//  03-May-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::load_map(void)
{
    n_assert(this->map_dirty);
    const char *p = this->image_path.GetAbsPath();
    if (p) {
        // load terrain data from height map
        if (!this->load_height_map(p)) {
            n_error("nTerrainNode: Could not load height map '%s'\n",
                      this->image_path.GetAbsPath());
        }
    } else {
        // load terrain data from tree file
        p = this->tree_path.GetAbsPath();
        if (!this->load_tree_file(p)) {
            n_error("nTerrainNode: Could not load tree file '%s'\n",
                      this->tree_path.GetAbsPath());
        }
    }

    this->map_dirty = false;
    this->first_run = true;
}


//-------------------------------------------------------------------
//  SaveTree()
//  Save quadtree into private fileformat.
//  26-Apr-00   floh    created
//-------------------------------------------------------------------
bool nTerrainNode::SaveTree(const char *fname)
{
    // make sure everything is loaded...
    if (this->map_dirty) this->load_map();
    
    // save tree
    nFile* file = ks->GetFileServer2()->NewFileObject();
    if (file->Open(fname, "wb"))
    {
        this->root_node->Save(this,this->root_corner_data,file);
        file->Close();
        n_delete file;
        return true;    
    } else {
        n_printf("nTerrainNode::SaveTree(): could not open file '%s' for writing!\n",fname);
    }
    n_delete file;
    return false;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

