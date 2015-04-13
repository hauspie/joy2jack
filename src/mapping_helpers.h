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
#ifndef __MAPPING_HELPERS_H__
#define __MAPPING_HELPERS_H__

#define BUTTON_EVENT(number,value) ((1 << 31) | ((number) & 0x7fff) << 16 | ((value) & 0xffff))
#define AXIS_EVENT(number,value) (((number) & 0x7fff) << 16 | ((value) & 0xffff))

#define IS_BUTTON(event_compact_description) ((event_compact_description) & (1 << 31) ? 1 : 0)
#define IS_AXIS(event_compact_description) ((event_compact_description) & (1 << 31) ? 0 : 1)
#define NUMBER(event_compact_description) ((int16_t)(((event_compact_description) >> 16) & 0x7fff))
#define VALUE(event_compact_description) ((int16_t) ( (event_compact_description) & 0xffff))

#define IS_SAME(joyevent_type, event_compact_description) (((joyevent_type) == JS_EVENT_BUTTON) && IS_BUTTON((event_compact_description)) || (((joyevent_type) == JS_EVENT_AXIS) && IS_AXIS((event_compact_description))))

#define CHECK_EVENT(joyevent, event_compact_description) (IS_SAME((joyevent).type, event_compact_description) && (joyevent).number == NUMBER((event_compact_description)) && (joyevent).value == VALUE((event_compact_description)))



#endif
