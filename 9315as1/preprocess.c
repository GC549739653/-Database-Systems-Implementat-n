#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>


typedef struct intset
{
	int  len;
	int	 *data;
	int  marker; 
} intset;

/* check whether is valid*/
static int isdigitstr(char *str)  
{  
	return (strspn(str, "0123456789")==strlen(str));  
}

int countDigits(char *str){
	char *string,*p;
	int numInteger = 0;
	string = (char*)malloc(strlen(str)*sizeof(char));
	strcpy(string,str);
	for (p = strtok(string, " {},\t\r\n"); p; p = strtok(NULL, " {},\t\r\n")) numInteger ++;
	return numInteger;
}

int countComma(char *str){
	int numComma = 0;
	for(int i= 0;i < strlen(str);i++)
		if(str[i] == ',') numComma ++;
	return numComma;
}

int countBrackets(char *str){
	int numbrack = 0;
	for (int i = 0; i < strlen(str); i++)
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



/*static int checkComma(char *str){
	
	char *string,*p;
	int numComma,numInteger,result;

	result = 0;
	string = (char*)malloc(strlen(str)*sizeof(char));

	strcpy(string,str);

	numInteger = 0;
	for (p = strtok(string, " {},\t\r\n"); p; p = strtok(NULL, " {},\t\r\n")) numInteger ++;

	numComma =0;
	for(int i= 0;i < strlen(str);i++)
		if(str[i] == ',') numComma ++;

	if(numComma + 1 == numInteger) result = 1;
	return result;
}

int checkEmpty(char *str){
	int numdigits,numComma,numbrack,result;
	result = 0;
	numdigits = countDigits(str);
	numbrack = countBrackets(str);
    numComma = countComma(str);

	if(numdigits == 0 && numbrack == 2 && numComma == 0) result = 1;
	return result;
} */

/* here we return 2 indicate empty set, 1 is valid set,0 is not valid*/

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

/*char deReplicate( *array){
	char *result;

	result = (char*)malloc()
}*/

//do preprocess, check valid first

intset *preprocess(char *str){
	int checker;
	char *string,*p;

	string = (char*)malloc(strlen(str)*sizeof(char));
	strcpy(string,str);

	intset *set;
    set = malloc(sizeof(intset));

    set -> data = (int*)malloc(strlen(str)*sizeof(int));
    set -> len = 0;
	checker = isValid(str);
	if(checker == 0) {
		set -> marker = -1;
	}
	else if(checker == 2){
		set -> marker = 0;
		set -> data = NULL;
	}
	else{
		set->marker = 1;
		for (p = strtok(string, " {}, \t\r\n"); p; p = strtok(NULL, " {},\t\r\n")) {
			int value = atoi(p);
			set -> data[set -> len] = value;
			set -> len ++;
			// printf("%d\n",value);
		}
	}
	// printf("\n");
	return set;
}




int main(){

	char *test0 = "{111111  1,1,1,3,3,3}";
	char *test1 = "{{}}";
	char *test2 = "{a,b,c}";
	char *test3 = "{2,3}";
	char *test4 = "{ 1 2 3 4 }";

/*	printf("checkComma %d\n",checkComma(test0));
	printf("checkEmpty %d\n",checkEmpty(test1));
	printf("isAlphabet %d\n",isAlphabet(test2));
	printf("isAlphabet %d\n",isAlphabet(test3));*/

	printf("%d\n",isValid(test0) );
	printf("%d\n",isValid(test1) );
	printf("%d\n",isValid(test2) );
	printf("%d\n",isValid(test3) );
	printf("%d\n",isValid(test4) );


	char *test5 = "{ }";
	char *test6 = "{2,3,1}";
	char *test7 = "{6,6,6,6,6,6}";
	char *test8 = "{10, 9, 8, 7, 6,5,4,3,2,1}";
	char *test9 = "{    1  ,  3  ,  5 , 7,9 }";

/*	printf("%d\n",preprocess(test5) );
	printf("%d\n",preprocess(test6) );
	printf("%d\n",preprocess(test7) );
	printf("%d\n",preprocess(test8) );
	printf("%d\n",preprocess(test9) );*/

	intset *set0;
	intset *set1;
	intset *set2;
	intset *set3;
	intset *set4;


	set0 = preprocess(test5);
	set1 = preprocess(test6);
	set2 = preprocess(test7);
	set3 = preprocess(test8) ;
	set4 =  preprocess(test9);

	printf("set length is %d\n", set0 -> len);
	printf("set length is %d\n", set1 -> len);
	printf("set length is %d\n", set2 -> len);
	printf("set length is %d\n", set3 -> len);
	printf("set length is %d\n", set4 -> len);

	int Num[5];

	for(int i; i < 5; i ++){
		Num[i] = i;
	}
	int result = sizeof(Num);

	printf("%d\n",result);


	return 0;
}