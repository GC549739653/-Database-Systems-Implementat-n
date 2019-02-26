#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h" 

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

PG_MODULE_MAGIC;

typedef struct intset
{
    int  len;
    int *data;
} intset;
 
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

void sort(int *src, int len)
{
    int tem,i,j;
    int length = sizeof(src);
    for (i = 0; i < len; i++)
    {
        for (j = 0; j < len - i - 1; j++)
            if (src[j] > src[j+1])
            {
                tem = src[j];
                src[j] = src[j+1];
                src[j+1] = tem;
            }
    }
}

char *preprocess(char *str){

    char *string,*p,*result,*content;
    int *array,length,checker;

    checker = isValid(str);
    result = malloc(sizeof(char));

    if(checker == 2){
        result = '{}';
/*        set =  (inttext*)palloc(VARHDRSZ + strlen(result));
        set -> length = (int)strlen(result);
        SET_VARSIZE(result, VARHDRSZ + strlen(str));
        memcpy(set->data, str, strlen(result));*/
        return result;
    }
    else if(checker == 1){

        array = malloc(sizeof(int));
        content = malloc(sizeof(char));
        string = (char*)malloc(strlen(str)*sizeof(char));
        strcpy(string,str);

        length = 0;

        for (p = strtok(string, " {}, \t\r\n"); p; p = strtok(NULL, " {},\t\r\n")) {
            int value = atoi(p);
            if(checkReplicate(array,length,value) == 1){            
                array[length] = value;
                length ++;
            }
        }

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

intset *toIntSet(char *str){

	char *p,*string;
	int length;
	intset *set;

	set = malloc(sizeof(intset));

	if(strlen(str) == 2){
		set -> len = 0;
		return set;
	}

	length = 0;
	int number = countDigits(str); 

	set -> data = malloc(number*sizeof(int));
	string = (char*)malloc(strlen(str)*sizeof(char));
    strcpy(string,str);

	for (p = strtok(string," {},\t\r\n"); p; p = strtok(NULL, " {},\t\r\n")) {
        int value = atoi(p);
        set -> data[set -> len] = value;
        set -> len ++;
    }
    return set; 
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
                 errmsg("invalid input syntax for intset: \"%s\"",
                        str)));

    recv = preprocess(str);
	result  = (inttext *) palloc(VARHDRSZ + strlen(recv));
	result  -> length = (int)strlen(recv);
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

/*****************************************************************************
 * Operator class for defining B-tree index
 *
 * It's essential that the comparison operators and support function for a
 * B-tree index opclass always agree on the relative ordering of any two
 * data values.  Experience has shown that it's depressingly easy to write
 * unintentionally inconsistent functions.  One way to reduce the odds of
 * making a mistake is to make all the functions simple wrappers around
 * an internal three-way-comparison function, as we do here.
 *****************************************************************************/
static bool

intset_con_internal(int i, intset *a)
{
    
    int l = VARSIZE_ANY_EXHDR(a) / 4;
    //len = VARSIZE_ANY_EXHDR(a) / 4;
	int copyofa[l];
    int f = 0;
    memcpy(copyofa , a->data, VARSIZE_ANY_EXHDR(a));
    int k;
    for (k = 0; k < l; k++) 
    {
        if (copyofa[k] == i) 
        {
            return true;
        }
        else
        {
            f+=1;
        }
    }
    return false;
}
//--------------ÅÐ¶Ï inset a ÊÇ²»ÊÇ inset b µÄ×Ó¼¯-------------------------------------------------
static bool
intset_sub_internal(intset * a, intset * b)
{
    int *copyofa ;
    //int l = a->len;
    //int len = VARSIZE(a);
    int l = VARSIZE_ANY_EXHDR(a) / 4;
    memcpy(copyofa , a->data, VARSIZE_ANY_EXHDR(a));
    int i;
    for (i = 0; i < l; i++) 
    {
        if ((intset_con_internal(copyofa[i], b)))
        {
            continue;
        } 
        else
        {
            return false;
        }
    }
    return true;
}
//-------------ÊµÏÖi <@ S£¬ÅÐ¶ÏiÊÇ²»ÊÇsÀïÃæµÄÒ»¸öÔªËØ------------------------------------------------
PG_FUNCTION_INFO_V1(intset_con);

Datum 

intset_con(PG_FUNCTION_ARGS)

{
    int i = PG_GETARG_INT32(0);
    intset *a = (intset *) PG_GETARG_POINTER(1);
    
    PG_RETURN_BOOL(intset_con_internal(i, a));
}   
//------------@ S ¼ÆËã intset ÔªËØÖÐÎÞÖØ¸´ÔªËØµÄ¸öÊý-----------------------------------------

PG_FUNCTION_INFO_V1(intset_cdn);

Datum
intset_cdn(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_POINTER(0);
    //int l = a -> len;
    int l =VARSIZE_ANY_EXHDR(a)/4;
    
    PG_RETURN_INT32(l);
}

//-------------A @> B ÅÐ¶ÏAÊÇ²»ÊÇBµÄÕæ×Ó¼¯ A------------------------------------------------

PG_FUNCTION_INFO_V1(intset_sub);

Datum
intset_sub(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_POINTER(0);
    intset *b = (intset *) PG_GETARG_POINTER(1);
    
    PG_RETURN_BOOL(intset_sub_internal(a, b));
}

//------------ÅÐ¶ÏintsetAÊÇ²»ÊÇÓëintsetBÏàµÈ------------------------------------------------

PG_FUNCTION_INFO_V1(intset_eq);

Datum
intset_eq(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_POINTER(0);
    intset *b = (intset *) PG_GETARG_POINTER(1);
    
    PG_RETURN_BOOL(intset_sub_internal(a, b) && intset_sub_internal(b, a));
}
//------------ÇóintsetAÓëinsetBµÄ½»¼¯-----------------------------------------------------------
PG_FUNCTION_INFO_V1(intset_int);

Datum
intset_int(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_POINTER(0);
    intset *b = (intset *) PG_GETARG_POINTER(1);
    intset *newset;
    int newsize;
 
    //int a_l = a->len;
    int a_l = VARSIZE_ANY_EXHDR(a)/4;
    //int b_l = b->len;
    int b_l = VARSIZE_ANY_EXHDR(b)/4;

    int copyofa[a_l];

    memcpy(copyofa, a->data, VARSIZE_ANY_EXHDR(a));

    if (a_l<=b_l) {newsize = b_l;}
    else {newsize = a_l;}

    
    //max_size = (a_len <= b_len) ? a_len : b_len;

    //int newbool = malloc(newsize * sizeof(int));
    int newbool[newsize];
    int count = 0;
    int i;
    for (i = 0; i < a_l; i++) 
    {
        if (intset_con_internal(copyofa[i], b)) 
        {
            newbool[count] = copyofa[i];
            count += 1;
        }
    }
    
    newset = (intset *) palloc(newsize*sizeof(intset)+4);
    SET_VARSIZE(newset ,(newsize*4)+VARHDRSZ);

    memcpy(newset->data, newbool, newsize * 4);

    PG_RETURN_POINTER(newset);
}
//--------------------ÇóintsetAÓëintsetBµÄ²¢¼¯------------------------------------------------
PG_FUNCTION_INFO_V1(intset_uni);

Datum
intset_uni(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_POINTER(0);
    intset *b = (intset *) PG_GETARG_POINTER(1);
    intset *newset;
    
    //int *copyofa;
    //int *copyofb;
    //int a_l = a->len;
    int a_l = VARSIZE_ANY_EXHDR(a) / 4;
    int b_l = VARSIZE_ANY_EXHDR(b) / 4;
    //int b_l = b->len;
    int copyofa[a_l];
    int copyofb[b_l];
    memcpy (copyofa, a->data, VARSIZE_ANY_EXHDR(a));
    memcpy (copyofb, b->data, VARSIZE_ANY_EXHDR(b));

    int newsize = a_l + b_l;
    int newbool[newsize];
    //int temp = malloc(max_size * sizeof(int));
    int count = 0;
    int i;
    for (i = 0; i < a_l; i++)
    {
        newbool[count] = copyofa[i];
        count += 1;
    }
    int j;
    for (j = 0; j < b_l; j++)
    { 
        newbool[count] = copyofb[j];
        count += 1;
    }
    
    //newset = (intset *) palloc(sizeof(Intset));
    newset = (intset *) palloc(newsize * sizeof(intset) + 4);
    SET_VARSIZE(newset, (4 * newsize) + VARHDRSZ);
    memcpy(newset->data, newbool, newsize * 4);
    PG_RETURN_POINTER(newset);
}

//--------------ÇóintsetAÓëintset BµÄdisjunction,---ÔÚAÖÐ²»ÔÚBÖÐºÍÔÚBÖÐ²»ÔÚAÖÐµÄÔªËØ----------------------------------------------
PG_FUNCTION_INFO_V1(intset_dis);

Datum
intset_dis(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_VARLENA_P(0);
    intset *b = (intset *) PG_GETARG_VARLENA_P(1);
    intset *newset;
    
    int a_l = VARSIZE_ANY_EXHDR(a) / 4;
    int b_l = VARSIZE_ANY_EXHDR(b) / 4;
    int copyofa[a_l];
    int copyofb[b_l];
    memcpy(copyofa, a->data, VARSIZE_ANY_EXHDR(a));
    memcpy(copyofb, b->data, VARSIZE_ANY_EXHDR(b));
    
    int  newsize = a_l + b_l;

    int newbool[newsize];

    int count = 0;
    int i;
    for (i = 0; i < a_l; i++)
    {
        if ((intset_con_internal(copyofa[i], b))) 
        {
            continue;
            //newbool[count]=copyofa[i];
        //  count+=1;
        }
        else
        {
            newbool[count]=copyofa[i];
            count+=1;
        }
    }
    int j;
    for (j = 0; j < b_l; j++) 
    {
        if ((intset_con_internal(copyofb[i], a)))
        {
            continue;
        //newbool[count]=copyofb[j];
            //count+=1;
        }
        else
        {
            newbool[count]=copyofb[j];
            count+=1;
        }
    }
    
    newset = (intset *) palloc(newsize * sizeof(intset) + 4);
    SET_VARSIZE(newset, (4 * newsize) + VARHDRSZ);
    memcpy(newset->data, newbool, newsize * 4);
    PG_RETURN_POINTER(newset);
}
//-----------ÇóÔÚAÖÐ¶ø²»ÔÚBÖÐµÄÔªËØ  A - B takes the set difference--------------------------------------------------
PG_FUNCTION_INFO_V1(intset_dif);

Datum
intset_dif(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_VARLENA_P(0);
    intset *b = (intset *) PG_GETARG_VARLENA_P(1);
    intset *newset;
    
    int a_l = VARSIZE_ANY_EXHDR(a) / 4;
    int b_l = VARSIZE_ANY_EXHDR(b) / 4;
    int copyofa[a_l];

    memcpy(copyofa, a->data, VARSIZE_ANY_EXHDR(a));
    
    int newsize = a_l;
    int newbool[newsize];

    int count = 0;
    int i;
    for (i = 0; i < a_l; i++)
    {
        if ((intset_con_internal(copyofa[i], b)))
        {
            continue;
            //newbool[count]=copyofa[i];
        //  count+=1;
        }
        else
        {
            newbool[count]=copyofa[i];
            count+=1;
        }
    }
    
    newset = (intset *) palloc(newsize * sizeof(intset) + 4);
    SET_VARSIZE(newset, (4 * newsize) + VARHDRSZ);
    memcpy(newset->data, newbool,newsize * 4);
    PG_RETURN_POINTER(newset);
}¡