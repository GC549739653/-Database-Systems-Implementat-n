#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h" 

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

PG_MODULE_MAGIC;
//--def two struct intSet and inttext ---inttext is for transfer char
typedef struct intSet
{
    int  len;
    int *data;
} intSet;
 
typedef struct inttext{
    int length;
    char data[FLEXIBLE_ARRAY_MEMBER];
} inttext;
/*all the functions define here*/

static int isdigitstr(char *str);
int countDigits(char *str);
int countComma(char *str);
int countBrackets(char *str);
int isAlphabet(char *str);
int isValid(char *str);
char *preprocess(char *str);

//---------------------------small func to check is vaild--

/* check whether is valid*/
static int isdigitstr(char *str){  
    return (strspn(str, "0123456789")==strlen(str));  
}

int countDigits(char *str){
    char *string,*p;
    int numInteger = 0;
    string = (char*)malloc(strlen(str)*sizeof(char));
    strcpy(string,str);
    for (p = strtok(string, " {},\t\r\n"); p; p = strtok(NULL, " {},\t\r\n")) {
        numInteger ++;
    }
    return numInteger;
}

int countComma(char *str){
    int numComma = 0;
    int i;
    for(i= 0;i < strlen(str);i++){
        if(str[i] == ',') numComma ++;
    }
    return numComma;
}

int countBrackets(char *str){
    int numbrack = 0;
    int i;
    for (i = 0; i < strlen(str); i++)
        if(str[i] == '{' || str[i] == '}') numbrack ++;
    return numbrack;
}

int isAlphabet(char *str){
    int result = 0;
    char *string,*p;
    int numInteger = 0;
    string = (char*)malloc(strlen(str)*sizeof(char));
    strcpy(string,str);
    for (p = strtok(string, " {},\t\r\n"); p; p = strtok(NULL, " {},\t\r\n")) 
        if(!isdigitstr(p)) result = 1;
    return result;
}

int isValid(char *str){
    int numdigits,numComma,numbrack,result;

    result = 0;

    numdigits = countDigits(str);
    numbrack = countBrackets(str);
    numComma = countComma(str);

    // check empty if verified return 
    if(numdigits == 0 && numbrack == 2 && numComma == 0) {
        result = 2;
        return result;
    }
    // check no comma 
    if(numComma + 1 == numdigits) result = 1;
    if(isAlphabet(str) == 1) {
        result = 0;
        return result;
    }; 
    return result;
}

int checkReplicate(int *array,int length,int num){
    int result,i;
    result = 1;
    for (i = 0; i < length; ++i)
    {
        if(array[i] == num) result =0;
    }
    return result;
}

