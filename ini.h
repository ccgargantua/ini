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
#include <stdint.h>
#include <stdio.h>



//////////////////////////
// Forward Declarations //
//////////////////////////

/**
 * @brief Represents a key-value pair in an INI section.
 */
typedef struct INIPair_t    INIPair_t;

/**
 * @brief Represents a named INI section containing key-value pairs.
 */
typedef struct INISection_t INISection_t;

/**
 * @brief Represents the full parsed INI dataset.
 */
typedef struct INIData_t    INIData_t;

/**
 * @brief Holds error information produced during parsing.
 */
typedef struct INIError_t   INIError_t;



/* Functions */

// Internals

/**
 * @brief Disable all internal heap allocation calls.
 */
void               ini_disable_heap        (void);

/**
 * @brief Set a custom memory allocator.
 *
 * @param allocator A malloc-compatible allocation function.
 */
void               ini_set_allocator       (void*(*)(size_t));

/**
 * @brief Set a custom memory deallocator.
 *
 * @param deallocator A free-compatible deallocation function.
 */
void               ini_set_free            (void(*)(void*));

/**
 * @brief Set a custom memory reallocator.
 *
 * @param reallocator A realloc-compatible reallocation function.
 */
void               ini_set_reallocator     (void*(*)(void*,    size_t));

// File I/O

/**
 * @brief Parse an INI file from a filesystem path.
 *
 * @param path Path to the INI file.
 * @param data Destination INIData_t structure.
 * @param error Error reporting structure.
 * @param flags Parser behavior flags.
 * @return Pointer to populated INIData_t or NULL on failure.
 */
INIData_t         *ini_read_file_path      (const char*,       INIData_t*,       INIError_t*, uint64_t);

/**
 * @brief Parse an INI file from an open FILE pointer.
 *
 * @param file File pointer to read from.
 * @param data Destination INIData_t structure.
 * @param error Error reporting structure.
 * @param flags Parser behavior flags.
 * @return Pointer to populated INIData_t or NULL on failure.
 */
INIData_t         *ini_read_file_pointer   (FILE*,             INIData_t*,       INIError_t*, uint64_t);

/**
 * @brief Write INI data to a file path.
 *
 * @param path Destination file path.
 * @param data INIData_t to serialize.
 */
void               ini_write_file_path     (const char*,       const INIData_t*);

/**
 * @brief Write INI data to a FILE pointer.
 *
 * @param file Destination file pointer.
 * @param data INIData_t to serialize.
 */
void               ini_write_file_pointer  (FILE*,             const INIData_t*);

// Database insertion

/**
 * @brief Add a new section to an INI dataset.
 *
 * @param data Target dataset.
 * @param name Name of the section.
 * @return Pointer to the new section or NULL.
 */
INISection_t      *ini_add_section         (INIData_t*,        const char*);

/**
 * @brief Add a key-value pair to a named section.
 *
 * @param data Target dataset.
 * @param section Section name.
 * @param pair Key-value pair to insert.
 * @return Pointer to inserted pair or NULL.
 */
INIPair_t         *ini_add_pair            (const INIData_t*,  const char*,      INIPair_t);

/**
 * @brief Add a key-value pair directly to a section.
 *
 * @param section Target section.
 * @param pair Pair to insert.
 * @return Pointer to inserted pair.
 */
INIPair_t         *ini_add_pair_to_section (INISection_t *,    INIPair_t);

// Database query

/**
 * @brief Check if a section exists.
 *
 * @param data INI dataset.
 * @param section Section name.
 * @return Pointer to section or NULL.
 */
INISection_t      *ini_has_section         (const INIData_t*,  const char*);

/**
 * @brief Retrieve a raw string value.
 *
 * @param data INI dataset.
 * @param section Section name.
 * @param key Key name.
 * @return Value string or NULL.
 */
const char        *ini_get_value           (const INIData_t*,  const char*,      const char*);

/**
 * @brief Retrieve a string value with default fallback.
 */
const char        *ini_get_string          (const INIData_t*,  const char*,      const char*, const char*);

/**
 * @brief Retrieve an unsigned integer value.
 */
unsigned long long ini_get_unsigned        (const INIData_t*,  const char*,      const char*, unsigned long long);

/**
 * @brief Retrieve a signed integer value.
 */
long long          ini_get_signed          (const INIData_t*,  const char*,      const char*, long long);

/**
 * @brief Retrieve a hexadecimal integer value.
 */
unsigned long long ini_get_hex             (const INIData_t*,  const char*,      const char*, unsigned long long);

/**
 * @brief Retrieve a floating-point value.
 */
long double        ini_get_float           (const INIData_t*,  const char*,      const char*, long double);

/**
 * @brief Retrieve a boolean value.
 */
bool               ini_get_bool            (const INIData_t*,  const char*,      const char*, bool);

// Parsing helpers

/**
 * @brief Determine whether a line is blank or a comment.
 */
bool               ini_is_blank_line       (const char*);

/**
 * @brief Parse an INI section declaration.
 */
bool               ini_parse_section       (const char*,       INISection_t*,    ptrdiff_t*);

/**
 * @brief Parse a key-value pair.
 */
bool               ini_parse_pair          (const char*,       INIPair_t*,       ptrdiff_t*);

/**
 * @brief Parse a key string.
 */
bool               ini_parse_key           (const char*,       char*,            unsigned,    ptrdiff_t*);

/**
 * @brief Parse a value string.
 */
bool               ini_parse_value         (const char*,       char*,            unsigned,    ptrdiff_t*);

// Heap

/**
 * @brief Allocate and initialize an INIData_t on the heap.
 */
INIData_t         *ini_create_data         (void);

/**
 * @brief Free an INIData_t and all associated resources.
 */
void               ini_free_data           (INIData_t*);

// Stack

/**
 * @brief Initialize INIData_t using caller-provided memory.
 */
void               ini_init_data           (INIData_t*,        INISection_t*,    INIPair_t**, unsigned, unsigned);

#endif //INI_H
