#include "bst.h"

Tree *root;

// constructs a temporary in place
#define GETPTR(x) (&(unsigned int){(x)})

void PrintInOrder(Tree *t)
{
	if (!t)
		return;

	unsigned char i = t->ref;

	if (t->left)
		PrintInOrder(t->left);
	while (i--) printf("%u ", *(unsigned int *)t->key);
	if (t->right)
		PrintInOrder(t->right);

	return;
}

void PrintPreOrder(Tree *t)
{
	if (!t)
		return;

	unsigned char i = t->ref;

	while (i--) printf("%u ", *(unsigned int *)t->key);
	if (t->left)
		PrintPreOrder(t->left);
	if (t->right)
		PrintPreOrder(t->right);

	return;
}

void PrintPostOrder(Tree *t)
{
	if (!t)
		return;

	unsigned char i = t->ref;

	if (t->left)
		PrintPostOrder(t->left);
	if (t->right)
		PrintPostOrder(t->right);
	while (i--) printf("%u ", *(unsigned int *)t->key);

	return;
}

static int cmp(const void *a, const void *b)    //example of user f
{
	unsigned int l = *(unsigned int *)a;
	unsigned int r = *(unsigned int *)b;
	if (l < r) return -1;
	else if (l > r) return 1;
	return 0;
}

int main(void)
{
	srand(0xdeadbeef);

	FILE *fp = fopen("./input.txt", "r");
	if (!fp) {
		puts("Failed to open input.txt: exiting...");
		return 1;
	}
	int T;
	fscanf(fp, "%d\n", &T);
	while (T--) {
		int id;
		fscanf(fp, "%d ", &id);
		switch (id) {
		case INSERT:
		{
			Key k = malloc(sizeof(unsigned int));
			fscanf(fp, "%u\n", k);
			if (!root) {
				root = CreateNode(k, NULL, NULL, NULL);
				if (!root) {
					log_oom();
					break;
				}
				printf("true\n");
			}
			else printf("%s\n", bool2str[Insert(root, k, &cmp)]);
		} break;
		case DELETE:
		{
			Key k = malloc(sizeof(unsigned int));
			fscanf(fp, "%u\n", k);
			printf("%s\n", bool2str[Delete(root, k, &cmp)]);
			/* warn but after calling the function */
			if (!root)
				printf("The root node is non-existant, please insert a node.");
            free(k);
		} break;
		case FIND:
		{
			Key k = malloc(sizeof(unsigned int));
			fscanf(fp, "%u\n", k);
			bool r = Find(root, k, &cmp);
			if (r == false) printf("false\n");
			/* warn but after calling the function */
			if (!root)
				printf("The root node is non-existent, please insert a node.");
            free(k);
		}
		 break;
		case PRINT_TREE:
		{
			if (!root) {
				printf("The root node is non-existent, please insert a node.");
				break;
			}
			PrintTree(root);
		} break;
		case PRINT_SUBTREE:
		{
			if (!root) {
				printf("The root node is non-existent, please insert a node.");
				break;
			}
			Key k = malloc(sizeof(unsigned int));
			fscanf(fp, "%u\n", k);
			PrintSubTree(root, k, &cmp);
			free(k);
		} break;
		case CALC_IMB:
		{
			if (!root) {
				printf("The root node is non-existent, please insert a node.");
				break;
			}
			Key k = malloc(sizeof(unsigned int));
			fscanf(fp, "%u\n", k);
			CalculateImbalance(root, k, &cmp);
			free(k);
		} break;
		default:
			printf("Unknown command %d\n", id);
			/* preserve T because this command was invalid */
			T++;
			break;
		}
	}
	/* let's not leak memory, valgrind gods will be angry */
	UnlinkNode(root, true);
	fclose(fp);

}

