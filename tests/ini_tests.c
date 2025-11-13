#include "rktest.h"
#include "../ini.h"



#include <assert.h>



TEST(ini_tests, blank_lines)
{
    const char empty_line[] = "";
    const char space_line[] = " ";
    const char multiple_space_line[] = "   ";
    const char tab_line[] = "\t";
    const char new_line[] = "\n";
    const char null_line[] = "\0";
    const char alt_comment_line[] = "; comment here!";

    ASSERT_TRUE(ini_is_blank_line(empty_line));
    ASSERT_TRUE(ini_is_blank_line(space_line));
    ASSERT_TRUE(ini_is_blank_line(multiple_space_line));
    ASSERT_TRUE(ini_is_blank_line(tab_line));
    ASSERT_TRUE(ini_is_blank_line(new_line));
    ASSERT_TRUE(ini_is_blank_line(null_line));
    ASSERT_TRUE(ini_is_blank_line(alt_comment_line));
}



TEST(ini_tests, nonblank_line)
{
    const char line[] = "hello world";
    const char spaces[] = "   hello    world   ";
    const char comment[] = "hello world ; hello world";
    ASSERT_FALSE(ini_is_blank_line(line));
    ASSERT_FALSE(ini_is_blank_line(spaces));
    ASSERT_FALSE(ini_is_blank_line(comment));
}



TEST(ini_tests, keys)
{
    const char normal[] = "key=value";
    const char spaces[] = " key = value ";
    const char comment[] = "key=value ; comment";
    const char string[] = "key=\"this is a value\"";
    const char caps[] = "KEY=value";
    const char number[] = "key5=value";

    int n = 256;
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
}



