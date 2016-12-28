#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BASE_LOW   480
#define ZERO_HI    380
#define ONE_HI     1247
#define MARGIN     100
#define INTRO_LOW  3510
#define INTRO_HI   1681
#define SEPARATOR  35440


char decode(int val1, int val2){
    if (abs(val1 - INTRO_LOW) < MARGIN && abs(val2 - INTRO_HI) < MARGIN)
        return '[';
    if (abs(val1 - BASE_LOW) < MARGIN && abs(val2 - ZERO_HI) < MARGIN)
        return '0';
    if (abs(val1 - BASE_LOW) < MARGIN && abs(val2 - ONE_HI) < MARGIN)
        return '1';
    if (abs(val1 - BASE_LOW) < MARGIN && abs(val2 - SEPARATOR) < MARGIN)
        return ']';
    
    return '.';
}

int abs(int val){
    if (val < 0)
        return -val;
    else
        return val;
}

int main(int argc, char** argv)
{
    int val1, val2;
    char delims[] = " ";
    char *result = NULL;
    char line[256];
    int cnt, index = 0;
    unsigned char num0[10];;
    unsigned char num1[10];
    char* nIndex = 0;
    FILE *f, * out;
    char dec;
    
    if (argc < 3) {
        printf("usage = %s fileName output\n", argv[0]);
        exit(0);
    }
    
    cnt = 0;
    memset(num0, 0, sizeof(num0));
    memset(num1, 0, sizeof(num1));
    f = fopen(argv[1],"r");
    out = fopen(argv[2],"w");
    while (fgets(line, 256, f) != NULL){
        result = strtok(line, delims);
        index = 0;
        while (result != NULL) {
            if (index & 1) {
                if ((nIndex = strchr(result,'\n'))) {
                    *nIndex = '\0';
                }
                val2 = atoi(result);
                dec = decode(val1,val2);
                if (dec == '[') {
                    cnt++;
                }
                else if (dec == '0') {
                    num0[cnt]++;
                }
                else if (dec == '1') {
                    num1[cnt]++;
                }
                
                fprintf(out,"%c",dec);
            }
            else{
                val1 = atoi(result);
            }
            result = strtok(NULL, delims);
            index++;
        }
    }
    cnt++;
    
    fprintf(out, "]\n");
    fclose(f);
    fclose(out);
    
    for (index=0; index!=cnt; index++) {
        printf("O: %i 1: %i [%i.%i B.b]\n", num0[index], num1[index], (num0[index]+num1[index])/8, (num0[index]+num1[index])%8);
    }
    
    return 0;
}
