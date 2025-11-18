#include "rktest.h"
#include "../ini.h"



#include <assert.h>
#include <math.h>
#include <stdlib.h>



// If you don't want some nasty bugs, call these appropriately at the beginning
// of each TEST
static void start_stack_use();
static void end_stack_use();



TEST(ini_tests, file_parsing)
{
    const char contents[] = "[Section1]\n"
                            "hello=world\n"
                            "[Section2]\n"
                            "boolean=true\n"
                            "integer=5\n"
                            "string=\"is a string\"\n"
                            "float=1.0\n";

    FILE *file = tmpfile();
    assert(file);
    fputs(contents, file);
    rewind(file);
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, NULL) != NULL);
    fclose(file);
    ASSERT_STREQ(ini_get_value(data, "Section1", "hello"), "world");
    ASSERT_TRUE(ini_get_bool(data, "Section2", "boolean", false));
    ASSERT_EQ(ini_get_signed(data, "Section2", "integer", 0), 5);
    ASSERT_EQ(ini_get_float(data, "Section2", "float", INFINITY), 1.0f);
    ini_free_data(data);
}







TEST(ini_tests, file_writing)
{
    const char contents[] = "[section]\n"
                            "hello=world\n"
                            "hi=true\n"
                            "val=5\n"
                            "this: is a string"
                            "this_one=\"is a string with   consecutive spaces\"\n";

    FILE *input_file = tmpfile();
    assert(input_file);
    fputs(contents, input_file);
    rewind(input_file);

    INIData_t *data = ini_create_data();
    ini_read_file(input_file, data, NULL);

    FILE *output_file = tmpfile();
    ini_write_file(output_file, data);
    rewind(output_file);

    INIData_t *copy = ini_create_data();
    ini_read_file(output_file, copy, NULL);
    ASSERT_TRUE(copy != NULL);
    ASSERT_TRUE(copy->sections != NULL);
    ASSERT_EQ(data->section_count, copy->section_count);

    for (unsigned i = 0; i < data->section_count; i++)
    {
        const INISection_t *section = &data->sections[i];
        for (unsigned j = 0; j < section->pair_count; j++)
        {
            const char *key = section->pairs[j].key;
            const char *value = section->pairs[j].value;

            ASSERT_STREQ(value, ini_get_value(copy, section->name, key));

        }
    }

    ini_free_data(data);
    ini_free_data(copy);
    fclose(input_file);
    fclose(output_file);
}

TEST(ini_tests, stack)
{
    start_stack_use();

    const char contents[] = "[Section]\n"
                            "key=value\n"
                            "other_key=other_value\n"
                            "[OtherSection]\n"
                            "final_key=final_value\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);

    const int max_sections = 32;
    const int max_pairs = 32;

    INISection_t sections[max_sections];
    INIPair_t pairs[max_sections][max_pairs];
    INIPair_t *row_ptrs[max_sections];
    for (int i = 0; i < max_sections; i++)
        row_ptrs[i] = pairs[i];
    INIData_t ini;
    ini_init_data(&ini, sections, row_ptrs, max_sections, max_pairs);

    ASSERT_TRUE(ini_read_file(file, &ini, NULL) != NULL);
    fclose(file);

    ASSERT_STREQ(ini_get_string(&ini, "Section", "key", ""), "value");
    ASSERT_STREQ(ini_get_string(&ini, "Section", "other_key", ""), "other_value");
    ASSERT_STREQ(ini_get_string(&ini, "OtherSection", "final_key", ""), "final_value");

    end_stack_use();
}



TEST(ini_tests, stack_insufficient_pair_allocation)
{
    start_stack_use();

    const char contents[] = "[Section]\n"
                            "key=value\n"
                            "other_key=other_value\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);

    const int max_sections = 8;
    const int max_pairs = 1;

    INISection_t sections[max_sections];
    INIPair_t pairs[max_sections][max_pairs];
    INIPair_t *row_ptrs[max_sections];
    for (int i = 0; i < max_sections; i++)
        row_ptrs[i] = pairs[i];
    INIData_t ini;
    ini_init_data(&ini, sections, row_ptrs, max_sections, max_pairs);

    ASSERT_TRUE(ini_read_file(file, &ini, NULL) == NULL);
    fclose(file);

    end_stack_use();
}



TEST(ini_tests, stack_insufficient_section_allocation)
{
    start_stack_use();

    const char contents[] = "[Section]\n"
                            "key=value\n"
                            "[OtherSection]\n"
                            "other_key=other_value\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);

    const int max_sections = 1;
    const int max_pairs = 2;

    INISection_t sections[max_sections];
    INIPair_t pairs[max_sections][max_pairs];
    INIPair_t *row_ptrs[max_sections];
    for (int i = 0; i < max_sections; i++)
        row_ptrs[i] = pairs[i];
    INIData_t ini;
    ini_init_data(&ini, sections, row_ptrs, max_sections, max_pairs);

    ASSERT_TRUE(ini_read_file(file, &ini, NULL) == NULL);
    fclose(file);

    end_stack_use();
}



static void end_stack_use()
{
    ini_set_allocator(malloc);
    ini_set_free(free);
    ini_set_reallocator(realloc);
}



static void start_stack_use()
{
    ini_disable_heap();
}