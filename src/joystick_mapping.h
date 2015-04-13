/* This file is part of joy2jack. Copyright  2015- Michael Hauspie
 *
 * joy2jack is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * joy2jack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with joy2jack.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __JOYSTICK_MAPPING_H__
#define __JOYSTICK_MAPPING_H__
#include "mapping_helpers.h"


/* Wii Rock band controler (from left to righ) */
#define RED_PAD      2
#define YELLOW_PAD   3
#define BLUE_PAD     0
#define GREEN_PAD    1
#define A_BUTTON     1
#define B_BUTTON     2
#define ONE_BUTTON   0
#define TWO_BUTTON   3
#define PLUS_BUTTON  9
#define MINUS_BUTTON 8

/* Names for values */
#define BUTTON_PRESSED 1
#define BUTTON_RELEASED 0

/* This mapping corresponds to the rockband wii drum kit
   
   Midi next/previous channels are + and - buttons
*/

#define MIDI_NEXT_CHANNEL     BUTTON_EVENT(PLUS_BUTTON, BUTTON_PRESSED)
#define MIDI_PREVIOUS_CHANNEL BUTTON_EVENT(MINUS_BUTTON, BUTTON_RELEASED)

#define MIDI_SEND_NOTE_OFF         AXIS_EVENT(4, 32767)
#define MIDI_DONT_SEND_NOTE_OFF    AXIS_EVENT(4, -32767)


/* Alternate Button Names */


#endif
