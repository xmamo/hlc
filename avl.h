#ifndef HLC_AVL_H
#define HLC_AVL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "base.h"

typedef struct hlc_AVL hlc_AVL;

/// @memberof hlc_AVL
/// @brief Creates a new AVL node.
HLC_API hlc_AVL* hlc_avl_new(double value);

/// @memberof hlc_AVL
/// @brief Computes the number of nodes of this subtree.
HLC_API size_t hlc_avl_count(const hlc_AVL* root);

/// @memberof hlc_AVL
/// @brief Computes the height of this subtree.
HLC_API size_t hlc_avl_height(const hlc_AVL* root);

/// @memberof hlc_AVL
/// @brief Gets the left/right child or the parent of this node.
/// @param direction -1 for the left child, 0 for the parent, +1 for the right child.
/// @pre node != NULL
HLC_API hlc_AVL* hlc_avl_link(const hlc_AVL* node, signed char direction);

/// @memberof hlc_AVL
/// @brief Gets the left/right child or the parent of this node.
/// @param direction -1 for the left child, 0 for the parent, +1 for the right child.
/// @pre node != NULL
#define hlc_avl_link(node, direction) _Generic(                  \
  true ? (node) : (void*)(node),                                 \
  void*: hlc_avl_link((node), (direction)),                      \
  const void*: (const hlc_AVL*)hlc_avl_link((node), (direction)) \
)

/// @memberof hlc_AVL
/// @brief Returns a reference to the value stored by this node.
/// @pre node != NULL
HLC_API double* hlc_avl_value_ref(const hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Returns a reference to the value stored by this node.
/// @pre node != NULL
#define hlc_avl_value_ref(node) _Generic(               \
  true ? (node) : (void*)(node),                        \
  void*: hlc_avl_value_ref((node)),                     \
  const void*: (const double*)hlc_avl_value_ref((node)) \
)

/// @memberof hlc_AVL
/// @brief Gets to the leftmost/topmost/rightmost node of this subtree.
/// @param direction -1 for the leftmost node, 0 for the topmost node, +1 for the rightmost node.
/// @pre node != NULL
HLC_API hlc_AVL* hlc_avl_xmost(const hlc_AVL* node, signed char direction);

/// @memberof hlc_AVL
/// @brief Gets to the leftmost/topmost/rightmost node of this subtree.
/// @param direction -1 for the leftmost node, 0 for the topmost node, +1 for the rightmost node.
/// @pre node != NULL
#define hlc_avl_xmost(node, direction) _Generic(                  \
  true ? (node) : (void*)(node),                                  \
  void*: hlc_avl_xmost((node), (direction)),                      \
  const void*: (const hlc_AVL*)hlc_avl_xmost((node), (direction)) \
)

/// @memberof hlc_AVL
/// @brief Gets the in-order predecessor/successor of this node.
/// @param direction -1 for the predecessor, +1 for the successor.
/// @pre node != NULL
HLC_API hlc_AVL* hlc_avl_xcessor(const hlc_AVL* node, signed char direction);

/// @memberof hlc_AVL
/// @brief Gets the in-order predecessor/successor of this node.
/// @param direction -1 for the predecessor, +1 for the successor.
/// @pre node != NULL
#define hlc_avl_xcessor(node, direction) _Generic(                  \
  true ? (node) : (void*)(node),                                    \
  void*: hlc_avl_xcessor((node), (direction)),                      \
  const void*: (const hlc_AVL*)hlc_avl_xcessor((node), (direction)) \
)

/// @memberof hlc_AVL
/// @brief Inserts a new node to the left/right of this node.
/// @param direction -1 to insert to the left, +1 to insert to the right.
/// @return The new root of the subtree where the node was inserted, after rebalancing.
/// @pre node != NULL && hlc_avl_link(node, direction) == NULL
HLC_API hlc_AVL* hlc_avl_insert(hlc_AVL* node, signed char direction, double value);

/// @memberof hlc_AVL
/// @brief Removes this node from its tree.
/// @return The new root of the subtree where the node was removed, after rebalancing.
/// @pre node != NULL
HLC_API hlc_AVL* hlc_avl_remove(hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Swaps two nodes.
/// @pre node1 != NULL && node2 != NULL
HLC_API void hlc_avl_swap(hlc_AVL* node1, hlc_AVL* node2);

/// @memberof hlc_AVL
/// @brief Produces an implementation-defined Graphviz representation of this subtree.
HLC_API void hlc_avl_dot(const hlc_AVL* node, FILE* stream);

#endif
