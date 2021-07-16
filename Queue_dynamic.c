    /* Simulation of a dynamic RR algorithm over a job queue */
    //the following code prints the necessary in terminal and also saves it in a text file named output
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include <assert.h>

    #define QUANTUM 50


    #define INITIAL_Q 100
    #define APPEND_JOBS 64

    static unsigned int idref = 1;

    //creating a struct named job
    struct Job {
      unsigned int id;                                          //includes the job id
      int time_slice;                                           //includes the amount of processor time that is being requested of the processor
      int rem;                                                  //rem is the remaining time before job must be placed at end of queue
      struct Job *next;                                         //creating a single link between the jobs
    };

    struct Job* init_queue(int size)                            //function to create a queue of specific size using linked lists
    {
      struct Job *head = calloc(1, sizeof(struct Job));         //creating a node of type job dynamically whose address is stored in head
      struct Job *ret = head;                                   //creating a temporary variable equals to head which stores the address of first node

      while (--size) {                                          //loop continues as many times as integer size
        head->next = calloc(1, sizeof(struct Job));             // creating nodes of (number=size) and Link elements of the list to each other
        head = head->next;                                      //keep moving the head to its next position
      }
      assert(head->next == NULL);                               //just checking if head reached the last element
      head = ret;                                               //getting back head to point first node
      while (head) {                                            //loop continues as long as last element in list is reached

        head->id = idref++;                                     //assigning id of the node
        head->time_slice = rand()%301;                          // generate a random number between 0 and 300
        if(QUANTUM < head->time_slice){
            head->rem = QUANTUM;                                //if time slice is greater than quantum assign rem of job as equal to quantum
        }else{
            head->rem = head->time_slice;                       //if time slice is less than quantum assign rem of job as equal to time slice
        }
        head = head->next;                                      //keep moving head to its next node in the list
      }
      return ret;                                               //returning the original head i.e pointer that stores address of first element
    }

    struct Job* fetch_tail(struct Job *j)                       //function to get the tail of list
    {
      while (j->next)                                           //loop continues as long as j reaches the last element
        j = j->next;                                            //keep moving the j to its next element
      return j;                                                 //returns the pointer to the last element/tail of list
    }

    static int size = APPEND_JOBS;

    struct Job* append_to(struct Job *j)                         //creating a function to append new batch of jobs at the end of queue
    {

      assert(!j->next);                                           // confirm this is the tail
      // starts with 64 to 32 to 16 ... up to 1
      struct Job *a = init_queue(size);                           //storing the address of first node of the queue in pointer a(head)
      j->next = a;                                                //creating a link between the tail of original queue and head of newly created queue
      size /= 2;                                                  //dividing size by 2 as asked in question
      return fetch_tail(a);                                       //returning the pointer of tail of newly created queue
    }

      void printq(struct Job *j,FILE *f)                              //function to print the entire queue
    {
      while (j) {                                           //loop continues until the last element of list is reached
        printf("J%d:%d;", j->id, j->time_slice);            //prints the job id and time corresponding to it
        fprintf(f,"J%d:%d;", j->id, j->time_slice);
        j = j->next;                                        //keep moving the pointer to its next position
        if (j){                                             //if next position is not null,i.e if it is not last element
            printf(" ");                                    //just for creating space between two jobs
            fprintf(f," ");
        }
      }
       printf("\n");
       fprintf(f,"\n");
    }

    struct Job* process(struct Job *j, struct Job *t, int qtm)                              //creating a function to perform RR algorithm for each job
    {
      if (!j || !t){
        return NULL;
      }
      /* Our specification and constraints:
       * A process request has a limited time quantum in the beginning, set to QUANTUM.
       * We take the head of job queue and process it for min(time,rem,qtm). rem is quota
       * of time available to the job, before it is moved to tail of the job queue.
       * It will be some value equal to or less than QUANTUM.
       *
       * After job is processed, we have to reduce the time processed from it's total
       * time slice, i.e. j->time_slice. Then, we have three options:
       * 1. If j->time_slice drops to zero, we delete the job and return next node.
       * 2. Otherwise, for consumed rem we move j to tail and return the next node.
       * 3. If job still has rem left, we need to reprocess it in next request, without
       *    moving it to tail for now.
       *
       */


      int min ;                                         // min is The minimum of qtm, j->rem, and j->time_slice gives us the time a job will be run
      if(j->rem < qtm){
        min = j->rem ;                                  //if rem is less than qtm than min is rem
      }else{
          min = qtm;                                    //if qtm is less than rem than min is qtm
      }
      if(min < j->time_slice){                          //if min is less than time slice than min in min itself
            min = min ;
        }else{
            min =j->time_slice;                         //if min is greater than time slice than min is changed to time slice
        }

    //now that we obtained min as minimum of all three parameters

      qtm -= min;                                       //qtm is reduced by min
      j->rem -= min;                                    //rem is also reduced by min
      j->time_slice -= min;                             //time slice is also reduced by min
      //we know that atleast one of the three parameters(qtm,rem,time slice) will become equal to zero

      /*just confirming that three parameters(qtm,rem,time slice) are greater than or equal to zero*/
      assert(qtm >= 0);
      assert(j->rem >= 0);
      assert(j->time_slice >= 0);


      struct Job* new;                              //creating a pointer new for knowing what to process next
      if(j->next){
        new = j->next ;                             //if next node is not null
      }else{
        new = NULL;                                 //if next node is null
      }
      if (!j->time_slice) {                         // if time slice is zero This job is over

        free(j);                                    //deleting it from queue
      } else if (!j->rem) {                         // if rem is zero replenish rem

        j->rem = QUANTUM;                           //making back rem to quantum

        /*and placing it at end of queue*/
        t->next = j;
        j->next = NULL;
        t = j;
      } else{                                       //if qtm is zero
       new = j;                                     // this node will be reprocessed
      }
      if (qtm){                                     //if qtm is not zero
        return process(new, t, qtm);                // recurse on remaining tail with left over time
      }else{                                        //if qtm is zero
          return new;                               //node will be returned again
      }
    }

    int main()
    {
      srand(time(NULL));                                    //giving time as seed for srand function
      FILE *f;
	  f = fopen("Output.txt","w");	                        //pointer for the output file
      struct Job *head = init_queue(INITIAL_Q);             //creating initial queue of size 100 and returning 1st job as head of queue
      struct Job *tail = fetch_tail(head);                  //calling fetch tail function and returning the last job as tail of queue
      unsigned int time = 0;
      printf("Initial Queue: ");
      fprintf(f,"Initial Queue: ");
      printq(head,f);                                         //calling print queue function to print the initial queue
      for (int i = 0; ;i++, time += QUANTUM) {
        if (i == 4 && size) {                               //for time 200us and its multiples
          // append and get new tail
          printf("\n\n******%dus passed, added %d new jobs to end\n", time , size);
          fprintf(f,"\n\n******%dus passed, added %d new jobs to end\n", time, size);
          tail = append_to(tail);

        }else if(i==0 || i==1 || i==2 || i==3){              //for i = 0,1,2,3 i.e for time 0,50,100,150 us
        head = process(head, tail, QUANTUM);                //RR algorithm is performed starting from head of queue and returning new head
        if (!head){                                         //if head is null,i.e no element breaks out of the for loop
           break;
        }
        tail = fetch_tail(head);                            //getting the new tail after performing line 168
        if (!tail){                                         //if tail is null,i.e no element breaks out of the for loop
            break;
        }
        printf("\nQueue at end of %d us: ", time + QUANTUM);            //prints chain at end of every 50 us
        fprintf(f,"\nQueue at end of %d us: ", time + QUANTUM);
        printq(head,f);
        }else{                                              //for i not equal y=to multiplies of 4 and not equal to 1,2,3,4
        head = process(head, tail, QUANTUM);                //RR algorithm is performed starting from head of queue and returning new head
        if (!head){                                         //if head is null,i.e no element breaks out of the for loop
           break;
        }
        tail = fetch_tail(head);                            //getting the new tail after performing line 168
        if (!tail){                                         //if tail is null,i.e no element breaks out of the for loop
            break;
        }
        printf("\nQueue at end of %d us: ", time);            //prints chain at end of every 50 us
        fprintf(f,"\nQueue at end of %d us: ", time);
        printq(head,f);
        }
      }
      printf("\nQueue at end of %d us: Empty\n", time);   //this gets printed if there is no element left i.e for lines 169 and 173
      fprintf(f,"\nQueue at end of %d us: Empty\n", time);
      return 0;
    }
