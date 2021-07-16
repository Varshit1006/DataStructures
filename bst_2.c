//One will compile it locally since One will have to set dynamic linking path(b/n bst.c,bst.h,a7) during compilation,the below comments are given on how to create a .so file
/* Compilation procedure for Linux:
 * 1. gcc -c -fpic bst.c
 * 2. gcc -shared -o libbst.so bst.o
 * Link with:
 * export LD_LIBRARY_PATH=/path/to/bst:$LD_LIBRARY_PATH
 * or use rpath (run path) to embed path in executable
 * gcc -L/path/to/bst -o app app.c -lbst
 */

 /* ON BRIEF ABOUT THE CODE:
 *for insert at root part we first insert the node normally
 *and then we use rotations to bring up that node as root for a particular sub tree
 *we use the fact that node becomes the root of subtree by rotating right if it is left sub child and vice versa.
 *for delete we use functions unref and join
 *we also typedef void* to key for generic part
 *rest part of the code is similar to assignment 6
 *comments are mentioned only where the code is not obvious , to look tidy :)
 *it is made sure there is no memory leak
 */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bst.h"

/* private methods */
static void RotateLeft(Tree*, Tree*);
static void RotateRight(Tree*, Tree*);
static inline Tree** ResetParentRef(Tree*);
static void UnrefNode(Tree*);
static int nhits(Tree*, int);

/* symbols provided by the user */
void PrintInOrder(Tree*);
void PrintPreOrder(Tree*);
void PrintPostOrder(Tree*);
extern Tree *root;



Tree* CreateNode(Key k, Tree *left, Tree *right, Tree *parent) //Function for creating node and assigning the parts of struct
{
	Tree *t = calloc(1, sizeof(*t));
	if (!t) {
		log_oom();
		return t;
	}

	t->key = k;
	/* set ref to 1 */
	t->ref = 1;                         //ref is for the count of duplicates with same key value
	t->left = left;
	t->right = right;
	t->parent = parent;

	return t;
}

Tree* FindTreePtr(Tree *t, Key k, int (*cmp)(const void *a, const void *b))     //function for finding the pointer with given key value of BST
{
	/* like Find but returns pointer to node holding the key */
	Tree *r = NULL;

	if (!t)
		return r;

	Key l = t->key;

	if (cmp(k, l) < 0)
		r = t->left ? FindTreePtr(t->left, k, cmp) : NULL;
	else if (cmp(k, l) > 0)
		r = t->right ? FindTreePtr(t->right, k, cmp) : NULL;
	else r = t;

	return r;
}

static void RotateLeft(Tree *child, Tree *parent)       //Function for rotating a given subtree towards left
{
	Tree **rptr = ResetParentRef(parent);
	*rptr = child;
	child->parent = parent->parent;
	parent->parent = child;
	parent->right = child->left;
	child->left = parent;
}

static void RotateRight(Tree *child, Tree *parent)      //Function for rotating a given sub tree towards right
{
	Tree **rptr = ResetParentRef(parent);
	*rptr = child;
	child->parent = parent->parent;
	parent->parent = child;
	parent->left = child->right;
	child->right = parent;
}

void InsertAtRoot(Tree *t, Tree *root_parent, Key k, int (*cmp)(const void *a, const void *b))  //function for inserting at root of given sub tree
{
	InsertInternal(t, k, cmp, true);
        t = FindTreePtr(t, k, cmp);
	while (t->parent != root_parent) {
		if (cmp(t->parent->key, t->key) < 0)
			RotateLeft(t, t->parent);                   //rotate left is called if node is the right child
		else if (cmp(t->parent->key, t->key) > 0)
			RotateRight(t, t->parent);                  //rotate right is called if node is the left child
		else assert(false);
	}
	if (!root_parent) root = t;             //we update the root of BST if changed
}

static inline Tree** ResetParentRef(Tree *t)        //function which unlinks given node with parent
{
	if (!t)
		return NULL;
	/* root symbol provided by user */
	if (!t->parent) {
		root = NULL;
		return &root;
	}
	if (t->parent->left == t) {
		t->parent->left = NULL;
		return &t->parent->left;
	} else if (t->parent->right == t) {
		t->parent->right = NULL;
		return &t->parent->right;
	}
	return NULL;
}

