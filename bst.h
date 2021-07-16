#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef void *Key;

typedef struct node {               //struct node with given parameters
	Key key;
	unsigned char ref;
	unsigned char __pad[3];
	unsigned int height;
	struct node *left;
	struct node *right;
	struct node *parent;
} Tree;

enum method_id {                //for the menu
	INSERT = 1,
	DELETE,
	FIND,
	PRINT_TREE,
	PRINT_SUBTREE,
	CALC_IMB,
	_METHOD_MAX,
};

static char* bool2str[] = { "false", "true" };  //for printing the output

#define log_oom() puts("Memory allocation failed.")

/* public methods */
bool Insert(Tree*, Key, int (*cmp)(const void *, const void *));
bool Delete(Tree*, Key, int (*cmp)(const void *, const void *));
bool Find(Tree*, Key, int (*cmp)(const void *, const void *));
void PrintTree(Tree*);
void PrintSubTree(Tree*, Key, int (*cmp)(const void *, const void *));
void CalculateImbalance(Tree*, Key, int (*cmp)(const void *, const void *));
/* helper methods */
Tree* CreateNode(Key, Tree*, Tree*, Tree*);
Tree* FindTreePtr(Tree*, Key, int (*cmp)(const void *, const void *));
void InsertAtRoot(Tree*, Tree*, Key, int (*cmp)(const void *, const void *));
bool InsertInternal(Tree*, Key, int (*cmp)(const void *, const void *), bool);
void UnlinkNode(Tree*, bool);

