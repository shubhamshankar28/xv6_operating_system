
#include<stdio.h> 
#include<stdlib.h>
#include"drawing.h"
int mdraw2(void *p,uint n) 
{ 

FILE *fp =fopen("art3" , "r");

if(fp==NULL)
{
    printf("wrong");
    return 0;
}
char c[1200];
int count1=0;
char ch;
while((ch=getc(fp))!=EOF)
{
    c[count1]=ch;count1++;

}
c[count1]='\0';
count1++;
fclose(fp);  
/*
char c[1200] = "%         #   @ %  (   #         /\n @        #      #     #       ..  \n   /@@@@                 #@@@@ \n"; 
int count1=0; 
while(c[count1]!='\0') 
count1++; 
count1++; 
*/
if(n>=count1) 
{ 
for(int i=0;i<count1;++i) 
*(char *)(p+i)=c[i];  
return count1; 
} 
return -1;
} 
 
