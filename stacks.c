#include <stdio.h>
#include<stdlib.h>
#include <math.h>//for including pow inorder to calculate exponents

//struct for storing input infix expression and to store left parenthesis during validation
struct expr
{
    char character;
    struct expr *right;
};

//struct for storing operators and left parenthesis for printing postfix expression
struct opStack
{
    char oprtr;
    struct opStack *down;
    int rank;
};

//struct used to evaluate postfix expression
struct postFix
{

    float num;
    struct postFix *down;
};

//struct used to store position of brackets during validation
struct pos
{
    int position;
    struct pos* back;
};

//function to create and return pointer of newly created struct expr
struct expr* createChar()
{
    struct expr *ptr=(struct expr *)malloc(sizeof(struct expr));    //dynamically allocates memory for struct expr
    ptr->right=NULL;
    return ptr;
};

struct opStack* createOp()
{
    struct opStack *ptr=(struct opStack *)malloc(sizeof(struct opStack)); //dynamically allocates memory for struct opStack used in stack making for operator stack
    ptr->down=NULL;
    return ptr;
};

struct postFix *createPF()
{
    struct postFix *ptr=(struct postFix *)malloc(sizeof(struct postFix));; //dynamically allocates memory for struct postFix used in stack making for postfix evaluation
    ptr->down=NULL;

    return ptr;
};

struct pos* createPos()
{
    struct pos *ptr=(struct pos *)malloc(sizeof(struct pos)); //dynamically allocates memory for struct pos
    ptr->back=NULL;
    return ptr;
};


//function to display evaluated postfix result
void show(struct postFix *h)
{
    struct postFix *temp=h;
    while(1)
    {
        printf("%f ",temp->num);
        if(temp->down==NULL)
            break;
        temp=temp->down;
    }
    printf("\n");
}

//function to return complementary bracket
char comp(char b)
{
    if(b==')')
        return '(';
    if(b=='}')
        return '{';
    if(b==']')
        return '[';
    if(b=='(')
        return ')';
    if(b=='{')
        return '}';
    if(b=='[')
        return ']';
}

//function which assigns precidence to operators
int assignRank(char c)
{
    if(c=='+' || c=='-')
        return 3;
    else if(c=='/' || c=='*')
        return 2;
    else if(c=='^')
        return 1;
    else if(c=='(' || c==')'||c=='{'||c=='}'||c=='['||c==']')
        return 4;
}

float add(float x,float y)//addition
{
    return x+y;//returns sum
}
float sub(float x,float y)//subtraction
{
    return x-y;//returns difference
}
float prod(float x,float y)//product
{
    return x*y;
}
float divi(float x,float y)//division
{
    return (float)x/y;
}
float raise(float x,float y)//to calculate x raised to the power of y
{
    return pow(x,y);
}

//function which calls the add,sub,divi,raise based on the operator
float perform(char op,struct postFix *pf_stack)
{
    float a,b;
    struct postFix *temp=pf_stack->down;
    b=pf_stack->num;
    a=temp->num;
    if(op=='+')
        return add(a,b);
    if(op=='-')
        return sub(a,b);
    if(op=='*')
        return prod(a,b);
    if(op=='/')
        return divi(a,b);
    if(op=='^')
        return raise(a,b);
};

//function to pop the postfix stack during evaluation
struct postFix* popPF(struct postFix* pf_stack)
{
    struct postFix *temp=pf_stack->down;
    if(temp==NULL)
    {
        free(pf_stack);//clearing space as it is not used any further
        return NULL;
    }
    free(pf_stack);//clearing space as it is not used any further
    pf_stack=temp;
    return pf_stack;
};

//function to push into the postfix stack during evaluation
struct postFix* pushPF(struct postFix *pf_stack,char op,int c,int s,int mul)
{
    float cal;
    if(c==1)//denotes argument op is an operator
    {
        cal=perform(op,pf_stack);//calculates operations of intermediate operators during postfix evaluation
        pf_stack=popPF(pf_stack);//pops second operand for any intermediate postfix operator during postfix evaluation
        pf_stack->num=cal;//replacing first operand with the result of the performed intermediate operation which is stored in cal
        return pf_stack;
    }
    if(s==0)//denotes argument op is an integer
    {
        struct postFix *temp=createPF();
        temp->num=(int)op-48;//converting digit stored as char to its integer form and storing it into float
        temp->num=(temp->num)*mul;
        if(pf_stack!=NULL)
            temp->down=pf_stack;
        pf_stack=temp;
        return pf_stack;
    }
    if(s==1)//denotes argument op is a digit of a non-single digit integer
    {
        float t=pf_stack->num;
        if(t>=0)
            t=t*10+(int)op-48;
        else
            t=t*10-(int)op+48;

        pf_stack->num=t;//assigning the integer to float
        return pf_stack;
    }
};

