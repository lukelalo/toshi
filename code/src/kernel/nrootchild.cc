#define N_IMPLEMENTS nRoot
#define N_KERNEL
//--------------------------------------------------------------------
//  nRoot -- Child Handling
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include "kernel/nroot.h"

//--------------------------------------------------------------------
/**
    - 17-May-99   floh    neues Listenhandling
*/
//--------------------------------------------------------------------
void nRoot::AddHead(nRoot *n)
{
    if (NULL == this->child_list) this->child_list = n_new nHashList;
    n->parent = this;
    this->child_list->AddHead(n);
}

//--------------------------------------------------------------------
/**
    - 17-May-99   floh    neues Listenhandling
*/
//--------------------------------------------------------------------
void nRoot::AddTail(nRoot *n)
{
    if (NULL == this->child_list) this->child_list = n_new nHashList;
    n->parent = this;
    this->child_list->AddTail(n);
}   

//--------------------------------------------------------------------
/**
    - 17-May-99   floh    neues Listenhandling
*/
//--------------------------------------------------------------------
nRoot *nRoot::RemHead(void)
{
    nRoot *n = NULL;
    if (this->child_list) {
        n = (nRoot *) this->child_list->RemHead();
        if (n) n->parent = NULL;
    }
    return n;
}

//--------------------------------------------------------------------
/**
    - 17-May-99   floh    neues Listenhandling
*/
//--------------------------------------------------------------------
nRoot *nRoot::RemTail(void)
{
    nRoot *n = NULL;
    if (this->child_list) {
        n = (nRoot *) this->child_list->RemTail();
        if (n) n->parent = NULL;
    }
    return n;
}

//--------------------------------------------------------------------
/**
    - 17-May-99   floh    neues Listenhandling
*/
//--------------------------------------------------------------------
void nRoot::Remove(void)
{
    nHashNode::Remove();
    this->parent = NULL;
}

//--------------------------------------------------------------------
/**
    - 17-May-99   floh    neues Listenhandling
*/
//--------------------------------------------------------------------
void nRoot::SetName(const char *str)
{
    nHashNode::SetName(str);
}

//--------------------------------------------------------------------
/**
    - 04-Nov-98 floh    akzeptiert jetzt die Spezial-Objekte
                        ".." und "."
    - 17-May-99   floh    etwas optimiert (strcmp() wird nicht mehr
                          aufgerufen).
*/
//--------------------------------------------------------------------
nRoot *nRoot::Find(const char *str)
{
    n_assert(str);
    nRoot *n = NULL;
    if (str[0]=='.') {
        if (str[1]==0) return this;
        else if ((str[1]=='.') && (str[2]==0)) return this->parent;
    }
    if (this->child_list) n = (nRoot *) this->child_list->Find(str);
    return n;
}

//--------------------------------------------------------------------
/**
    @brief Comparison hook for child objects.

    - 18-May-99   floh    created
*/
//--------------------------------------------------------------------
int child_cmp(const void *e0, const void *e1)
{
    nRoot *r0 = *((nRoot **)e0);
    nRoot *r1 = *((nRoot **)e1);
    return strcmp(r1->GetName(),r0->GetName());
}

//--------------------------------------------------------------------
/**
    @brief Sort children objects alphabetically.

    Not particularly optimized for runtime.

    - 18-May-99   floh    created
*/
//--------------------------------------------------------------------
void nRoot::Sort(void)
{
    int num,i;
    nRoot *c;
    for (num=0,c=this->GetHead(); c; c=c->GetSucc(),num++);
    if (num > 0) {
        nRoot **c_array = (nRoot **) n_malloc(num*sizeof(nRoot *));
        if (!c_array) n_error("Out of mem!");
        for (i=0,c=this->GetHead(); c; c=c->GetSucc(),i++) {
            c_array[i] = c;
        }
        qsort(c_array,num,sizeof(nRoot *),child_cmp);
        for (i=0; i<num; i++) {
            c_array[i]->Remove();
            this->AddHead(c_array[i]);
        }
        n_free(c_array);
    }
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------



