#define N_IMPLEMENTS nWeightTree
//-------------------------------------------------------------------
//  node/nwtree_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/npersistserver.h"
#include "node/nweighttree.h"

static void n_addleaf(void *, nCmd *);
static void n_addnode(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nweighttree

    @superclass
    nvisnode

    @classinfo
    A nweighttree is able to fade out channels if other dominating
    channels increase in value. This relationships are defined in
    the form of a binary weight tree. Each leaf node in the tree
    represents a channel, each node defines a dominance 
    relationship between its two child nodes.

    On the computation of the tree, the sum of the child nodes
    is not allowed to be more than the actual value of the node
    (the nodes are initialized with 1.0) If this case happens the
    dominating child node 'displaces' the subordinate child nodes
    by reducing their value and recoursively adapting the values of
    their child nodes too.

    In the end you have a tree where all leaf nodes sum up to
    1.0, under consideration of the dominance rules.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_addleaf_s",'ALEF',n_addleaf);
    cl->AddCmd("b_addnode_sss",'ANOD',n_addnode);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addleaf

    @input
    s (Name)

    @output
    b (Success)

    @info
    Defines a leaf node. The name of the node is identical to
    the name of the channel to be manipulated. False is returned
    if a node with this name already exists.
*/
static void n_addleaf(void *o, nCmd *cmd)
{
    nWeightTree *self = (nWeightTree *) o;
    cmd->Out()->SetB(self->AddLeaf(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    addnode

    @input
    s (NodeName), s (LeftChild), s (RightChild)

    @output
    b (Success)

    @info
    Defines a dominance node. The first parameter is the
    name of the node, the second parameter is the name
    of the dominating child node, the third parameter
    defines the subordinate node. False is returned if 
    a node with the same name already exists, or one of
    the child nodes was not found.
*/
static void n_addnode(void *o, nCmd *cmd)
{
    nWeightTree *self = (nWeightTree *) o;
    {
        const char *s0 = cmd->In()->GetS();
        const char *s1 = cmd->In()->GetS();
        const char *s2 = cmd->In()->GetS();
        cmd->Out()->SetB(self->AddNode(s0,s1,s2));
    }
}

//-------------------------------------------------------------------
//  SaveCmds()
//  02-Jan-99   floh    machine generated
//-------------------------------------------------------------------
bool nWeightTree::SaveCmds(nPersistServer *fs)
{
    bool retval = false;
    if (nVisNode::SaveCmds(fs)) {
        nWeightNode *wn;
        for (wn = (nWeightNode *) this->wlist.GetHead();
             wn;
             wn = (nWeightNode *) wn->GetSucc())
        {
            nCmd *cmd;
            if (wn->c[0]) {
                // eine Nicht-Leaf-Node
                if ((cmd = fs->GetCmd(this,nID('A','N','O','D')))) {
                    cmd->In()->SetS(wn->GetName());
                    cmd->In()->SetS(wn->c[0]->GetName());
                    cmd->In()->SetS(wn->c[1]->GetName());
                    fs->PutCmd(cmd);
                }
            } else {
                // eine Leaf-Node
                if ((cmd = fs->GetCmd(this,nID('A','L','E','F')))) {
                    cmd->In()->SetS(wn->GetName());
                    fs->PutCmd(cmd);
                }
            }
        }
         retval = true;
    }
    return retval;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
