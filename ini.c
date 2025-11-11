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



#include "ini.h"



#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define INITIAL_ALLOCATED_PAIRS 32
#define INITIAL_ALLOCATED_SECTIONS 8



static void *(*ini_malloc_) (size_t) = INI_DEFAULT_ALLOC;
static void (*ini_free_) (void *) = INI_DEFAULT_FREE;
static void *(*ini_realloc_) (void *, size_t) = INI_DEFAULT_REALLOC;



// Static helpers
static void set_parse_error_(INIError_t *error, const char *line, ptrdiff_t offset, const char *msg);
static void clear_parse_error_(INIError_t *error);
static void init_data_(INIData_t *data);
static bool contains_spaces_(const char *str);
static void section_init_(const char *name, INISection_t *section);
static const char *skip_ignored_characters_(const char *c);
static bool is_valid_section_starting_character_(char c);
static bool is_valid_section_character_(char c);
static bool is_valid_key_starting_value_(char c);
static bool is_valid_key_character_(char c);
static bool is_valid_value_character_(char c, bool quoted);



void ini_set_allocator(void *(*allocator) (size_t))
{
    ini_malloc_ = allocator;
}



void ini_set_free(void (*deallocator) (void *))
{
    ini_free_ = deallocator;
}



void ini_set_reallocator(void *(*reallocator) (void *,size_t))
{
    ini_realloc_ = reallocator;
}



INIData_t *ini_read_file(FILE *file, INIError_t *error)
{
    if (!file) return NULL;
    clear_parse_error_(error);

    INIData_t *data = ini_malloc_(sizeof(INIData_t));
    assert(data);
    if (!data) return NULL;
    init_data_(data);

    char line[INI_MAX_LINE_SIZE];
    INISection_t *current_section = NULL;
    while (fgets(line, INI_MAX_LINE_SIZE, file))
    {
        // Blank line?
        if (ini_is_blank_line(line)) continue;

        ptrdiff_t discrepency_offset = 0;

        // Pair?
        INIPair_t pair;
        if (ini_parse_pair(line, &pair, &discrepency_offset))
        {
            if (!current_section)
            {
                set_parse_error_(error, line, discrepency_offset, "Pairs must reside within a section.");
                goto parse_failure;
            }
            ini_add_pair_to_section(current_section, pair);
            continue;
        }

        // It's not a pair... is it a section?
        if (line[discrepency_offset] != '[')
        {
            set_parse_error_(error, line, discrepency_offset, "Failed to parse pair.");
            goto parse_failure;
        }

        // It's a section... but is it valid?
        INISection_t dest_section;
        if (ini_parse_section(line, &dest_section, &discrepency_offset))
        {
            const INISection_t *existing_section = ini_has_section(data, dest_section.name);
            if (existing_section)
            {
                char buffer[INI_MAX_LINE_SIZE];
                snprintf(buffer, INI_MAX_LINE_SIZE, "Duplicate section '%s'.", dest_section.name);
                set_parse_error_(error, line, discrepency_offset, buffer);
                goto parse_failure;
            }
            current_section = ini_add_section(data, dest_section.name);
            continue;
        }

        // It's not a valid section
        set_parse_error_(error, line, discrepency_offset, "Failed to parse section.");
        goto parse_failure;
    }

    goto done;
    parse_failure:
    ini_free_data(data);
    return NULL;

    done:
    return data;
}



void ini_write_file(const INIData_t *data, FILE *file)
{
    assert(data);
    assert(file);
    if (!data || !file) return;

    for (unsigned i = 0; i < data->section_count; i++)
    {
        const INISection_t *section = &data->sections[i];
        fprintf(file, "[%s]\n", section->name);
        for (unsigned j = 0; j < section->pair_count; j++)
        {
            if (contains_spaces_(section->pairs[j].value))
                fprintf(file, "%s=\"%s\"\n", section->pairs[j].key, section->pairs[j].value);
            else
                fprintf(file, "%s=%s\n", section->pairs[j].key, section->pairs[j].value);
        }
    }
}



INISection_t *ini_add_section(INIData_t *data, const char *name)
{
    if (ini_has_section(data, name)) return NULL;

    if (data->section_count >= data->section_allocation)
    {
        data->section_allocation *= 2;
        INISection_t *re = ini_realloc_(data->sections, sizeof(INISection_t) * data->section_allocation);
        if (!re) return NULL;
        data->sections = re;
    }
    INISection_t *section = &data->sections[data->section_count++];
    section_init_(name, section);
    return section;
}



INIPair_t *ini_add_pair(const INIData_t *data, const char *section, const INIPair_t pair)
{
    INISection_t *existing_section = ini_has_section(data, section);
    if (!existing_section) return NULL;
    return ini_add_pair_to_section(existing_section, pair);
}



