#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include <assert.h>
#include <ctype.h>

//--------�ж�iԪ���ǲ��� inset �е�һ���Ӽ�-----------------
static bool
intset_con_internal(int i, intset *a)
{
    int *copyofa;
    int l = ARSIZE_ANY_EXHDR(a) / 4;
	//len = VARSIZE_ANY_EXHDR(a) / 4;
	int f = 0;
    memcpy(copyofa , a->data, ARSIZE_ANY_EXHDR(a));
    
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
//--------------�ж� inset a �ǲ��� inset b ���Ӽ�-------------------------------------------------
static bool
intset_sub_internal(intset * a, intset * b)
{
    int *copyofa ;
	//int l = a->len;
    //int len = VARSIZE(a);
	int l = ARSIZE_ANY_EXHDR(a) / 4;
    memcpy(copyofa , a->data, ARSIZE_ANY_EXHDR(a));
    
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
//-------------ʵ��i <@ S���ж�i�ǲ���s�����һ��Ԫ��------------------------------------------------
PG_FUNCTION_INFO_V1(intset_con);

Datum 

intset_con(PG_FUNCTION_ARGS)

{
    int *i = (int *) PG_GETARG_POINTER(0);
    intset *a = (intset *) PG_GETARG_POINTER(1);
    
    PG_RETURN_BOOL(intset_con_internal(*i, a));
}	
//------------@ S ���� intset Ԫ�������ظ�Ԫ�صĸ���-----------------------------------------

PG_FUNCTION_INFO_V1(intset_cdn);

Datum
intset_cdn(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_POINTER(0);
    //int l = a -> len;
	int l =ARSIZE_ANY_EXHDR(a)/4;
    
    PG_RETURN_INT32(l);
}

//-------------A @> B �ж�A�ǲ���B�����Ӽ� A------------------------------------------------

PG_FUNCTION_INFO_V1(intset_sub);

Datum
intset_sub(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_POINTER(0);
    intset *b = (intset *) PG_GETARG_POINTER(1);
    
    PG_RETURN_BOOL(intset_sub_internal(a, b));
}

//------------�ж�intsetA�ǲ�����intsetB���-----------------------------------------------------PG_FUNCTION_INFO_V1(intset_eq);

Datum
intset_eq(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_POINTER(0);
    intset *b = (intset *) PG_GETARG_POINTER(1);
    
    PG_RETURN_BOOL(intset_sub_internal(a, b) && intset_sub_internal(b, a));
}
//------------��intsetA��insetB�Ľ���-----------------------------------------------------------
PG_FUNCTION_INFO_V1(intset_int);

Datum
intset_int(PG_FUNCTION_ARGS)
{
    intset *a = (intset *) PG_GETARG_POINTER(0);
    intset *b = (intset *) PG_GETARG_POINTER(1);
    intset *newset;
    
    int *copyofa;
    //int a_l = a->len;
	int a_l = ARSIZE_ANY_EXHDR(a)/4;
    //int b_l = b->len;
	int b_l = ARSIZE_ANY_EXHDR(b)/4;

	int copyofa[a_l];

    memcpy(copyofa, a->data, ARSIZE_ANY_EXHDR(a));

	if (a_l<=b_l) {newsize = b_l;}
	else {newsize = a_l;}

    
    //max_size = (a_len <= b_len) ? a_len : b_len;

    //int newbool = malloc(newsize * sizeof(int));
	int newbool[newsize];
    int count = 0;
    for (int i = 0; i < a_l; i++) 
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
//--------------------��intsetA��intsetB�Ĳ���------------------------------------------------
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

    newsize = a_l + b_l;
	int newbool[newsize];
    //int temp = malloc(max_size * sizeof(int));
    int count = 0;
    for (int i = 0; i < a_l; i++)
	{
        newbool[count] = copyofa[i];
        count += 1;
    }

    for (int j = 0; j < b_l; j++)
	{ 
        newbool[count] = copyofb[j];
        count += 1;
    }
    
    //newset = (intset *) palloc(sizeof(Intset));
	newset = (intset *) palloc(newsize * sizeof(intset) + 4);
	SET_VARSIZE(newset, (4 * newsize) + VARHDRSZ);
    memcpy(newset->data, newbool, max_size * 4);
    PG_RETURN_POINTER(newset);
}

//--------------��intsetA��intset B��disjunction,---��A�в���B�к���B�в���A�е�Ԫ��----------------------------------------------
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
    
    newsize = a_l + b_l;

    int newbool[newsize];

    int count = 0;

    for (int i = 0; i < a_l; i++)
	{
        if ((intset_con_internal(copyofa[i], b))) 
		{
			continue;
 			//newbool[count]=copyofa[i];
		//	count+=1;
		}
		else
		{
			newbool[count]=copyofa[i];
			count+=1;
		}
    }
    for (int j = 0; j < b_l; j++) 
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
//-----------����A�ж�����B�е�Ԫ��  A - B takes the set difference--------------------------------------------------
PG_FUNCTION_INFO_V1(intset_dif);

Datum
intset_dif(PG_FUNCTION_ARGS)
{
    intset *a = (Intset *) PG_GETARG_VARLENA_P(0);
    intset *b = (Intset *) PG_GETARG_VARLENA_P(1);
    intset *newset;
    
    int a_l = VARSIZE_ANY_EXHDR(a) / 4;
    int b_l = VARSIZE_ANY_EXHDR(b) / 4;
    int copyofa[a_l];

    memcpy(copyofa, a->data, VARSIZE_ANY_EXHDR(a));
    
    newsize = a_l;
    int newbool[newsize];

    int count = 0;

    for (int i = 0; i < a_l; i++)
	{
        if ((intset_con_internal(copyofa[i], b)))
		{
			continue;
			//newbool[count]=copyofa[i];
		//	count+=1;
        }
		else
		{
			newbool[count]=copyofa[i];
			count+=1;
		}
    }
    
    newset = (intset *) palloc(newsize * sizeof(intset) + 4);
    SET_VARSIZE(newset, (4 * newsize) + VARHDRSZ);
	//
    memcpy(newset->data, newbool,newsize * 4);

    PG_RETURN_POINTER(newset);
}

//--------------------------------------------------------------------------------
