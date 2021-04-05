#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rbt.h"

#define RED 0
#define BLACK 1

typedef unsigned char color_t;

typedef struct rbt_node
{
    color_t color;
    rbt_pair key_val;
    struct rbt_node *left;
    struct rbt_node *right;
    struct rbt_node *parent;
} rbt_node;

typedef struct rbt
{
    rbt_node *root;
    /// 用来比较key的函数
    rbt_compare_f compare;
    /// 用来释放value的函数
    rbt_val_dealloc_f val_dealloc;
    /// 哨兵nil
    rbt_node *nil;
} rbt;

rbt *rbt_init(rbt_compare_f compare, rbt_val_dealloc_f val_dealloc)
{
    rbt *tree = malloc(sizeof(rbt));
    tree->nil = malloc(sizeof(rbt_node));

    tree->compare = compare;
    tree->val_dealloc = val_dealloc;
    tree->root = tree->nil;

    tree->nil->color = BLACK;
    tree->nil->key_val.key = NULL;
    tree->nil->key_val.value = NULL;

    tree->nil->left = tree->nil;
    tree->nil->right = tree->nil;
    tree->nil->parent = tree->nil;
    return tree;
}

static void free_one_node(rbt *t, rbt_node *node)
{
    if (!node || node == t->nil)
        return;
    free(node->key_val.key);
    t->val_dealloc(node->key_val.value);
    free(node);
}

static void free_node_and_sub(rbt *t, rbt_node *node)
{
    if (!node || node == t->nil)
        return;

    free(node->key_val.key);
    t->val_dealloc(node->key_val.value);
    free_node_and_sub(t, node->left);
    free_node_and_sub(t, node->right);
    free(node);
}

void rbt_deinit(rbt *t)
{
    free_node_and_sub(t, t->root);
    free(t->nil);
    free(t);
}

static void left_rotate(rbt *t, rbt_node *x)
{
    rbt_node *y = x->right;
    // 设置x和y->left的父-右子关系
    x->right = y->left;
    if (y->left != t->nil)
        y->left->parent = x;
    // 设置y和x->parent的父子关系，x原本是x->parent的左/右子，y仍是左/右子；
    y->parent = x->parent;
    if (x->parent == t->nil)
        t->root = y;               // 如果x原本是根结点，y就变成新的根
    else if (x == x->parent->left) // 如果x原本是左孩子……
        x->parent->left = y;
    else // 如果x原本是右孩子……
        x->parent->right = y;
    // 设置x和y之间的父-左孩子关系
    y->left = x;
    x->parent = y;
}

static void right_rotate(rbt *t, rbt_node *x)
{
    rbt_node *y = x->left;
    x->left = y->right;
    if (y->right != t->nil)
        y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == t->nil)
        t->root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

static void insert_fixup(rbt *t, rbt_node *z)
{
    rbt_node *y;
    while (z->parent->color == RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            y = z->parent->parent->right;
            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    left_rotate(t, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(t, z->parent->parent);
            }
        }
        else
        {
            y = z->parent->parent->left;
            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    right_rotate(t, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(t, z->parent->parent);
            }
        }
    }
    t->root->color = BLACK;
}

rbt_modify_result rbt_insert(rbt *t, rbt_pair key_val)
{
    // 结点z是新建的结点，用来插入或代替旧结点
    rbt_node *z = malloc(sizeof(rbt_node));
    z->key_val = key_val;

    rbt_node *y = t->nil;
    rbt_node *x = t->root;

    // 找新结点z应该放的位置x。y是x的父结点。
    while (x != t->nil)
    {
        y = x;
        int diff = t->compare(z->key_val.key, x->key_val.key); //z-x
        if (diff < 0)
            x = x->left;
        else if (diff > 0)
            x = x->right;
        else
        {
            // 重复的key不会插入
            free(z);
            return RBT_MODIFY_DUP_OR_MISSING;
        }
    }

    // 设置y和z的父子关系
    z->parent = y;
    if (y == t->nil) // 树中本来没结点，那z就是根结点
        t->root = z;
    else if (t->compare(z->key_val.key, y->key_val.key) < 0) // z < y
        y->left = z;
    else // z >= y
        y->right = z;

    z->left = t->nil;
    z->right = t->nil;
    z->color = RED;
    // 修复红黑树的性质
    insert_fixup(t, z);
    return RBT_MODIFY_SUCCESS;
}

void *rbt_at(const rbt *tree, const char *key)
{
    rbt_node *x = tree->root;
    // 找结点，结果是x
    while (x != tree->nil)
    {
        int diff = tree->compare(key, x->key_val.key);
        if (diff < 0)
            x = x->left;
        else if (diff > 0)
            x = x->right;
        else // 找到了
        {
            return x->key_val.value;
        }
    }
    return NULL;
}