void swap(int *s, int i, int j) {
    int temp;
    temp = s[i];
    s[i] = s[j];
    s[j] = temp;
}
void sort(int *array, int n) {
    if (n > 1) {
        int pivot = 0, j;
        for (j = 1; j < n; j++)
            if (array[j] < array[0]) swap(array, ++pivot, j);
        swap(array, 0, pivot);
        sort(array, pivot);
        sort(array + pivot + 1, n - pivot - 1);
    }
}
//-----------function to preprocess what input----
char *preprocess(char *str){

    char *string,*p,*result,*content;
    int *array,length,checker;

    checker = isValid(str);

    if(checker == 2){
    result =(char*)malloc(3*sizeof(char));
        result = "{}";
        return result;
    }
    else if(checker == 1){
    int lang = countDigits(str);

        array = (int*)malloc(lang*sizeof(int));
        string = (char*)malloc(strlen(str)+1*sizeof(char));
        strcpy(string,str);

        length = 0;

        for (p = strtok(string, " {}, \t\r\n"); p; p = strtok(NULL, " {},\t\r\n")) {
            int value = atoi(p);
            if(checkReplicate(array,length,value) == 1){           
                array[length] = value;
                length ++;
            }
        }

        result =(char*)malloc((length + 2 + length -1 + 1) * sizeof(char));
        content = (char*)malloc((length + length -1 + 1) *sizeof(char));

		sort(array,length);

        if(length == 1) sprintf(result,"{%d}",array[0]);
        else{   
            sprintf(result,"{%d,",array[0]);
            int m,i;
            for (i = 1; i < length-1; ++i)
            {
                sprintf(content,"%d,",array[i]);
                strcat(result,content);
                m = i;
            }
            m ++;
            sprintf(content,"%d}",array[length - 1]);
            strcat(result,content);
        }
        return result;
    }
}
//----char to intSet-------------------
intSet *tointSet(char *str){

	char *p,*string;
	int length;
	intSet *set;

	set = malloc(sizeof(intSet));

	if(strlen(str) == 2){
		set -> len = 0;
		return set;
	}

	length = 0;
	int number = countDigits(str); 

	set -> data = malloc(number*sizeof(int));
	string = (char*)malloc(strlen(str)*sizeof(char));
    strcpy(string,str);
	        //ereport(NOTICE,
                //(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                // errmsg("invalid input syntax for intSet: \"%s\"",str)));
				       //  ereport(NOTICE,
               // (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                // errmsg("invalid input syntax for intSet: \"%s\"",string)));
	
	for (p = strtok(string," {},\t\r\n"); p; p = strtok(NULL, " {},\t\r\n")) {
        int value = atoi(p);
		        //ereport(NOTICE,
                //(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                // errmsg("invalid input syntax for intSet value: \"%d\"",value)));
				//         ereport(NOTICE,
                //(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                // errmsg("invalid input syntax for intSet p: \"%d\"",p)));
		
        set -> data[length] = value;
		length++;
        set ->len = length;
    }
    return set; 
}
//???------------------------intSet to inttext---------------------
char *toInttext(intSet *inset){
    char *p,*result,*content;



    int length = inset -> len;

    if(inset -> len == 0)
	{
    result = malloc(3*sizeof(char));
        result = "{}";
        return result;
    }
    result =(char*)malloc(2000 * sizeof(char));
    content = (char*)malloc(100 *sizeof(char));
    if(inset -> len == 1) sprintf(result,"{%d}",inset-> data[0]);
    else{
        sprintf(result,"{%d,",inset-> data[0]);
        int m,i;
        for (i = 1; i < inset -> len-1; ++i)
        {
            sprintf(content,"%d,",inset-> data[i]);
            strcat(result,content);
            m = i;
        }
        m ++;
        sprintf(content,"%d}",inset-> data[inset -> len - 1]);
        strcat(result,content);
    }
    result = (char *)realloc(result, strlen(result));
    return result;
}



/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/
PG_FUNCTION_INFO_V1(intset_in);

Datum 
intset_in(PG_FUNCTION_ARGS)
{
    char      *str = PG_GETARG_CSTRING(0);
    char      *recv;
    inttext    *result;

    if (isValid(str) == 0)
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid input syntax for intSet: \"%s\"",
                        str)));

    recv = preprocess(str);
	result  = (inttext *) palloc(VARHDRSZ + strlen(recv));
	result  -> length = strlen(recv);
	SET_VARSIZE(result, VARHDRSZ + strlen(recv));
	memcpy(result->data, recv, strlen(recv));
    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(intset_out);

Datum
intset_out(PG_FUNCTION_ARGS)
 {
	inttext    *inttext1 = (inttext *) PG_GETARG_POINTER(0);
	char	   *result;

	result = (char *) palloc(strlen(inttext1->data)+1);
	snprintf(result, strlen(inttext1->data)+1, "%s", inttext1->data);
	PG_RETURN_CSTRING(result);
}

//*****************************************************************************
 //* Operators
 //*****************************************************************************/
 
 //------------------------------a belong to set b---
static int

