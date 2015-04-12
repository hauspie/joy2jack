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

/* This mapping corresponds to the rockband wii drum kit
   
   Midi next/previous channels are + and - buttons
*/

#define MIDI_NEXT_CHANNEL     BUTTON_EVENT(9, 1)
#define MIDI_PREVIOUS_CHANNEL BUTTON_EVENT(8, 1)

#endif
