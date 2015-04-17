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
#ifndef __VECTOR_H__
#define __VECTOR_H__


typedef struct
{
   void *element_array;
   int element_size;
   int max_elements;
   int element_count;
} vector_t;


int vector_create(vector_t *v, int max_element, int element_size);

int vector_add_value(vector_t *v, void *value);

int vector_free(vector_t *v);
#endif