intset_con_internal(int i, char *a)
{
    //int *copyofa;
    //int l = VARSIZE_ANY_EXHDR(a) / 4;
	
	// helper function tointSet()
	int len;
	intSet *set = tointSet(a);
	len = set -> len;
	int *array = set -> data;
	if (len == 0)
	{return 0;}
	//int copyofa[l];
    //len = VARSIZE_ANY_EXHDR(a) / 4;
    int f = 0;
    //memcpy(copyofa , a->data, VARSIZE_ANY_EXHDR(a));
    int k;
    for (k = 0; k < len; k++) 
    {
        if (array[k] == i) 
        {
            return 1;
        }
        else
        {
            f+=1;
        }
    }
    return 0;
}
//---------------set a belong to set b----------------------------------------
static int
intset_sub_internal(char * a, char * b)
{
    //int *copyofa ;
    //int l = a->len;
    //int len = VARSIZE(a);
    //int l = VARSIZE_ANY_EXHDR(a) / 4;
	//int copyofa[l];
    //memcpy(copyofa , a->data, VARSIZE_ANY_EXHDR(a));
	int lena;
	int lenb;
	intSet *seta = tointSet(a);
	intSet *setb = tointSet(b);
	lena = seta -> len;
	int *arraya = seta -> data;
	lenb = setb -> len;
	int *arrayb = setb -> data;
    int i;
    for (i = 0; i < lena; i++) 
    {
        if ((intset_con_internal(arraya[i], b)))
        {
            continue;
        } 
        else
        {
            return 0;
        }
    }
    return 1;
}
//--------------a belong to intSet---------------------------------------
PG_FUNCTION_INFO_V1(intset_con);

Datum 

intset_con(PG_FUNCTION_ARGS)

{
    int i = PG_GETARG_INT32(0);
    inttext *a = (inttext *) PG_GETARG_POINTER(1);
		//ereport(NOTICE,
        //(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
        //errmsg("invalid input syntax for intSet: \"%s\"",i)));
	//ereport(NOTICE,
        //(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
        //errmsg("invalid input syntax for intSet: \"%s\"",a->data)));
    
    PG_RETURN_BOOL(intset_con_internal(i, a->data)==1);
}   
//------------------length of intSet--------------------------------

PG_FUNCTION_INFO_V1(intset_cdn);

Datum
intset_cdn(PG_FUNCTION_ARGS)
{
    inttext *a = (inttext *) PG_GETARG_POINTER(0);
	int len ;
    //int l = a -> len;
    //int l =VARSIZE_ANY_EXHDR(a)/4;
    intSet *set = tointSet(a->data);
	len = set -> len;
    PG_RETURN_INT32(len);
}

//--------------------a belong to b-------------------------------------

PG_FUNCTION_INFO_V1(intset_sub);

Datum
intset_sub(PG_FUNCTION_ARGS)
{
    inttext *a = (inttext *) PG_GETARG_POINTER(0);
    inttext *b = (inttext *) PG_GETARG_POINTER(1);
    
    PG_RETURN_BOOL(intset_sub_internal(a->data, b->data));
}

//------------------a=b-------------------------------------------
PG_FUNCTION_INFO_V1(intset_eq);
Datum
intset_eq(PG_FUNCTION_ARGS)
{
    inttext *a = (inttext *) PG_GETARG_POINTER(0);
    inttext *b = (inttext *) PG_GETARG_POINTER(1);
    
    PG_RETURN_BOOL(intset_sub_internal(a->data, b->data) && intset_sub_internal(b->data, a->data));
}
//---------------------a intersection b--------------------------------------------
PG_FUNCTION_INFO_V1(intset_int);

