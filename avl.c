#include "avl.h"

#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "layout.h"
#include "math.h"
#include "traits/assign.h"
#include "traits/destroy.h"


struct hlc_AVL {
  hlc_AVL* _links[3];
  signed char direction;
  signed char balance;
};


#define HLC_AVL_LINKS(node) _Generic(                                                                  \
  true ? (node) : (void*)(node),                                                                       \
  void*: (hlc_AVL**)((char*)(node) + (offsetof(hlc_AVL, _links) + sizeof(hlc_AVL*))),                  \
  const void*: (hlc_AVL* const*)((const char*)(node) + (offsetof(hlc_AVL, _links) + sizeof(hlc_AVL*))) \
)


hlc_AVL* hlc_avl_new(
  const void* element,
  hlc_Layout element_layout,
  hlc_Assign_instance element_assign_instance
) {
  hlc_Layout node_layout = {.size = offsetof(hlc_AVL, balance) + sizeof(signed char), .alignment = alignof(hlc_AVL)};
  size_t element_offset = hlc_layout_add(&node_layout, element_layout);
  hlc_layout_pad(&node_layout);

  hlc_AVL* node = malloc(node_layout.size);

  if (node != NULL) {
    HLC_AVL_LINKS(node)[-1] = NULL;
    HLC_AVL_LINKS(node)[+1] = NULL;
    HLC_AVL_LINKS(node)[0] = NULL;
    node->direction = -1;
    node->balance = 0;

    if (hlc_assign((char*)node + element_offset, element, element_assign_instance)) {
      return node;
    } else {
      free(node);
    }
  }

  return NULL;
}


size_t hlc_avl_count(const hlc_AVL* root) {
  return root == NULL ? 0 : hlc_avl_count(HLC_AVL_LINKS(root)[-1]) + hlc_avl_count(HLC_AVL_LINKS(root)[+1]) + 1;
}


size_t hlc_avl_height(const hlc_AVL* root) {
  return root == NULL ? 0 : HLC_MAX(hlc_avl_height(HLC_AVL_LINKS(root)[-1]), hlc_avl_height(HLC_AVL_LINKS(root)[+1])) + 1;
}


hlc_AVL* (hlc_avl_link)(const hlc_AVL* node, signed char direction) {
  assert(node != NULL);
  assert(direction >= -1 && direction <= +1);

  return HLC_AVL_LINKS(node)[direction];
}


void* (hlc_avl_element)(const hlc_AVL* node, hlc_Layout element_layout) {
  assert(node != NULL);

  hlc_Layout node_layout = {.size = offsetof(hlc_AVL, balance) + sizeof(signed char), .alignment = alignof(hlc_AVL)};
  size_t element_offset = hlc_layout_add(&node_layout, element_layout);

  return (char*)node + element_offset;
}


hlc_AVL* (hlc_avl_xmost)(const hlc_AVL* node, signed char direction) {
  assert(node != NULL);
  assert(direction >= -1 && direction <= +1);

  while (HLC_AVL_LINKS(node)[direction] != NULL) {
    node = HLC_AVL_LINKS(node)[direction];
  }

  return (hlc_AVL*)node;
}


hlc_AVL* (hlc_avl_xcessor)(const hlc_AVL* node, signed char direction) {
  assert(node != NULL);
  assert(direction == -1 || direction == +1);

  if (HLC_AVL_LINKS(node)[direction] != NULL)
    return hlc_avl_xmost(HLC_AVL_LINKS(node)[direction], -direction);

  if (node->direction != direction) {
    assert(node->direction == -direction);
    return HLC_AVL_LINKS(node)[0];
  }

  while (true) {
    node = HLC_AVL_LINKS(node)[0];

    if (node == NULL)
      return NULL;

    if (node->direction != direction) {
      assert(node->direction == -direction);
      return HLC_AVL_LINKS(node)[0];
    }
  }
}


