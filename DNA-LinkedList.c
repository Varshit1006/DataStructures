#include <stdio.h>
#include <stdlib.h>

struct DNA//structure representing a nucleotide
{
    char nucleotide;
    struct DNA *right;
    struct DNA *across;
};

struct list//structure to deal with data of a full DNA
{
    int molecule_id;
    struct DNA *dna;
    struct list *next;
};

struct list *first=NULL;//pointer to point always to the first DNA in the entire DNA data provided

struct DNA *CreateNucleotide()//function to allocate space for creating a nucleotide and its complementary nucleotide
{
    struct DNA *a=(struct DNA*)malloc(2*sizeof(struct DNA));
    a->right=NULL;
    a->across=a+1;
    (a+1)->right=NULL;//doubly linked between the complementary nucleotides
    (a+1)->across=a;

    return a;
}

struct list *CreateDNA()//function to allocate space for storing data of a new DNA
{
    struct list *t=(struct list*)malloc(sizeof(struct list));
    t->dna=NULL;
    t->next=NULL;

    return t;
}

struct DNA *CreateChainNucleotide()//function to allocate space for relinking connections for split
{
    struct DNA *c=(struct DNA*)malloc(sizeof(struct DNA));
    c->right=NULL;
    c->across=NULL;
    return c;
}
char compPair(char nuc)//returns the complementary nucleotide of given nucleotide
{
    char comp;
    switch(nuc)
    {
    case 'A':
        comp='T';
        break;
    case 'T':
        comp='A';
        break;
    case 'C':
        comp='G';
        break;
    case 'G':
        comp='C';
        break;
    }
    return comp;
}

struct DNA* Create(struct DNA *header)//creates new DNA
{
    int id;
    char chain,ch,buf;//buf is used to discard the spaces and '\n' inputs
    printf("Enter your new Molecule's Id : ");
    scanf("%d",&id);
    struct list *location=first,*prev=NULL;//location is pointer which points to the DNA under consideration

    while(location!=NULL && id!=location->molecule_id)//the loop iterates till either a DNA with given molecule id is found or the end of the DNAs is reached
    {
        prev=location;//prev points to the dna previous to the dna in consideration, i.e location
        location=location->next;
    }
    if(location!=NULL)
    {
        printf("Molecule Id already Exists!!\n");
        return header;
    }

    location=CreateDNA();//creating new DNA

    struct DNA *temp;
    int i=0;
    printf("Which chain you want to input (a-alpha/b-beta) : ");
    scanf(" %c",&chain);
    while(chain!='a' && chain!='b')
    {
        printf("Enter valid input : ");
        scanf(" %c",&chain);
    }
    scanf("%c",&buf);//buf is used to ignore the sp input
    printf("Enter your nucleotides (Input UpperCase letters separated by single spaces): ");
    do
    {
        scanf("%c",&ch);
        if(ch!='A' && ch!='T' && ch!='C' && ch!='G' && ch!='\n')
        {
            printf("Invalid Input!\n");
            do
            {
                scanf("%c",&buf);
            }
            while(buf!='\n');
            free(location);//deallocation of allotted memory in case of invalid input
            return header;
        }
        if(ch=='\n')
            break;
        scanf("%c",&buf);
        struct DNA *new_nucleotide=CreateNucleotide();//creating new nucleotide
        if(chain=='a')
        {
            new_nucleotide->nucleotide=ch;
            (new_nucleotide+1)->nucleotide=compPair(ch);//assigning the beta chain nucleotide with complementary nucleotide pair
            if(i==0)
            {
                location->dna=new_nucleotide;//location->dna stores the address of the first nucleotide pair in the DNA molecule
                i++;
                temp=new_nucleotide;//temp points to the nucleotide previous to the current nucleotide
            }
            else
            {
                temp->right=new_nucleotide;//linking nucleotides in alpha chain
                temp++;
                new_nucleotide++;
                temp->right=new_nucleotide;//linking nucleotides in beta chain
                temp--;
                new_nucleotide--;
                temp=new_nucleotide;
            }
        }
        else
        {
            (new_nucleotide+1)->nucleotide=ch;
            new_nucleotide->nucleotide=compPair(ch);//assigning complementary nucleotide  to the nucleotide of alpha chain
            if(i==0)
            {
                location->dna=new_nucleotide;//location->dna points to the first nucleotide of the dna in consideration
                i++;
                temp=new_nucleotide;//temp points to the nucleotide previous to the current nucleotide
            }
            else
            {
                temp->right=new_nucleotide;//linking nucleotides in alpha chain
                temp++;
                new_nucleotide++;
                temp->right=new_nucleotide;//linking nucleotides in beta chain
                temp--;
                new_nucleotide--;
                temp=new_nucleotide;
            }

        }
    }
    while(buf!='\n');

    location->molecule_id=id;
    header=location->dna;
    if(first==NULL)
        first=location;
    else
        prev->next=location;

    return header;
}

