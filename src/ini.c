#include "ini.h"



#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define INITIAL_ALLOCATED_PAIRS 32
#define INITIAL_ALLOCATED_SECTIONS 8



static void set_parse_error_(INIData_t *data, const char *line, const char *msg)
{
    assert(data);
    if (!data || data->error.offset < 0) return;

    data->error.encountered = true;
    memset(data->error.line, 0, sizeof(data->error.line));
    strncpy(data->error.line, line, strnlen(line, INI_MAX_LINE_SIZE));
    memset(data->error.msg, 0, sizeof(data->error.msg));
    strncpy(data->error.msg, msg, strnlen(msg, INI_MAX_LINE_SIZE));
}



static void free_data_sections_(INIData_t *data)
{
    if (data)
    {
        if (data->sections)
        {
            for (int i = 0; i < data->section_count; i++)
                if (data->sections[i].pairs)
                    free(data->sections[i].pairs);
            free(data->sections);
        }
        data->sections = NULL;
    }
}




INIData_t *ini_parse_file(FILE *file)
{
    if (!file) return NULL;

    INIData_t *data = malloc(sizeof(INIData_t));
    assert(data);
    if (!data) return NULL;

    data->error.encountered = false;
    memset(data->error.line, 0, sizeof(data->error.line));
    data->error.offset = 0;
    data->section_count = 0;
    data->section_allocation = INITIAL_ALLOCATED_SECTIONS;
    data->sections = malloc(sizeof(INISection_t) * data->section_allocation);

    char line[INI_MAX_LINE_SIZE];
    INISection_t *current_section = NULL;
    while (fgets(line, INI_MAX_LINE_SIZE, file))
    {
        // Blank line?
        if (ini_is_blank_line(line)) continue;

        // Pair?
        INIPair_t pair;
        if (ini_parse_pair(line, &pair, &data->error.offset))
        {
            if (!current_section)
            {
                set_parse_error_(data, line, "Pairs must reside within a section.");
                goto parse_failure;
            }
            ini_add_pair_to_section(current_section, pair);
            continue;
        }

        // It's not a pair... is it a section?
        if (line[data->error.offset] != '[')
        {
            set_parse_error_(data, line, "Failed to parse pair.");
            goto parse_failure;
        }

        // It's a section... but is it valid?
        INISection_t dest_section;
        if (ini_parse_section(line, &dest_section, &data->error.offset))
        {
            const INISection_t *existing_section = ini_has_section(data, dest_section.name);
            if (existing_section)
            {
                char buffer[INI_MAX_LINE_SIZE];
                snprintf(buffer, INI_MAX_LINE_SIZE, "Duplicate section '%s'.", dest_section.name);
                set_parse_error_(data, line, buffer);
                goto parse_failure;
            }
            current_section = ini_add_section(data, dest_section.name);
            continue;
        }

        // It's not a valid section
        set_parse_error_(data, line, "Failed to parse section.");
        goto parse_failure;
    }
    return data;

    parse_failure:
    free_data_sections_(data);
    return data;

}



static bool contains_spaces_(const char *str)
{
    while (*str++ != '\0')
        if (*str == ' ') return true;
    return false;
}



void ini_write_file(const INIData_t *data, FILE *file)
{
    assert(data);
    assert(file);
    if (!data || !file) return;

    for (int i = 0; i < data->section_count; i++)
    {
        const INISection_t *section = &data->sections[i];
        fprintf(file, "[%s]\n", section->name);
        for (int j = 0; j < section->pair_count; j++)
        {
            if (contains_spaces_(section->pairs[j].value))
                fprintf(file, "%s=\"%s\"\n", section->pairs[j].key, section->pairs[j].value);
            else
                fprintf(file, "%s=%s\n", section->pairs[j].key, section->pairs[j].value);
        }
    }
}



INISection_t *ini_has_section(const INIData_t *data, const char *section)
{
    if (!data || !section || !data->sections) return NULL;
    static INIData_t *cached_data = NULL;
    static INISection_t *cached = NULL;
    if (cached && cached_data)
        if (data == cached_data && strncmp(section, cached->name, INI_MAX_STRING_SIZE) == 0)
            return cached;
    for (int i = 0; i < data->section_count; i++)
        if (strncmp(section, data->sections[i].name, INI_MAX_STRING_SIZE) == 0)
        {
            cached = &data->sections[i];
            return cached;
        }
    return NULL;
}



void ini_section_init(const char *name, INISection_t *section)
{
    assert(section);
    if (!section) return;
    memset(section->name, 0, INI_MAX_STRING_SIZE);
    strncpy(section->name, name, INI_MAX_STRING_SIZE - 1);
    section->pair_count = 0;
    section->pairs = malloc(sizeof(INIPair_t) * INITIAL_ALLOCATED_PAIRS);
    section->pair_allocation = INITIAL_ALLOCATED_PAIRS;
}



INISection_t *ini_add_section(INIData_t *data, const char *name)
{
    if (ini_has_section(data, name)) return NULL;

    if (data->section_count >= data->section_allocation)
    {
        data->section_allocation *= 2;
        INISection_t *re = realloc(data->sections, sizeof(INISection_t) * data->section_allocation);
        if (!re) return NULL;
        data->sections = re;
    }
    INISection_t *section = &data->sections[data->section_count++];
    ini_section_init(name, section);
    return section;
}