TEST(ini_tests, values)
{
    const char normal[] = "key=value";
    const char spaces[] = " key = value ";
    const char number[] = "key=5";
    const char alnum[] = "key=5val5";
    const char special[] = "key=[]()/";
    const char comment[] = "key=value ; comment";
    const char string[] = "key=\"this is a value\"";

    int n = 256;
    char buffer[n];
    ASSERT_TRUE(ini_parse_value(normal, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");

    ASSERT_TRUE(ini_parse_value(spaces, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");

    ASSERT_TRUE(ini_parse_value(number, buffer, n, NULL));
    ASSERT_STREQ(buffer, "5");

    ASSERT_TRUE(ini_parse_value(alnum, buffer, n, NULL));
    ASSERT_STREQ(buffer, "5val5");

    ASSERT_TRUE(ini_parse_value(special, buffer, n, NULL));
    ASSERT_STREQ(buffer, "[]()/");

    ASSERT_TRUE(ini_parse_value(comment, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");

    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "this is a value");
}



TEST(ini_tests, bad_values)
{
    const char two[] = "key=value value";
    const char bad_string[] = "key=\"the man said \"hello\"\"";
    const char forbidden[] = "key=~value~";

    ASSERT_FALSE(ini_parse_value(two, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_value(bad_string, NULL, 0, NULL));
    ASSERT_FALSE(ini_parse_value(forbidden, NULL, 0, NULL));
}



TEST(ini_tests, pairs)
{
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
    const char line_invalid_key[] = "1key=value";
    const char line_invalid_value[] = "key=va lue";
    const char line_invalid_value_spaces[] = "key = value space";
    const char line_half[] = "key";
    const char line_empty[] = "";
    const char line_null[] = "\0";
    const char line_comment[] = "#key=value";
    const char line_bad_string[] = "key=\"this is a # bad string\"";

    ptrdiff_t error_offset;

    ASSERT_FALSE(ini_parse_pair(line_invalid_key, NULL, &error_offset));
    ASSERT_EQ(error_offset, 0);

    ASSERT_FALSE(ini_parse_pair(line_invalid_value, NULL, &error_offset));
    ASSERT_EQ(error_offset, 7);

    ASSERT_FALSE(ini_parse_pair(line_invalid_value_spaces, NULL, &error_offset));
    ASSERT_EQ(error_offset, 12);

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
    const char line[] = "[section]";
    const char line_spaces[] = " [ section ] ";
    const char line_comment[] = "[section] ; comment here";

    INISection_t section;
    ASSERT_TRUE(ini_parse_section(line, &section, NULL));
    ASSERT_STREQ(section.name, "section");

    ASSERT_TRUE(ini_parse_section(line_spaces, &section, NULL));
    ASSERT_STREQ(section.name, "section");

    ASSERT_TRUE(ini_parse_section(line_comment, &section, NULL));
    ASSERT_STREQ(section.name, "section");
}



TEST(ini_tests, bad_sections)
{
    const char line_invalid[] = "x[section]";
    const char line_invalid_double[] = "[section   section]";
    const char line_empty[] = "";
    const char line_spaces[] = "  ";
    const char line_null[] = "\0";
    const char line_comment[] = "# comment here";

    ptrdiff_t error_offset;

    ASSERT_FALSE(ini_parse_section(line_invalid, NULL, &error_offset));
    ASSERT_EQ(error_offset, 0);

    ASSERT_FALSE(ini_parse_section(line_invalid_double, NULL, &error_offset));
    ASSERT_EQ(error_offset, 11);

    ASSERT_FALSE(ini_parse_section(line_empty, NULL, &error_offset));
    ASSERT_EQ(error_offset, 0);

    ASSERT_FALSE(ini_parse_section(line_spaces, NULL, &error_offset));
    ASSERT_EQ(error_offset, 2);

    ASSERT_FALSE(ini_parse_section(line_null, NULL, &error_offset));
    ASSERT_EQ(error_offset, 0);

    ASSERT_FALSE(ini_parse_section(line_comment, NULL, &error_offset));
    ASSERT_EQ(error_offset, 14);

}



TEST(ini_tests, file_parsing)
{
    const char contents[] = "[section]\n"
                            "hello=world\n"
                            "hi=true\n"
                            "val=5\n"
                            "this_one=\"is a string\"\n";

    FILE *file = tmpfile();
    assert(file);
    fputs(contents, file);
    rewind(file);
    INIData_t *data = ini_create_data();
    ini_read_file(file, data, NULL);
    ASSERT_TRUE(data != NULL);
    ASSERT_STREQ(ini_get_value(data, "section", "hello"), "world");
    ASSERT_STREQ(ini_get_value(data, "section", "hi"), "true");
    ASSERT_STREQ(ini_get_value(data, "section", "val"), "5");
    ASSERT_STREQ(ini_get_value(data, "section", "this_one"), "is a string");
    ini_free_data(data);
}



TEST(ini_tests, get_string)
{
    const char contents[] = "[section]\n"
                            "hello=world\n"
                            "message=\"hello world\"\n";

    FILE *file = tmpfile();
    assert(file);
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ini_read_file(file, data, &error);
    fclose(file);
    ASSERT_TRUE(data != NULL);
    ASSERT_STREQ(ini_get_string(data, "section", "hello", ""), "world");
    ASSERT_STREQ(ini_get_string(data, "section", "message", ""), "hello world");
    ini_free_data(data);
}



TEST(ini_tests, get_unsigned)
{
    const unsigned expected = 5;
    FILE *file = tmpfile();
    assert(file);
    fprintf(file,"[section]\nval=%d", expected);
    rewind(file);
    INIData_t *data = ini_create_data();
    ini_read_file(file, data, NULL);
    ASSERT_TRUE(data != NULL);
    const unsigned result = ini_get_unsigned(data, "section", "val", 0);
    ASSERT_EQ(result, expected);
    ini_free_data(data);
}



TEST(ini_tests, get_signed)
{
    const int expected = -5;
    FILE *file = tmpfile();
    assert(file);
    fprintf(file,"[section]\nval=%d", expected);
    rewind(file);
    INIData_t *data = ini_create_data();
    ini_read_file(file, data, NULL);
    ASSERT_TRUE(data != NULL);
    const int result = (int)ini_get_signed(data, "section", "val", 0);
    ASSERT_EQ(result, expected);
    ini_free_data(data);
}



TEST(ini_tests, get_float)
{
    const float expected = 5.5;
    FILE *file = tmpfile();
    assert(file);
    fprintf(file,"[section]\nval=%f", expected);
    rewind(file);
    INIData_t *data = ini_create_data();
    ini_read_file(file, data, NULL);
    ASSERT_TRUE(data != NULL);
    const float result = (float)ini_get_float(data, "section", "val", 0);
    ASSERT_FLOAT_EQ(result, expected);
    ini_free_data(data);
}



TEST(ini_tests, get_bool)
{
    const bool expected = 5;
    FILE *file = tmpfile();
    assert(file);
    fprintf(file,"[section]\nval=%s", (expected ? "true" : "false"));
    rewind(file);
    INIData_t *data = ini_create_data();
    ini_read_file(file, data, NULL);
    ASSERT_TRUE(data != NULL);
    const bool result = ini_get_bool(data, "section", "val", !expected);
    ASSERT_EQ(result, expected);
    ini_free_data(data);

}



TEST(ini_tests, file_writing)
{
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
    const char contents[] = "[ValidSection]\n"
                            "bad=pa$ir\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, &error) == NULL);
    ASSERT_TRUE(error.encountered);
    ASSERT_STREQ(error.line, "bad=pa$ir\n");
    ASSERT_STREQ(error.msg, "Failed to parse pair.");
    ini_free_data(data);
    fclose(file);
}



TEST(ini_tests, parse_error_no_section)
{
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
    const char contents[] = "[Bad Section]\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, &error) == NULL);
    ASSERT_TRUE(error.encountered);
    ASSERT_STREQ(error.line, "[Bad Section]\n");
    ASSERT_STREQ(error.msg, "Failed to parse section.");
    ini_free_data(data);
    fclose(file);
}



TEST(ini_tests, parse_error_duplicate_section)
{
    const char contents[] = "[Section]\n"
                            "[Section]\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);
    INIError_t error;
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_read_file(file, data, &error) == NULL);
    ASSERT_TRUE(error.encountered);
    ASSERT_STREQ(error.line, "[Section]\n");
    ASSERT_STREQ(error.msg, "Duplicate section 'Section'.");
    ini_free_data(data);
    fclose(file);
}



TEST(ini_tests, stack)
{
    const char contents[] = "[Section]\n"
                            "key=value\n"
                            "other_key=other_value\n";
    FILE *file = tmpfile();
    fputs(contents, file);
    rewind(file);
}