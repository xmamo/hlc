#include "avl.h"

#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "math.h"


struct hlc_AVL {
  hlc_AVL* _links[3];
  signed char direction;
  signed char balance;
  double value;
};


#define HLC_AVL_LINKS(node) ((hlc_AVL**)((char*)(node) + (offsetof(hlc_AVL, _links) + sizeof(hlc_AVL*))))


hlc_AVL* hlc_avl_new(double value) {
  hlc_AVL* node = malloc(sizeof(hlc_AVL));

  if (node != NULL) {
    HLC_AVL_LINKS(node)[-1] = NULL;
    HLC_AVL_LINKS(node)[+1] = NULL;
    HLC_AVL_LINKS(node)[0] = NULL;
    node->direction = -1;
    node->balance = 0;
    node->value = value;
  }

  return node;
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


double* (hlc_avl_value_ref)(const hlc_AVL* node) {
  assert(node != NULL);
  return (double*)&node->value;
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


static hlc_AVL* hlc_avl_rotate(hlc_AVL* xy, signed char direction) {
  assert(xy != NULL);
  assert(direction == -1 || direction == +1);
  assert(HLC_AVL_LINKS(xy)[-direction] != NULL);

  //          -1          |          +1
  // ---------------------|---------------------
  //   X              Y   |     Y          X
  //  / \            / \  |    / \        / \
  // a   Y    =>    X   c |   X   c  =>  a   Y
  //    / \        / \    |  / \            / \
  //   b   c      a   b   | a   b          b   c

  hlc_AVL* yx = HLC_AVL_LINKS(xy)[-direction];
  hlc_AVL* xy_parent = HLC_AVL_LINKS(xy)[0];
  signed char xy_direction = xy->direction;
  signed char xy_balance = xy->balance;

  hlc_AVL* b = HLC_AVL_LINKS(yx)[direction];
  signed char yx_direction = yx->direction;
  signed char yx_balance = yx->balance;

  if (b != NULL) {
    HLC_AVL_LINKS(b)[0] = xy;
    b->direction *= -1;
  }

  //                                                |
  // -----------------------------------------------|--------------------------------------------
  // X0_b = Y0_h - a_h                              | Y0_b = c_h - X0_h
  // X1_b = b_h - a_h                               | Y1_b = c_h - b_h
  //                                                |
  // X1_b - X0_b = b_h - a_h - (Y0_h - a_h)         | Y1_b - Y0_b = c_h - b_h - (c_h - X0_h)
  // X1_b - X0_b = b_h - a_h - Y0_h + a_h           | Y1_b - Y0_b = c_h - b_h - c_h + X0_h
  // X1_b - X0_b = b_h - Y0_h                       | Y1_b - Y0_b = -b_h + X0_h
  // X1_b - X0_b = b_h - (max(b_h, c_h) + 1)        | Y1_b - Y0_b = -b_h + max(a_h, b_h) + 1
  // X1_b - X0_b = -(max(b_h - b_h, c_h - b_h) + 1) | Y1_b - Y0_b = max(a_h - b_h, b_h - b_h) + 1
  // X1_b - X0_b = -(max(0, Y0_b) + 1)              | Y1_b - Y0_b = max(-X0_b, 0) + 1

  HLC_AVL_LINKS(xy)[0] = yx;
  HLC_AVL_LINKS(xy)[-direction] = b;
  xy->direction = -yx_direction;
  xy->balance = xy_balance + direction * (HLC_MAX(0, -direction * yx_balance) + 1);

  //                                                |
  // -----------------------------------------------|--------------------------------------------
  // Y0_b = c_h - b_h                               | X0_b = b_h - a_h
  // Y1_b = c_h - X1_h                              | X1_b = Y1_h - a_h
  //                                                |
  // Y1_b - Y0_b = c_h - X1_h - (c_h - b_h)         | X1_b - X0_b = Y1_h - a_h - (b_h - a_h)
  // Y1_b - Y0_b = c_h - X1_h - c_h + b_h           | X1_b - X0_b = Y1_h - a_h - b_h + a_h
  // Y1_b - Y0_b = -X1_h + b_h                      | X1_b - X0_b = Y1_h - b_h
  // Y1_b - Y0_b = -(max(a_h, b_h) + 1) + b_h       | X1_b - X0_b = max(b_h, c_h) + 1 - b_h
  // Y1_b - Y0_b = -(max(a_h - b_h, b_h - b_h) + 1) | X1_b - X0_b = max(b_h - b_h, c_h - b_h) + 1
  // Y1_b - Y0_b = -(max(-X1_b, 0) + 1)             | X1_b - X0_b = max(0, Y1_b) + 1

  HLC_AVL_LINKS(yx)[0] = xy_parent;
  HLC_AVL_LINKS(yx)[direction] = xy;
  yx->direction = xy_direction;
  yx->balance = yx_balance + direction * (HLC_MAX(direction * xy->balance, 0) + 1);

  return yx;
}


static hlc_AVL* hlc_avl_rebalance(hlc_AVL* node) {
  if (node->balance < -1) {
    if (HLC_AVL_LINKS(node)[-1]->balance > 0) {
      //   N          N
      //  /          /
      // X    =>    Y
      //  \        /
      //   Y      X

      HLC_AVL_LINKS(node)[-1] = hlc_avl_rotate(HLC_AVL_LINKS(node)[-1], -1);
    }

    //     N
    //    /         Y
    //   Y    =>   / \
    //  /         X   N
    // X

    node = hlc_avl_rotate(node, +1);

    if (HLC_AVL_LINKS(node)[0] != NULL) {
      assert(node->direction == -1 || node->direction == +1);
      HLC_AVL_LINKS(HLC_AVL_LINKS(node)[0])[node->direction] = node;
    }
  } else if (node->balance > +1) {
    if (HLC_AVL_LINKS(node)[+1]->balance < 0) {
      // N        N
      //  \        \
      //   Y  =>    X
      //  /          \
      // X            Y

      HLC_AVL_LINKS(node)[+1] = hlc_avl_rotate(HLC_AVL_LINKS(node)[+1], +1);
    }

    // N
    //  \           X
    //   X    =>   / \
    //    \       N   Y
    //     Y

    node = hlc_avl_rotate(node, -1);

    if (HLC_AVL_LINKS(node)[0] != NULL) {
      assert(node->direction == -1 || node->direction == +1);
      HLC_AVL_LINKS(HLC_AVL_LINKS(node)[0])[node->direction] = node;
    }
  }

  hlc_avl_check(node);
  return node;
}


static hlc_AVL* hlc_avl_swap(hlc_AVL* x, hlc_AVL* y) {
  assert(x != NULL);
  assert(y != NULL);

  hlc_AVL* x_left = HLC_AVL_LINKS(x)[-1];
  hlc_AVL* x_right = HLC_AVL_LINKS(x)[+1];
  hlc_AVL* x_parent = HLC_AVL_LINKS(x)[0];
  signed char x_direction = x->direction;
  signed char x_balance = x->balance;

  HLC_AVL_LINKS(x)[-1] = HLC_AVL_LINKS(y)[-1];
  HLC_AVL_LINKS(x)[+1] = HLC_AVL_LINKS(y)[+1];
  HLC_AVL_LINKS(x)[0] = HLC_AVL_LINKS(y)[0];
  x->balance = y->balance;
  x->direction = y->direction;

  if (HLC_AVL_LINKS(x)[-1] != NULL) {
    HLC_AVL_LINKS(HLC_AVL_LINKS(x)[-1])[0] = x;
  }

  if (HLC_AVL_LINKS(x)[+1] != NULL) {
    HLC_AVL_LINKS(HLC_AVL_LINKS(x)[+1])[0] = x;
  }

  if (HLC_AVL_LINKS(x)[0] != NULL) {
    assert(x->direction == -1 || x->direction == +1);
    HLC_AVL_LINKS(HLC_AVL_LINKS(x)[0])[x->direction] = x;
  }

  HLC_AVL_LINKS(y)[-1] = x_left;
  HLC_AVL_LINKS(y)[0] = x_parent;
  HLC_AVL_LINKS(y)[+1] = x_right;
  y->balance = x_balance;
  y->direction = x_direction;

  if (HLC_AVL_LINKS(y)[-1] != NULL) {
    HLC_AVL_LINKS(HLC_AVL_LINKS(y)[-1])[0] = y;
  }

  if (HLC_AVL_LINKS(y)[+1] != NULL) {
    HLC_AVL_LINKS(HLC_AVL_LINKS(y)[+1])[0] = y;
  }

  if (HLC_AVL_LINKS(y)[0] != NULL) {
    assert(y->direction == -1 || y->direction == +1);
    HLC_AVL_LINKS(HLC_AVL_LINKS(y)[0])[y->direction] = y;
  }

  return y;
}


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

  return node;
}


static hlc_AVL* hlc_avl_update_after_removal(hlc_AVL* node) {
  assert(node != NULL);

  while (true) {
    node = hlc_avl_rebalance(node);

    if (node->balance != 0) {
      assert(node->balance == -1 || node->balance == +1);
      break;
    } else if (HLC_AVL_LINKS(node)[0] != NULL) {
      assert(node->direction == -1 || node->direction == +1);
      HLC_AVL_LINKS(node)[0]->balance -= node->direction;
      node = HLC_AVL_LINKS(node)[0];
    } else {
      break;
    }
  }

  return node;
}


hlc_AVL* hlc_avl_insert(hlc_AVL* node, signed char direction, double value) {
  assert(node != NULL);
  assert(direction == -1 || direction == +1);
  assert(hlc_avl_link(node, direction) == NULL);

  hlc_AVL* new = malloc(sizeof(hlc_AVL));

  if (new != NULL) {
    HLC_AVL_LINKS(new)[-1] = NULL;
    HLC_AVL_LINKS(new)[0] = node;
    HLC_AVL_LINKS(new)[+1] = NULL;
    new->direction = direction;
    new->balance = 0;
    new->value = value;

    HLC_AVL_LINKS(node)[direction] = new;

    return hlc_avl_update_after_insertion(new);
  } else {
    return NULL;
  }
}


hlc_AVL* hlc_avl_remove(hlc_AVL* node) {
  assert(node != NULL);

  if (HLC_AVL_LINKS(node)[-1] == NULL) {
    hlc_AVL* node_right = HLC_AVL_LINKS(node)[+1];
    hlc_AVL* node_parent = HLC_AVL_LINKS(node)[0];
    signed char node_direction = node->direction;
    free(node);

    if (node_right != NULL) {
      HLC_AVL_LINKS(node_right)[0] = node_parent;
      node_right->direction = node_direction;
    }

    if (node_parent != NULL) {
      assert(node_direction == -1 || node_direction == +1);
      HLC_AVL_LINKS(node_parent)[node_direction] = node_right;
      node_parent->balance -= node_direction;
      return hlc_avl_update_after_removal(node_parent);
    } else {
      assert(node_right == NULL || (node_right->balance >= -1 && node_right->balance <= +1));
      return node_right;
    }
  } else if (HLC_AVL_LINKS(node)[+1] == NULL) {
    hlc_AVL* node_left = HLC_AVL_LINKS(node)[-1];
    hlc_AVL* node_parent = HLC_AVL_LINKS(node)[0];
    signed char node_direction = node->direction;
    free(node);

    if (node_left != NULL) {
      HLC_AVL_LINKS(node_left)[0] = node_parent;
      node_left->direction = node_direction;
    }

    if (node_parent != NULL) {
      assert(node_direction == -1 || node_direction == +1);
      HLC_AVL_LINKS(node_parent)[node_direction] = node_left;
      node_parent->balance -= node_direction;
      return hlc_avl_update_after_removal(node_parent);
    } else {
      assert(node_left == NULL || (node_left->balance >= -1 && node_left->balance <= +1));
      return node_left;
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
    free(node);

    HLC_AVL_LINKS(a)[0] = x;

    // N_b = X0_h - a_h
    // X1_b = b_h - a_h
    //
    // X1_b - N_b = b_h - a_h - (X0_h - a_h)
    // X1_b - N_b = b_h - a_h - X0_h + a_h
    // X1_b - N_b = b_h - X0_h
    // X1_b - N_b = -1

    HLC_AVL_LINKS(x)[-1] = a;
    HLC_AVL_LINKS(x)[0] = node_parent;
    x->direction = node_direction;
    x->balance = node_balance - 1;

    if (node_parent != NULL) {
      assert(node_direction == -1 || node_direction == +1);
      HLC_AVL_LINKS(node_parent)[node_direction] = x;
    }

    return hlc_avl_update_after_removal(x);
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

    hlc_AVL* parent = HLC_AVL_LINKS(node)[0];
    hlc_AVL* x = hlc_avl_swap(node, hlc_avl_xmost(HLC_AVL_LINKS(node)[+1], -1));
    hlc_avl_remove(node);

    while (HLC_AVL_LINKS(x)[0] != parent) {
      x = HLC_AVL_LINKS(x)[0];
    }

    return x;
  }
}


static void hlc_avl_dot_do(const hlc_AVL* node, bool root, FILE* stream) {
  assert(node != NULL && (node->direction == -1 || node->direction == +1));
  assert(stream != NULL);

  fprintf(
    stream,
    "\tN%0*" PRIXPTR " [label=\"%g [%c, %hhd]\"];\n",
    (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
    (uintptr_t)node,
    node->value,
    node->direction < 0 ? 'L' : 'R',
    node->balance
  );

  if (HLC_AVL_LINKS(node)[-1] != NULL) {
    fprintf(
      stream,
      "\tN%0*" PRIXPTR " -> N%0*" PRIXPTR ";\n",
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node,
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)HLC_AVL_LINKS(node)[-1]
    );

    hlc_avl_dot_do(HLC_AVL_LINKS(node)[-1], false, stream);
  } else {
    fprintf(
      stream,
      "\tN%0*" PRIXPTR " -> L%0*" PRIXPTR ";\n",
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node,
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node
    );

    fprintf(
      stream,
      "\tL%0*" PRIXPTR " [shape=point];\n",
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node
    );
  }

  if (HLC_AVL_LINKS(node)[+1] != NULL) {
    fprintf(
      stream,
      "\tN%0*" PRIXPTR " -> N%0*" PRIXPTR ";\n",
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node,
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)HLC_AVL_LINKS(node)[+1]
    );

    hlc_avl_dot_do(HLC_AVL_LINKS(node)[+1], false, stream);
  } else {
    fprintf(
      stream,
      "\tN%0*" PRIXPTR " -> R%0*" PRIXPTR ";\n",
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node,
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node
    );

    fprintf(
      stream,
      "\tR%0*" PRIXPTR " [shape=point];\n",
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node
    );
  }

  if (!root && HLC_AVL_LINKS(node)[0] != NULL) {
    fprintf(
      stream,
      "\tN%0*" PRIXPTR " -> N%0*" PRIXPTR ";\n",
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node,
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)HLC_AVL_LINKS(node)[0]
    );
  }
}


void hlc_avl_dot(const hlc_AVL* node, FILE* stream) {
  assert(stream != NULL);

  if (node != NULL) {
    fputs("strict digraph {\n", stream);
    hlc_avl_dot_do(node, true, stream);
    fputs("}\n", stream);
  } else {
    fputs("strict digraph {}\n", stream);
  }
}
