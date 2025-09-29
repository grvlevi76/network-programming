#include<stdio.h>
#include<string.h>

int main() {
    char s[100];
    fgets(s,sizeof(s),stdin);
    printf("%s\n",s);
    printf("%d\n",strlen(s));
 }
