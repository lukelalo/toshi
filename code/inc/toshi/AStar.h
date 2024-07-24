//////////////////////////////////////////////////////////////////
// Class:	CAStar class (27/6/2001)
// File:	AStar.h
// Author:	James Matthews
//
// Implements the A* algorithm.
// 
//
// Please visit http://www.generation5.org/ for the latest
// in Artificial Intelligence news, interviews, articles and
// discussion forums.
//

#ifndef _ASTAR_H_
#define _ASTAR_H_

#include <memory.h>


//////////////////////////////////////////////////////////////////
// Class:	CAStar class (27/6/2001)
// Author:	James Matthews
//
// Implements the A* algorithm.
//

#ifndef NULL
#define NULL 0
#endif

#define ASNL_ADDOPEN		0
#define ASNL_STARTOPEN		1
#define ASNL_DELETEOPEN		2
#define ASNL_ADDCLOSED		3

#define ASNC_INITIALADD		0
#define ASNC_OPENADD_UP		1
#define ASNC_OPENADD		2
#define ASNC_CLOSEDADD_UP	3
#define ASNC_CLOSEDADD		4
#define ASNC_NEWADD			5



class _asNode {
	public:
		_asNode(int a = -1,int b = -1) : x(a), y(b), number(0), numchildren(0) {
			parent = next = NULL; dataptr = NULL;
			memset(children, 0, sizeof(children));
		}

		int			f,g,h;			// Fitness, goal, heuristic.
		int			x,y;			// Coordinate position
		int			numchildren;
		int			number;			// x*m_iRows+y
		_asNode		*parent;
		_asNode		*next;			// For Open and Closed lists
		_asNode		*children[8];	// Assumes square tiles
		void		*dataptr;		// Associated data
};

// Stack for propagation.
struct _asStack {
	_asNode	 *data;
	_asStack *next;
};

typedef int(*_asFunc)(_asNode *, _asNode *, int, void *);


class CAStar {
	public:
		CAStar();
		~CAStar();

		_asFunc	 udCost;			// Called when cost value is need
		_asFunc  udValid;			// Called to check validity of a coordinate
		_asFunc  udNotifyChild;		// Called when child is added/checked (LinkChild)
		_asFunc	 udNotifyList;		// Called when node is added to Open/Closed list

		void	*m_pCBData;			// Data passed back to callback functions
		void	*m_pNCData;			// Data passed back to notify child functions

		bool	GeneratePath(int, int, int, int);
		int		Step();
		void	StepInitialize(int, int, int, int);
		void	SetRows(int r)		 { m_iRows = r;    }
		void	Reset() { m_pBest = NULL; }

		_asNode	*GetBestNode() { return m_pBest; }
		_asNode *GetParent(_asNode *n) {return n->parent;}
		inline int Coord2Num(int x, int y) { return x * m_iRows + y; }

	protected:
		int		m_iRows;			// Used to calculate node->number
		int		m_iSX, m_iSY, m_iDX, m_iDY, m_iDNum;

		_asNode	*m_pOpen;			// The open list
		_asNode	*m_pClosed;			// The closed list
		_asNode *m_pBest;			// The best node
		_asStack*m_pStack;			// Propagation stack

		// Functions.
		void	AddToOpen(_asNode *);
		void	ClearNodes();
		void	CreateChildren(_asNode *);
		void	LinkChild(_asNode *, _asNode *);
		void	UpdateParents(_asNode *);

		// Stack Functions.
		void	Push(_asNode *);
		_asNode *Pop();
		
		_asNode *CheckList(_asNode *, int);
		_asNode	*GetBest();
		
		// Inline functions.
		inline int udFunc(_asFunc, _asNode *, _asNode *, int, void *);

};

#endif