INIPair_t *ini_add_pair_to_section(INISection_t *section, const INIPair_t pair)
{
    assert(section);
    if (!section) return NULL;

    if (section->pair_count >= section->pair_allocation)
    {
        section->pair_allocation *= 2;
        INIPair_t *re = ini_realloc_(section->pairs, sizeof(INIPair_t) * section->pair_allocation);
        if (!re)
            return NULL;
        section->pairs = re;
    }

    INIPair_t *new_pair = &section->pairs[section->pair_count++];
    *new_pair = pair;
    return new_pair;
}



INISection_t *ini_has_section(const INIData_t *data, const char *section)
{
    if (!data || !section || !data->sections) return NULL;
    static INIData_t *cached_data = NULL;
    static INISection_t *cached = NULL;
    if (cached && cached_data)
        if (data == cached_data && strncmp(section, cached->name, INI_MAX_STRING_SIZE) == 0)
            return cached;
    for (unsigned i = 0; i < data->section_count; i++)
        if (strncmp(section, data->sections[i].name, INI_MAX_STRING_SIZE) == 0)
        {
            cached = &data->sections[i];
            return cached;
        }
    return NULL;
}



const char *ini_get_value(const INIData_t *data, const char *section, const char *key)
{
    assert(data);
    assert(data->sections);
    assert(section);
    assert(key);

    if (!data || !section || !key || !data->sections) return NULL;

    INISection_t *found_section = NULL;
    for (unsigned i = 0; i < data->section_count; i++)
    {
        if (strncmp(data->sections[i].name, section, INI_MAX_STRING_SIZE) == 0)
        {
            found_section = &data->sections[i];
            break;
        }
    }

    if (!found_section) return NULL;

    for (unsigned i = 0; i < found_section->pair_count; i++)
    {
        if (strncmp(found_section->pairs[i].key, key, INI_MAX_STRING_SIZE) == 0)
            return found_section->pairs[i].value;
    }

    return NULL;
}



const char *ini_get_string(const INIData_t *data, const char *section, const char *key, const char *default_value)
{
    const char *str = ini_get_value(data, section, key);
    if (!str) return default_value;
    return str;
}



unsigned long long ini_get_unsigned(const INIData_t *data, const char *section, const char *key, const unsigned long long default_value)
{
    const char *str = ini_get_value(data, section, key);
    if (!str) return default_value;

    char *end = NULL;
    const unsigned long long value = strtoull(str, &end, 10);
    if (end == str) return default_value;
    return value;
}



long long ini_get_signed(const INIData_t *data, const char *section, const char *key, const long long default_value)
{
    const char *str = ini_get_value(data, section, key);
    if (!str) return default_value;

    char *end = NULL;
    const long long value = strtoll(str, &end, 10);
    if (end == str) return default_value;
    return value;
}



long double ini_get_float(const INIData_t *data, const char *section, const char *key, const long double default_value)
{
    const char *str = ini_get_value(data, section, key);
    if (!str) return default_value;

    char *end = NULL;
    const long double value = strtold(str, &end);
    if (end == str) return default_value;
    return value;
}



bool ini_get_bool(const INIData_t *data, const char *section, const char *key, const bool default_value)
{
    const char *str = ini_get_value(data, section, key);
    if (!str) return default_value;
    if (strcmp(str, "true") == 0) return true;
    if (strcmp(str, "false") == 0) return false;
    return default_value;
}



// Assumes line is null-terminated.
bool ini_is_blank_line(const char *line)
{
    const char *c = skip_ignored_characters_(line);
    return *c == '\0';
}



bool ini_parse_section(const char *line, INISection_t *section, ptrdiff_t *discrepancy)
{
    assert(line);
    if (!line) return false;

    const char *c = line;
    if (discrepancy) *discrepancy = 0;

    char *dest_c = NULL;
    if (section)
    {
        memset(section->name, 0, sizeof(section->name));
        dest_c = section->name;
    }

    c = skip_ignored_characters_(c);
    if (*c != '[') goto is_not_section;
    c++;
    c = skip_ignored_characters_(c);

    if (!is_valid_section_starting_character_(*c)) goto is_not_section;
    while (is_valid_section_character_(*c))
    {
        if (dest_c)
        {
            if (dest_c - section->name >= INI_MAX_STRING_SIZE)
                return false;
            *dest_c++ = *c;
        }
        c++;
    }
    if (dest_c) *dest_c = '\0';

    c = skip_ignored_characters_(c);
    if (*c != ']') goto is_not_section;
    c++;

    c = skip_ignored_characters_(c);
    if (*c != '\0') goto is_not_section;

    return true;

    is_not_section:
    if (discrepancy) *discrepancy = c - line;
    if (section)
        section->name[0] = '\0';
    return false;
}



bool ini_parse_pair(const char *line, INIPair_t *pair, ptrdiff_t *discrepancy)
{
    assert(line);
    if (!line) return false;

    if (discrepancy) *discrepancy = 0;

    if (pair)
        memset(pair->key, 0, sizeof(pair->key));

    char key[INI_MAX_STRING_SIZE];
    if (!ini_parse_key(line, key, INI_MAX_STRING_SIZE, discrepancy)) goto is_not_pair;

    char value[INI_MAX_STRING_SIZE];
    if (!ini_parse_value(line, value, INI_MAX_STRING_SIZE, discrepancy)) goto is_not_pair;

    if (pair)
    {
        memcpy(pair->key, key, INI_MAX_STRING_SIZE);
        memcpy(pair->value, value, INI_MAX_STRING_SIZE);
    }

    return true;

    is_not_pair:
    if (pair)
    {
        pair->key[0] = '\0';
        pair->value[0] = '\0';
    }
    return false;
}