static size_t hlc_avl_check(const hlc_AVL* node) {
  if (node != NULL) {
    const hlc_AVL* node_left = HLC_AVL_LINKS(node)[-1];
    size_t left_height = hlc_avl_check(node_left);

    const hlc_AVL* node_right = HLC_AVL_LINKS(node)[+1];
    size_t right_height = hlc_avl_check(node_right);

    assert(left_height == right_height - node->balance);
    assert(node_left == NULL || (HLC_AVL_LINKS(node_left)[0] == node && node_left->direction == -1));
    assert(node_right == NULL || (HLC_AVL_LINKS(node_right)[0] == node && node_right->direction == +1));

    return HLC_MAX(left_height, right_height) + 1;
  } else {
    return 0;
  }
}


static hlc_AVL* hlc_avl_rotate_left(hlc_AVL* x) {
  assert(x != NULL && HLC_AVL_LINKS(x)[+1] != NULL);

  //   X              Y
  //  / \            / \
  // a   Y    =>    X   c
  //    / \        / \
  //   b   c      a   b

  hlc_AVL* y = HLC_AVL_LINKS(x)[+1];
  hlc_AVL* x_parent = HLC_AVL_LINKS(x)[0];
  signed char x_direction = x->direction;
  signed char x_balance = x->balance;

  hlc_AVL* b = HLC_AVL_LINKS(y)[-1];
  signed char y_balance = y->balance;

  if (b != NULL) {
    HLC_AVL_LINKS(b)[0] = x;
    b->direction = +1;
  }

  // X0_b = Y0_h - a_h
  // X1_b = b_h - a_h
  //
  // X1_b - X0_b = b_h - a_h - (Y0_h - a_h)
  // X1_b - X0_b = b_h - a_h - Y0_h + a_h
  // X1_b - X0_b = b_h - Y0_h
  // X1_b - X0_b = b_h - (max(b_h, c_h) + 1)
  // X1_b - X0_b = -(max(b_h - b_h, c_h - b_h) + 1)
  // X1_b - X0_b = -(max(0, Y0_b) + 1)

  HLC_AVL_LINKS(x)[+1] = b;
  HLC_AVL_LINKS(x)[0] = y;
  x->direction = -1;
  x->balance = x_balance - HLC_MAX(1, 1 + y_balance);

  // Y0_b = c_h - b_h
  // Y1_b = c_h - X1_h
  //
  // Y1_b - Y0_b = c_h - X1_h - (c_h - b_h)
  // Y1_b - Y0_b = c_h - X1_h - c_h + b_h
  // Y1_b - Y0_b = -X1_h + b_h
  // Y1_b - Y0_b = -(max(a_h, b_h) + 1) + b_h
  // Y1_b - Y0_b = -(max(a_h - b_h, b_h - b_h) + 1)
  // Y1_b - Y0_b = -(max(-X1_b, 0) + 1)

  HLC_AVL_LINKS(y)[-1] = x;
  HLC_AVL_LINKS(y)[0] = x_parent;
  y->direction = x_direction;
  y->balance = y_balance - HLC_MAX(1 - x->balance, 1);

  return y;
}


static hlc_AVL* hlc_avl_rotate_right(hlc_AVL* y) {
  assert(y != NULL && HLC_AVL_LINKS(y)[-1] != NULL);

  //     Y          X
  //    / \        / \
  //   X   c  =>  a   Y
  //  / \            / \
  // a   b          b   c

  hlc_AVL* x = HLC_AVL_LINKS(y)[-1];
  hlc_AVL* y_parent = HLC_AVL_LINKS(y)[0];
  signed char y_direction = y->direction;
  signed char y_balance = y->balance;

  hlc_AVL* b = HLC_AVL_LINKS(x)[+1];
  signed char x_balance = x->balance;

  if (b != NULL) {
    HLC_AVL_LINKS(b)[0] = y;
    b->direction = -1;
  }

  // Y0_b = c_h - X0_h
  // Y1_b = c_h - b_h
  //
  // Y1_b - Y0_b = c_h - b_h - (c_h - X0_h)
  // Y1_b - Y0_b = c_h - b_h - c_h + X0_h
  // Y1_b - Y0_b = -b_h + X0_h
  // Y1_b - Y0_b = -b_h + max(a_h, b_h) + 1
  // Y1_b - Y0_b = max(a_h - b_h, b_h - b_h) + 1
  // Y1_b - Y0_b = max(-X0_b, 0) + 1

  HLC_AVL_LINKS(y)[-1] = b;
  HLC_AVL_LINKS(y)[0] = x;
  y->direction = +1;
  y->balance = y_balance + HLC_MAX(1 - x_balance, 1);

  // X0_b = b_h - a_h
  // X1_b = Y1_h - a_h
  //
  // X1_b - X0_b = Y1_h - a_h - (b_h - a_h)
  // X1_b - X0_b = Y1_h - a_h - b_h + a_h
  // X1_b - X0_b = Y1_h - b_h
  // X1_b - X0_b = max(b_h, c_h) + 1 - b_h
  // X1_b - X0_b = max(b_h - b_h, c_h - b_h) + 1
  // X1_b - X0_b = max(0, Y1_b) + 1

  HLC_AVL_LINKS(x)[+1] = y;
  HLC_AVL_LINKS(x)[0] = y_parent;
  x->direction = y_direction;
  x->balance = x_balance + HLC_MAX(1, 1 + y->balance);

  return x;
}


