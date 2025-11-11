#include <assert.h>
#include <stdio.h>

#include "../ini.h"

void print_error(INIError_t *error);

int main(void)
{
    FILE *file = fopen("example/example.ini", "r");
    assert(file);

    INIError_t error;
    INIData_t *ini = ini_read_file(file, &error);
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

    return 0;
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