bool ini_parse_key(const char *line, char *dest, const unsigned n, ptrdiff_t *discrepancy)
{
    if (discrepancy) *discrepancy = 0;

    const char *c = line;
    c = skip_ignored_characters_(c);
    const char *beginning = c;
    if (!is_valid_key_starting_value_(*beginning)) goto is_not_key;

    while (is_valid_key_character_(*c))
    {
        if (dest)
        {
            if (dest - beginning >= n) goto is_not_key;
            *dest++ = *c;
        }
        c++;
    }
    if (dest) *dest = '\0';

    c = skip_ignored_characters_(c);
    if (*c == '=') return true;

    is_not_key:
    if (discrepancy) *discrepancy = c - line;
    return false;
}



bool ini_parse_value(const char *line, char *dest, const unsigned n, ptrdiff_t *discrepancy)
{
    if (discrepancy) *discrepancy = 0;

    const char *c = line;

    while (*c != '\0' && *c != '=') c++;
    if (*c != '=') goto is_not_value;
    c++;

    c = skip_ignored_characters_(c);
    const char *beginning = c;

    const bool quoted = (*c == '"');
    if (quoted) c++;

    while (is_valid_value_character_(*c, quoted))
    {
        if (dest)
        {
            if (dest - beginning >= n) goto is_not_value;;
            *dest++ = *c;
        }
        c++;
    }

    if (quoted)
    {
        if (*c != '"') goto is_not_value;
        c++;
    }

    if (dest) *dest = '\0';

    c = skip_ignored_characters_(c);
    if (*c == '\0') return true;

    is_not_value:
    if (discrepancy) *discrepancy = c - line;
    return false;
}



void ini_free_data(INIData_t *data)
{
    if (data)
    {
        if (data->sections)
        {
            for (unsigned i = 0; i < data->section_count; i++)
                if (data->sections[i].pairs)
                    ini_free_(data->sections[i].pairs);
            ini_free_(data->sections);
        }
        ini_free_(data);
    }
}



static void set_parse_error_(INIError_t *error, const char *line, const ptrdiff_t offset, const char *msg)
{
    if (!error || offset < 0) return;

    error->encountered = true;
    error->offset = offset;

    memcpy(error->msg, msg, strnlen(msg, INI_MAX_LINE_SIZE-1)+1);
    memcpy(error->line, line, strnlen(line, INI_MAX_LINE_SIZE-1)+1);
}



static void clear_parse_error_(INIError_t *error)
{
    if (!error) return;
    error->encountered = false;
    memset(error->line, 0, sizeof(error->line));
    memset(error->msg, 0, sizeof(error->msg));
    error->offset = 0;
}



static void init_data_(INIData_t *data)
{
    if (!data) return;
    data->section_count = 0;
    data->section_allocation = INITIAL_ALLOCATED_SECTIONS;
    data->sections = ini_malloc_(sizeof(INISection_t) * data->section_allocation);
}



static bool contains_spaces_(const char *str)
{
    while (*str++ != '\0')
        if (*str == ' ') return true;
    return false;
}



static void section_init_(const char *name, INISection_t *section)
{
    assert(section);
    if (!section) return;
    memset(section->name, 0, INI_MAX_STRING_SIZE);
    strncpy(section->name, name, INI_MAX_STRING_SIZE - 1);
    section->pair_count = 0;
    section->pairs = ini_malloc_(sizeof(INIPair_t) * INITIAL_ALLOCATED_PAIRS);
    section->pair_allocation = INITIAL_ALLOCATED_PAIRS;
}



static const char *skip_ignored_characters_(const char *c)
{
    while (isspace(*c)) c++;
    if (*c == ';' || *c == '#')
        while (*c != '\0') c++;
    return c;
}



static bool is_valid_section_starting_character_(const char c)
{
    return (isalpha( (unsigned char) c)) || c == '_';
}



static bool is_valid_section_character_(const char c)
{
    return (isalnum((unsigned char)c)) || c == '_';
}



static bool is_valid_key_starting_value_(const char c)
{
    return (isalpha( (unsigned char) c)) || c == '_';
}



static bool is_valid_key_character_(const char c)
{
    return (isalnum((unsigned char)c)) || c == '_';
}



static bool is_valid_value_character_(const char c, const bool quoted)
{
    if (isalnum((unsigned char)c)) return true;
    const char valid_special[] = "_-+.,:\'()[]{}\\/";
    for (const char *p = valid_special; *p != '\0'; p++)
        if (c == *p) return true;
    if (quoted && c == ' ') return true;
    return false;
}
