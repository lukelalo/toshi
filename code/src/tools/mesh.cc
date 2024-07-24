/*
 * meshifier: Converts an arbitrary list of polygons into meshes.
 *
 * Copyright 1995, 1996 Brad Grantham.  All Rights Reserved.
 *
 * Should compile in any ANSI C environment.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#include "mesh.h"


/* If you actually want to generate strips */
#define NOSTRIPS	0
/* Whether next-triangle-chooser should be smart or greedy */ 
#define GREEDY		0


/* Vertices */
static int		vertcnt;
static meshVertex	verts[MAX_TRIS * 3];


/* Triangles */
static int		tricnt;
static meshTriangle	ts[MAX_TRIS];


/* Edges */
static int		edgecnt;
static meshEdge		edges[MAX_TRIS * 3];


static int 		doDrawStripOutlines = 0;
static int 		doDrawConnectivity = 0;
static int 		doPrintStripContents = 0;
static int 		doPrintAddTriangles = 0;


static void nullText(REAL[3], char *)
{
}


static void nullLine(REAL[3], REAL[3])
{
}


static void nullVertPair(int, int)
{
}


static void nullContStrip(int)
{
}


static void nullEndStrip(void)
{
}


static meshDrawTextProc		drawText =		nullText;
static meshDrawLineProc		drawLine =		nullLine;
static meshDrawEdgeProc		drawEdge =		nullVertPair;
static meshBeginStripProc	drawBeginStrip =	nullVertPair;
static meshContStripProc	drawContStrip =		nullContStrip;
static meshEndStripProc		drawEndStrip =		nullEndStrip;


void meshSetProcs(meshDrawTextProc dt, meshDrawLineProc dl, meshDrawEdgeProc de,
    meshBeginStripProc bs, meshContStripProc cs, meshEndStripProc es)
{
    drawText = (dt == NULL) ? nullText : dt;
    drawLine = (dl == NULL) ? nullLine : dl;
    drawEdge = (de == NULL) ? nullVertPair : de;
    drawBeginStrip = (bs == NULL) ? nullVertPair : bs;
    drawContStrip = (cs == NULL) ? nullContStrip : cs;
    drawEndStrip = (es == NULL) ? nullEndStrip : es;
}


void meshSetFlag(int flag, int value)
{
    switch(flag)
    {
        case MESH_DRAWSTRIPOUTLINES:
	    doDrawStripOutlines = value; break;

        case MESH_DRAWCONNECTIVITY:
	    doDrawConnectivity = value; break;

        case MESH_PRINTSTRIPCONTENTS:
	    doPrintStripContents = value; break;

        case MESH_PRINTADDTRIANGLES:
	    doPrintAddTriangles = value; break;
    }
}


static int notSharedVertex(int	t1, int	t2)
{
    if((ts[t1].v[0] != ts[t2].v[0])  &&
        (ts[t1].v[0] != ts[t2].v[1])   &&
        (ts[t1].v[0] != ts[t2].v[2]))
    {
        return(ts[t1].v[0]);
    }

    if((ts[t1].v[1] != ts[t2].v[0])  &&
        (ts[t1].v[1] != ts[t2].v[1])   &&
        (ts[t1].v[1] != ts[t2].v[2]))
    {
        return(ts[t1].v[1]);
    }

    /*
     * Well, must be the last one; if they shared all three, then they 
     *  would be the same triangle.
     */

    return(ts[t1].v[2]);
}


static int firstSharedVertex(int t1, int t2)
{
    if((ts[t1].v[0] == ts[t2].v[0])  ||
        (ts[t1].v[0] == ts[t2].v[1]) ||
        (ts[t1].v[0] == ts[t2].v[2]))
    {
        return(ts[t1].v[0]);
    }

    if((ts[t1].v[1] == ts[t2].v[0]) ||
        (ts[t1].v[1] == ts[t2].v[1]) ||
        (ts[t1].v[1] == ts[t2].v[2]))
    {
        return(ts[t1].v[1]);
    }

    /*
     * Well, can't be either; if this was the first shared, it's the ONLY
     * one shared.
     */

    fprintf(stderr, "firstSharedVertex: only one shared?  Internal error.\n");
    exit(1);
    return 0;
}