static hlc_AVL* hlc_avl_rebalance(hlc_AVL* node) {
  assert(node != NULL);

  if (node->balance < -1) {
    assert(HLC_AVL_LINKS(node)[-1] != NULL);

    if (HLC_AVL_LINKS(node)[-1]->balance > 0) {
      //   N          N
      //  /          /
      // X    =>    Y
      //  \        /
      //   Y      X

      HLC_AVL_LINKS(node)[-1] = hlc_avl_rotate_left(HLC_AVL_LINKS(node)[-1]);
    }

    //     N
    //    /         Y
    //   Y    =>   / \
    //  /         X   N
    // X

    node = hlc_avl_rotate_right(node);

    if (HLC_AVL_LINKS(node)[0] != NULL) {
      assert(node->direction == -1 || node->direction == +1);
      HLC_AVL_LINKS(HLC_AVL_LINKS(node)[0])[node->direction] = node;
    }
  } else if (node->balance > +1) {
    assert(HLC_AVL_LINKS(node)[+1] != NULL);

    if (HLC_AVL_LINKS(node)[+1]->balance < 0) {
      // N        N
      //  \        \
      //   Y  =>    X
      //  /          \
      // X            Y

      HLC_AVL_LINKS(node)[+1] = hlc_avl_rotate_right(HLC_AVL_LINKS(node)[+1]);
    }

    // N
    //  \           X
    //   X    =>   / \
    //    \       N   Y
    //     Y

    node = hlc_avl_rotate_left(node);

    if (HLC_AVL_LINKS(node)[0] != NULL) {
      assert(node->direction == -1 || node->direction == +1);
      HLC_AVL_LINKS(HLC_AVL_LINKS(node)[0])[node->direction] = node;
    }
  }

  hlc_avl_check(node);
  return node;
}


/// @param node The node which was inserted.
/// @return The new root of the subtree where the node was inserted, after rebalancing.
static hlc_AVL* hlc_avl_update_after_insertion(hlc_AVL* node) {
  assert(node != NULL);

  while (HLC_AVL_LINKS(node)[0] != NULL) {
    assert(node->direction == -1 || node->direction == +1);
    HLC_AVL_LINKS(node)[0]->balance += node->direction;

    node = HLC_AVL_LINKS(node)[0];
    node = hlc_avl_rebalance(node);

    if (node->balance == 0)
      break;
  }

  assert(node->balance >= -1 && node->balance <= +1);
  return node;
}


/// @param node The parent of the node which was removed. The balance of said parent must already have been updated.
/// @param root The ancestor to be returned if this function returns early.
/// @return The new root of the subtree where the node was removed, after rebalancing.
static hlc_AVL* hlc_avl_update_after_removal(hlc_AVL* node, hlc_AVL* ancestor) {
  assert(node != NULL);
  assert(ancestor != NULL);

  bool ancestor_found = false;

  while (true) {
    ancestor_found |= node == ancestor;
    node = hlc_avl_rebalance(node);

    if (node->balance != 0 || HLC_AVL_LINKS(node)[0] == NULL)
      break;

    assert(node->direction == -1 || node->direction == +1);
    HLC_AVL_LINKS(node)[0]->balance -= node->direction;
    node = HLC_AVL_LINKS(node)[0];
  }

  node = ancestor_found ? node : ancestor;
  assert(node->balance >= -1 && node->balance <= +1);
  return node;
}


