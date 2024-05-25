#include "avl.h"

#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "math.h"


struct hlc_AVL {
  hlc_AVL* left;
  hlc_AVL* right;
  hlc_AVL* parent;
  signed char direction;
  signed char balance;
  double value;
};


hlc_AVL* hlc_avl_new(double value) {
  hlc_AVL* node = malloc(sizeof(hlc_AVL));

  if (node != NULL) {
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->direction = -1;
    node->balance = 0;
    node->value = value;
  }

  return node;
}


size_t hlc_avl_count(const hlc_AVL* root) {
  return root == NULL ? 0 : hlc_avl_count(root->left) + hlc_avl_count(root->right) + 1;
}


size_t hlc_avl_height(const hlc_AVL* root) {
  return root == NULL ? 0 : HLC_MAX(hlc_avl_height(root->left), hlc_avl_height(root->right)) + 1;
}


hlc_AVL* (hlc_avl_left)(const hlc_AVL* node) {
  assert(node != NULL);
  return node->left;
}


hlc_AVL* (hlc_avl_right)(const hlc_AVL* node) {
  assert(node != NULL);
  return node->right;
}


hlc_AVL* (hlc_avl_parent)(const hlc_AVL* node) {
  assert(node != NULL);
  return node->parent;
}


double* (hlc_avl_value_ref)(const hlc_AVL* node) {
  assert(node != NULL);
  return (double*)&node->value;
}


hlc_AVL* (hlc_avl_root)(const hlc_AVL* node) {
  assert(node != NULL);

  while (node->parent != NULL) {
    node = node->parent;
  }

  return (hlc_AVL*)node;
}


hlc_AVL* (hlc_avl_leftmost)(const hlc_AVL* node) {
  assert(node != NULL);

  while (node->left != NULL) {
    node = node->left;
  }

  return (hlc_AVL*)node;
}


hlc_AVL* (hlc_avl_rightmost)(const hlc_AVL* node) {
  assert(node != NULL);

  while (node->right != NULL) {
    node = node->right;
  }

  return (hlc_AVL*)node;
}


hlc_AVL* (hlc_avl_predecessor)(const hlc_AVL* node) {
  assert(node != NULL);

  if (node->left != NULL)
    return hlc_avl_rightmost(node->left);

  if (node->direction > 0)
    return node->parent;

  while (true) {
    node = node->parent;

    if (node == NULL)
      return NULL;

    if (node->direction == +1)
      return node->parent;
  }
}


hlc_AVL* (hlc_avl_successor)(const hlc_AVL* node) {
  assert(node != NULL);

  if (node->right != NULL)
    return hlc_avl_leftmost(node->right);

  if (node->direction < 0)
    return node->parent;

  while (true) {
    node = node->parent;

    if (node == NULL)
      return NULL;

    if (node->direction == -1)
      return node->parent;
  }
}


static size_t hlc_avl_check(const hlc_AVL* node) {
  if (node != NULL) {
    size_t left_height = hlc_avl_check(node->left);
    size_t right_height = hlc_avl_check(node->right);

    assert(left_height == right_height - node->balance);
    assert(node->left == NULL || (node->left->parent == node && node->left->direction == -1));
    assert(node->right == NULL || (node->right->parent == node && node->right->direction == +1));

    return HLC_MAX(left_height, right_height) + 1;
  } else {
    return 0;
  }
}


