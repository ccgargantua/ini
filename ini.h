#ifndef INI_H
#define INI_H



/*
 *   MIT License
 *
 *   Copyright (c) 2025 Carter Dugan
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */



#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>



//////////////////////////
// Forward Declarations //
//////////////////////////

/* Structs */

typedef struct INIPair_t    INIPair_t;
typedef struct INISection_t INISection_t;
typedef struct INIData_t    INIData_t;
typedef struct INIError_t   INIError_t;



/* Functions */

INIData_t         *ini_parse_file          (FILE*, INIError_t*);
void               ini_write_file          (const INIData_t*, FILE*);
INISection_t      *ini_has_section         (const INIData_t*, const char*);
void               ini_section_init        (const char*,      INISection_t*);
INISection_t      *ini_add_section         (INIData_t*,       const char*);
INIPair_t         *ini_add_pair            (INIData_t*,       const char*,   INIPair_t);
INIPair_t         *ini_add_pair_to_section (INISection_t *,   INIPair_t );
const char        *ini_get_value           (const INIData_t*, const char*,   const char*);
unsigned long long ini_get_unsigned        (const INIData_t*, const char*,   const char*, unsigned long long);
long long          ini_get_signed          (const INIData_t*, const char*,   const char*, long long);
long double        ini_get_float           (const INIData_t*, const char*,   const char*, long double);
bool               ini_get_bool            (const INIData_t*, const char*,   const char*, bool);
void               ini_free                (INIData_t*);
bool               ini_is_blank_line       (const char*);
bool               ini_parse_pair          (const char*,      INIPair_t*,    ptrdiff_t*);
bool               ini_parse_section       (const char*,      INISection_t*, ptrdiff_t*);
bool               ini_parse_key           (const char*,      char*,         unsigned,    ptrdiff_t*);
bool               ini_parse_value         (const char*,      char*,         unsigned,    ptrdiff_t*);



/* Macros */

#define INI_MAX_STRING_SIZE 256
#define INI_MAX_LINE_SIZE   1024



////////////////////////
// Struct Definitions //
////////////////////////

/*
 * Key=value pair
 */
struct INIPair_t
{
    char key[INI_MAX_STRING_SIZE];
    char value[INI_MAX_STRING_SIZE];
};



/*
 * [Section]
 *
 * Keeps track of encapsulated pairs, the number of pairs,
 * and the number of allocated pairs.
 */
struct INISection_t
{
    char name[INI_MAX_STRING_SIZE];
    INIPair_t *pairs;
    unsigned pair_count;
    unsigned pair_allocation;
};



/*
 * Data structure for INI contents. Keeps track of
 * sections and the number of sections.
 */
struct INIData_t
{
    INISection_t *sections;
    unsigned section_count;
    unsigned section_allocation;
};



// TODO document
struct INIError_t
{
    bool encountered;
    char msg[INI_MAX_LINE_SIZE];
    char line[INI_MAX_LINE_SIZE];
    char culprit[INI_MAX_LINE_SIZE * 2 + 4]; // two lines, two newlines, two nulls
    ptrdiff_t offset;
};



///////////////////////////
// Function Declarations //
///////////////////////////

/*
 * Parse an ini file and populate a data structure
 * with contents. User will need to free the returned
 * object on their own later on with a call to ini_free()
 *
 * Params:
 *   file   - File to parse
 *   buffer - Line buffer to store erroneous line. Must be
 *
 *
 * Returns:
 *   A pointer to an INIData_t object. Object contains
 *   heap-allocated data. On failure, returns NULL.
 */
INIData_t *ini_parse_file(FILE *file, INIError_t *error);



/*
 * Use the contents of an INIData_t object to generate an
 * INI file (or overwrite an existing one)
 *
 * Params:
 *   data - A pointer to the INIData_t object whose data
 *          you would like to write
 *   file - Destination file pointer.
 */
void ini_write_file(const INIData_t *data, FILE *file);



/*
 * Query for a section object based on the section name.
 *
 * Params:
 *   data    - The INIData_t object that represents an INI file.
 *   section - The name of the section you are checking for.
 *
 * Returns:
 *   A pointer to the located INISection_t object, or NULL if
 *   the section is not found.
 */
INISection_t *ini_has_section(const INIData_t *data, const char *section);



