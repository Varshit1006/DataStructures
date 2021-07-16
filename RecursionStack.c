#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//creating a structure with required parameters
struct list {
  int a; int b; //a and b are the array indices of lo and hi
  int line; // line number
  struct list *next;
};


typedef struct list* stack;
static stack top = NULL; //top is pointer that stores address of struct list type

//function to swap two integer elements using pointers
void swap(int *a, int *b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
  return;
}

//function to partition array with all elements less than pivot on to left side and high elements to the right side,by fixing pivot as first element of array
int partition(int *arr, int low, int high)
{
  int pivot = arr[low]; //fixing pivot as the first element of given array
  int i = low - 1, j = high + 1; //initializing left and right index

  while (1) {
    do {
      i++;
    } while (arr[i] < pivot); //find a value in left side greater than pivot

    do {
      j--;
    } while (arr[j] > pivot); //find a value in right side smaller than pivot

    if (i >= j)
      return j; //function returns the pivot's index
    swap(&arr[i], &arr[j]);
  }
}

//function that does the same thing as partition function but chooses pivot as random element in array
int partition_r(int *arr, int low, int high)
{
    srand(time(NULL));   //we use srand with time as seed so that rand doesn't generate same number always.
    int random = low + rand() % (high - low);

    swap(&arr[random], &arr[low]);

    return partition(arr, low, high); //returns the index of random pivot after sorting the array
}

//function to push elements into struct list
void push(int a, int b, int line)
{
  if (!top) { //if top is null that is entry of first element
    top = calloc(1, sizeof(struct list)); //top stores address of allocated memory to node of struct list
    top->a = a; top->b = b; top->line = line; // assigning the elements into struct list using pointer top
    return;
  } else // for entry of all other elements other than first element
    {
    stack t = calloc(1, sizeof(struct list));   //temporary pointer t that stores address of allocated memory to node of struct list
    t->a = a; t->b = b; // assigning the elements into struct list using pointer t
    t->line = line;
    t->next = top;  // creates link between the current top element and node whose address is stored in t
    top = t; // now t (which is address of newly created node) is made our new top
    return;
  }
}

//pop function to delete the topmost element
void pop(void)
{
  stack f = top; //creating a pointer f which stores the address that initially pointer top was storing
  top = top->next; // pointer top now stores the address of next node
  if (f) free(f); // now we are deleting the address of topmost element
  return;
}

//function for printing the entire stack
void printstack(stack t, int offset)
{
  printf("%*s", offset, ""); //for printing tab spaces properly we use offset
  if (t) {  //if top is not null that is there is atleast one element
    printf("{ ");
    while (t) {  // loop continues as long t pointer is null
       if (t->next){ //checking if it reached the end of its modification history
             printf("(%d, %d, %d), ", t->a, t->b, t->line);
       }else{
       printf("(%d, %d, %d) ", t->a, t->b, t->line); //printing the required parameters
       }
       t = t->next; // keeps iterating to the next element (i.e history of modification is being printed)
    } printf("}\n");
  } else printf("{ }\n");
}

void quicksort(int *arr, int low, int high, int offset) //offset is for the spaces that is to be printed in output
{
  printstack(top, offset);
  if (low < high) { //as long as low is less than high
        int pi = partition_r(arr, low, high); //getting the pivots index
        push(low, pi, __LINE__ + 1 ); // pushing array indeces of lo and pi-1
        quicksort(arr, low, pi, offset+2); // passing the same arguments into quicksort function
        pop(); // popping the record once the above function returns
        printstack(top, offset);
        push(pi+1, high, __LINE__ + 1); //similarly pushing the array indices of pi+1 and hi
        quicksort(arr, pi+1, high, offset+2); // passing the same arguments into quicksort function
        pop(); // popping the record once the above function returns
        printstack(top, offset);
    }
}

//function to print array
void printarray(int *arr, int size)
{
    int i;
    for (i = 0; i < size; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main()
{
    printf("Enter list of numbers: ");
    int n;
    scanf("%d", &n); //storing the array size into variable n
    int arr[n];
    for (int i = n-1; i >= 0; i--){
        printf("Enter your number(%d)- ", n-i);
        scanf("%d", &arr[i]); //storing the values entered into array
    }

    // visualise recursion stack
    printf("The stack modification history is as follows: \n");
    printstack(top, 0); //calling the printstack function
    push(0, n-1, __LINE__ + 1);//calling the push function
    quicksort(arr, 0, n - 1, 2); //calling the quicksort function
    pop();//calling the pop function
    printstack(top, 0);
    printf("Sorted array: \n");
    printarray(arr, n);
    return 0;
}