struct DNA* Insert(struct DNA *header)//inserting new nucleotide pair in the inputted position and dna
{
    if(first==NULL)
    {
        printf("Sorry! No DNA Data found for insertion!\n");
        return header;
    }
    int id;
    struct list *location,*prev=NULL;
    location=first;
    printf("Enter your Molecule Id : ");
    scanf("%d",&id);
    while(location!=NULL && id!=location->molecule_id)//to check whether a dna with given molecule id exists or not
    {
        prev=location;
        location=location->next;
    }

    if(location!=NULL)
    {
        header=location->dna;
    }
    else
    {
        printf("Sorry! MoleculeId not found!\n");
        return header;
    }

    struct DNA *new_nucleotide=CreateNucleotide();//creating new nucleotide pair to insert
    struct DNA *previous=NULL;
    struct DNA *current=header;
    char N,chain;

    int position=0;
    int count=1;//to keep in count with the position


    while(current!=NULL)//count = number of nucleotides in a dna + 1
    {
        previous=current;
        current=current->right;
        count++;
    }

    printf("Enter the position where you want to insert your nucleotide molecule : ");
    scanf("%d",&position);

    while(position<1 || position>count)//checking whether a valid position is entered
    {
        printf("Please enter a valid position : ");
        scanf("%d",&position);
    }

    if(position==1)
    {
        new_nucleotide->right=header;//relinking alpha chain nucleotides
        header=header->across;//going into beta chain
        (new_nucleotide+1)->right=header;//relinking beta chain nucleotides
        header=header->across;//back to alpha chain
        header=new_nucleotide;//making header point to the start of the dna, i.e. the first nucleotide
    }
    else
    {
        previous=NULL;
        current=header;
        count = 1;
        while(current!=NULL && count!=position)
        {
            previous=current;
            current=current->right;
            count++;
        }
        if(current!=NULL)
        {
            previous->right=new_nucleotide;//relinking alpha chain nucleotides (previous with new)
            previous=previous->across;
            previous->right=(new_nucleotide+1);//relinking beta chain nucleotides(previous with new)
            new_nucleotide->right=current;//relinking alpha chain nucleotides (new and next)
            current=current->across;
            (new_nucleotide+1)->right=current;//relinking alpha chain nucleotides(new and next
        }
        else//inserting at the end of dna
        {
            previous->right=new_nucleotide;//relinking alpha of previous with new
            previous=previous->across;
            previous->right=(new_nucleotide+1);//relinking beta of previous with new
        }

    }

    printf("In which chain do you want to insert (a=alpha / b=beta) : ");
    scanf(" %c",&chain);
    while(chain!='a' && chain!='b')
    {
        printf("Enter valid input : ");
        scanf(" %c",&chain);
    }
    printf("Enter your new nucleotide (A/T/C/G): ");
    scanf(" %c",&N);

    if(chain=='a')//assigning nucleotides and complementary nucleotides correspondingly as the chain input
    {
        new_nucleotide->nucleotide=N;
        (new_nucleotide+1)->nucleotide=compPair(N);
    }
    else
    {
        (new_nucleotide+1)->nucleotide=N;
        new_nucleotide->nucleotide=compPair(N);
    }
    location->dna=header;
    return header;
}

void Print(struct DNA *header)//Prints the dna based on the header argument
{
    if(first == NULL)
    {
        printf("No Data Found\n");
        return;
    }
    struct DNA *current;
    current=header;

    if(header==NULL)
    {
        printf("No DNA found!\n");
        return;
    }

    while(current!=NULL)
    {
        if(current->right==NULL)//when end of the chain is reached
        {
            printf("%c",current->nucleotide);
            break;
        }
        printf("%c - ",current->nucleotide);
        current=current->right;
    }
    printf("\n");
    current=header;
    while(current!=NULL)
    {
        printf("|   ");
        current=current->right;
    }
    printf("\n");
    current=header->across;
    while(current!=NULL)
    {
        if(current->right==NULL)//when end of the chain is reached
        {
            printf("%c",current->nucleotide);
            break;
        }
        printf("%c - ",current->nucleotide);
        current=current->right;
    }

    printf("\n");
}

void PrintChain(struct DNA *header)//printing specific chain
{
    if(first==NULL)
    {
        printf("No DATA found!!\n");
        return;
    }
    int id;
    struct list* location;
    location=first;
    printf("Enter the molecule id : ");
    scanf("%d",&id);
    while(location!=NULL && id!=location->molecule_id)//checking if the inputted id exists
    {
        location=location->next;
    }
    if(location==NULL)
    {
        printf("Molecule not found!!\n");
        return;
    }
    Print(location->dna);//sending header of the required dna to "Print" function

}

void PrintAllChains(struct DNA *header)//prints all chains
{
    if(first==NULL)
    {
        printf("No DATA found!!\n");
        return;
    }

    struct list *location;

    location=first;
    while(location!=NULL)
    {
        printf("DNA %d : \n",location->molecule_id);
        Print(location->dna);//sending the header of each dna(i.e. the first nucleotide) to Print function
        location=location->next;
        printf("\n");
    }
}