//function to pop the operator stack while printing out postfix expression
struct opStack* pop(struct opStack *op_stack)
{
    char t=op_stack->oprtr;

    if(op_stack->down==NULL)
    {
        if(t!='(' && t!='{' && t!='[')//to not print brackets while printing out postfix expression
        {
            printf("%c ",op_stack->oprtr);
        }
        free(op_stack);
        return NULL;
    }

    struct opStack *temp;
    temp=op_stack->down;

    if(t!='(' && t!='{' && t!='[') //to not print brackets while printing out postfix expression
    {
        printf("%c ",op_stack->oprtr);

    }
    free(op_stack);
    op_stack=temp;

    return op_stack;
}

//function to push the operator stack while printing out postfix expression
struct opStack* push(struct opStack *op_stack,char ch)
{
    if(ch==')'||ch=='}'||ch==']')
    {
        while((op_stack->oprtr)!=comp(ch))//to print out all operators stored in the operator stack by popping them out if a right bracket is found while printing out postfix expression
        {
            op_stack=pop(op_stack);
        }
        op_stack=pop(op_stack);//pops the element of stack where the left bracket is stored
        return op_stack;
    }

    while(op_stack!=NULL && assignRank(ch)>=op_stack->rank && ch!='(' && ch!='{' && ch!='[')
    {
        op_stack=pop(op_stack);//to pop elements of higher precedence having operators of operator stack if a lower precedence having operator is found
    }
    struct opStack *temp=createOp();//creating space to push the operator into operator stack
        temp->oprtr=ch;
        temp->rank=assignRank(ch);
    if(op_stack==NULL)
        op_stack=temp;
    else
    {
        temp->down=op_stack;
        op_stack=temp;
    }
    return op_stack;
}

//function to validate the input expression
int validate(struct expr *header)
{
    struct expr *ptr=header,*temp;

    if(ptr->character==42 || ptr->character==47 || ptr->character==94)
    {
        printf("INVALID INPUT : Invalid operator at position 1\n");
        return 0;
    }

    int position=0;
    char a;
    char b;
    while(ptr->right!=NULL)
    {
        position++;
        a=ptr->character;
        temp=ptr->right;
        b=temp->character;
        if(a!='(' && a!=')' && a!='{' && a!='}' && a!='[' && a!=']' && a!=42 && a!=43 && a!=45 && a!=47 && a!=94)
        {
            if(a<48 || a>57)
            {
                printf("INVALID INPUT : Invalid character found at position %d\n",position);
                return 0;
            }
        }
        if(a==42 || a==43 || a==45 || a==47 || a==94)
        {
            if(b==42 || b==43 || b==45 || b==47 || b==94)
            {
                printf("INVALID INPUT : Consecutive operators found at position %d and %d\n",position,position+1);
                return 0;
            }
        }
        if(a>=48 && a<=57)
        {
            if(b=='(' || b== '{' || b=='[')
            {
                printf("INVALID INPUT : No operand between number and bracket - position %d and %d\n",position,position+1);
                return 0;
            }
        }
        ptr=ptr->right;//to iterate through the input expression
    }
    struct expr* bracketC=NULL,*newS=NULL;//bracketC points to top of stack having left brackets
    struct pos* bracketPos=NULL,*newPos=NULL;//bracketPos points to top of stack which maintains data of position of left brackets
    ptr=header;
    position=0;

    while(ptr->right!=NULL)
    {
        position++;
        a=ptr->character;
        temp=ptr->right;
        b=temp->character;
        if(b==')' || b=='}' || b==']')
        {
            if(a==42 || a==43 || a==45 || a==47 || a==94)
            {
                printf("INVALID INPUT : Invalid operator at position %d without a second operator\n",position);
                return 0;
            }
        }
        if(a=='(' || a==')' || a=='{' || a=='}' || a=='[' || a==']')
        {
            if(a=='(' || a== '{' || a=='[')
            {
                if(b==42 || b==47 || b==94)
                {
                    printf("INVALID INPUT : Invalid operator at position %d\n",position+1);
                    return 0;
                }
                newS=createChar();//stack to store left brackets
                newS->character=a;
                newPos=createPos();//stack to keep data of the position of left brackets to find any unmatched brackets
                newPos->position=position;
                if(bracketC==NULL)
                {
                    bracketC=newS;
                    bracketPos=newPos;
                }
                else
                {
                    newS->right=bracketC;
                    bracketC=newS;
                    newPos->back=bracketPos;
                    bracketPos=newPos;
                }
            }
            else
            {
                if(b=='(' || b== '{' || b=='[')
                {
                    printf("INVALID INPUT : No operator in between brackets of position %d and %d\n",position,position+1);
                    return 0;
                }
                if(bracketC==NULL)
                {
                    printf("INVALID INPUT : Unmatched right bracket found at position %d\n",position);
                    return 0;
                }
                if(bracketC->character!=comp(a))
                {
                    printf("INVALID INPUT : Invalid right bracket found at position %d, there should be a \" %c \"\n",position,comp(bracketC->character));
                    return 0;
                }
                bracketC=bracketC->right;
                bracketPos=bracketPos->back;
            }

        }
        ptr=ptr->right;
    }
    if(bracketC!=NULL)
    {
        printf("INVALID INPUT : Unmatched bracket found at position %d\n",bracketPos->position);
        return 0;
    }

    return 1;
}

