#include<stdio.h>
#include<stdlib.h>

struct links //structure for matrix elements and links
{
    int elem;//stores value of element of the matrix
    struct links *right;
    struct links *down;
};

typedef struct links matrix;

matrix *CreateElement()//allocates space for element and sets the links to NULL
{
    matrix *a=(matrix *)malloc(sizeof(matrix));
    a->right=NULL;
    a->down=NULL;
    return a;
}

matrix *ReadMatrix(int status)//Reading matrix from ".txt" file
{
    matrix *first=NULL;//pointer to point to first row's first element of the matrix under consideration
    FILE *fp;//file pointer
    int rows=0;
    if(status==1)//status is used for representing path of file
        fp=fopen("Matrix1.txt","r");
    else
        fp=fopen("Matrix2.txt","r");
    if(fp==NULL)
    {
        printf("ERROR in opening the file!\n");
        fclose(fp);
            return first;
    }
    int n=0,num=0,count=0;
    matrix *temp=NULL;
    while(n!=-1)
    {
        while(1)
        {
            n=getc(fp);
            if(n==44 || n==-1 || n==59)//ascii values of comma and semicolon are 44 and 59
                break;
            num=num*10+n-48;//to get the number from digits
        }
        matrix *e=CreateElement();//creating new element
        e->elem=num;
        if(first==NULL)
            first=e;

        if(rows!=0 && count%rows==0)//keeping in mid the row change
        {
            temp->down=e;//link in first column just after row change
        }
        else if(temp!=NULL)
        {
            temp->right=e;
        }
        temp=e;

        count++;
        if(n==59)
        {
            temp=first;
            while(temp->down!=NULL)
            {
                temp=temp->down;
            }
            if(rows==0)
                rows=count;
            else if(count!=rows && status==1)
            {
                printf("MATRIX 1 : Invalid Matrix Input!\n");
                return NULL;
            }
            else if(count!=rows && status==2)
            {
                printf("MATRIX 2 : Invalid Matrix Input!\n");
                return NULL;
            }

            count=0;
        }
        num=0;
    }

    if(count==1)
        return first;
    matrix *vert,*x;
    x=first;
    temp=first;
    while(x->down!=NULL)//to create vertical links between rows for each elements
    {
        vert=x;
        temp=x->down;

        do
        {
            temp=temp->right;
            vert=vert->right;

            if(temp!=NULL)
                vert->down=temp;

        }while(temp->right!=NULL);

        x=x->down;

    }

    fclose(fp);//closing file

    return first;
}

void DisplayMatrix(matrix *m,int i)//To print the matrix
{
    if(i!=0)
        printf(" MATRIX - %d :\n",i);
    else
        printf("\n PRODUCT MATRIX : \n");
    printf("------------------\n");
    if(m==NULL)
    {
        printf("Matrix does not exist\n");
        return;
    }
    matrix *current=m,*location=m;
    while(current!=NULL)
    {
        printf("%d\t",current->elem);
        current=current->right;
        if(current==NULL)
        {
            printf("\n");
            location=location->down;//goes to next row
            current=location;
            if(location==NULL)//after the last row
                break;
        }
    }
}

int IsSquare(matrix *m)//checks whether matrix is a square matrix
{
    matrix *r,*c;
    r=c=m;
    while(1)
    {
        r=r->right;
        c=c->down;
        if(r==NULL || c==NULL)//if either the end is reached in row or column
        {
            if(r==NULL && c==NULL)//checking whether the end is reached in both row and column
            {
                printf("\nIt is a Square Matrix!\n");
                return 1;
            }
            printf("\nIt is Not a Square Matrix!\n");
            return 0;
        }
    }
}

matrix *Transpose(matrix *m)//Creating transpose of matrix
{
    matrix *pos,*t;
    pos=m;
    matrix *m_t=NULL;//pointer to point to the first element of transposed matrix

    int rows=0;
    int n=0,num=0,count=0;
    matrix *temp=NULL,*loc=NULL;
    while(pos!=NULL)
    {
        t=pos;
        temp=loc;
        while(t!=NULL)
        {
            num=t->elem;
            matrix *e=CreateElement();
            e->elem=num;
            if(m_t==NULL)
            {
                m_t=e;
                loc=m_t;
            }

            if(rows!=0 && count%rows==0)
            {
                temp->down=e;
                loc=loc->down;
            }
            else if(temp!=NULL)
            {
                temp->right=e;
            }
            temp=e;
            t=t->down;

            count++;
        }
        if(rows==0)
            rows=count;
        pos=pos->right;
    }
    return m_t;

}