hlc_AVL* hlc_avl_insert(
  hlc_AVL* node,
  signed char direction,
  const void* element,
  hlc_Layout element_layout,
  hlc_Assign_instance element_assign_instance
) {
  assert(node != NULL);
  assert(direction == -1 || direction == +1);
  assert(hlc_avl_link(node, direction) == NULL);

  hlc_AVL* new = hlc_avl_new(element, element_layout, element_assign_instance);

  if (new != NULL) {
    HLC_AVL_LINKS(node)[direction] = new;
    HLC_AVL_LINKS(new)[0] = node;
    new->direction = direction;
    return hlc_avl_update_after_insertion(new);
  } else {
    return NULL;
  }
}


hlc_AVL* hlc_avl_remove(
  hlc_AVL* node,
  hlc_Layout element_layout,
  hlc_Destroy_instance element_destroy_instance
) {
  assert(node != NULL);

  if (HLC_AVL_LINKS(node)[-1] == NULL) {
    // N
    //  \   =>  a
    //   a

    hlc_AVL* a = HLC_AVL_LINKS(node)[+1];
    hlc_AVL* node_parent = HLC_AVL_LINKS(node)[0];
    signed char node_direction = node->direction;
    hlc_destroy(hlc_avl_element(node, element_layout), element_destroy_instance);
    free(node);

    if (a != NULL) {
      HLC_AVL_LINKS(a)[0] = node_parent;
      a->direction = node_direction;
    }

    if (node_parent != NULL) {
      assert(node_direction == -1 || node_direction == +1);
      HLC_AVL_LINKS(node_parent)[node_direction] = a;
      node_parent->balance -= node_direction;
      return hlc_avl_update_after_removal(node_parent, node_parent);
    } else {
      assert(a == NULL || (a->balance >= -1 && a->balance <= +1));
      return a;
    }
  } else if (HLC_AVL_LINKS(node)[+1] == NULL) {
    //   N
    //  /   =>  a
    // a

    hlc_AVL* a = HLC_AVL_LINKS(node)[-1];
    hlc_AVL* node_parent = HLC_AVL_LINKS(node)[0];
    signed char node_direction = node->direction;
    hlc_destroy(hlc_avl_element(node, element_layout), element_destroy_instance);
    free(node);

    if (a != NULL) {
      HLC_AVL_LINKS(a)[0] = node_parent;
      a->direction = node_direction;
    }

    if (node_parent != NULL) {
      assert(node_direction == -1 || node_direction == +1);
      HLC_AVL_LINKS(node_parent)[node_direction] = a;
      node_parent->balance -= node_direction;
      return hlc_avl_update_after_removal(node_parent, node_parent);
    } else {
      assert(a == NULL || (a->balance >= -1 && a->balance <= +1));
      return a;
    }
  } else if (HLC_AVL_LINKS(HLC_AVL_LINKS(node)[+1])[-1] == NULL) {
    //   N
    //  / \           X
    // a   X    =>   / \
    //      \       a   b
    //       b

    hlc_AVL* a = HLC_AVL_LINKS(node)[-1];
    hlc_AVL* x = HLC_AVL_LINKS(node)[+1];
    hlc_AVL* node_parent = HLC_AVL_LINKS(node)[0];
    signed char node_direction = node->direction;
    signed char node_balance = node->balance;
    hlc_destroy(hlc_avl_element(node, element_layout), element_destroy_instance);
    free(node);

    HLC_AVL_LINKS(a)[0] = x;

    // N_b = X0_h - a_h
    // X1_b = b_h - a_h
    //
    // X1_b - N_b = b_h - a_h - (X0_h - a_h)
    // X1_b - N_b = b_h - a_h - X0_h + a_h
    // X1_b - N_b = b_h - X0_h
    // X1_b - N_b = b_h - (b_h + 1)
    // X1_b - N_b = b_h - b_h - 1
    // X1_b - N_b = -1

    HLC_AVL_LINKS(x)[-1] = a;
    HLC_AVL_LINKS(x)[0] = node_parent;
    x->direction = node_direction;
    x->balance = node_balance - 1;

    if (node_parent != NULL) {
      assert(node_direction == -1 || node_direction == +1);
      HLC_AVL_LINKS(node_parent)[node_direction] = x;
    }

    return hlc_avl_update_after_removal(x, x);
  } else {
    //   N            X
    //  / \          / \           X
    // a   Z        a   Z         / \
    //    / \          / \       a   Z
    //   Y   d  =>    Y   d  =>     / \
    //  / \          / \           Y   d
    // X   c        N   c         / \
    //  \            \           b   c
    //   b            b

    hlc_AVL* x = hlc_avl_xmost(HLC_AVL_LINKS(node)[+1], -1);
    hlc_AVL* b = HLC_AVL_LINKS(x)[+1];
    hlc_AVL* y = HLC_AVL_LINKS(x)[0];

    hlc_avl_swap(node, x);
    hlc_destroy(hlc_avl_element(node, element_layout), element_destroy_instance);
    free(node);

    if (b != NULL) {
      HLC_AVL_LINKS(b)[0] = y;
      b->direction = -1;
    }

    HLC_AVL_LINKS(y)[-1] = b;
    y->balance += 1;

    return hlc_avl_update_after_removal(y, x);
  }
}