int main()
{
    struct expr *ch,*temp=NULL;
    struct postFix *eval=NULL;//points to top of stack which is used to evaluate postfix expression
    char input;//used as temporary variable to store the input infix expression character wise into input linked-list pointed by head pointer
    struct expr *head;//points to beginning of input infix expression taken from user

    printf("Enter Input : ");
    do
    {
        ch = createChar();
        scanf("%c",&input);//character wise scanning input
        ch->character=input;
        if(temp!=NULL)
            temp->right=ch;//creates link
        else
            head=ch;
        temp=ch;

    }while(input!='\n');

    int validity=validate(head);//zero is returned in case of invalidity
    if(validity==0){
        return 0;
    }else{
        printf("Valid\n");
    }

    struct opStack *stack=NULL;//points to top of operator stack

    ch=head;//ch is used to go through the input infix expression linked list
    char a,b,c;//a stores current char, b stores char of next node, and c stores char of previous node
    int mul=1;
    while(1)
    {
        if(ch->right!=NULL)
        {
            temp=ch->right;
            a=ch->character;
            b=temp->character;

            if(eval==NULL && a=='-')
                mul=-1;
            else if(eval==NULL && a=='+')
                mul=1;
            else if(a>=48 && a<=57 && b>=48 && b<=57)//for more than one digit numbers
            {
                if(eval!=NULL && c>=48 && c<=57)//for more than one digit numbers
                {
                    eval=pushPF(eval,a,0,1,mul) ;//'1' tells the ppushPF function that this a is not a new independent digit, but the digit of a non_single digit number being pushed
                }
                else
                    eval=pushPF(eval,a,0,0,mul);
                printf("%c",a);
                mul=mul*mul;
            }
            else if(a>=48 && a<=57)//single digit numbers
            {
                if(eval!=NULL && c>=48 && c<=57)
                {
                    eval=pushPF(eval,a,0,1,mul);
                }
                else
                    eval=pushPF(eval,a,0,0,mul);
                printf("%c ",a);
                mul=mul*mul;
            }
            else//for brackets and operators
            {
                if(stack!=NULL)
                {   char chr=a;
                    if(chr==')'||chr=='}'||chr==']')
                    {
                        struct opStack *op_stack=stack;
                        while((op_stack->oprtr)!=comp(chr))
                        {
                            eval=pushPF(eval,op_stack->oprtr,1,0,mul);//simultaneous evaluation of postfix expression
                            op_stack=op_stack->down;//going through operator stack
                        }//for the case when a right bracket is found
                        op_stack=op_stack->down;
                    }
                    else
                    {
                        char chr=a;
                        struct opStack *op_stack=stack;
                        while(op_stack!=NULL && assignRank(chr)>=op_stack->rank && chr!='(' && chr!='{' && chr!='[')
                        {
                            eval=pushPF(eval,op_stack->oprtr,1,0,mul);//simultaneous evaluation of postfix expression
                            op_stack=op_stack->down;//going through operator stack
                        }//for the case of finding a lower precedent operator relative to the last existing
                    }
                }
                stack=push(stack,a);
                mul=mul*mul;
            }
            c=a;
        }
        else
            break;

        ch=ch->right;

    }


    while(stack!=NULL)//this loop is for popping the remained operators in the operator stack
    {
        eval=pushPF(eval,stack->oprtr,1,0,mul);//simultaneous operator wise evaluation during the final popping
        stack=pop(stack);
    }
    printf("\nRESULT : ");
    show(eval);//only the final evaluated post fix result will be present in the eval stack


    free(stack);

    return 0;
}