Datum
intset_int(PG_FUNCTION_ARGS)
{
    inttext *a = (inttext *) PG_GETARG_POINTER(0);
    inttext *b = (inttext *) PG_GETARG_POINTER(1);
    intSet *newset;
	inttext *newtext;
    int newsize;
	intSet *seta = tointSet(a->data);
	intSet *setb = tointSet(b->data);
	int lena;
	int lenb;
	lena = seta -> len;
	int *arraya = seta -> data;
	lenb = setb -> len;
	int *arrayb = setb -> data;
	
    //int a_l = a->len;
    //int a_l = VARSIZE_ANY_EXHDR(a)/4;
    //int b_l = b->len;
    //int b_l = VARSIZE_ANY_EXHDR(b)/4;

    //int copyofa[a_l];

    //memcpy(copyofa, a->data, VARSIZE_ANY_EXHDR(a));

    if (lena<=lenb) {newsize = lenb;}
    else {newsize = lena;}

    
    //max_size = (a_len <= b_len) ? a_len : b_len;

    //int newbool = malloc(newsize * sizeof(int));
    int newbool[newsize];
    int count = 0;
    int i;
    for (i = 0; i <lena; i++) 
    {
        if (intset_con_internal(arraya[i], b->data)) 
        {
            newbool[count] = arraya[i];
            count += 1;
        }
    }
	
    newtext = (inttext *) palloc(sizeof(inttext));
	
    newset = (intSet *) malloc(sizeof(intSet));
    //SET_VARSIZE(newset ,(newsize*4)+VARHDRSZ);
	//SET_VARSIZE(newtext ,(newsize*4)+VARHDRSZ);
    memcpy(newset->data, newbool, newsize * 4);
	newset->len = count;
	char *res = toInttext(newset);
	SET_VARSIZE(newtext ,strlen(res)+VARHDRSZ);
	memcpy(newtext->data, res, strlen(res));
	newtext ->length = strlen(res);
    PG_RETURN_POINTER(newtext);
}
//-------------------a union b-----------------------------------------------
PG_FUNCTION_INFO_V1(intset_uni);

Datum
intset_uni(PG_FUNCTION_ARGS)
{
    inttext *a = (inttext *) PG_GETARG_POINTER(0);
    inttext *b = (inttext *) PG_GETARG_POINTER(1);
    intSet *newset;
	inttext *newtext;
   
	intSet *seta = tointSet(a->data);
	intSet *setb = tointSet(b->data);
	int lena;
	int lenb;
	lena = seta -> len;
	int *arraya = seta -> data;
	lenb = setb -> len;
	int *arrayb = setb -> data;
    //int b_l = b->len;
    // copyofa[a_l];
    //int copyofb[b_l];
    //memcpy (copyofa, a->data, VARSIZE_ANY_EXHDR(a));
    //memcpy (copyofb, b->data, VARSIZE_ANY_EXHDR(b));

    int newsize = lena+ lenb;
    int newbool[newsize];
    //int temp = malloc(max_size * sizeof(int));
    int count = 0;
    int i;
    for (i = 0; i < lena; i++)
    {
        newbool[count] = arraya[i];
        count += 1;
    }
    int j;
    for (j = 0; j < lenb; j++)
    { 
        newbool[count] = arrayb[j];
        count += 1;
    }
    
    //newset = (intSet *) palloc(sizeof(intSet));
	newtext = (inttext *) palloc(sizeof(inttext));
    newset = (intSet *) malloc(sizeof(intSet));
    //SET_VARSIZE(newset, (4 * newsize) + VARHDRSZ);
    //memcpy(newset->data, newbool, newsize * 4);
    //PG_RETURN_POINTER(newset);
	memcpy(newset->data, newbool, newsize * 4);
	newset->len = count;
	char *res = toInttext(newset);
	SET_VARSIZE(newtext ,strlen(res)+VARHDRSZ);
	memcpy(newtext->data, res, strlen(res));
	newtext ->length = strlen(res);
    PG_RETURN_POINTER(newtext);
}

//-------------a disjunction b----------------------------------------------
PG_FUNCTION_INFO_V1(intset_dis);

