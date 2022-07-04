/**
 * MIT License
 *
 * Copyright (c) 2022 MCUSec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef RBTREE_H
#define RBTREE_H

#include <stddef.h>
#include <stdint.h>

struct rb_node {
    struct rb_node *child[2];
    struct rb_node *parent;
    const void *data;
    uint8_t color;
} __attribute__((packed));

typedef int (*node_comp_cb_t)(const struct rb_node *node, const unsigned long key);

struct rb_tree {
    struct rb_node *root;
    unsigned int size;
    node_comp_cb_t compare;
};

typedef void (*node_free_cb_t)(struct rb_node *node);

#ifdef __cplusplus
extern "C" {
#endif

struct rb_tree *rb_tree_create(node_comp_cb_t compre_callback);
void rb_tree_destroy(struct rb_tree **self, node_free_cb_t free_callback);

struct rb_node *rb_tree_node_get(struct rb_tree *self, const unsigned long key);
struct rb_node *rb_tree_node_put(struct rb_tree *self, const unsigned long key, struct rb_node *new_node);

#ifdef __cplusplus
}
#endif

#define container_of(ptr, type, member) ({ \
    const typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); \
})

#define rb_tree_get(rb_tree, key, type, node) \
    container_of(rb_tree_node_get((rb_tree), (key)), type, node)

#define rb_tree_put(rb_tree, key, entry, node) \
    rb_tree_node_put((rb_tree), (key), &(entry)->node)

#endif /* RBTREE_H */