struct DNA *Delete(struct DNA *header)
{
    if(first==NULL)
    {
        printf("No DATA found!!\n");
        return header;
    }
    int id;
    struct list* location,*prev=NULL;
    location=first;
    printf("Enter the molecule id : ");
    scanf("%d",&id);
    while(location!=NULL && id!=location->molecule_id)
    {
        prev=location;
        location=location->next;
    }
    if(location==NULL)
    {
        printf("Molecule not found!!\n");
        return header;
    }
    header=location->dna;//pointing to the first nucleotide of the dna from which nucleotide should be deleted

    struct DNA *previous,*current;//previous points to the nucleotide previous to the current nucleotide(the nucleotide to be deleted)
    int position,count=1;

    printf("Enter the position from which you want to delete : ");
    scanf("%d",&position);

    previous=NULL;
    current=header;
    while(current!=NULL && count!=position)
    {
        previous=current;
        current=current->right;
        count++;
    }
    if(current==NULL)
    {
        printf("Invalid position! NO nucleotide is found at %d\n",position);
        return header;
    }
    else if(previous!=NULL)
    {
        previous->right=current->right;
        previous=previous->across;
        current=current->across;
        previous->right=current->right;//relinking and eliminating links with the current nucleotide
    }
    else//if nucleotide from first position is deleted
    {
        header=current->right;
    }

    if(header==NULL)//if header points to null , it means all the nucleotides are deleted//so the struct list addresses should be relinked to remove the null dna
    {
        printf("~YOU HAVE COMPLETELY DELETED DNA:MoleculeID-%d\n",location->molecule_id);
        if(prev==NULL)
        {
            first=location->next;
        }
        else if(location->next!=NULL)
        {
            prev->next=NULL;
        }
        else
        {
            prev->next=location->next;
        }
        free(location);

        return header;
    }
    location->dna=header;
    return header;
}

struct DNA *Split(struct DNA *header)//splitting parent dna
{
    if(first==NULL)
    {
        printf("No DATA found!!\n");
        return header;
    }
    int id;
    struct list *location;
    location=first;
    printf("Enter the molecule id : ");
    scanf("%d",&id);
    while(location!=NULL && id!=location->molecule_id)
    {
        location=location->next;
    }
    if(location==NULL)
    {
        printf("Molecule not found!!\n");
        return header;
    }

    header=location->dna;
    struct DNA *current,*previous=NULL;
    current=header;
    do//removing links of alpha chain with beta and linking them with a new chain of nucleotides which are later assigned to the complementary nucleotides
    {
        struct DNA *beta_new=CreateChainNucleotide();
        current->across=beta_new;
        beta_new->across=current;//double links with new chain
        beta_new->nucleotide=compPair(current->nucleotide);
        if(previous!=NULL)
            previous->right=beta_new;
        previous=beta_new;
        current=current->right;
    }while(current!=NULL);//loop iterates as many times as the number of nucleotides in the parent DNA

    current=header;
    current++;
    previous=NULL;
    do//removing links of beta chain with alpha and linking them with a new chain of nucleotides which are later assigned the complementary nucleotides
    {
        struct DNA *alpha_new=CreateChainNucleotide();
        if(previous==NULL)
            header=alpha_new;
        current->across=alpha_new;
        alpha_new->across=current;//double links with new chain
        alpha_new->nucleotide=compPair(current->nucleotide);
        if(previous!=NULL)
            previous->right=alpha_new;
        previous=alpha_new;
        current=current->right;
    }while(current!=NULL);//loop iterates as many times as the number of nucleotides in the parent DNA

    struct list *split=(struct list*)malloc(sizeof(struct list));//the daughter dna having the inherited beta chain
    split->dna=header;//->dna points to the first nucleotide of the DNA


    split->next=location->next;//relinking with the new split DNA
    location->next=split;

    printf("Enter the new Molecule Id for the splitted DNA : ");
    scanf("%d",&id);
    do
    {
        location=first;
        while(location!=NULL && id!=location->molecule_id)//the loop iterates till the end in order to find if any existing dna matches with the inputted molecule id
        {
            location=location->next;
            if(location==split)
            {
                location=location->next;
            }
        }
        if(location!=NULL)
        {
            printf("Molecule Id already exists! Enter valid Id : ");
            scanf("%d",&id);
        }
    }while(location!=NULL);

    split->molecule_id=id;//assigning the dna the inputted molecule id

    return header;
}

int main()
{
    struct DNA *header=NULL;//header always points to first nucleotides of DNAs
    int input;

    do
    {
        printf("--------------------\n");
        printf("1 - Create\n");
        printf("2 - Insert\n");
        printf("3 - Delete\n");
        printf("4 - Split\n");
        printf("5 - Print Specific DNA\n");
        printf("6 - Print All\n");
        printf("0 - EXIT\n");
        printf("--------------------\n");
        printf("Enter your Input : ");
        scanf("%d",&input);
        switch(input)
        {
        case 1:
            header=Create(header);
            break;

        case 2:
            header=Insert(header);
            break;

        case 3:
            header=Delete(header);
            break;

        case 4:
            header=Split(header);
            break;

        case 5:
            PrintChain(header);
            break;

        case 6:
            PrintAllChains(header);
            break;

        case 0:
            return 0;
        default:
            printf("Please enter proper input!!\n");
            break;
        }
    }
    while(input!=0);
    return 0;
}