void UnlinkNode(Tree *t, bool rec)          //Function to unlink the node from all its links
{
	if (!t)
		return;
	if (rec) {
		UnlinkNode(t->left, rec);
		UnlinkNode(t->right, rec);
	}
	free(t->key);
	free(t);
}


static int SubTreeSize(Tree *t)         //function for obtaining size at the given node
{
	return nhits(t, 0);
}

Tree* Join(Tree *left, Tree *right)         //join function helper for delete
{
	int lhs = SubTreeSize(left);
	int rhs = SubTreeSize(right);
	int total = lhs + rhs;
	assert(total);
	int r = rand() % total; // range is [0, total)
	if (r < lhs) {
		Tree *ret = left;
		while (left->right)                             //reach the right most sub tree in left sub tree
			left = left->right;
		if (left->left)	RotateRight(left->left, left);  //call rotate right if the right most part has a left child
		left->right = right;
		right->parent = left;
		return ret;
	} else {
		Tree *ret = right;
		while (right->left)                             //reach the left most sub tree in right sub tree
			right = right->left;
		if (right->right) RotateLeft(right->right, right);  //call rotate left if the leftmost part has a right child
		right->left = left;
		left->parent = right;
		return ret;
	}
	assert(false);
	return NULL;
}

static void UnrefNode(Tree *t)
{
	assert(t);

	/*                     Abstract
	 * If reference count drops to zero, we unlink the node.
	 * Reference counts are a way to merge duplicates into
	 * the same node to save space and maintain BST shape.
	 *                        --
	 * We follow a replacement by predecessor policy, there
	 * are three deletion strategies for three cases, assume
	 * that the node being deleted is N, then:
	 * 1) N has no children: N is simply freed, and references
	 *    in parent's slot reset to NULL.
	 * 2) N has one child: N is simply freed, but the only
	 *    child is substituted in N's place. We use pointer
	 *    manipulation to avoid expensive copying operations.
	 * 3) N has two children: Probabilistically choose a subtree
	 *    based on the subtree size and attach promote it to the
	 *    position of node being deleted, and join its detached
	 *    branch to the newly promoted node's correct side.
	 */
	if (!(--t->ref)) {
		if (!t->left && !t->right) {
			/* Case 1: N has no children */
			ResetParentRef(t);
			UnlinkNode(t, false);
			return;
		} else if ((t->left && !t->right) || (t->right && !t->left)) {
			/* Case 2: N has left OR right child, not both */
			/* This returns pointer to Tree * in parent that was reset */
			Tree **slot = ResetParentRef(t);
			/* select the child */
			Tree *n = t->left ? t->left : t->right;
			/* put the child in t's parent's free slot */
			*slot = n;
		        n->parent = t->parent;
			UnlinkNode(t, false);
			return;
		} else if (t->left && t->right) {
			/* Case 3: Promote in place and reattach
			   subtree on the other side */
			Tree **slot = ResetParentRef(t);
			Tree *r = Join(t->left, t->right);
			*slot = r;
			r->parent = t->parent;
			UnlinkNode(t, false);
		} else {
			/* This should be unreachable */
			puts("Unreachable");
			assert(false);
		}
	}
	/* if we reached this point, t survived */
	assert(t->ref >= 0);
	return;
}

bool Insert(Tree *t, Key k, int (*cmp)(const void *a, const void *b))           //The main insert function
{
	/* prob 1/n */
	if ((rand() % SubTreeSize(t)) == 0) {
		InsertAtRoot(t, t->parent, k, cmp);
		return true;
	} else return InsertInternal(t, k, cmp, false);
}