int CheckEquality(matrix *m1,matrix *m2)//to check equality of matrix and its transpose
{
    matrix *pos1,*pos2,*temp1,*temp2;
    pos1=m1;
    pos2=m2;
    while(pos1!=NULL)
    {
        temp1=pos1;
        temp2=pos2;
        while(temp1!=NULL)
        {
            if(temp1->elem!=temp2->elem)
                return 0;//zero is returned for unequal element, denoting unsymmetricity
            temp1=temp1->right;
            temp2=temp2->right;
        }
        pos1=pos1->down;
        pos2=pos2->down;
    }
    return 1;//one is returned if all elements are equal
}

void IsSymmetric(matrix *m,int square)//check symmetricity
{
    if(square==0)
    {
        printf("\nSymmetricity is not defined for NON-SQUARE Matrices!\n\n");
        return;
    }
    int status=0;
    status=CheckEquality(m,Transpose(m));
    if(status==1)
    {
        printf("\nMatrix is Symmetric!\n\n");
        return;
    }
    printf("\nMatrix is Unsymmetric!\n\n");
}

matrix *Multiply(matrix *A,matrix *B)//multiplies matrices
{
    int m1=0,n1=0,m2=0,n2=0;//m1 x n1 and m2 x n2 are dimensions of matrix A and B
    matrix *M1=A,*M2=B,*product=NULL;
    while(M1!=NULL)
    {
        M1=M1->right;
        n1++;//counts the n1
    }
    while(M2!=NULL)
    {
        M2=M2->down;
        m2++;//counts the m2
    }
    if(n1!=m2)//condition for conformity of product
    {
        printf("\nThe Given Two Matrices are not conformable for Multiplication!\n");
        return NULL;
    }
    M1=A;
    M2=B;
    while(M1!=NULL)
    {
        M1=M1->down;
        m1++;//counts m1
    }
    while(M2!=NULL)
    {
        M2=M2->right;
        n2++;//counts n2
    }
    M1=A;
    M2=B;
    int i,j,k,sum;
    matrix *a,*b,*temp,*location;
    for(i=0;i<m1;i++)//product of all rows with all columns
    {
        for(j=0;j<n2;j++)//product of a row with all columns
        {
            a=M1;
            b=M2;
            sum=0;
            for(k=0;k<n1;k++)//deals with product of a row of matrix 1 and column of matrix 2
            {
                sum=sum+(a->elem)*(b->elem);//
                a=a->right;//next element in row
                b=b->down;//next element in column
            }
            matrix *e=CreateElement();
            e->elem=sum;
            M2=M2->right;
            if(j==0 && i==0)//denotes first element of matrix
            {
                product=e;
                temp=product;
                location=product;
                continue;
            }
            else if(j==0)//denotes row change
            {
                temp=location;
                temp->down=e;
                location=location->down;
                temp=temp->down;
            }
            temp->right=e;
            temp=e;
        }
        M1=M1->down;
        M2=B;
    }

    DisplayMatrix(product,0);

    return product;
}

void WriteIntoProduct(matrix *m)//writes product matrix into file
{
    FILE *fp;
    fp=fopen("Product.txt","w");//opening the file in write mode

    if(fp==NULL)
    {
        printf("ERROR in opening the file!\n");
        fclose(fp);
            return;
    }

    matrix *loc,*temp;
    loc=m;//location points to the first element of row under consideration
    while(loc!=NULL)
    {
        temp=loc;//for temp to point back to beginning of the next row
        while(temp->right!=NULL)//the condition is for the last element of the row under consideration
        {
            fprintf(fp,"%d\t",temp->elem);
            temp=temp->right;
        }

        fprintf(fp,"%d\n",temp->elem);
        loc=loc->down;//going to next row
    }

    fclose(fp);
}
int main()
{
    matrix *A,*B,*P;
    int sq;
    A=ReadMatrix(1);//Reading matrix 1 from file Matrix1.txt
    DisplayMatrix(A,1);//printing matrix-1 read from file

    sq=IsSquare(A);//checking whether the matrix 1 is a square matrix

    IsSymmetric(A,sq);//checking whether the matrix 1 is symmetric


    B=ReadMatrix(2);//Reading matrix 2 from file Matrix2.txt
    DisplayMatrix(B,2);//printing matrix-2 read from file

    P=Multiply(A,B);//product of inputted matrices, the return value is a pointer pointing to the first element of the product matrix

    WriteIntoProduct(P);//writing the product matrix into Product.txt File

    return 0;
}
