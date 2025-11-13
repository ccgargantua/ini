#include <assert.h>
#include <stdio.h>

#include "../ini.h"

/*
 * There are two examples of using the library in this file. One utilizes the heap,
 * the other uses the stack. If you want to go stackless, make sure to call
 * ini_disable_heap() first!
 */



void heap();
void stack();
void print_error(INIError_t *error);



int main(void)
{
    heap();
    stack();
    return 0;
}



void heap()
{
    FILE *file = fopen("example/example.ini", "r");
    assert(file);

    INIData_t *ini = ini_create_data();

    INIError_t error;
    ini_read_file(file, ini, &error);

    if (error.encountered)
        print_error(&error);
    fclose(file);

    const char *greeting = ini_get_value(ini, "Text", "greeting");
    assert(greeting);

    const char *subject = ini_get_value(ini, "Text", "subject");
    assert(subject);

    bool enthusiastic = ini_get_bool(ini, "Text", "enthusiastic", false);
    const char enthusiasm = enthusiastic ? '!' : '.';

    int iterations = ini_get_signed(ini, "Execution", "iterations", 1);

    for (int i = 0; i < iterations; i++)
        printf("%s, %s%c\n", greeting, subject, enthusiasm);

    const char *pi_string = ini_get_string(ini, "Pi", "string", "pi =");
    float pi = ini_get_float(ini, "Pi", "pi", 3);
    printf("%s %f\n", pi_string, pi);

    ini_free_data(ini);
}



void stack()
{
    // Very important to call this first...
    ini_disable_heap();
    FILE *file = fopen("example/example.ini", "r");
    assert(file);

    const int max_sections = 32;
    const int max_pairs = 32;

    INISection_t sections[max_sections];

    INIPair_t pairs[max_sections][max_pairs];
    for (int i = 0; i < max_sections; i++)
    {
        sections[i].pair_allocation = max_pairs;
        sections[i].pairs = pairs[i];
    }

    INIData_t ini = {
        .sections = sections,
        .section_count = 0,
        .section_allocation = max_sections
        };

    INIError_t error;
    ini_read_file(file, &ini, &error);
    if (error.encountered)
        print_error(&error);
    fclose(file);

    const char *greeting = ini_get_value(&ini, "Text", "greeting");
    assert(greeting);

    const char *subject = ini_get_value(&ini, "Text", "subject");
    assert(subject);

    bool enthusiastic = ini_get_bool(&ini, "Text", "enthusiastic", false);
    const char enthusiasm = enthusiastic ? '!' : '.';

    int iterations = ini_get_signed(&ini, "Execution", "iterations", 1);

    for (int i = 0; i < iterations; i++)
        printf("%s, %s%c\n", greeting, subject, enthusiasm);

    const char *pi_string = ini_get_string(&ini, "Pi", "string", "pi =");
    float pi = ini_get_float(&ini, "Pi", "pi", 3);
    printf("%s %f\n", pi_string, pi);
}



void print_error(INIError_t *error)
{
    char buffer[error->offset+1];
    int i = 0;
    for (i = 0; i < error->offset; i++)
        buffer[i] = ' ';
    buffer[i++] = '^';
    buffer[i] = '\0';
    printf("%s\n%s\n%s\n", error->msg, error->line, buffer);
}