Datum
intset_dis(PG_FUNCTION_ARGS)
{
    inttext *a = (inttext *) PG_GETARG_VARLENA_P(0);
    inttext *b = (inttext *) PG_GETARG_VARLENA_P(1);
    //intSet *newset;
    
    //int a_l = VARSIZE_ANY_EXHDR(a) / 4;
    //int b_l = VARSIZE_ANY_EXHDR(b) / 4;
    //int copyofa[a_l];
    //int copyofb[b_l];
    //memcpy(copyofa, a->data, VARSIZE_ANY_EXHDR(a));
    //memcpy(copyofb, b->data, VARSIZE_ANY_EXHDR(b));
    intSet *newset;
	inttext *newtext;
   
	intSet *seta = tointSet(a->data);
	intSet *setb = tointSet(b->data);
	int lena;
	int lenb;
	lena = seta -> len;
	int *arraya = seta -> data;
	lenb = setb -> len;
	int *arrayb = setb -> data;
    int  newsize = lena + lenb;

    int newbool[newsize];

    int count = 0;
    int i;
    for (i = 0; i <lena; i++)
    {
        if ((intset_con_internal(arraya[i], b->data))) 
        {
            continue;
            //newbool[count]=copyofa[i];
        //  count+=1;
        }
        else
        {
            newbool[count]=arraya[i];
            count+=1;
        }
    }
    int j;
    for (j = 0; j < lenb; j++) 
    {
        if ((intset_con_internal(arrayb[i], a->data)))
        {
            continue;
        //newbool[count]=copyofb[j];
            //count+=1;
        }
        else
        {
            newbool[count]=arrayb[j];
            count+=1;
        }
    }
    
//newset = (intSet *) palloc(sizeof(intSet));
	newtext = (inttext *) palloc(sizeof(inttext));
    newset = (intSet *) malloc(sizeof(intSet));
    //SET_VARSIZE(newset, (4 * newsize) + VARHDRSZ);
    //memcpy(newset->data, newbool, newsize * 4);
    //PG_RETURN_POINTER(newset);
	memcpy(newset->data, newbool, newsize * 4);
	newset->len = count;
	char *res = toInttext(newset);
	SET_VARSIZE(newtext ,strlen(res)+VARHDRSZ);
	memcpy(newtext->data, res, strlen(res));
	newtext ->length = strlen(res);
    PG_RETURN_POINTER(newtext);
}
//---------a difference b  in a not in b-----------------------------------------------
PG_FUNCTION_INFO_V1(intset_dif);

Datum
intset_dif(PG_FUNCTION_ARGS)
{
    inttext *a = (inttext *) PG_GETARG_VARLENA_P(0);
    inttext *b = (inttext *) PG_GETARG_VARLENA_P(1);
    intSet *newset;
	inttext *newtext;
   
	intSet *seta = tointSet(a->data);
	intSet *setb = tointSet(b->data);
	int lena;
	int lenb;
	lena = seta -> len;
	int *arraya = seta -> data;
	lenb = setb -> len;
	int *arrayb = setb -> data;
    //
    
    int newsize = lena;
    int newbool[newsize];

    int count = 0;
    int i;
    for (i = 0; i < lena; i++)
    {
        if ((intset_con_internal(arraya[i], b->data)))
        {
            continue;
            //newbool[count]=copyofa[i];
        //  count+=1;
        }
        else
        {
            newbool[count]=arraya[i];
            count+=1;
        }
    }
    
//newset = (intSet *) palloc(sizeof(intSet));
	newtext = (inttext *) palloc(sizeof(inttext));
    newset = (intSet *) malloc(sizeof(intSet));
    //SET_VARSIZE(newset, (4 * newsize) + VARHDRSZ);
    //memcpy(newset->data, newbool, newsize * 4);
    //PG_RETURN_POINTER(newset);
	memcpy(newset->data, newbool, newsize * 4);
	newset->len = count;
	char *res = toInttext(newset);
	SET_VARSIZE(newtext ,strlen(res)+VARHDRSZ);
	memcpy(newtext->data, res, strlen(res));
	newtext ->length = strlen(res);
    PG_RETURN_POINTER(newtext);
}