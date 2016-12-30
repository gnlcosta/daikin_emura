/**
 * By Gianluca Costa <g.costa@xplico.org>
 * Copyright 2016 Gianluca Costa
 *
 * This  program  is free software:  you can  redistribute it and/or
 * modify  it  under  the terms  of  the  GNU Affero  General Public
 * License  as  published  by the  Free Software Foundation,  either 
 * version 3 of the License,  or (at your option) any later version.
 * 
 * This program is  distributed in  the hope that it will be useful,
 * but WITHOUT ANY WARRANTY;  without even  the implied  warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * Affero General Public License for more details.
 * 
 * You should have received a copy of the  GNU Affero General Public
 * License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <wiringPi.h>

#include "cmd.h"
#include "version.h"


#define ONE_LOW               475 // us
#define ONE_HI               1253 // us
#define ZERO_LOW              475 // us
#define ZERO_HI               391 // us
#define SEPARATOR           35440 // us
#define INTRO_LOW            3510 // us
#define INTRO_HI             1688 // us

#define HEADER_SPEPARATOR   25000 // us
#define ZERO_SEQ_NUM            5

#define DAIKIN_HDR_0         0x11 // header
#define DAIKIN_HDR_1         0xDA // header
#define DAIKIN_HDR_2         0x27 // header

#define CMD_TRANSM_TIMES     3

#define FILE_LINE_MAX_SIZE   512

static inline void One(void)
{
    // low
    pwmWrite(1, 128);
    delayMicroseconds(ONE_LOW);
    
    // hi
    pwmWrite(1, 0);
    delayMicroseconds(ONE_HI);
}


static inline void Zero(void)
{
    // low
    pwmWrite(1, 128);
    delayMicroseconds(ZERO_LOW);
    
    // hi
    pwmWrite(1, 0);
    delayMicroseconds(ZERO_HI);
}


static inline void Header(void)
{
    unsigned short i;

    for (i=0; i!=ZERO_SEQ_NUM; i++) {
        Zero();
    }
    
    pwmWrite(1, 128);
    delayMicroseconds(ZERO_LOW);
    pwmWrite(1, 0);
    delayMicroseconds(HEADER_SPEPARATOR);
}

static inline void StartBlock(void)
{
    // low
    pwmWrite(1, 128);
    delayMicroseconds(INTRO_LOW);
    
    // hi
    pwmWrite(1, 0);
    delayMicroseconds(INTRO_HI);
}


static inline void SepartBlock(void)
{
    pwmWrite(1, 128);
    delayMicroseconds(ZERO_LOW);
    pwmWrite(1, 0);
    delayMicroseconds(SEPARATOR);
}


static inline void Tail(void)
{
    pwmWrite(1, 128);
    delayMicroseconds(ZERO_LOW);
    pwmWrite(1, 0);
}


static void DaikinChecksum(daikin_msg *msg)
{
    unsigned short i;
    unsigned char *cmd;
    unsigned char checksum;
    
    checksum = 0;
    cmd = (unsigned char *)&msg->cmd_a;
    for (i=0; i!=sizeof(msg->cmd_a); i++) {
        checksum += cmd[i];
    }
    msg->cmd_a.checksum = checksum;
    //printf("Checksum: 0x%X\n", checksum);
    
    checksum = 0;
    cmd = (unsigned char *)&msg->cmd_b;
    for (i=0; i!=sizeof(msg->cmd_b); i++) {
        checksum += cmd[i];
    }
    msg->cmd_b.checksum = checksum;
    //printf("Checksum: 0x%X\n", checksum);
    
    checksum = 0;
    cmd = (unsigned char *)&msg->cmd_c;
    for (i=0; i!=sizeof(msg->cmd_c); i++) {
        checksum += cmd[i];
    }
    msg->cmd_c.checksum = checksum;
    //printf("Checksum: 0x%X\n", checksum);
}

static void CmdDaikin(daikin_msg *msg)
{
    unsigned short i, j;
    unsigned char *cmd;
    
    Header();
    
    StartBlock();
    cmd = (unsigned char *)&msg->cmd_a;
    for (i=0; i!=sizeof(msg->cmd_a); i++) {
        for (j=1; j!= 0x100; j<<=1) {
            if (cmd[i] & j) {
                One();
            }
            else {
                Zero();
            }
        }
    }
    SepartBlock();
    
    StartBlock();
    cmd = (unsigned char *)&msg->cmd_b;
    for (i=0; i!=sizeof(msg->cmd_b); i++) {
        for (j=1; j!= 0x100; j<<=1) {
            if (cmd[i] & j) {
                One();
            }
            else {
                Zero();
            }
        }
    }
    SepartBlock();
    
    StartBlock();
    cmd = (unsigned char *)&msg->cmd_c;
    for (i=0; i!=sizeof(msg->cmd_c); i++) {
        for (j=1; j!= 0x100; j<<=1) {
            if (cmd[i] & j) {
                One();
            }
            else {
                Zero();
            }
        }
    }
    
    Tail();
}


static void InitIR(void)
{

    if (wiringPiSetup() == -1)
        exit (1) ;
        
    pinMode(1, PWM_OUTPUT) ;
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(2);
    pwmSetRange(253);
    
    pwmWrite(1, 0);
}


static void Usage(const char *app)
{
    printf("Usage:\n%s commands_file.cmd\n", app);
    printf("Exmaple commands_file.cmd:\n");
    printf("\t# On/Off Emura 0:Off; 1:On\n");
    printf("\ton_off 1\n");
    printf("\t# Mode  0:auto; 1:dehumidifier; 2:air conditioning; 3:heat pump; 4:fan\n");
    printf("\tmode 0\n");
    printf("\t# Temperature 18 to 30 C\n");
    printf("\ttemp 18\n");
    printf("\t# Fan 0:fan1; 1:fan2; 2:fan3; 3:fan4; 4:fan5; 5:auto; 6:night\n");
    printf("\tfan 5\n");
    printf("\t# Comfort 0:Off; 1:On\n");
    printf("\tcomfort 0\n");
    printf("\t# Swing 0:Off; 1:On\n");
    printf("\tswing 0\n");
    printf("\t# Powerful 0:Off; 1:On\n");
    printf("\tpowerful 0\n");
    printf("\t# Quiet 0:Off; 1:On\n");
    printf("\tquiet 0\n");
    printf("\t# Sensor 0:Off; 1:On\n");
    printf("\tsensor 0\n");
    printf("\t# Econo 0:Off; 1:On\n");
    printf("\tecono 0\n");
}


int main(int argc, char **argv)
{
    int i;
    time_t tnow;
    daikin_msg msg;
    struct sched_param sched;
    memset(&sched, 0, sizeof(struct sched_param));
    memset(&msg, 0, sizeof(daikin_msg));
    unsigned char comfort, on_off, mode, temp, deumid, fan, swing, powerful, quiet, sensor, econo;
    FILE *fpcmd;
    char line[FILE_LINE_MAX_SIZE];
    char param[FILE_LINE_MAX_SIZE];
    int value;
    

    printf("Raspberry Pi Daikin Emura v.%d.%d.%d\n", VER_MAJ, VER_MIN, VER_REV);
    if (argc != 2) {
        Usage(argv[0]);
        return -1;
    }

    // default value
    comfort = 0; // off
    on_off = 0; // off
    mode = 6; // fan
    temp = 18; // 18C
    deumid = 1;
    fan = 10; // auto
    swing = 0; // off
    powerful = 0; // off
    quiet = 0; // off
    sensor = 0; // off
    econo = 0; // off 

    // read command file
    fpcmd = fopen(argv[1], "r");
    if (fpcmd != NULL) {
        while (fgets(line, FILE_LINE_MAX_SIZE, fpcmd) != NULL) {
            if (line[0] == '#')
                continue;

            i = sscanf(line, "%s %d", param, &value);
            if (i != 2)
                continue;
                
            if (strcmp(param, "on_off") == 0) {
                if (value == 1)
                    on_off = 1;
            }
            else if (strcmp(param, "mode") == 0) {
                switch (value) {
                case 0: // auto
                    mode = 0;
                    break;
                
                case 1: // dehumidifier
                    mode = 2;
                    break;
                   
                case 2: // air conditioning
                    mode = 3;
                    break;
                   
                case 3: // heat pump
                    mode = 4;
                    break;
                   
                case 4: // fan
                    mode = 6;
                    break;
                }
            }
            else if (strcmp(param, "temp") == 0) {
                if (value > 17 && value < 31) {
                    temp = value;
                }
            }
            else if (strcmp(param, "fan") == 0) {
                switch (value) {
                case 0: // level 1
                    fan = 3;
                    break;
                    
                case 1: // level 2
                    fan = 4;
                    break;
                    
                case 2: // level 3
                    fan = 5;
                    break;
                    
                case 3: // level 4
                    fan = 6;
                    break;
                    
                case 4: // level 5
                    fan = 7;
                    break;
                    
                case 5: // auto
                    fan = 10;
                    break;
                    
                case 6: // night
                    fan = 11;
                    break;
                }
            }
            else if (strcmp(param, "comfort") == 0) {
                if (value == 1) {
                    comfort = 1;
                }
            }
            else if (strcmp(param, "swing") == 0) {
                if (value == 1) {
                    swing = 1;
                }
            }
            else if (strcmp(param, "powerful") == 0) {
                if (value == 1) {
                    powerful = 1;
                }
            }
            else if (strcmp(param, "quiet") == 0) {
                if (value == 1) {
                    quiet = 1;
                }
            }
            else if (strcmp(param, "sensor") == 0) {
                if (value == 1) {
                    sensor = 1;
                }
            }
            else if (strcmp(param, "econo") == 0) {
                if (value == 1) {
                    econo = 1;
                }
            }
        }
    }

    // fixed bit/byte
    msg.cmd_a.header_0 = DAIKIN_HDR_0;
    msg.cmd_a.header_1 = DAIKIN_HDR_1;
    msg.cmd_a.header_2 = DAIKIN_HDR_2;
    msg.cmd_a.dummy4 = 0xC5; // fixed value (not decodec)
    msg.cmd_a.dummy5 = 0x10;

    msg.cmd_b.header_0 = DAIKIN_HDR_0;
    msg.cmd_b.header_1 = DAIKIN_HDR_1;
    msg.cmd_b.header_2 = DAIKIN_HDR_2;
    msg.cmd_b.dummy4 = 0x42; // fixed value (not decodec)

    msg.cmd_c.header_0 = DAIKIN_HDR_0;
    msg.cmd_c.header_1 = DAIKIN_HDR_1;
    msg.cmd_c.header_2 = DAIKIN_HDR_2;
    msg.cmd_c.dummy5_a = 0x04; // fixed value (not decodec)
    msg.cmd_c.dummy11 = 0x06;
    msg.cmd_c.dummy12 = 0x60;
    msg.cmd_c.dummy15 = 0xC1;
    msg.cmd_c.dummy16 = 0x10;

    // compose command with user data
    msg.cmd_a.comfort = comfort;
    
    tnow = time(NULL);
    msg.cmd_b.minutes = (tnow/60)%(24*60); // minutes from midnight
    msg.cmd_b.wday = localtime(&tnow)->tm_wday + 1; // week day

    msg.cmd_c.on_off = on_off;
    msg.cmd_c.mode = mode;
    msg.cmd_c.temp = (temp-18+2);
    msg.cmd_c.deumid = deumid;
    msg.cmd_c.fan = fan;
    if (swing) {
        msg.cmd_c.swing = 15;
    }
    else {
        msg.cmd_c.swing = 0;
    }
    msg.cmd_c.powerful = powerful;
    msg.cmd_c.quiet = quiet;
    msg.cmd_c.sensor = sensor;
    msg.cmd_c.econo = econo;

    DaikinChecksum(&msg);

    sched.sched_priority = 8;  // set priority
    if ( sched_setscheduler(getpid(), SCHED_FIFO, &sched) < 0 )
        fprintf(stderr, "SETSCHEDULER failed - err = %s\n", strerror(errno));
    else
        printf("Priority set to \"%d\"\n", sched.sched_priority);
    
    InitIR();

    for (i=0; i!=CMD_TRANSM_TIMES; i++) {
        CmdDaikin(&msg);
        delay(150);
    }
    
    return 0;
}

