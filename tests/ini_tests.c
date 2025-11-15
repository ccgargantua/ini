#include "rktest.h"
#include "../ini.h"



#include <assert.h>
#include <math.h>
#include <stdlib.h>



// If you don't want some nasty bugs, call these appropriately at the beginning
// of each TEST
static void use_heap();
static void use_stack();



TEST(ini_tests, blank_lines)
{
    use_heap();

    const char empty_line[] = "";
    const char space_line[] = " ";
    const char multiple_space_line[] = "   ";
    const char tab_line[] = "\t";
    const char new_line[] = "\n";
    const char null_line[] = "\0";
    const char comment_line[] = "; comment here";
    const char alt_comment_line[] = "# and here!";

    ASSERT_TRUE(ini_is_blank_line(empty_line));
    ASSERT_TRUE(ini_is_blank_line(space_line));
    ASSERT_TRUE(ini_is_blank_line(multiple_space_line));
    ASSERT_TRUE(ini_is_blank_line(tab_line));
    ASSERT_TRUE(ini_is_blank_line(new_line));
    ASSERT_TRUE(ini_is_blank_line(null_line));
    ASSERT_TRUE(ini_is_blank_line(comment_line));
    ASSERT_TRUE(ini_is_blank_line(alt_comment_line));
}



TEST(ini_tests, nonblank_line)
{
    use_heap();

    const char line[] = "hello world";
    const char spaces[] = "   hello    world   ";
    const char comment[] = "hello world ; hello world";
    ASSERT_FALSE(ini_is_blank_line(line));
    ASSERT_FALSE(ini_is_blank_line(spaces));
    ASSERT_FALSE(ini_is_blank_line(comment));
}



