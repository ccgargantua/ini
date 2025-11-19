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
    INIData_t *ini = ini_create_data();

    INIError_t error;
    ini_read_file("example/example.ini", ini, &error, 0);

    if (error.encountered)
    {
        printf("%s\n%s\n%*s^\n", error.msg, error.line, error.offset, "");
        return;
    }

    const char *greeting = ini_get_value(ini, "Text", "greeting");
    assert(greeting);

    const char *subject = ini_get_string(ini, "Text", "subject", "nobody...");

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

    const int max_sections = 32;
    const int max_pairs = 32;

    // Create our data on the stack
    INISection_t sections[max_sections];
    INIPair_t pairs[max_sections][max_pairs];
    INIPair_t *section_pairs[max_sections];
    for (int i = 0; i < max_sections; i++)
        section_pairs[i] = pairs[i];

    INIData_t ini;
    ini_init_data(&ini, sections, section_pairs, max_sections, max_pairs);

    INIError_t error;
    ini_read_file("example/example.ini", &ini, &error, 0);
    if (error.encountered)
    {
        printf("%s\n%s\n%*s^\n", error.msg, error.line, error.offset, "");
        return;
    }

    const char *greeting = ini_get_value(&ini, "Text", "greeting");
    assert(greeting);

    const char *subject = ini_get_string(&ini, "Text", "subject", "nobody...");

    bool enthusiastic = ini_get_bool(&ini, "Text", "enthusiastic", false);
    const char enthusiasm = enthusiastic ? '!' : '.';

    int iterations = ini_get_signed(&ini, "Execution", "iterations", 1);

    for (int i = 0; i < iterations; i++)
        printf("%s, %s%c\n", greeting, subject, enthusiasm);

    const char *pi_string = ini_get_string(&ini, "Pi", "string", "pi =");
    float pi = ini_get_float(&ini, "Pi", "pi", 3);
    printf("%s %f\n", pi_string, pi);
}
