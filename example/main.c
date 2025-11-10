#include <assert.h>
#include <stdio.h>

#include "../src/ini.h"

void print_error(INIData_t *ini);

int main(void)
{
    FILE *file = fopen("example.ini", "r");
    assert(file);

    INIData_t *ini = ini_parse_file(file);
    if (ini->error.encountered)
        print_error(ini);

    const char *greeting = ini_get_value(ini, "Text", "greeting");
    assert(greeting);

    const char *subject = ini_get_value(ini, "Text", "subject");
    assert(subject);

    bool enthusiastic = ini_get_bool(ini, "Text", "enthusiastic", false);
    const char enthusiasm = enthusiastic ? '!' : '.';

    int iterations = ini_get_signed(ini, "Execution", "iterations", 1);

    for (int i = 0; i < iterations; i++)
        printf("%s, %s%c\n", greeting, subject, enthusiasm);

    float pi = ini_get_float(ini, "Pi", "pi", 3);
    printf("Pi with a sufficient number of digits: %f\n", pi);

    return 0;
}

void print_error(INIData_t *ini)
{
    char buffer[ini->error.offset+1];
    int i = 0;
    for (i = 0; i < ini->error.offset; i++)
        buffer[i] = ' ';
    buffer[i++] = '^';
    buffer[i] = '\0';
    printf("%s\n%s\n%s\n", ini->error.msg, ini->error.line, buffer);
}