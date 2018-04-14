#include <stdio.h>  
#include <string.h>  
void initBuf(char *buf);  
void prtBuf(char *buf);  
char mem[8];  
char buf[128];  

int main()  
{  
	int i=0, j = 0;

	i = 1;
	i = 2;
	i = 3;
	i = 4;
	i = 5;

    //initBuf(buf);  
    //prtBuf(buf);  
    return 0;  
}  
  
void initBuf(char *pBuf)  
{  
    int i, j;  
    mem[0]='0';  
    mem[1]='1';  
    mem[2]='2';  
    mem[3]='3';  
    mem[4]='4';  
    mem[5]='5';  
    mem[6]='6';  
    mem[7]='7';  
    //ascii table first 32 is not printable  
    for(i=2;i<8;i++)  
    {  
        for(j=0;j<16;j++)  
            pBuf[i*16+j]=i*16+j;  
    }  
}  
  
void prtBuf(char *pBuf)  
{  
    int i, j;  
    for(i=2;i<8;i++)  
    {  
        for(j=0;j<16;j++)  
            printf("%c  ", pBuf[i*16+j]);  
        printf("\n");  
    }  
} 