/*
 * Initialize a section with a name, an allocation for pairs,
 * and starting its pair count at 0.
 *
 * Params:
 *   name    - The name of the section.
 *   section - Destination pointer for the setion to be initialized.
 */
void ini_section_init(const char *name, INISection_t *section);



/*
 * Add a section to an INIData_t object by providing the name
 * of the new section. This internally will call ini_section_init()
 * (see above).
 *
 * Param:
 *   data - The INIData_t object that will acquire the new section.
 *   name - The name of the section to be added.
 *
 * Returns:
 *   A pointer to the newly-created section, or NULL on error or
 *   if the section already exists.
 */
INISection_t *ini_add_section(INIData_t *data, const char *name);



/*
 * Add a pair to an INIData_t object by providing the section
 * name and indirectly adding it to the section.
 *
 * Param:
 *   data    - The INIData_t object to add the pair to.
 *   section - The **name** of the section to add the pair
 *             to.
 *   pair    - The pair to be added.
 *
 * Return:
 *   A pointer to the pair after being added to the proper
 *   section within `data`, or NULL on failure (i.e., providing
 *   a name for a section that does not exist in `data`)
 */
INIPair_t *ini_add_pair(INIData_t *data, const char *section, INIPair_t pair);



/*
 * Add a pair directly to a section, agnostic to the parent
 * INIData_t object.
 *
 * Param:
 *   section - The section to acquire the pair.
 *   pair    - A pair object whose data will be copied into
 *             a new pair in the section.
 *
 * Returns:
 *   A pointer to the newly-added pair within the section.
 */
INIPair_t *ini_add_pair_to_section(INISection_t *section, INIPair_t pair);



/*
 * Retrieve a value from an INIData_t object given a section
 * name and a key value.
 *
 * Params:
 *   data    - The INIData_t object to be searched.
 *   section - The section to search for.
 *   key     - The key to search for.
 *
 * Returns:
 *   The value in the form of a null-terminated C-string, or
 *   NULL if not found.
 */
const char *ini_get_value(const INIData_t *data, const char *section, const char *key);



/*
 * Attempt to fetch an string value from INI data given a
 * section and key. If unfound, returns a provided default.
 *
 * Params:
 *   data    - Pointer to the INIData_t object to search
 *   section - The section title being searched for.
 *   key     - The key being searched for.
 *   default - Default value to be used if searched
 *             value is not found or fails to be parsed.
 *
 * Returns:
 *   As said above, returns the searched value or the provided
 *   default if the searched value could not be found or parsing
 *   failed.
 */
const char *ini_get_string(INIData_t *data, const char *section, const char *key, const char *default_value);



/*
 * Attempt to fetch an unsigned integer value from INI data given a
 * section and key. If unfound, returns a provided default.
 *
 * Params:
 *   data    - Pointer to the INIData_t object to search
 *   section - The section title being searched for.
 *   key     - The key being searched for.
 *   default - Default value to be used if searched
 *             value is not found or fails to be parsed.
 *
 * Returns:
 *   As said above, returns the searched value or the provided
 *   default if the searched value could not be found or parsing
 *   failed.
 */
unsigned long long ini_get_unsigned(const INIData_t *data, const char *section, const char *key, unsigned long long default_value);



/*
 * Attempt to fetch a signed integer value from INI data given a
 * section and key. If unfound, returns a provided default.
 *
 * Params:
 *   data    - Pointer to the INIData_t object to search
 *   section - The section title being searched for.
 *   key     - The key being searched for.
 *   default - Default value to be used if searched
 *             value is not found or fails to be parsed.
 *
 * Returns:
 *   As said above, returns the searched value or the provided
 *   default if the searched value could not be found or parsing
 *   failed.
 */
long long ini_get_signed(const INIData_t *data, const char *section, const char *key, long long default_value);



/*
 * Attempt to fetch an floating point value from INI data given a
 * section and key. If unfound, returns a provided default.
 *
 * Params:
 *   data    - Pointer to the INIData_t object to search
 *   section - The section title being searched for.
 *   key     - The key being searched for.
 *   default - Default value to be used if searched
 *             value is not found or fails to be parsed.
 *
 * Returns:
 *   As said above, returns the searched value or the provided
 *   default if the searched value could not be found or parsing
 *   failed.
 */
