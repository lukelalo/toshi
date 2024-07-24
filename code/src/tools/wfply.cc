//--------------------------------------------------------------------
//  wfply
//
//  Schreibt Wavefront-File als .ply raus, das ist das Format,
//  welches vom LOD-Demo benutzt wird, nur zum sehen, ob meine
//  Version davon korrekt mit unseren Modellen funktioniert.
//--------------------------------------------------------------------
#include "tools/wavefront.h"
#include "tools/wftools.h"

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    long retval = 0;
    nWavefront wf;
    nV *v;
    nFace *f;
    fprintf(stderr,"-> wfply\n");
        
    // Args auswerten
    help = wf_getboolarg(argc,argv,"-help");
    
    // Hilfe anzeigen?
    if (help) {
        fprintf(stderr,"wfply [-help]\n"
               "(C) 1998 Andre Weissflog\n"
               "Save wavefront file as .ply\n"
               "-help: show help and exit\n");
        return 0;
    }
    
    // und los
    fprintf(stderr,"loading...\n");
    if (!wf.Load(stdin)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }
    
    printf("ply\n");
    printf("format ascii 1.0\n");
    printf("element vertex %d\n",(int)wf.NumV());
    printf("property float32 x\n");
    printf("property float32 y\n");
    printf("property float32 z\n");
    printf("element face %d\n",(int)wf.NumFaces());
    printf("property list uint8 int32 vertex_indices\n");
    printf("end_header\n");
    for (v = (nV *) wf.v_list.GetHead();
         v;
         v = (nV *) v->GetSucc())
    {
        printf("%f %f %f\n",v->x,v->y,v->z);
    }
    for (f = (nFace *) wf.face_list.GetHead();
         f;
         f = (nFace *) f->GetSucc())
    {
        printf("%d ",(int)f->NumPoints());
        nPoint *p;
        for (p = (nPoint *) f->points.GetHead();
             p;
             p = (nPoint *) p->GetSucc())
        {
            printf("%d ",(int)wf.v_list.IndexOf((nNode *)p->v));
        }
        printf("\n");
    }
    
ende:
    fprintf(stderr,"<- wfply\n");
    return retval;
}  
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