static hlc_AVL* hlc_avl_rotate_left(hlc_AVL* x) {
  assert(x != NULL && x->right != NULL);

  //   X              Y
  //  / \            / \
  // a   Y    =>    X   c
  //    / \        / \
  //   b   c      a   b

  hlc_AVL* y = x->right;
  hlc_AVL* x_parent = x->parent;
  signed char x_direction = x->direction;
  signed char x_balance = x->balance;

  hlc_AVL* b = y->left;
  signed char y_direction = y->direction;
  signed char y_balance = y->balance;

  if (b != NULL) {
    b->parent = x;
    b->direction *= -1;
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

  x->right = b;
  x->parent = y;
  x->direction = -y_direction;
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

  y->left = x;
  y->parent = x_parent;
  y->direction = x_direction;
  y->balance = y_balance - HLC_MAX(1 - x->balance, 1);

  return y;
}


static hlc_AVL* hlc_avl_rotate_right(hlc_AVL* y) {
  assert(y != NULL && y->left != NULL);

  //     Y          X
  //    / \        / \
  //   X   c  =>  a   Y
  //  / \            / \
  // a   b          b   c

  hlc_AVL* x = y->left;
  hlc_AVL* y_parent = y->parent;
  signed char y_direction = y->direction;
  signed char y_balance = y->balance;

  hlc_AVL* b = x->right;
  signed char x_direction = x->direction;
  signed char x_balance = x->balance;

  if (b != NULL) {
    b->parent = y;
    b->direction *= -1;
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

  y->left = b;
  y->parent = x;
  y->direction = -x_direction;
  y->balance = y_balance + HLC_MAX(1 - x_balance, 1);

  //     Y          X
  //    / \        / \
  //   X   c  =>  a   Y
  //  / \            / \
  // a   b          b   c

  x->right = y;
  x->parent = y_parent;
  x->direction = y_direction;
  x->balance = x_balance + HLC_MAX(1, 1 + y->balance);

  return x;
}


static hlc_AVL* hlc_avl_rebalance(hlc_AVL* node) {
  if (node->balance < -1) {
    if (node->left->balance > 0) {
      //   N          N
      //  /          /
      // X    =>    Y
      //  \        /
      //   Y      X

      node->left = hlc_avl_rotate_left(node->left);
    }

    //     N
    //    /         Y
    //   Y    =>   / \
    //  /         X   N
    // X

    node = hlc_avl_rotate_right(node);

    if (node->parent != NULL) {
      assert(node->direction == -1 || node->direction == +1);
      *(node->direction < 0 ? &node->parent->left : &node->parent->right) = node;
    }
  } else if (node->balance > +1) {
    if (node->right->balance < 0) {
      // N        N
      //  \        \
      //   Y  =>    X
      //  /          \
      // X            Y

      node->right = hlc_avl_rotate_right(node->right);
    }

    // N
    //  \           X
    //   X    =>   / \
    //    \       N   Y
    //     Y

    node = hlc_avl_rotate_left(node);

    if (node->parent != NULL) {
      assert(node->direction == -1 || node->direction == +1);
      *(node->direction < 0 ? &node->parent->left : &node->parent->right) = node;
    }
  }

  hlc_avl_check(node);
  return node;
}


static hlc_AVL* hlc_avl_swap(hlc_AVL* x, hlc_AVL* y) {
  assert(x != NULL);
  assert(y != NULL);

  hlc_AVL* x_left = x->left;
  hlc_AVL* x_right = x->right;
  hlc_AVL* x_parent = x->parent;
  signed char x_direction = x->direction;
  signed char x_balance = x->balance;

  x->left = y->left;
  x->right = y->right;
  x->parent = y->parent;
  x->balance = y->balance;
  x->direction = y->direction;

  if (x->left != NULL) {
    x->left->parent = x;
  }

  if (x->right != NULL) {
    x->right->parent = x;
  }

  if (x->parent != NULL) {
    assert(x->direction == -1 || x->direction == +1);
    *(x->direction < 0 ? &x->parent->left : &x->parent->right) = x;
  }

  y->left = x_left;
  y->right = x_right;
  y->parent = x_parent;
  y->balance = x_balance;
  y->direction = x_direction;

  if (y->left != NULL) {
    y->left->parent = y;
  }

  if (y->right != NULL) {
    y->right->parent = y;
  }

  if (y->parent != NULL) {
    assert(y->direction == -1 || y->direction == +1);
    *(y->direction < 0 ? &y->parent->left : &y->parent->right) = y;
  }

  return y;
}


static hlc_AVL* hlc_avl_update_after_insertion(hlc_AVL* node) {
  assert(node != NULL);

  while (node->parent != NULL) {
    assert(node->direction == -1 || node->direction == +1);
    node->parent->balance += node->direction;

    node = node->parent;
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
    } else if (node->parent != NULL) {
      node->parent->balance -= node->direction;
      node = node->parent;
    } else {
      break;
    }
  }

  return node;
}


hlc_AVL* hlc_avl_insert_left(hlc_AVL* node, double value) {
  assert(node != NULL && hlc_avl_left(node) == NULL);

  hlc_AVL* new = malloc(sizeof(hlc_AVL));

  if (new != NULL) {
    new->left = NULL;
    new->right = NULL;
    new->parent = node;
    new->direction = -1;
    new->balance = 0;
    new->value = value;

    node->left = new;

    return hlc_avl_update_after_insertion(new);
  } else {
    return NULL;
  }
}


hlc_AVL* hlc_avl_insert_right(hlc_AVL* node, double value) {
  assert(node != NULL && hlc_avl_right(node) == NULL);

  hlc_AVL* new = malloc(sizeof(hlc_AVL));

  if (new != NULL) {
    new->left = NULL;
    new->right = NULL;
    new->parent = node;
    new->direction = +1;
    new->balance = 0;
    new->value = value;

    node->right = new;

    return hlc_avl_update_after_insertion(new);
  } else {
    return NULL;
  }
}


hlc_AVL* hlc_avl_remove(hlc_AVL* node) {
  assert(node != NULL);

  if (node->left == NULL) {
    hlc_AVL* node_right = node->right;
    hlc_AVL* node_parent = node->parent;
    signed char node_direction = node->direction;
    free(node);

    if (node_right != NULL) {
      node_right->parent = node_parent;
      node_right->direction = node_direction;
    }

    if (node_parent != NULL) {
      *(node_direction < 0 ? &node_parent->left : &node_parent->right) = node_right;
      node_parent->balance -= node_direction;
      return hlc_avl_update_after_removal(node_parent);
    } else {
      assert(node_right == NULL || (node_right->balance >= -1 && node_right->balance <= +1));
      return node_right;
    }
  } else if (node->right == NULL) {
    hlc_AVL* node_left = node->left;
    hlc_AVL* node_parent = node->parent;
    signed char node_direction = node->direction;
    free(node);

    if (node_left != NULL) {
      node_left->parent = node_parent;
      node_left->direction = node_direction;
    }

    if (node_parent != NULL) {
      *(node_direction < 0 ? &node_parent->left : &node_parent->right) = node_left;
      node_parent->balance -= node_direction;
      return hlc_avl_update_after_removal(node_parent);
    } else {
      assert(node_left == NULL || (node_left->balance >= -1 && node_left->balance <= +1));
      return node_left;
    }
  } else if (node->right->left == NULL) {
    //   N
    //  / \           X
    // a   X    =>   / \
    //      \       a   b
    //       b

    hlc_AVL* a = node->left;
    hlc_AVL* x = node->right;
    hlc_AVL* node_parent = node->parent;
    signed char node_direction = node->direction;
    signed char node_balance = node->balance;
    free(node);

    a->parent = x;

    // N_b = X0_h - a_h
    // X1_b = b_h - a_h
    //
    // X1_b - N_b = b_h - a_h - (X0_h - a_h)
    // X1_b - N_b = b_h - a_h - X0_h + a_h
    // X1_b - N_b = b_h - X0_h
    // X1_b - N_b = -1

    x->left = a;
    x->parent = node_parent;
    x->direction = node_direction;
    x->balance = node_balance - 1;

    if (node_parent != NULL) {
      *(node_direction < 0 ? &node_parent->left : &node_parent->right) = x;
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

    hlc_AVL* parent = node->parent;
    hlc_AVL* x = hlc_avl_swap(node, hlc_avl_leftmost(node->right));
    hlc_avl_remove(node);

    while (x->parent != parent) {
      x = x->parent;
    }

    return x;
  }
}


static void hlc_avl_dot_do(const hlc_AVL* node, FILE* stream) {
  assert(node != NULL && (node->direction == -1 || node->direction == +1));

  fprintf(
    stream,
    "\tN%0*" PRIXPTR " [label=\"%g [%c, %hhd]\"];\n",
    (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
    (uintptr_t)node,
    node->value,
    node->direction < 0 ? 'L' : 'R',
    node->balance
  );

  if (node->left != NULL) {
    fprintf(
      stream,
      "\tN%0*" PRIXPTR " -> N%0*" PRIXPTR ";\n",
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node,
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node->left
    );

    hlc_avl_dot_do(node->left, stream);
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

  if (node->right != NULL) {
    fprintf(
      stream,
      "\tN%0*" PRIXPTR " -> N%0*" PRIXPTR ";\n",
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node,
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node->right
    );

    hlc_avl_dot_do(node->right, stream);
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

  if (node->parent != NULL) {
    fprintf(
      stream,
      "\tN%0*" PRIXPTR " -> N%0*" PRIXPTR ";\n",
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node,
      (int)((CHAR_BIT * sizeof(uintptr_t) + 3) / 4),
      (uintptr_t)node->parent
    );
  }
}


void hlc_avl_dot(const hlc_AVL* node, FILE* stream) {
  assert(stream != NULL);

  if (node != NULL) {
    fputs("strict digraph {\n", stream);
    hlc_avl_dot_do(node, stream);
    fputs("}\n", stream);
  } else {
    fputs("strict digraph {}\n", stream);
  }
}
