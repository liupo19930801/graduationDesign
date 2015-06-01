/*************************************************************************
	> File Name: a.c
	> Author: 
	> Mail: 
	> Created Time: Mon 30 Mar 2015 08:18:19 PM CST
 ************************************************************************/

#include<stdio.h>


int a = 2;
int b = -3;
int z = 4;

int sum(int a, int b)
{
    int c[] = {1, 2,3, 4, 546};
    return a+b;
}

int main()
{
    static int a = 3;
    int b = 4;
    printf("%d + %d = %d\n", a, b, sum(a, b));
    return 0;
}