INIPair_t *ini_add_pair(INIData_t *data, const char *section, const INIPair_t pair)
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
        INIPair_t *re = realloc(section->pairs, sizeof(INIPair_t) * section->pair_allocation);
        if (!re)
            return NULL;
        section->pairs = re;
    }

    INIPair_t *new_pair = &section->pairs[section->pair_count++];
    *new_pair = pair;
    return new_pair;
}



const char *ini_get_value(const INIData_t *data, const char *section, const char *key)
{
    assert(data);
    assert(data->sections);
    assert(section);
    assert(key);

    if (!data || !section || !key || !data->sections) return NULL;

    INISection_t *found_section = NULL;
    for (int i = 0; i < data->section_count; i++)
    {
        if (strncmp(data->sections[i].name, section, INI_MAX_STRING_SIZE) == 0)
        {
            found_section = &data->sections[i];
            break;
        }
    }

    if (!found_section) return NULL;

    for (int i = 0; i < found_section->pair_count; i++)
    {
        if (strncmp(found_section->pairs[i].key, key, INI_MAX_STRING_SIZE) == 0)
            return found_section->pairs[i].value;
    }

    return NULL;
}



const char *ini_get_string(INIData_t *data, const char *section, const char *key, const char *default_value)
{
    const char *str = ini_get_value(data, section, key);
    if (!str) return default_value;
    return str;
}



unsigned long long ini_get_unsigned(const INIData_t *data, const char *section, const char *key, unsigned long long default_value)
{
    const char *str = ini_get_value(data, section, key);
    if (!str) return default_value;

    char *end = NULL;
    const unsigned long long value = strtoull(str, &end, 10);
    if (end == str) return default_value;
    return value;
}



long long ini_get_signed(const INIData_t *data, const char *section, const char *key, long long default_value)
{
    const char *str = ini_get_value(data, section, key);
    if (!str) return default_value;

    char *end = NULL;
    const long long value = strtoll(str, &end, 10);
    if (end == str) return default_value;
    return value;
}



long double ini_get_float(const INIData_t *data, const char *section, const char *key, long double default_value)
{
    const char *str = ini_get_value(data, section, key);
    if (!str) return default_value;

    char *end = NULL;
    const long double value = strtold(str, &end);
    if (end == str) return default_value;
    return value;
}



bool ini_get_bool(const INIData_t *data, const char *section, const char *key, bool default_value)
{
    const char *str = ini_get_value(data, section, key);
    if (!str) return default_value;
    if (strcmp(str, "true") == 0) return true;
    if (strcmp(str, "false") == 0) return false;
    return default_value;
}



void ini_free(INIData_t *data)
{
    if (!data) return;
    free_data_sections_(data);
    free(data);
}



static const char *skip_ignored_characters_(const char *c)
{
    while (isspace(*c)) c++;
    if (*c == ';' || *c == '#')
        while (*c != '\0') c++;
    return c;
}



// Assumes line is null-terminated.
bool ini_is_blank_line(const char *line)
{
    const char *c = skip_ignored_characters_(line);
    return *c == '\0';
}



static bool is_valid_section_starting_character_(const char c)
{
    return (isalpha( (unsigned char) c)) || c == '_';
}



static bool is_valid_section_character_(const char c)
{
    return (isalnum((unsigned char)c)) || c == '_';
}



// Assumes line is null-terminated.
bool ini_parse_section(const char *line, INISection_t *section, ptrdiff_t *error_offset)
{
    assert(line);
    if (!line) return false;

    const char *c = line;
    if (error_offset) *error_offset = 0;

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
    if (error_offset) *error_offset = c - line;
    if (section)
        section->name[0] = '\0';
    return false;
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



// Assumes line is null-terminated.
bool ini_parse_pair(const char *line, INIPair_t *pair, ptrdiff_t *error_offset)
{
    assert(line);
    if (!line) return false;

    if (error_offset) *error_offset = 0;
    const char *c = line;
    char *dest_c = NULL;

    if (pair)
    {
        memset(pair->key, 0, sizeof(pair->key));
        dest_c = pair->key;
    }
    c = skip_ignored_characters_(c);


    // Key
    if (!is_valid_key_starting_value_(*c)) goto is_not_pair;
    while (is_valid_key_character_(*c))
    {
        if (dest_c)
        {
            if (dest_c - pair->key >= INI_MAX_STRING_SIZE) return false;
            *dest_c++ = *c;
        }
        c++;
    }
    if (dest_c) *dest_c = '\0';

    c = skip_ignored_characters_(c);
    if (*c != '=') goto is_not_pair;
    c++;
    c = skip_ignored_characters_(c);

    if (pair)
    {
        memset(pair->value, 0, sizeof(pair->value));
        dest_c = pair->value;
    }


    // value
    bool quoted = (*c == '"');
    if (quoted) c++;

    while (is_valid_value_character_(*c, quoted))
    {
        if (dest_c)
        {
            if (dest_c - pair->value >= INI_MAX_STRING_SIZE) return false;
            *dest_c++ = *c;
        }
        c++;
    }

    if (quoted)
    {
        if (*c != '"') goto is_not_pair;
        c++;
    }

    if (dest_c) *dest_c = '\0';
    c = skip_ignored_characters_(c);

    if (*c != '\0')
        goto is_not_pair;

    return true;

    is_not_pair:
    if (error_offset) *error_offset = c - line;
    if (pair)
    {
        pair->key[0] = '\0';
        pair->value[0] = '\0';
    }
    return false;
}