long double ini_get_float(const INIData_t *data, const char *section, const char *key, long double default_value);



/*
 * Attempt to fetch an boolean from INI data given a section
 * and key. If unfound, returns a provided default.
 *
 * Params:
 *   data    - Pointer to the INIData_t object to search
 *   section - The section title being searched for.
 *   key     - The key being searched for.
 *   default - Default value to be used if searched
 *             value is not found or fails to be parsed.
 *
 * Returns:
 *   As said above, returns the searched value or the provided
 *   default if the searched value could not be found or parsing
 *   failed.
 */
bool ini_get_bool(const INIData_t *data, const char *section, const char *key, bool default_value);



/*
 * Free the memory resources used by an INIData_t object.
 * This should be called if you have created an INIData_t
 * object with ini_parse_file()
 *
 * Params:
 *   data - The INIData_t object to be free'd.
 */
void ini_free(INIData_t *data);



/*
 * A helper function that parses a character array and
 * determines if the array represents a blank line via
 * INI syntax rules (contains only whitespace or comments)
 *
 * Params:
 *   line - The character array to be parsed.
 *
 * Returns:
 *   True if the line is considered blank, false
 *   otherwise.
 */
bool ini_is_blank_line(const char *line);



/*
 * A helper function that parses a character array and
 * attempts to parse a valid section.
 *
 * Params:
 *   line         - The character array to be parsed.
 *   section      - A pointer to a destination section to
 *                  store name strings. If NULL is provided,
 *                  has no effect. If a string is not a valid
 *                  section, then the name string is zero-length
 *                  and null-terminated.
 *   discrepancy  - A pointer to an integer representing the
 *                  offset of the erroneous character if
 *                  present. If no error found, will be given
 *                  0. If NULL, has no effect.
 *
 * Returns:
 *   True if the line is considered a valid section,
 *   false otherwise.
 */
bool ini_parse_section(const char *line, INISection_t *section, ptrdiff_t *discrepancy);



/*
 * A helper function that parses a character array and
 * attempts to parse a valid key.
 *
 * Params:
 *   line         - The character array to be parsed.
 *   dest         - A pointer to a destination buffer to
 *                  store key string. If NULL is provided,
 *                  has no effect. If a string is not a valid
 *                  section, then the string is invalid and
 *                  not guaranteed to be null-terminated.
 *   discrepancy  - A pointer to an integer representing the
 *                  offset of the erroneous character if
 *                  present. If no error found, will be given
 *                  0. If NULL, has no effect.
 *
 * Returns:
 *   True if the line is considered a valid key, false
 *   otherwise.
 */
bool ini_parse_key(const char *line, char *dest, unsigned n, ptrdiff_t *discrepancy);



/*
 * A helper function that parses a character array and
 * attempts to parse a valid value.
 *
 * Params:
 *   line         - The character array to be parsed.
 *   dest         - A pointer to a destination buffer to
 *                  store value string. If NULL is provided,
 *                  has no effect. If a string is not a valid
 *                  section, then the string is invalid and
 *                  not guaranteed to be null-terminated.
 *   discrepancy  - A pointer to an integer representing the
 *                  offset of the erroneous character if
 *                  present. If no error found, will be given
 *                  0. If NULL, has no effect.
 *
 * Returns:
 *   True if the line is considered a valid value, false
 *   otherwise.
 */
bool ini_parse_value(const char *line, char *dest, unsigned n, ptrdiff_t *discrepancy);



/*
 * A helper function that reads a character array and
 * attempts to parse a valid pair.
 *
 * Params:
 *   line         - The character array to be parsed.
 *   pair         - A pointer to a destination pair to store
 *                  key and value strings. If NULL is provided,
 *                  has no effect. If a string is not a valid
 *                  pair, then the key and value strings are
 *                  zero-length and null-terminated.
 *   discrepancy  - A pointer to an integer representing the
 *                  offset of the erroneous character if
 *                  present. If no error found, will be given
 *                  0. If NULL, has no effect.
 *
 * Returns:
 *   True if the line is considered a legal k=v pair,
 *   false otherwise.
 */
bool ini_parse_pair(const char *line, INIPair_t *pair, ptrdiff_t *discrepancy );


#endif //INI_H
