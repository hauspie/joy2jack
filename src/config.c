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


/* Strips comments and ending space charaters and returns a pointer on
 * the first non space charater.
 * DOES modify the string!
 */
static char *chomp(char *buffer)
{
   /* strip comments */
   char *ptr = strchr(buffer, '#');
   if (ptr)
      *ptr = '\0';
   if (buffer[0] == '\0')
      return buffer;
   ptr = buffer + strlen(buffer) - 1;
   /* strip trailing white spaces */
   while (isspace(*ptr) && ptr != buffer)
      --ptr;
   if (ptr == buffer)
   {
      *ptr = '\0';
      return buffer;
   }
   ptr[1] = '\0';
   /* strip starting white spaces */
   ptr = buffer;
   while (*ptr && isspace(*ptr))
      ptr++;
   return ptr;
}

static char *subcpy(char *dst, const char *str, int so, int eo)
{
   strncpy(dst, str+so, eo-so);
   dst[eo-so] = '\0';
   return dst;
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

   int symbolic_values_count = 10;
   symbolic_value_t *symbolics = calloc(sizeof(symbolic_value_t), symbolic_values_count);
   if (!symbolics)
   {
      perror("calloc");
      fclose(f);
      return -1;
   }

   /* Build regex for parsing lines */
   regex_t assignments;
   regex_t mappings;

   if (regcomp(&assignments, "([[:alnum:]_]+)[[:space:]]*=[[:space:]]*([[:digit:]]*)", REG_EXTENDED) != 0)
   {
      perror("regcomp(assignments)");
      free(symbolics);
      fclose(f);
      return -1;
   }

   if (regcomp(&mappings, "([[:alnum:]_]+)\\(([[:alnum:][:space:],_]+)\\)[[:space:]]*->[[:space:]]*([[:alnum:]_]+)\\(([[:alnum:][:space:],_]+)\\)", REG_EXTENDED) != 0)
   {
      perror("regcomp(mappings)");
      free(symbolics);
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
   regfree(&assignments);
   regfree(&mappings);
   free(symbolics);
   fclose(f);
   return 0;
}
