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
#include <sys/types.h>
#include <regex.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define MAX_TOKEN_LENGTH 128

typedef struct {
   char symbol[MAX_TOKEN_LENGTH];
   int  value;
} symbolic_value_t;


typedef struct {
   symbolic_value_t *symbols;
   int symbol_count;
   int max_symbols;
} symbol_list_t;


/* Strips comments and ending space charaters and returns a pointer on
 * the first non space charater.
 * DOES modify the string!
 */
static char *chomp(char *buffer)
{
   char *ptr = buffer;

   /* Strip first spaces */
   while (*ptr && isspace(*ptr))
      ++ptr;
   if (*ptr == '\0')
      return ptr;
   char *start = ptr;
   /* Find comments */
   while (*ptr && *ptr != '#')
      ++ptr;
   /* Even if '#' was not found, it does not harm and just overwrites
    * \0 */
   *ptr = '\0';
   if (ptr == buffer)
      return ptr;
   /* strip last spaces */
   --ptr;
   while (ptr != start && isspace(*ptr))
      --ptr;
   if (ptr != start)
      ptr[1] = '\0';
   return start;
}

static char *subcpy(char *dst, const char *str, int so, int eo)
{
   strncpy(dst, str+so, eo-so);
   dst[eo-so] = '\0';
   return dst;
}

int create_list(symbol_list_t *list, int starting_size)
{
   list->symbols = calloc(sizeof(symbolic_value_t), starting_size);
   if (!list->symbols)
   {
      perror("calloc");
      return -1;
   }
   list->symbol_count = 0;
   list->max_symbols = starting_size;
   return 0;
}

int add_symbol(symbol_list_t *list, const char *name, int value)
{
   if (list->symbol_count == list->max_symbols)
   {
      list->max_symbols *= 2;
      symbolic_value_t *tmp = realloc(list->symbols, list->max_symbols*sizeof(symbolic_value_t));
      if (tmp == NULL)
         return -1;
      list->symbols = tmp;
   }
   symbolic_value_t *s = &list->symbols[list->symbol_count];
   strncpy(s->symbol, name, MAX_TOKEN_LENGTH-1);
   s->symbol[MAX_TOKEN_LENGTH-1] = '\0';
   s->value = value;
   list->symbol_count++;
   return 0;
}

/* Returns the value of the symbol. Warning, returns -1 if not found
   but as it is, symbols can not have negative values (because of the
   regexp used to parse)
*/
int get_symbol_value(symbol_list_t *list, const char *name)
{
   int i;
   for (i = 0 ; i < list->symbol_count ; ++i)
   {
      symbolic_value_t *s = &list->symbols[i];
      if (strncmp(s->symbol, name, MAX_TOKEN_LENGTH) == 0)
         return s->value;
   }
   return -1;
}


void free_list(symbol_list_t *list)
{
   free(list->symbols);
}

void display_symbol_list(symbol_list_t *list)
{
   int i;
   for (i = 0 ; i < list->symbol_count ; ++i)
   {
      symbolic_value_t *s = &list->symbols[i];
      printf("%s = %d\n", s->symbol, s->value);
   }
}

int parse_config_file(const char *path)
{
   FILE *f;

   f = fopen(path, "r");
   if (!f)
   {
      perror(path);
      return -1;
   }

   symbol_list_t symbols;
   if (create_list(&symbols, 10) == -1)
   {
      fclose(f);
      return -1;
   }

   /* Build regex for parsing lines */
   regex_t assignments;
   regex_t mappings;

   if (regcomp(&assignments, "([[:alnum:]_]+)[[:space:]]*=[[:space:]]*([[:digit:]]*)", REG_EXTENDED) != 0)
   {
      perror("regcomp(assignments)");
      free_list(&symbols);
      fclose(f);
      return -1;
   }

   if (regcomp(&mappings, "([[:alnum:]_]+)\\(([[:alnum:][:space:],_]+)\\)[[:space:]]*->[[:space:]]*([[:alnum:]_]+)\\(([[:alnum:][:space:],_]+)\\)", REG_EXTENDED) != 0)
   {
      perror("regcomp(mappings)");
      free_list(&symbols);
      fclose(f);
      return -1;
   }

   
   char buffer[1024]; /* 1024 bytes lines should be large enough :p */

   int line = 0;
   while (fgets(buffer, sizeof(buffer), f))
   {
      char *ptr = chomp(buffer);
      ++line;
      if (*ptr == '\0')
         continue;
      regmatch_t pmatch[5];
      if (regexec(&assignments, ptr, 3, pmatch, 0) == 0)
      {
         char symbol[256], value[256];
         subcpy(symbol, ptr, pmatch[1].rm_so, pmatch[1].rm_eo);
         subcpy(value, ptr, pmatch[2].rm_so, pmatch[2].rm_eo);
         printf("New symbol %s with value %s\n", symbol, value);
         add_symbol(&symbols, symbol, atoi(value));
         continue;
      }

      if (regexec(&mappings, ptr, 5, pmatch, 0) == 0)
      {
         char event[256], event_param[256], action[256], action_param[256];
         subcpy(event, ptr, pmatch[1].rm_so, pmatch[1].rm_eo);
         subcpy(event_param, ptr, pmatch[2].rm_so, pmatch[2].rm_eo);
         subcpy(action, ptr, pmatch[3].rm_so, pmatch[3].rm_eo);
         subcpy(action_param, ptr, pmatch[4].rm_so, pmatch[4].rm_eo);
         printf("New mapping: %s(%s) -> %s(%s)\n", event, event_param, action, action_param);
         continue;
      }
      fprintf(stderr, "Warning: possible bogus line: %s:%d\n", path, line);
   }
   display_symbol_list(&symbols);
   regfree(&assignments);
   regfree(&mappings);
   free_list(&symbols);
   fclose(f);
   return 0;
}