static int secondSharedVertex(int t1, int t2)
{
    if((ts[t1].v[2] == ts[t2].v[0])  ||
        (ts[t1].v[2] == ts[t2].v[1]) ||
        (ts[t1].v[2] == ts[t2].v[2]))
    {
        return(ts[t1].v[2]);
    }

    if((ts[t1].v[1] == ts[t2].v[0]) ||
        (ts[t1].v[1] == ts[t2].v[1]) ||
        (ts[t1].v[1] == ts[t2].v[2]))
    {
        return(ts[t1].v[1]);
    }

    /*
     * Well, can't be either; if we think the first vertex was the
     * SECOND shared, something is way out of whack.
     */

    fprintf(stderr, "secondSharedVertex: less than two shared?  Internal "
        "error.\n");
    exit(1);
    return 0;
}


static int triangleUsesVertex(int t, int v)
{
    if((v == ts[t].v[0]) || (v == ts[t].v[1]) || (v == ts[t].v[2]))
        return(1);
    return(0);
}


static void followStrip(int strip[], int *count, int last)
{
    int	i;
    int	next;

    while(*count < MAX_STRIP){

        /* find next triangle to add */
        next = -1;
        if(*count == 0){
                /* for 2nd tri, just pick one. */
            for(i = 0; i < ts[last].tcnt; i++)
                if(!ts[ts[last].t[i]].done){
                    next = ts[last].t[i];
                    break;
                }
            if(next != -1){
                strip[0] = notSharedVertex(last, next);
                strip[1] = firstSharedVertex(last, next);
                strip[2] = secondSharedVertex(last, next);
                *count = 3;
            }else{
                strip[0] = ts[last].v[0];
                strip[1] = ts[last].v[1];
                strip[2] = ts[last].v[2];
                *count = 3;
            }
        }else{
            /* third and later tris must share prev two verts */
            for(i = 0; i < ts[last].tcnt; i++)
                if(!ts[ts[last].t[i]].done &&
                    triangleUsesVertex(ts[last].t[i], strip[*count - 2]) &&
                    triangleUsesVertex(ts[last].t[i], strip[*count - 1]))
                {
                    next = ts[last].t[i];
                    break;
                }
        }

        if(next == -1)
            break;

	if(doPrintAddTriangles)
	    fprintf(stderr, "adding %d to strip.\n", next);

        strip[*count] = notSharedVertex(next, last);
        *count = *count + 1;

        ts[next].done = 1;

        last = next;
    }
}


static void getNextStrip(int start, int *cnt, int strip[])
{
    int	i;
    int	istrip[MAX_STRIP];
#if !GREEDY
    int 	min, mincnt;
#endif /* GREEDY */

#if NOSTRIPS

    *cnt = 3;
    strip[0] = ts[start].v[0];
    strip[1] = ts[start].v[1];
    strip[2] = ts[start].v[2];
    ts[start].done = 1;

#else /* !NOSTRIPS */

#if GREEDY

    /* do greedy strips */

    /* start is the start triangle */

#else /* !GREEDY */

    /* do slightly more wise strips; pick one of least connected
        triangles */

    mincnt = 4;
    min = -1;	/* so compiler doesn't complain */
    for(i = 0; i < tricnt; i++)
        if((! ts[i].done) && (ts[i].tcnt < mincnt))
	{
	    min = i;
	    mincnt = ts[i].tcnt;
        }

    if(min != -1)
    {
        start = min;
    }
    else
    {
        fprintf(stderr, "getNextStrip: min == -1?  Internal Error!\n");
	exit(1);
    }

#endif /* GREEDY */

    ts[start].done = 1;
    *cnt = 0;
    followStrip(istrip, cnt, start);

    for(i = 0; i < *cnt; i++)
        strip[i] = istrip[*cnt - i - 1];

    followStrip(strip, cnt, start);

#endif /* NOSTRIPS */
}


