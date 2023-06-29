#include <stdio.h>
#include <stdlib.h>
int main(void) 
{
    char str[100] = {0};
    scanf("%s", str);

    char *end;
    double res = strtod(str, &end);
    printf("%c\n", *end);
    printf("%lf\n", res);
    
    return 0;
}
