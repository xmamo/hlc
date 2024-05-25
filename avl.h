#ifndef HLC_AVL_H
#define HLC_AVL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct hlc_AVL hlc_AVL;

/// @memberof hlc_AVL
/// @brief Creates a new AVL node.
hlc_AVL* hlc_avl_new(double value);

/// @memberof hlc_AVL
/// @brief Computes the number of nodes of this subtree.
size_t hlc_avl_count(const hlc_AVL* root);

/// @memberof hlc_AVL
/// @brief Computes the height of this subtree.
size_t hlc_avl_height(const hlc_AVL* root);

/// @memberof hlc_AVL
/// @brief Gets the left child of this node.
/// @pre node != NULL
hlc_AVL* hlc_avl_left(const hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Gets the left child of this node.
/// @pre node != NULL
#define hlc_avl_left(node) _Generic(                \
  true ? (node) : (void*)(node),                    \
  void*: hlc_avl_left((node)),                      \
  const void*: (const hlc_AVL*)hlc_avl_left((node)) \
)

/// @memberof hlc_AVL
/// @brief Gets the right child of this node.
/// @pre node != NULL
hlc_AVL* hlc_avl_right(const hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Gets the right child of this node.
/// @pre node != NULL
#define hlc_avl_right(node) _Generic(                \
  true ? (node) : (void*)(node),                     \
  void*: hlc_avl_right((node)),                      \
  const void*: (const hlc_AVL*)hlc_avl_right((node)) \
)

/// @memberof hlc_AVL
/// @brief Gets the parent of this node.
/// @pre node != NULL
hlc_AVL* hlc_avl_parent(const hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Gets the parent of this node.
/// @pre node != NULL
#define hlc_avl_parent(node) _Generic(                \
  true ? (node) : (void*)(node),                      \
  void*: hlc_avl_parent((node)),                      \
  const void*: (const hlc_AVL*)hlc_avl_parent((node)) \
)

/// @memberof hlc_AVL
/// @brief Returns a reference to the value stored by this node.
/// @pre node != NULL
double* hlc_avl_value_ref(const hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Returns a reference to the value stored by this node.
/// @pre node != NULL
#define hlc_avl_value_ref(node) _Generic(               \
  true ? (node) : (void*)(node),                        \
  void*: hlc_avl_value_ref((node)),                     \
  const void*: (const double*)hlc_avl_value_ref((node)) \
)

/// @memberof hlc_AVL
/// @brief Gets to the root of this tree.
/// @pre node != NULL
hlc_AVL* hlc_avl_root(const hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Gets to the root of this tree.
/// @pre node != NULL
#define hlc_avl_root(node) _Generic(                \
  true ? (node) : (void*)(node),                    \
  void*: hlc_avl_root((node)),                      \
  const void*: (const hlc_AVL*)hlc_avl_root((node)) \
)

/// @memberof hlc_AVL
/// @brief Gets to the leftmost node of this subtree.
/// @pre node != NULL
hlc_AVL* hlc_avl_leftmost(const hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Gets to the leftmost node of this subtree.
/// @pre node != NULL
#define hlc_avl_leftmost(node) _Generic(                \
  true ? (node) : (void*)(node),                        \
  void*: hlc_avl_leftmost((node)),                      \
  const void*: (const hlc_AVL*)hlc_avl_leftmost((node)) \
)

/// @memberof hlc_AVL
/// @brief Gets to the rightmost node of this subtree.
/// @pre node != NULL
hlc_AVL* hlc_avl_rightmost(const hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Gets to the rightmost node of this subtree.
/// @pre node != NULL
#define hlc_avl_rightmost(node) _Generic(                \
  true ? (node) : (void*)(node),                         \
  void*: hlc_avl_rightmost((node)),                      \
  const void*: (const hlc_AVL*)hlc_avl_rightmost((node)) \
)

/// @memberof hlc_AVL
/// @brief Gets the in-order predecessor of this node.
/// @pre node != NULL
hlc_AVL* hlc_avl_predecessor(const hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Gets the in-order predecessor of this node.
/// @pre node != NULL
#define hlc_avl_predecessor(node) _Generic(                \
  true ? (node) : (void*)(node),                           \
  void*: hlc_avl_predecessor((node)),                      \
  const void*: (const hlc_AVL*)hlc_avl_predecessor((node)) \
)

/// @memberof hlc_AVL
/// @brief Gets the in-order successor of this node.
/// @pre node != NULL
hlc_AVL* hlc_avl_successor(const hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Gets the in-order successor of this node.
/// @pre node != NULL
#define hlc_avl_successor(node) _Generic(                \
  true ? (node) : (void*)(node),                         \
  void*: hlc_avl_successor((node)),                      \
  const void*: (const hlc_AVL*)hlc_avl_successor((node)) \
)

/// @memberof hlc_AVL
/// @brief Inserts a new node to the left of this node.
/// @return The new root of the subtree where the node was inserted, after rebalancing.
/// @pre node != NULL && hlc_avl_left(node) == NULL
hlc_AVL* hlc_avl_insert_left(hlc_AVL* node, double value);

/// @memberof hlc_AVL
/// @brief Inserts a new node to the right of this node.
/// @return The new root of the subtree where the node was inserted, after rebalancing.
/// @pre node != NULL && hlc_avl_right(node) == NULL
hlc_AVL* hlc_avl_insert_right(hlc_AVL* node, double value);

/// @memberof hlc_AVL
/// @brief Removes this node from its tree.
/// @return The new root of the subtree where the node was removed, after rebalancing.
/// @pre node != NULL
hlc_AVL* hlc_avl_remove(hlc_AVL* node);

/// @memberof hlc_AVL
/// @brief Produces an implementation-defined Graphviz representation of this subtree.
void hlc_avl_dot(const hlc_AVL* node, FILE* stream);

#endif
