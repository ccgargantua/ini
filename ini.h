#ifndef INI_H
#define INI_H

/**
 * MIT License
 *
 * Copyright (c) 2025 Carter Dugan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

//////////////////////////
// Forward Declarations //
//////////////////////////

/** Represents a key=value pair in an INI file */
typedef struct INIPair_t    INIPair_t;

/** Represents a [section] in an INI file */
typedef struct INISection_t INISection_t;

/** Represents the full parsed INI data */
typedef struct INIData_t    INIData_t;

/** Represents error information encountered during parsing */
typedef struct INIError_t   INIError_t;

/////////////////
// Internals  //
/////////////////

/**
 * Disables all internal heap allocations.
 * Any allocation attempts will fail.
 */
void ini_disable_heap(void);

/**
 * Sets a custom allocator function.
 *
 * @param allocator malloc-like allocator
 */
void ini_set_allocator(void *(*)(size_t));

/**
 * Sets a custom deallocator function.
 *
 * @param deallocator free-like deallocator
 */
void ini_set_free(void (*)(void*));

/**
 * Sets a custom reallocator function.
 *
 * @param reallocator realloc-like reallocator
 */
void ini_set_reallocator(void *(*)(void*, size_t));

//////////////
// File I/O //
//////////////

/**
 * Reads and parses an INI file from a file path.
 *
 * @param path  Path to the INI file
 * @param data  Destination INIData_t
 * @param error Error output container
 * @param flags Parsing flags
 * @return Pointer to populated INIData_t or NULL on failure
 */
INIData_t *ini_read_file_path(const char*, INIData_t*, INIError_t*, uint64_t);

/**
 * Reads and parses an INI file from a FILE pointer.
 *
 * @param file  File pointer
 * @param data  Destination INIData_t
 * @param error Error output container
 * @param flags Parsing flags
 * @return Pointer to populated INIData_t or NULL on failure
 */
INIData_t *ini_read_file_pointer(FILE*, INIData_t*, INIError_t*, uint64_t);

/**
 * Writes INI data to a file path.
 *
 * @param path Output file path
 * @param data INI data to write
 */
void ini_write_file_path(const char*, const INIData_t*);

/**
 * Writes INI data to a FILE pointer.
 *
 * @param file Output file pointer
 * @param data INI data to write
 */
void ini_write_file_pointer(FILE*, const INIData_t*);

////////////////////////
// Database Insertion //
////////////////////////

/**
 * Adds a new section to the INI data.
 *
 * @param data INIData_t container
 * @param name Section name
 * @return Pointer to new section or NULL on failure
 */
INISection_t *ini_add_section(INIData_t*, const char*);

/**
 * Adds a key=value pair to a section by name.
 *
 * @param data    INIData_t container
 * @param section Section name
 * @param pair    Pair to insert
 * @return Pointer to inserted pair or NULL on failure
 */
INIPair_t *ini_add_pair(const INIData_t*, const char*, INIPair_t);

/**
 * Adds a key=value pair directly to a section.
 *
 * @param section Target section
 * @param pair    Pair to insert
 * @return Pointer to inserted pair
 */
INIPair_t *ini_add_pair_to_section(INISection_t*, INIPair_t);

////////////////////
// Database Query //
////////////////////

/**
 * Checks if a section exists.
 *
 * @param data    INIData_t container
 * @param section Section name
 * @return Pointer to section or NULL if not found
 */
INISection_t *ini_has_section(const INIData_t*, const char*);

/**
 * Retrieves a value string by section and key.
 *
 * @param data    INIData_t container
 * @param section Section name
 * @param key     Key name
 * @return Value string or NULL
 */
const char *ini_get_value(const INIData_t*, const char*, const char*);

/**
 * Retrieves a string value or default.
 */
const char *ini_get_string(const INIData_t*, const char*, const char*, const char*);

/**
 * Retrieves an unsigned integer value or default.
 */
unsigned long long ini_get_unsigned(const INIData_t*, const char*, const char*, unsigned long long);

/**
 * Retrieves a signed integer value or default.
 */
long long ini_get_signed(const INIData_t*, const char*, const char*, long long);

/**
 * Retrieves a hexadecimal value or default.
 */
unsigned long long ini_get_hex(const INIData_t*, const char*, const char*, unsigned long long);

/**
 * Retrieves a floating-point value or default.
 */
long double ini_get_float(const INIData_t*, const char*, const char*, long double);

/**
 * Retrieves a boolean value or default.
 */
bool ini_get_bool(const INIData_t*, const char*, const char*, bool);

/////////////
// Parsing //
/////////////

/**
 * Determines whether a line is blank or a comment.
 */
bool ini_is_blank_line(const char*);

/**
 * Attempts to parse a section header.
 */
bool ini_parse_section(const char*, INISection_t*, ptrdiff_t*);

/**
 * Attempts to parse a key=value pair.
 */
bool ini_parse_pair(const char*, INIPair_t*, ptrdiff_t*);

/**
 * Attempts to parse a key string.
 */
bool ini_parse_key(const char*, char*, unsigned, ptrdiff_t*);

/**
 * Attempts to parse a value string.
 */
bool ini_parse_value(const char*, char*, unsigned, ptrdiff_t*);

//////////
// Heap //
//////////

/**
 * Allocates and initializes a heap-based INIData_t object.
 *
 * @return Pointer to new INIData_t
 */
INIData_t *ini_create_data(void);

/**
 * Frees a heap-allocated INIData_t object.
 *
 * @param data INIData_t to free
 */
void ini_free_data(INIData_t*);

///////////
// Stack //
///////////

/**
 * Initializes an INIData_t object using stack-allocated memory.
 *
 * @param data         INIData_t to initialize
 * @param sections     Section array
 * @param pairs        Pair arrays
 * @param num_sections Number of sections
 * @param num_pairs    Number of pairs per section
 */
void ini_init_data(
    INIData_t* data,
    INISection_t* sections,
    INIPair_t** pairs,
    unsigned num_sections,
    unsigned num_pairs
);

#endif // INI_H