bool InsertInternal(Tree *t, Key k, int (*cmp)(const void *a, const void *b), bool raw) //the usual insert function of inserting recursively
{

	assert(t);

	bool r = false;

	if (!t)
		return r;

	Tree *new = NULL;

	Key l = t->key;

	if (cmp(k, l) < 0) {
		/* see if left slot is empty */
		if (!t->left) {
			new = CreateNode(k, NULL, NULL, t);
			if (!new) {
				log_oom();
				return false;
			}
			t->left = new;
			r = true;
		} else r = raw ? Insert(t->left, k, cmp) : InsertInternal(t->left, k, cmp, raw);
	} else if (cmp(k, l) > 0) {
		/* see if right slot is empty */
		if (!t->right) {
			new = CreateNode(k, NULL, NULL, t);
			if (!new) {
				log_oom();
				return false;
			}
			t->right = new;
			r = true;
		} else r = raw ? Insert(t->right, k, cmp) : InsertInternal(t->right, k, cmp, raw);
	} else {
		/* increase node reference but check for overflow */
		if (t->ref < UCHAR_MAX) {
 			t->ref++;
			r = true;
		} else r = false;
	}
	return r;
}

bool Delete(Tree *t, Key k, int (*cmp)(const void *a, const void *b))   //The main function delete
{
	bool r = false;

	if (!t)
		return r;

	/* protect against underflow */
	assert(t->ref >= 1);

	Key l = t->key;

	if (cmp(k, l) < 0) {
		if (!t->left) {
			/* not found */
			r = false;
		} else r = Delete(t->left, k, cmp);
	} else if (cmp(k, l) > 0) {
		if (!t->right) {
			/* not found */
			r = false;
		} else r = Delete(t->right, k, cmp);
	} else {
		/* key matches that of current node, and
		 * predecessor based replacement policy
		 * is nicely encapsulated in UnrefNode()
		 */
		UnrefNode(t);
		r = true;
		/* t was either freed or it survived, but we
		 * don't really care, both are valid delete ops
		 */
	}

	return r;
}

bool Find(Tree *t, Key k, int (*cmp)(const void *a, const void *b)) //The main function find
{

	bool r = false;

	if (!t)
		return r;

	Key l = t->key;

	/* if left or right slot is null, we return false */
	if (cmp(k, l) < 0)
		r = t->left ? Find(t->left, k, cmp) : false;
	else if (cmp(k, l) > 0)
		r = t->right ? Find(t->right, k, cmp) : false;
	else {
		/* equal to t->key */
		r = true;
		/* to find the depth, we walk up the BST until we reach
		 * NULL, which is the value of parent field of root's node
		 */
		int depth = -1;
		while (t) {
			depth++;
			t = t->parent;
		}
		/* depth is one subtracted from number of nodes followed */
		printf("true, depth: %d\n", depth);
	}

	return r;
}

void PrintTree(Tree* t)
{
	assert(t);

	printf("InOrder Traversal: ");
	PrintInOrder(t);
	printf("\n");

	printf("PreOrder Traversal: ");
	PrintPreOrder(t);
	printf("\n");

	printf("PostOrder Traversal: ");
	PrintPostOrder(t);
	printf("\n");

	return;
}

void PrintSubTree(Tree *t, Key k, int (*cmp)(const void *a, const void *b))     //the main function to print sub tree of given node
{
	assert(t);
	Tree *f = FindTreePtr(t, k, cmp);
	if (f)
		PrintTree(f);

	return;
}

static int nhits(Tree *t, int h)        //returns the number of nodes it hit while reaching to given a node , helper for calculating imbalance
{
	int r = h;
	if (!t)
		return r;

	r = nhits(t->left, r);
	r = nhits(t->right, r);

	return ++r;
}

void CalculateImbalance(Tree *t, Key k, int (*cmp)(const void *a, const void *b))       //the main function calculating imbalance
{
	assert(t);

	Tree *g = FindTreePtr(t, k, cmp);

	/* we can assume this is only invoked for valid keys
	 * but let's still play safe and not segfault if the
	 * user is malicious...
	 */
	if (g) {
		int left = nhits(g->left, 1);
		int right = nhits(g->right, 1);

		printf("%d\n", left > right ? left - right : right - left);
	} else
		printf("User provided a key that is non-existent");

	return;
}

