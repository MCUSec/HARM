// MIT License

// Copyright (c) 2020 Jiameng Shi

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdlib.h>
#include "rb_tree.h"

#define L_CHILD 0
#define R_CHILD 1

#define RED 0
#define BLACK 1

static inline int is_root(const struct rb_node *this)
{
    if (this) {
        return this->parent == NULL;
    }

    return 0;
}

static inline int is_left_child(const struct rb_node *this)
{
    if (this && this->parent) {
        return this == this->parent->child[L_CHILD];
    }

    return 0;
}

static inline int is_right_child(const struct rb_node *this)
{
    if (this && this->parent) {
        return this == this->parent->child[R_CHILD];
    }

    return 0;
}

static inline int color_of(const struct rb_node *this)
{
    return this ? this->color : BLACK;
}

static inline struct rb_node *parent(struct rb_node *this)
{
    if (this) {
        return this->parent;
    }

    return NULL;
}

static inline struct rb_node *grandparent(struct rb_node *this)
{
    if (this && parent(this)) {
        return parent(this)->parent;
    }

    return NULL;
}

static inline struct rb_node *uncle(struct rb_node *this)
{
    if (this && grandparent(this)) {
        return grandparent(this)->child[is_left_child(parent(this))];
    }

    return NULL;
}

static inline void node_color_swap(struct rb_node *a, struct rb_node *b)
{
    int t = a->color;
    a->color = b->color;
    b->color = t;
}

static int default_node_compare_callback(const struct rb_node *node, const unsigned long key)
{
    if ((const unsigned long)node > key) {
        return 1;
    } else if ((const unsigned long)node == key) {
        return 0;
    } else {
        return -1;
    }
}

static void rotate_left(struct rb_tree *self, struct rb_node *this)
{
    if (self && this) {
        struct rb_node *r_node = this->child[R_CHILD];
        struct rb_node *p_node = parent(this);

        node_color_swap(this, r_node);

        this->child[R_CHILD] = r_node->child[L_CHILD];
        r_node->child[L_CHILD] = this;

        r_node->parent = p_node;
        this->parent = r_node;

        if (!p_node) {
            self->root = r_node;
        } else if (this == p_node->child[L_CHILD]) {
            p_node->child[L_CHILD] = r_node;
        } else {
            p_node->child[R_CHILD] = r_node;
        }

    }
}

static void rotate_right(struct rb_tree *self, struct rb_node *this)
{
    if (self && this) {
        struct rb_node *l_node = this->child[L_CHILD];
        struct rb_node *p_node = parent(this);

        node_color_swap(this, l_node);

        this->child[L_CHILD] = l_node->child[R_CHILD];
        l_node->child[R_CHILD] = this;

        l_node->parent = p_node;
        this->parent = l_node;

        if (!p_node) {
            self->root = l_node;
        } else if (this == p_node->child[L_CHILD]) {
            p_node->child[L_CHILD] = l_node;
        } else {
            p_node->child[R_CHILD] = l_node;
        }
    }
}

static int node_search(struct rb_tree *self, const unsigned long key, struct rb_node **target)
{
    if (self) {
        struct rb_node *this = self->root;
        int cmp_result = 0;

        *target = this;

        while (this) {
            cmp_result = self->compare(this, key);
            *target = this;
            if (cmp_result) {
                this = this->child[cmp_result > 0];
            } else {
                break;
            }
        }
        return cmp_result;
    }

    *target = NULL;
    return 0;
}

static void node_traverse_and_free(struct rb_tree *self, struct rb_node *this, node_free_cb_t free_callback)
{
    if (this) {
        node_traverse_and_free(self, this->child[L_CHILD], free_callback);
        node_traverse_and_free(self, this->child[R_CHILD], free_callback);

        if (free_callback) {
            free_callback(this);
        }
    }
}

struct rb_tree *rb_tree_create(node_comp_cb_t node_compare_callback)
{
    struct rb_tree *self;

    self = malloc(sizeof(struct rb_tree));
    if (self) {
        self->root = NULL;
        self->size = 0;

        if (node_compare_callback) {
            self->compare = node_compare_callback;
        } else {
            self->compare = default_node_compare_callback;
        }
    }

    return self;
}

void rb_tree_destroy(struct rb_tree **self, node_free_cb_t free_callack)
{
    if (*self) {
        if (free_callack) {
            node_traverse_and_free(*self, (*self)->root, free_callack);
        }
        free(*self);
        *self = NULL;
    }
}

struct rb_node *rb_tree_node_get(struct rb_tree *self, const unsigned long key)
{
    if (self) {
        struct rb_node *target;

        if (node_search(self, key, &target) == 0) {
            return target;
        }
    }

    return NULL;
}

struct rb_node *rb_tree_node_put(struct rb_tree *self, const unsigned long key, struct rb_node *new_node)
{
    if (self) {
        struct rb_node *this = new_node;

        this->parent = NULL;
        this->color = RED;
        this->child[L_CHILD] = this->child[R_CHILD] = NULL;

        if (self->root == NULL) {
            self->root = this;
            this->color = BLACK;
            return this;
        } else {
            struct rb_node *target;
            int result = node_search(self, key, &target);
            if (result) {
                target->child[result > 0] = this;
                this->parent = target;
            } else {
                // the key already exists
                // replace the node with the new one
                this->parent = target->parent;
                if (is_left_child(target)) {
                    parent(this)->child[L_CHILD] = this;
                } else {
                    parent(this)->child[R_CHILD] = this;
                }
                target->parent = NULL;
                target->child[L_CHILD] = target->child[R_CHILD] = NULL;
                return target;
            }
        }

        while (!is_root(this) && color_of(parent(this)) == RED) {
            if (color_of(uncle(this)) == RED) {
                uncle(this)->color = BLACK;
                parent(this)->color = BLACK;
                grandparent(this)->color = RED;
                this = grandparent(this);
            } else {
                if (is_left_child(parent(this)) && is_left_child(this)) {
                    rotate_right(self, grandparent(this));
                } else if (is_left_child(parent(this)) && is_right_child(this)) {
                    rotate_left(self, parent(this));
                    rotate_right(self, parent(this));
                } else if (is_right_child(parent(this)) && is_right_child(this)) {
                    rotate_left(self, grandparent(this));
                } else {
                    rotate_right(self, parent(this));
                    rotate_left(self, parent(this));
                }
            }
        }

        self->root->color = BLACK;
        return new_node;
    }

    return NULL;
}
