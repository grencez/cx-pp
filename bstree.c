/**
 * \file bstree.c
 * Binary search tree.
 **/
#include "bstree.h"
#include <assert.h>

#define positive_bit( x )  ((x) > 0 ? 1 : 0)

    BSTree
dflt2_BSTree (BSTNode* sentinel, PosetCmp cmp)
{
    BSTree t;
    sentinel->joint = 0;
    sentinel->split[0] = 0;
    sentinel->split[1] = 0;
    t.sentinel = sentinel;
    t.cmp = cmp;
    return t;
}

    void
init_BSTree (BSTree* t, BSTNode* sentinel, PosetCmp cmp)
{
    *t = dflt2_BSTree (sentinel, cmp);
}

    void
lose_BSTree (BSTree* t, void (* lose) (BSTNode*))
{
    BSTNode* y = root_of_BSTree (t);

    while (y)
    {
        BSTNode* x = y;
        Bit side = 0;

        /* Descend to the lo side.*/
        do
        {
            y = x;
            x = x->split[0];
        } while (x);

        /* Ascend until we can descend to the hi side.*/
        if (!y->split[1])  do
        {
            x = y;
            y = x->joint;
            side = side_of_BSTNode (x);
            lose (x);
        } while (y->joint && (side == 1 || !y->split[1]));

        y = (y->joint ? y->split[1] : 0);
    }
}

/**
 * Preorder, postorder, and inorder traversals are supported by
 * values of Nil, Yes, and May for /postorder/ respectively.
 **/
    void
walk_BSTree (BSTree* t, Trit postorder,
             void (* f) (BSTNode*, void*), void* dat)
{
    BSTNode* y = root_of_BSTree (t);

    while (y)
    {
        BSTNode* x = y;
        Bit side = 0;

        /* Descend to the lo side.*/
        do
        {
            if (postorder == Nil)  f (x, dat);
            y = x;
            x = x->split[0];
        } while (x);

        /* Ascend until we can descend to the hi side.*/
        if (!y->split[1])  do
        {
            if (postorder == May && side == 0)  f (y, dat);
            x = y;
            y = x->joint;
            side = side_of_BSTNode (x);
            if (postorder == Yes)  f (x, dat);
        } while (y->joint && (side == 1 || !y->split[1]));

        if (postorder == May && y->joint)  f (y, dat);
        y = (y->joint ? y->split[1] : 0);
    }
}

  BSTNode*
find_BSTree (BSTree* t, const void* key)
{
  BSTNode* y = root_of_BSTree (t);

  while (y)
  {
    Sign si = poset_cmp_lhs (t->cmp, key, y);
    if (si == 0)  return y;
    y = y->split[positive_bit (si)];
  }
  return 0;
}

  void
insert_BSTree (BSTree* t, BSTNode* x)
{
  BSTNode* a = t->sentinel;
  BSTNode* y = root_of_BSTree (t);
  Bit side = side_of_BSTNode (y);

  while (y)
  {
    side = positive_bit (poset_cmp (t->cmp, x, y));
    a = y;
    y = y->split[side];
  }

  a->split[side] = x;
  x->joint = a;
  x->split[0] = 0;
  x->split[1] = 0;
}

/** If a node matching /x/ exists, return that node.
 * Otherwise, add /x/ to the tree and return it.
 **/
  BSTNode*
ensure_BSTree (BSTree* t, BSTNode* x)
{
  BSTNode* a = t->sentinel;
  BSTNode* y = root_of_BSTree (t);
  Bit side = side_of_BSTNode (y);

  while (y)
  {
    Sign si = poset_cmp (t->cmp, x, y);
    if (si == 0)  return y;
    side = positive_bit (si);
    a = y;
    y = y->split[side];
  }

  a->split[side] = x;
  x->joint = a;
  x->split[0] = 0;
  x->split[1] = 0;
  return x;
}

