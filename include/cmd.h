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

#ifndef __CMD_H
#define __CMD_H

typedef struct {
    struct __attribute__((packed)) {
        // byte 0
        unsigned char header_0:8;
        
        // byte 1
        unsigned char header_1:8;
        
        // byte 2
        unsigned char header_2:8;
        
        // byte 3
        unsigned char dummy3:8;
        
        // byte 4
        unsigned char dummy4:8;
        
        // byte 5
        unsigned char dummy5:8;
        
        // byte 6
        unsigned char dummy6_a :4;
        unsigned char comfort  :1;  // 0:Off; 1:on
        unsigned char dummy6_b :3;
        
        // byte 7
        unsigned char checksum:8;  // checksum
    } cmd_a;
    
    struct __attribute__((packed)) {
        // byte 0
        unsigned char header_0:8;
        
        // byte 1
        unsigned char header_1:8;
        
        // byte 2
        unsigned char header_2:8;
        
        // byte 3
        unsigned char dummy3:8;
        
        // byte 4
        unsigned char dummy4:8;
        
        // byte 5-6
        unsigned short minutes:11; // minute (starting from 0:00)
        unsigned short wday:5;     // day of the week: 1:Sun; 2:Mon; 3:Tue; 4:Wed; 5:Thu; 6:Fri; 7:Sat;
        
        // byte 7
        unsigned char checksum:8;  // checksum
    } cmd_b;
    
    struct __attribute__((packed)) {
        // byte 0
        unsigned char header_0:8;
        
        // byte 1
        unsigned char header_1:8;
        
        // byte 2
        unsigned char header_2:8;
        
        // byte 3
        unsigned char dummy3:8;
        
        // byte 4
        unsigned char dummy4:8;
        
        // byte 5
        unsigned char on_off   :1;    // 1: on, 0: off
        unsigned char dummy5_a :3;
        unsigned char mode     :3;    // 0:auto; 2:deumidificatore; 3:climatizzatore; 4:pompa di calore; 6:ventilatore
        unsigned char dummy5   :1;
        
        // byte 6
        unsigned char dummy6_a :1;    // 0
        unsigned char temp     :4;    // 2: 18C -> 14: 30C; 0: deumidificatore
        unsigned char deumid   :3;    // 1: temperatura: 6: deumidificatore
        
        // byte 7
        unsigned char dummy7:8;
        
        // byte 8
        unsigned char swing    :4;    // 0:off; 15:on
        unsigned char fan      :4;    // 3:fan1; 4:fan2; 5:fan3; 6:fan4; 7:fan5; 10:auto; 11:notte
        
        // byte 9
        unsigned char dummy8:8;
        
        // byte 10
        unsigned char dummy10:8;
        
        // byte 11
        unsigned char dummy11:8;
        
        // byte 12
        unsigned char dummy12:8;
        
        // byte 13
        unsigned char powerful  :1;   // 0:off; 1:on
        unsigned char dummy13_a :4;
        unsigned char quiet     :1;   // 0:off; 1:on
        unsigned char dummy13   :2;
        
        // byte 14
        unsigned char dummy14:8;
        
        // byte 15
        unsigned char dummy15:8;
        
        // byte 16
        unsigned char dummy16_a :1;
        unsigned char sensor    :1;   // 0:off; 1:on
        unsigned char econo     :1;   // 0:off; 1:on
        unsigned char dummy16   :5;
        
        // byte 17
        unsigned char dummy17:8;
        
        // byte 18
        unsigned char checksum:8;  // checksum
    } cmd_c;
    
} daikin_msg;
#endif
