#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CMD_MAX_SIZE      1024

int main(int argc, char** argv)
{
    char cmd1_bit[CMD_MAX_SIZE];
    char cmd2_bit[CMD_MAX_SIZE];
    char diff[CMD_MAX_SIZE];
    char diff_full[CMD_MAX_SIZE];
    char byte_grp[CMD_MAX_SIZE];
    char byte_name[CMD_MAX_SIZE];
    char byte_value[CMD_MAX_SIZE];
    unsigned char byte_val;
    int c, c1num, c2num, i, sim, n, fn;
    int byte_num, bit_num;
    FILE *fcmd1, *fcmd2;
    
    if (argc < 3) {
        printf("usage: %s cmd_1 cmd_2\n", argv[0]);
        exit(0);
    }
    
    fn = 1;
    memset(diff_full, ' ', sizeof(diff_full));
    do {
        fcmd1 = fopen(argv[fn++],"r");
        fcmd2 = fopen(argv[fn],"r");
        
        if (fcmd1 == NULL || fcmd2 == NULL) {
            fclose(fcmd1);
            fclose(fcmd2);
            printf("File error\n");
            return -1;
        }
        // load commands
        c1num = 0;
        while ( (c=fgetc(fcmd1)) != EOF && c != '\n') {
            cmd1_bit[c1num++] = c;
        }
        c2num = 0;
        while ( (c=fgetc(fcmd2)) != EOF && c != '\n') {
            cmd2_bit[c2num++] = c;
        }
        
        // check commands similarity
        sim = 1;
        if (c1num != c2num) {
            printf("Commannd not similar %i!=%i\n", c1num, c2num);
            sim = 0;
        }
        for (i=0; i!=c1num; i++) {
            if (cmd1_bit[i] == '[' && cmd2_bit[i] != '[') {
                sim = 0;
                printf("Commannd not similar bit %i\n", i);
                break;
            }
            if (cmd2_bit[i] == '[' && cmd1_bit[i] != '[') {
                sim = 0;
                printf("Commannd not similar bit %i\n", i);
                break;
            }
            if (cmd1_bit[i] == ']' && cmd2_bit[i] != ']') {
                sim = 0;
                printf("Commannd not similar bit %i\n", i);
                break;
            }
            if (cmd2_bit[i] == ']' && cmd1_bit[i] != ']') {
                sim = 0;
                printf("Commannd not similar bit %i\n", i);
                break;
            }
        }
        if (sim == 0)
            return -1;
            
        // compare
        memset(diff, ' ', sizeof(diff));
        for (i=0; i!=c1num; i++) {
            if (cmd1_bit[i] != cmd2_bit[i]) {
                diff[i] = '*';
                diff_full[i] = '*';
            }
        }
        
        // display
        memset(byte_grp, ' ', sizeof(byte_grp));
        memset(byte_name, ' ', sizeof(byte_name));
        memset(byte_value, ' ', sizeof(byte_value));
        for (i=0; i!=c1num; i++) {
            printf("%c", cmd1_bit[i]);
        }
        printf(" <- %s\n", argv[fn-1]);
        for (i=0; i!=c2num; i++) {
            printf("%c", cmd2_bit[i]);
        }
        printf(" <- %s\n", argv[fn]);
        for (i=0; i!=c1num; i++) {
            printf("%c", diff[i]);
        }
        printf("\n");
    } while (fn+1 != argc);
    
    byte_num = 0;
    bit_num = 0;
    for (i=0; i!=c1num; i++) {
        if (cmd1_bit[i] == '[' || cmd1_bit[i] == ']') {
            if (cmd1_bit[i] == ']') {
                // byte value
                n = sprintf(byte_value+i-bit_num-1, "| 0x%X", byte_val);
                byte_value[i+n-bit_num-1] = ' ';
                byte_value[i] = '|';
                byte_grp[i] = '|';
                byte_name[i] = '|';
            }
            byte_num = 0;
            bit_num = -1;
            byte_val = 0;
        }
        else {
            if (cmd1_bit[i] == '0' || cmd1_bit[i] == '1') {
                bit_num = (bit_num+1)%8;
                if (bit_num == 0) {
                    // group
                    byte_grp[i] = '|';
                    // byte name
                    n = sprintf(byte_name+i, "| B %i", byte_num);
                    byte_name[i+n] = ' ';
                    // byte value
                    if (byte_num != 0) {
                        n = sprintf(byte_value+i-8, "| 0x%X", byte_val);
                        byte_value[i+n-8] = ' ';
                    }
                    byte_val = 0;
                    byte_num++;
                }
                else  {
                    byte_grp[i] = '0'+bit_num;
                }
                if (cmd1_bit[i] == '1' && diff_full[i] != '*') {
                    byte_val |= 0x01<<bit_num;
                }
            }
        }
    }
    for (i=0; i!=c1num; i++) {
        printf("%c", diff_full[i]);
    }
    printf("\n");
    for (i=0; i!=c1num; i++) {
        printf("%c", byte_grp[i]);
    }
    printf("\n");
    for (i=0; i!=c1num; i++) {
        printf("%c", byte_name[i]);
    }
    printf("\n");
    for (i=0; i!=c1num; i++) {
        printf("%c", byte_value[i]);
    }
    printf("\n");
    
    
    return 0;
}