void hlc_avl_swap(hlc_AVL* node1, hlc_AVL* node2) {
  assert(node1 != NULL);
  assert(node2 != NULL);

  hlc_AVL* node1_left = HLC_AVL_LINKS(node1)[-1];
  hlc_AVL* node1_right = HLC_AVL_LINKS(node1)[+1];
  hlc_AVL* node1_parent = HLC_AVL_LINKS(node1)[0];
  signed char node1_direction = node1->direction;
  signed char node1_balance = node1->balance;

  HLC_AVL_LINKS(node1)[-1] = HLC_AVL_LINKS(node2)[-1];
  HLC_AVL_LINKS(node1)[+1] = HLC_AVL_LINKS(node2)[+1];
  HLC_AVL_LINKS(node1)[0] = HLC_AVL_LINKS(node2)[0];
  node1->balance = node2->balance;
  node1->direction = node2->direction;

  if (HLC_AVL_LINKS(node1)[-1] != NULL) {
    HLC_AVL_LINKS(HLC_AVL_LINKS(node1)[-1])[0] = node1;
  }

  if (HLC_AVL_LINKS(node1)[+1] != NULL) {
    HLC_AVL_LINKS(HLC_AVL_LINKS(node1)[+1])[0] = node1;
  }

  if (HLC_AVL_LINKS(node1)[0] != NULL) {
    assert(node1->direction == -1 || node1->direction == +1);
    HLC_AVL_LINKS(HLC_AVL_LINKS(node1)[0])[node1->direction] = node1;
  }

  HLC_AVL_LINKS(node2)[-1] = node1_left;
  HLC_AVL_LINKS(node2)[+1] = node1_right;
  HLC_AVL_LINKS(node2)[0] = node1_parent;
  node2->balance = node1_balance;
  node2->direction = node1_direction;

  if (HLC_AVL_LINKS(node2)[-1] != NULL) {
    HLC_AVL_LINKS(HLC_AVL_LINKS(node2)[-1])[0] = node2;
  }

  if (HLC_AVL_LINKS(node2)[+1] != NULL) {
    HLC_AVL_LINKS(HLC_AVL_LINKS(node2)[+1])[0] = node2;
  }

  if (HLC_AVL_LINKS(node2)[0] != NULL) {
    assert(node2->direction == -1 || node2->direction == +1);
    HLC_AVL_LINKS(HLC_AVL_LINKS(node2)[0])[node2->direction] = node2;
  }
}


void hlc_avl_delete(
  hlc_AVL* root,
  hlc_Layout element_layout,
  hlc_Destroy_instance element_destroy_instance
) {
  if (root != NULL) {
    hlc_avl_delete(HLC_AVL_LINKS(root)[-1], element_layout, element_destroy_instance);
    hlc_avl_delete(HLC_AVL_LINKS(root)[+1], element_layout, element_destroy_instance);

    hlc_destroy(hlc_avl_element(root, element_layout), element_destroy_instance);
    free(root);
  }
}