/**
 * Ensure /x/ exists in the tree.
 * It replaces a matching node if one exists.
 * The matching node (which was replaced) is returned.
 * If no matching node was replaced, 0 is returned.
 **/
    BSTNode*
setf_BSTree (BSTree* t, BSTNode* x)
{
    BSTNode* y = ensure_BSTree (t, x);
    if (y == x)  return 0;
    x->joint = y->joint;
    x->joint->split[side_of_BSTNode (y)] = x;
    join_BSTNode (x, y->split[0], 0);
    join_BSTNode (x, y->split[1], 1);
    return y;
}

/** Remove a given node from the tree.
 *
 * /a/ will hold information about what had to be moved,
 * which is used by the red-black tree removal.
 *
 * Let /y/ be the node which will simply slide into place to replace /a/
 * or has a split member which will replace /a/.
 *
 * /a->joint/ will hold /y/ unless /y/ simply replaced /a/
 * without changing its split, in which case /a-joint/ is not changed.
 *
 * /a->split/ will hold the split of the resulting /a->joint/
 * which changed depth.
 **/
    void
remove_BSTNode (BSTNode* a)
{
    Bit side_a = side_of_BSTNode (a);
    Bit side, oside;
    BSTNode* x;
    BSTNode* y;

    /*  a          y 
     *   \        / \
     *    y   => *   *
     *   / \
     *  *   *
     *
     * /y/ can be Nil when /i == 0/.
     */
    {unsigned int i =0;for (;i<( 2);++i){
        oside = (Bit) i;
        if (!a->split[oside])
        {
            side = !oside;
            y = a->split[side];
            join_BSTNode (a->joint, y, side_a);
                /* a->joint = a->joint; */
            a->split[side_a] = y;
            a->split[!side_a] = 0;
            return;
        }
    }}

    /*   a          y
     *  / \        / \
     *     y   =>     *
     *      \
     *       *
     */
    {unsigned int i =0;for (;i<( 2);++i){
        side = (Bit) i;
        oside = !side;
        y = a->split[side];

        if (!y->split[oside])
        {
            join_BSTNode (a->joint, y, side_a);
            join_BSTNode (y, a->split[oside], oside);
            a->joint = y;
            a->split[side] = y->split[side];
            a->split[oside] = 0;
            return;
        }
    }}

    /* Roughly this...
     * /x/ descends as far as it can in one direction.
     * /y/ follows one step behind.
     *     a            x
     *    / \          / \
     *  1*   y    => 1*   y
     *      / \          / \
     *     x   *3      2*   *3
     *      \
     *       *2
     */
    side = 1;  /* Arbitrary side.*/
    oside = !side;
    y = a->split[side];
    x = y->split[oside];
    do
    {
        y = x;
        x = y->split[oside];
    } while (x);
    x = y;
    y = x->joint;

    Claim2( oside ,==, side_of_BSTNode (x) );
    join_BSTNode (y, x->split[side], oside);

    x->joint = a->joint;
    x->joint->split[side_a] = x;
    join_BSTNode (x, a->split[0], 0);
    join_BSTNode (x, a->split[1], 1);

    a->joint = y;
    a->split[side] = 0;
    a->split[oside] = y->split[oside];
}

/** Do a tree rotation,
 *
 *       b          a
 *      / \        / \
 *     a   *z => x*   b
 *    / \            / \
 *  x*   *y        y*   *z
 *
 * When /side/ is 1, opposite direction when 0.
 * (/b/ always starts as /a/'s joint)
 **/
    void
rotate_BSTNode (BSTNode* b, Bit side)
{
    const int p = side ? 0 : 1;
    const int q = side ? 1 : 0;
    BSTNode* a = b->split[p];
    BSTNode* y = a->split[q];

    join_BSTNode (b->joint, a, side_of_BSTNode (b));
    join_BSTNode (a, b, q);
    join_BSTNode (b, y, p);
}

