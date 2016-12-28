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


int main(void)
{
    daikin_msg msg;
    struct sched_param sched;
    
    memset(&sched, 0, sizeof(struct sched_param));
    memset(&msg, 0, sizeof(daikin_msg));

    printf("Raspberry Pi Daikin [%i ,%i , %i, %i]\n", sizeof(msg.cmd_a), sizeof(msg.cmd_b), sizeof(msg.cmd_c), sizeof(daikin_msg));
    
    sched.sched_priority = 8;        /* set priority */

    if ( sched_setscheduler(getpid(), SCHED_FIFO, &sched) < 0 )
        fprintf(stderr, "SETSCHEDULER failed - err = %s\n", strerror(errno));
    else
        printf("Priority set to \"%d\"\n", sched.sched_priority);

    // fixed bit/byte
    msg.cmd_a.header_0 = DAIKIN_HDR_0;
    msg.cmd_a.header_1 = DAIKIN_HDR_1;
    msg.cmd_a.header_2 = DAIKIN_HDR_2;
    msg.cmd_a.dummy4 = 0xC5;
    msg.cmd_a.dummy5 = 0x10;

    msg.cmd_b.header_0 = DAIKIN_HDR_0;
    msg.cmd_b.header_1 = DAIKIN_HDR_1;
    msg.cmd_b.header_2 = DAIKIN_HDR_2;
    msg.cmd_b.dummy4 = 0x42;

    msg.cmd_c.header_0 = DAIKIN_HDR_0;
    msg.cmd_c.header_1 = DAIKIN_HDR_1;
    msg.cmd_c.header_2 = DAIKIN_HDR_2;
    msg.cmd_c.dummy5_a = 0x04;
    msg.cmd_c.dummy11 = 0x06;
    msg.cmd_c.dummy12 = 0x60;
    msg.cmd_c.dummy15 = 0xC1;
    msg.cmd_c.dummy16 = 0x10;
    
    msg.cmd_b.minutes = 0;//(time(NULL)/60)%(24*60); // minutes from midnight
    msg.cmd_b.wday = 1;
    msg.cmd_c.on_off = 1;
    msg.cmd_c.mode = 7;

    DaikinChecksum(&msg);
    
    InitIR();
    CmdDaikin(&msg);
    
    return 0;
}