int meshCreateStrips(void)
{
    int		done;
    int		first;
    int		sl, sv[100];
    int		i, j;
    int		meshcnt;

    if(doDrawConnectivity)
    {
	meshVec3	v1, v2;
	char		str[512];
	int		t;

	for(i = 0; i < tricnt; i++){
	    for(j = 0; j < ts[i].tcnt; j++){

		t = ts[i].t[j];

		v1[X] = (verts[ts[i].v[0]].v[X] + verts[ts[i].v[1]].v[X] +
		    verts[ts[i].v[2]].v[X]) / 3;
		v1[Y] = (verts[ts[i].v[0]].v[Y] + verts[ts[i].v[1]].v[Y] +
		    verts[ts[i].v[2]].v[Y]) / 3;
		v1[Z] = (verts[ts[i].v[0]].v[Z] + verts[ts[i].v[1]].v[Z] +
		    verts[ts[i].v[2]].v[Z]) / 3;

		v2[X] = (verts[ts[t].v[0]].v[X] + verts[ts[t].v[1]].v[X] +
		    verts[ts[t].v[2]].v[X]) / 3;
		v2[Y] = (verts[ts[t].v[0]].v[Y] + verts[ts[t].v[1]].v[Y] +
		    verts[ts[t].v[2]].v[Y]) / 3;
		v2[Z] = (verts[ts[t].v[0]].v[Z] + verts[ts[t].v[1]].v[Z] +
		    verts[ts[t].v[2]].v[Z]) / 3;

		sprintf(str, "%d", t);

		drawText(v1, str);
		drawLine(v1, v2);
	    }
	}
    }

    done = 0;

    meshcnt = 0;

    do{
        /* Find out if we are done (no more triangles left) and */
        /*  if so, use that first triangle as a starting suggestion */
        for(first = 0; ts[first].done && first < tricnt; first++);

        if(first == tricnt){
            break;	/* all done; no more triangles */
        }

        /* extract the next strip out of the data set */
        getNextStrip(first, &sl, sv);
        meshcnt ++;

	if(doPrintStripContents)
	{
            fprintf(stderr, "Strip:");
	    for(i = 0; i < sl; i++){
		fprintf(stderr, " %d", sv[i]);
	    }
	    fprintf(stderr, "\n");
	}

        /* draw the triangles in each strip */
	drawBeginStrip(sv[0], sv[1]);

        for(i = 2; i < sl; i++)
	    drawContStrip(sv[i]);

	drawEndStrip();

	if(doDrawStripOutlines)
	{
	    /* draw the outline of each strip */
	    drawEdge(sv[0], sv[1]);
	    for(i = 0; i < sl - 2; i++){
		drawEdge(sv[i], sv[i + 2]);
		if(i < sl - 3)
		    drawEdge(sv[i + 1], sv[i + 3]);
	    }
	    drawEdge(sv[sl - 2], sv[sl - 1]);
	}

    }while(!done);

    return(meshcnt);
}


static void addEdge(int tnum, int vnum1, int vnum2)
{
    meshTriangle		*t, *t2;
    meshVertex		*v1, *v2;
    int			e;	/* index of edge record */

    t = &ts[tnum];
    v1 = &verts[vnum1];
    v2 = &verts[vnum2];

    /* set up edge between v1 and v2 unless one exists */

    for(e = 0; (e < v1->conncnt) && (v1->conn[e][0] != vnum2); e++);

    if(e < v1->conncnt){
            /* found existing edge */
        e = v1->conn[e][1];
        edges[e].t[1] = tnum;
        t2 = &ts[edges[e].t[0]];
        t->t[t->tcnt++] = edges[e].t[0];
        t2->t[t2->tcnt++] = tnum;

    }else{
            /* have to make new edge */

        e = edgecnt++;
        v1->conn[v1->conncnt][0] = vnum2;
        v1->conn[v1->conncnt++][1] = e;
        v2->conn[v2->conncnt][0] = vnum1;
        v2->conn[v2->conncnt++][1] = e;
        edges[e].t[0] = tnum;
    }
}


static void prepareTriangle(int t)
{
    /* set up edge connectivity */

    addEdge(t, ts[t].v[0], ts[t].v[1]);
    addEdge(t, ts[t].v[1], ts[t].v[2]);
    addEdge(t, ts[t].v[2], ts[t].v[0]);
}


void meshReset(void)
{
    vertcnt = 0;
    edgecnt = 0;
    tricnt = 0;
}


void meshAddVertex(meshVec3 v, meshVec3 n, meshVec4 c, meshVec3 t)
{
    verts[vertcnt].v[X] = v[X];
    verts[vertcnt].v[Y] = v[Y];
    verts[vertcnt].v[Z] = v[Z];

    verts[vertcnt].n[X] = n[X];
    verts[vertcnt].n[Y] = n[Y];
    verts[vertcnt].n[Z] = n[Z];

    verts[vertcnt].c[R] = c[R];
    verts[vertcnt].c[G] = c[G];
    verts[vertcnt].c[B] = c[B];
    verts[vertcnt].c[A] = c[A];

    verts[vertcnt].t[R] = t[R];
    verts[vertcnt].t[S] = t[S];
    verts[vertcnt].t[T] = t[T];

    verts[vertcnt].conncnt = 0;

    vertcnt ++;
}


void meshAddTriangle(int v1, int v2, int v3)
{
    ts[tricnt].v[0] = v1;
    ts[tricnt].v[1] = v2;
    ts[tricnt].v[2] = v3;
    ts[tricnt].done = 0;

    prepareTriangle(tricnt);

    tricnt ++;
}