TEST(ini_tests, keys)
{
    use_heap();

    const char normal[] = "key=value";
    const char spaces[] = " key = value ";
    const char comment[] = "key=value ; comment";
    const char string[] = "key=\"this is a value\"";
    const char caps[] = "KEY=value";
    const char number[] = "key5=value";

    const int n = 256;
    char buffer[n];
    ASSERT_TRUE(ini_parse_key(normal, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key");

    ASSERT_TRUE(ini_parse_key(spaces, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key");

    ASSERT_TRUE(ini_parse_key(comment, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key");

    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key");

    ASSERT_TRUE(ini_parse_key(caps, buffer, n, NULL));
    ASSERT_STREQ(buffer, "KEY");

    ASSERT_TRUE(ini_parse_key(number, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key5");
}



TEST(ini_tests, bad_keys)
{
    use_heap();

    const char number_start[] = "5key=value";
    const char special_start[] = "]key=value";
    const char special_middle[] = "k:ey=value";
    const char two[] = "key key=value";
    const char alone[] = "KEY";

    ASSERT_FALSE(ini_parse_key(number_start, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_key(special_start, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_key(special_middle, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_key(two, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_key(alone, NULL, 0, NULL));


    // Exceed string size
    char key[INI_MAX_STRING_SIZE + 3];
    memset(key, 'a', sizeof(key));
    key[INI_MAX_STRING_SIZE + 1] = '=';
    key[INI_MAX_STRING_SIZE + 2] = '\0';

    char buffer[INI_MAX_STRING_SIZE];
    ASSERT_FALSE(ini_parse_key(key, buffer, INI_MAX_STRING_SIZE, NULL));
}



TEST(ini_tests, values)
{
    use_heap();

    const char normal[] = "key=value";
    const char space[] = "key=value ";
    const char spaces[] = " key = value ";
    const char number[] = "key=5";
    const char alnum[] = "key=5val5";
    const char special[] = "key=~!@$%^&*()_+-{}|\\:'<>?,./";
    const char comment[] = "key=value ; comment";
    const char string[] = "key=this is a value";
    const char quoted_string[] = "key=\"extra   spaces\"";

    const int n = 256;
    char buffer[n];
    ASSERT_TRUE(ini_parse_value(normal, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");

    ASSERT_TRUE(ini_parse_value(space, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");

    ASSERT_TRUE(ini_parse_value(spaces, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");

    ASSERT_TRUE(ini_parse_value(number, buffer, n, NULL));
    ASSERT_STREQ(buffer, "5");

    ASSERT_TRUE(ini_parse_value(alnum, buffer, n, NULL));
    ASSERT_STREQ(buffer, "5val5");

    ASSERT_TRUE(ini_parse_value(special, buffer, n, NULL));
    ASSERT_STREQ(buffer, "~!@$%^&*()_+-{}|\\:'<>?,./");

    ASSERT_TRUE(ini_parse_value(comment, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");

    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "this is a value");

    ASSERT_TRUE(ini_parse_value(quoted_string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "extra   spaces");
}



TEST(ini_tests, bad_values)
{
    use_heap();

    const char two_spaces[] = "key=value  value";
    const char bad_string_0[] = "key=\"the man said \"hello\"\"";
    const char bad_string_1[] = "key=\"the man said hello";
    const char forbidden_0[] = "key=val[ue";
    const char forbidden_1[] = "key=val]ue";
    const char forbidden_2[] = "key=val\nue";
    const char forbidden_3[] = "key=val\rue";
    const char alone[] = "value";

    ASSERT_FALSE(ini_parse_value(two_spaces, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_value(bad_string_0, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_value(bad_string_1, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_value(forbidden_0, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_value(forbidden_1, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_value(forbidden_2, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_value(forbidden_3, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_value(alone, NULL, 0, NULL));

    // Exceed string size
    char value[INI_MAX_STRING_SIZE + 2];
    memset(value, 'a', sizeof(value));
    value[0] = '=';
    value[INI_MAX_STRING_SIZE + 1] = '\0';

    char buffer[INI_MAX_STRING_SIZE];
    ASSERT_FALSE(ini_parse_value(value, buffer, INI_MAX_STRING_SIZE, NULL));
}



TEST(ini_tests, pairs)
{
    use_heap();

    const char line[] = "key=value";
    const char line_spaces[] = " key = value ";
    const char line_comment[] = "key=value ; comment";
    const char line_string[] = "key=\"this is a value\"";

    INIPair_t pair;
    ASSERT_TRUE(ini_parse_pair(line, &pair, NULL));
    ASSERT_STREQ(pair.key, "key");
    ASSERT_STREQ(pair.value, "value");

    ASSERT_TRUE(ini_parse_pair(line_spaces, &pair, NULL));
    ASSERT_STREQ(pair.key, "key");
    ASSERT_STREQ(pair.value, "value");

    ASSERT_TRUE(ini_parse_pair(line_comment, &pair, NULL));
    ASSERT_STREQ(pair.key, "key");
    ASSERT_STREQ(pair.value, "value");

    ASSERT_TRUE(ini_parse_pair(line_string, &pair, NULL));
    ASSERT_STREQ(pair.key, "key");
    ASSERT_STREQ(pair.value, "this is a value");
}



TEST(ini_tests, bad_pairs)
{
    use_heap();

    const char line_invalid_key[] = "1key=value";
    const char line_half[] = "key";
    const char line_empty[] = "";
    const char line_null[] = "\0";
    const char line_comment[] = "#key=value";
    const char line_bad_string[] = "key=\"this is a # bad string\"";

    ptrdiff_t error_offset;

    ASSERT_FALSE(ini_parse_pair(line_invalid_key, NULL, &error_offset));
    ASSERT_EQ(error_offset, 0);

    ASSERT_FALSE(ini_parse_pair(line_half, NULL, &error_offset));
    ASSERT_EQ(error_offset, 3);

    ASSERT_FALSE(ini_parse_pair(line_empty, NULL, &error_offset));
    ASSERT_EQ(error_offset, 0);

    ASSERT_FALSE(ini_parse_pair(line_null, NULL, &error_offset));
    ASSERT_EQ(error_offset, 0);

    ASSERT_FALSE(ini_parse_pair(line_comment, NULL, &error_offset));
    ASSERT_EQ(error_offset, 10);

    ASSERT_FALSE(ini_parse_pair(line_bad_string, NULL, &error_offset));
    ASSERT_EQ(error_offset, 15);

}



TEST(ini_tests, sections)
{
    use_heap();

    const char line[] = "[section]";
    const char line_spaces[] = " [ This is a section ] ";
    const char line_comment[] = "[section] ; comment here";

    INISection_t section;
    ASSERT_TRUE(ini_parse_section(line, &section, NULL));
    ASSERT_STREQ(section.name, "section");

    ASSERT_TRUE(ini_parse_section(line_spaces, &section, NULL));
    ASSERT_STREQ(section.name, "This is a section");

    ASSERT_TRUE(ini_parse_section(line_comment, &section, NULL));
    ASSERT_STREQ(section.name, "section");
}



TEST(ini_tests, bad_sections)
{
    use_heap();

    const char line_invalid[] = "x[section]";
    const char line_invalid_spaces[] = "[section   section]";
    const char line_empty[] = "";
    const char line_spaces[] = "  ";
    const char line_null[] = "\0";
    const char line_comment[] = "# comment here";

    ptrdiff_t error_offset;

    ASSERT_FALSE(ini_parse_section(line_invalid, NULL, &error_offset));
    ASSERT_EQ(error_offset, 0);

    ASSERT_FALSE(ini_parse_section(line_invalid_spaces, NULL, &error_offset));
    ASSERT_EQ(error_offset, 9);

    ASSERT_FALSE(ini_parse_section(line_empty, NULL, &error_offset));
    ASSERT_EQ(error_offset, 0);

    ASSERT_FALSE(ini_parse_section(line_spaces, NULL, &error_offset));
    ASSERT_EQ(error_offset, 2);

    ASSERT_FALSE(ini_parse_section(line_null, NULL, &error_offset));
    ASSERT_EQ(error_offset, 0);

    ASSERT_FALSE(ini_parse_section(line_comment, NULL, &error_offset));
    ASSERT_EQ(error_offset, 14);

    // Exceed string size
    char too_big_section[INI_MAX_STRING_SIZE + 3];
    memset(too_big_section, 'a', sizeof(too_big_section));
    too_big_section[0] = '[';
    too_big_section[INI_MAX_STRING_SIZE  + 1] = ']';
    too_big_section[INI_MAX_STRING_SIZE + 2] = '\0';

    INISection_t section;
    ASSERT_FALSE(ini_parse_section(too_big_section, &section, NULL));
}



TEST(ini_tests, file_parsing)
{
    use_heap();

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



TEST(ini_tests, add_pairs)
{
    use_heap();

    const char contents[] = "[Section]\n";

    FILE *file = tmpfile();
    assert(file);
    fputs(contents, file);
    rewind(file);
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, NULL) != NULL);
    fclose(file);
    ASSERT_TRUE(ini_add_pair(data, "Section", (INIPair_t){"key", "value"}) != NULL);
    ASSERT_TRUE(ini_add_pair(data, "SectionThatDoesNotExist", (INIPair_t){"key", "value"}) == NULL);
    ini_free_data(data);
}



TEST(ini_tests, get_value)
{
    use_heap();

    const char contents[] = "[section]\n"
                            "hello=world\n"
                            "message=\"hello world\"\n";

    FILE *file = tmpfile();
    assert(file);
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, &error) != NULL);
    fclose(file);
    ASSERT_STREQ(ini_get_value(data, "section", "hello"), "world");
    ASSERT_STREQ(ini_get_value(data, "section", "message"), "hello world");
    ASSERT_TRUE(ini_get_value(data, "FakeSection", "hello") == NULL);
    ASSERT_TRUE(ini_get_value(data, "section", "FakeKey") == NULL);
    ini_free_data(data);
}



TEST(ini_tests, get_string)
{
    use_heap();

    const char contents[] = "[section]\n"
                            "hello=world\n"
                            "message=\"hello world\"\n";

    FILE *file = tmpfile();
    assert(file);
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, &error) != NULL);
    fclose(file);
    ASSERT_STREQ(ini_get_string(data, "section", "hello", ""), "world");
    ASSERT_STREQ(ini_get_string(data, "section", "message", ""), "hello world");
    ASSERT_STREQ(ini_get_string(data, "FakeSection", "message", "Nope"), "Nope");
    ASSERT_STREQ(ini_get_string(data, "section", "FakeKey", "Nope"), "Nope");
    ini_free_data(data);
}



TEST(ini_tests, get_unsigned)
{
    use_heap();

    const unsigned expected = 5;
    FILE *file = tmpfile();
    assert(file);
    fprintf(file,"[section]\nval=%d", expected);
    rewind(file);
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, NULL) != NULL);
    const unsigned result = ini_get_unsigned(data, "section", "val", 0);
    ASSERT_EQ(result, expected);
    ini_free_data(data);
}



TEST(ini_tests, get_signed)
{
    use_heap();

    const int expected = -5;
    FILE *file = tmpfile();
    assert(file);
    fprintf(file,"[section]\nval=%d", expected);
    rewind(file);
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, NULL) != NULL);
    const int result = (int)ini_get_signed(data, "section", "val", 0);
    ASSERT_EQ(result, expected);
    ini_free_data(data);
}



TEST(ini_tests, get_float)
{
    use_heap();

    const float expected = 5.5f;
    FILE *file = tmpfile();
    assert(file);
    fprintf(file,"[section]\nval=%f", expected);
    rewind(file);
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, NULL) != NULL);
    const float result = (float)ini_get_float(data, "section", "val", 0);
    ASSERT_FLOAT_EQ(result, expected);
    ini_free_data(data);
}



TEST(ini_tests, get_bool)
{
    use_heap();

    FILE *file = tmpfile();
    assert(file);
    fprintf(file,"[section]\n"
        "true_key=true\n"
        "false_key=false\n");
    rewind(file);
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, NULL) != NULL);
    bool result = ini_get_bool(data, "section", "true_key", false);
    ASSERT_TRUE(result);
    result = ini_get_bool(data, "section", "false_key", true);
    ASSERT_FALSE(result);
    ini_free_data(data);

}



TEST(ini_tests, file_writing)
{
    use_heap();

    const char contents[] = "[section]\n"
                            "hello=world\n"
                            "hi=true\n"
                            "val=5\n"
                            "this_one=\"is a string\"\n";

    FILE *input_file = tmpfile();
    assert(input_file);
    fputs(contents, input_file);
    rewind(input_file);

    INIData_t *data = ini_create_data();
    ini_read_file(input_file, data, NULL);

    FILE *output_file = tmpfile();
    ini_write_file(data, output_file);
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



TEST(ini_tests, parse_error_bad_key)
{
    use_heap();

    const char contents[] = "[ValidSection]\n"
                            "b$ad=pair\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, &error) == NULL);
    ASSERT_TRUE(error.encountered);
    ASSERT_EQ(error.offset, 1);
    ASSERT_STREQ(error.line, "b$ad=pair\n");
    ASSERT_STREQ(error.msg, "Failed to parse pair.");
    ini_free_data(data);
    fclose(file);
}



TEST(ini_tests, parse_error_bad_value)
{
    use_heap();
    const char contents[] = "[ValidSection]\n"
                            "bad=pa\"ir\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, &error) == NULL);
    ASSERT_TRUE(error.encountered);
    ASSERT_STREQ(error.line, "bad=pa\"ir\n");
    ASSERT_STREQ(error.msg, "Failed to parse pair.");
    ini_free_data(data);
    fclose(file);
}



TEST(ini_tests, parse_error_no_section)
{
    use_heap();
    const char contents[] = "key=value\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, &error) == NULL);
    ASSERT_TRUE(error.encountered);
    ASSERT_STREQ(error.line, "key=value\n");
    ASSERT_STREQ(error.msg, "Pairs must reside within a section.");
    ini_free_data(data);
    fclose(file);
}



TEST(ini_tests, parse_error_bad_section)
{
    use_heap();
    const char contents[] = "[Bad-Section]\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, &error) == NULL);
    ASSERT_TRUE(error.encountered);
    ASSERT_STREQ(error.line, "[Bad-Section]\n");
    ASSERT_STREQ(error.msg, "Failed to parse section.");
    ASSERT_EQ(error.offset, 4);
    ini_free_data(data);
    fclose(file);
}



TEST(ini_tests, parse_error_duplicate_section)
{
    use_heap();
    const char contents[] = "[Section]\n"
                            "[Section]\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, &error) == NULL);
    fclose(file);
    ASSERT_TRUE(error.encountered);
    ASSERT_STREQ(error.line, "[Section]\n");
    ASSERT_STREQ(error.msg, "Duplicate section 'Section'.");
    ini_free_data(data);
}



TEST(ini_tests, stack)
{
    use_stack();

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
}



TEST(ini_tests, stack_insufficient_pair_allocation)
{
    use_stack();

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
}



TEST(ini_tests, stack_insufficient_section_allocation)
{
    use_stack();

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
}



static void use_heap()
{
    ini_set_allocator(malloc);
    ini_set_free(free);
    ini_set_reallocator(realloc);
}



static void use_stack()
{
    ini_disable_heap();
}