void rbt_delete_fixup(rbt *t, rbt_node *x)
{
    rbt_node *w;
    while (x != t->root && x->color == BLACK)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate(t, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else
            {
                if (w->right->color == BLACK)
                {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(t, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate(t, x->parent);
                x = t->root;
            }
        }
        else
        {
            w = x->parent->left;
            if (w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate(t, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else
            {
                if (w->left->color == BLACK)
                {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(t, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = BLACK;
}

/// 将v和父亲的链接断开，改接到u
void transplant(rbt *t, rbt_node *u, rbt_node *v)
{
    if (u->parent == t->nil)
    {
        t->root = v;
    }
    else if (u == u->parent->left)
    {
        u->parent->left = v;
    }
    else
    {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

/// 取得最左叶子结点
rbt_node *tree_minimum(rbt *t, rbt_node *node)
{
    while (node->left != t->nil)
    {
        node = node->left;
    }
    return node;
}

/// 删除结点
void rbt_delete_node(rbt *t, rbt_node *z)
{
    rbt_node *x;
    rbt_node *y = z;
    unsigned char y_orig_color = y->color;
    if (z->left == t->nil)
    {
        x = z->right;
        transplant(t, z, z->right);
    }
    else if (z->right == t->nil)
    {
        x = z->left;
        transplant(t, z, z->left);
    }
    else
    {
        y = tree_minimum(t, z->right);
        y_orig_color = y->color;
        x = y->right;
        if (y->parent == z)
            x->parent = y;
        else
        {
            transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (y_orig_color == BLACK)
        rbt_delete_fixup(t, x);
    free_one_node(t, z);
}

int rbt_remove(rbt *tree, const char *node_name)
{
    rbt_node *x = tree->root;
    // 找结点，结果是x
    while (x != tree->nil)
    {
        int diff = tree->compare(node_name, x->key_val.key);
        if (diff < 0)
        {
            x = x->left;
        }
        else if (diff > 0)
        {
            x = x->right;
        }
        else // 找到了
        {
            rbt_delete_node(tree, x);
            return 0;
        }
    }
    // 找不到
    return 1;
}

void rbt_inorder_trav_node(const rbt *t, const rbt_node *node, rbt_travfunc_t f)
{
    if (node == t->nil)
        return;

    rbt_inorder_trav_node(t, node->left, f);
    f(node->key_val.key, node->key_val.value);
    rbt_inorder_trav_node(t, node->right, f);
}

void rbt_trav(const rbt *t, rbt_travfunc_t f)
{
    rbt_inorder_trav_node(t, t->root, f);
}

void rbt_inorder_trav_node_with_arg(const rbt *t, const rbt_node *node, rbt_travfunc_wa_t f, void *arg)
{
    if (node == t->nil)
        return;
    rbt_inorder_trav_node_with_arg(t, node->left, f, arg);
    f(node->key_val.key, node->key_val.value, arg);
    rbt_inorder_trav_node_with_arg(t, node->right, f, arg);
}

void rbt_trav_with_arg(const rbt *t, rbt_travfunc_wa_t f, void *arg)
{
    rbt_inorder_trav_node_with_arg(t, t->root, f, arg);
}

#ifndef NDEBUG

#include <stdio.h>

int computeBlackHeight(const rbt *t, const rbt_node *currNode)
{
    if (currNode == t->nil)
        return 0;
    int leftHeight = computeBlackHeight(t, currNode->left);
    int rightHeight = computeBlackHeight(t, currNode->right);
    int add = currNode->color == BLACK ? 1 : 0;
    if (leftHeight == -1 || rightHeight == -1 || leftHeight != rightHeight)
        return -1;
    else
        return leftHeight + add;
}

void rbtdebug_check_no_2_reds(const rbt *t, const rbt_node *n)
{
    if (n == t->nil)
        return;
    if (n->color == RED)
    {
        assert(n->left->color == BLACK && "consecutive red nodes not allowed");
        assert(n->right->color == BLACK && "consecutive red nodes not allowed");
    }
    rbtdebug_check_no_2_reds(t, n->left);
    rbtdebug_check_no_2_reds(t, n->right);
}

void rbtdebug_property_asserts(const rbt *t)
{
    if (t->root == t->nil)
        return;
    // 性质1：根结点为黑色
    assert(t->root->color == BLACK && "root node must be black");
    // 性质2：任意路径无连续红色结点
    rbtdebug_check_no_2_reds(t, t->root);
    // 性质3：黑高等
    assert(computeBlackHeight(t, t->root) != -1 && "black heights not equal");
}

#endif
