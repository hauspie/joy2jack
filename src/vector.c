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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vector.h"

#define ADDR_AT(ptr, idx, elem_size) (((uint8_t*)(ptr)) + (idx)*(elem_size))


int vector_create(vector_t *v, int max_element, int element_size)
{
   v->max_elements = max_element;
   v->element_size = element_size;
   v->element_count = 0;
   v->element_array = malloc(v->max_elements*v->element_size);
   if (!v->element_array)
   {
      perror("vector_create");
      return -1;
   }
   return 0;
}

int vector_add_value(vector_t *v, void *value)
{
   if (v->element_count == v->max_elements)
   {
      int new_size = v->max_elements * 2;
      void *tmp = realloc(v->element_array, new_size*v->element_size);
      if (tmp == NULL)
         return -1;
      v->max_elements = new_size;
      v->element_array = tmp;
   }
   memcpy(ADDR_AT(v->element_array, v->element_count, v->element_size), value, v->element_size);
   v->element_count++;
   return 0;
}

int vector_free(vector_t *v)
{
   free(v->element_array);
   return 0;
}
