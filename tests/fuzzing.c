#include "rktest.h"
#include "../ini.h"



TEST(fuzzing, file_parse_invalid_path)
{
    INIData_t data;
    ASSERT_TRUE(ini_read_file_path("./does/not/exist.nope", &data, NULL, 0) == NULL);
}



TEST(fuzzing, file_parse_null_data)
{
    FILE *fp = tmpfile();
    ASSERT_TRUE(ini_read_file(fp, NULL, NULL, 0) == NULL);
    fclose(fp);
}



TEST(fuzzing, add_section_null_data)
{
    ASSERT_TRUE(ini_add_section(NULL, "section") == NULL);
}



TEST(fuzzing, add_section_null_name)
{
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_add_section(data, NULL) == NULL);
    ini_free_data(data);
}



TEST(fuzzing, add_section_long_name)
{
    INIData_t *data = ini_create_data();
    const int n = INI_MAX_STRING_SIZE * 3;
    char section[n];
    int i = 0;
    section[i++] = '[';
    for (; i < n - 1; i++) section[i] = 'a';
    section[i] = '\0';
    const INISection_t *sp = ini_add_section(data, section);
    ASSERT_TRUE(sp != NULL);
    ASSERT_EQ(strlen(sp->name), INI_MAX_STRING_SIZE-1);
}



TEST(fuzzing, add_pair_null_data)
{
    ASSERT_TRUE(ini_add_pair(NULL, "section", (INIPair_t){"key", "value"}) == NULL);
}



TEST(fuzzing, add_pair_null_section)
{
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_add_pair(data, "section", (INIPair_t){"key", "value"}) == NULL);
    ini_free_data(data);
}



TEST(fuzzing, has_section_null_data)
{
    ASSERT_TRUE(ini_has_section(NULL, "section") == NULL);
}



TEST(fuzzing, has_section_null_name)
{
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_has_section(data, NULL) == NULL);
    ini_free_data(data);
}



TEST(fuzzing, get_value_null_data)
{
    ASSERT_TRUE(ini_get_value(NULL, "section", "key") == NULL);
}



TEST(fuzzing, get_value_null_section)
{
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_get_value(data, NULL, "key") == NULL);
    ini_free_data(data);
}



TEST(fuzzing, get_section_null_key)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ASSERT_TRUE(ini_get_value(data, "section", NULL) == NULL);
    ini_free_data(data);
}



TEST(fuzzing, get_string_null_data)
{
    ASSERT_STREQ(ini_get_string(NULL, "section", "key", "default"), "default");
}



TEST(fuzzing, get_string_null_section)
{
    INIData_t *data = ini_create_data();
    ASSERT_STREQ(ini_get_string(data, NULL, "key", "default"), "default");
    ini_free_data(data);
}



TEST(fuzzing, get_string_null_key)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ASSERT_STREQ(ini_get_string(data, "section", NULL, "default"), "default");
    ini_free_data(data);
}



TEST(fuzzing, get_unsigned_null_data)
{
    ASSERT_TRUE(ini_get_unsigned(NULL, "section", "key", 123) == 123);
}



TEST(fuzzing, get_unsigned_null_section)
{
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_get_unsigned(data, NULL, "key", 123) == 123);
    ini_free_data(data);
}



TEST(fuzzing, get_unsigned_null_key)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ASSERT_TRUE(ini_get_unsigned(data, "section", NULL, 123) == 123);
    ini_free_data(data);
}



TEST(fuzzing, get_signed_null_data)
{
    ASSERT_TRUE(ini_get_signed(NULL, "section", "key", -5) == -5);
}



TEST(fuzzing, get_signed_null_section)
{
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_get_signed(data, NULL, "key", -5) == -5);
    ini_free_data(data);
}



TEST(fuzzing, get_signed_null_key)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ASSERT_TRUE(ini_get_signed(data, "section", NULL, -5) == -5);
    ini_free_data(data);
}



TEST(fuzzing, get_hex_null_data)
{
    ASSERT_TRUE(ini_get_hex(NULL, "section", "key", 0xABC) == 0xABC);
}



TEST(fuzzing, get_hex_null_section)
{
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_get_hex(data, NULL, "key", 0xABC) == 0xABC);
    ini_free_data(data);
}



TEST(fuzzing, get_hex_null_key)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ASSERT_TRUE(ini_get_hex(data, "section", NULL, 0xABC) == 0xABC);
    ini_free_data(data);
}



TEST(fuzzing, get_float_null_data)
{
    ASSERT_TRUE(ini_get_float(NULL, "section", "key", 1.5L) == 1.5L);
}



TEST(fuzzing, get_float_null_section)
{
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_get_float(data, NULL, "key", 1.5L) == 1.5L);
    ini_free_data(data);
}



TEST(fuzzing, get_float_null_key)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ASSERT_TRUE(ini_get_float(data, "section", NULL, 1.5L) == 1.5L);
    ini_free_data(data);
}



TEST(fuzzing, get_bool_null_data)
{
    ASSERT_TRUE(ini_get_bool(NULL, "section", "key", true) == true);
}



TEST(fuzzing, get_bool_null_section)
{
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_get_bool(data, NULL, "key", true) == true);
    ini_free_data(data);
}



TEST(fuzzing, get_bool_null_key)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ASSERT_TRUE(ini_get_bool(data, "section", NULL, true) == true);
    ini_free_data(data);
}



TEST(fuzzing, is_blank_line_null_line)
{
    ASSERT_FALSE(ini_is_blank_line(NULL));
}



TEST(fuzzing, parse_section_null_line)
{
    INISection_t section;
    ASSERT_FALSE(ini_parse_section(NULL, &section, NULL));
}



TEST(fuzzing, parse_section_null_section)
{
    const char *line = " [section] ";
    ASSERT_TRUE(ini_parse_section(line, NULL, NULL));
}



TEST(fuzzing, parse_pair_null_line)
{
    INIPair_t pair;
    ASSERT_FALSE(ini_parse_pair(NULL, &pair, NULL));
}



TEST(fuzzing, parse_pair_null_pair)
{
    const char *line = "key = value";
    ASSERT_TRUE(ini_parse_pair(line, NULL, NULL));
}



TEST(fuzzing, parse_key_null_line)
{
    char key[INI_MAX_STRING_SIZE];
    ASSERT_FALSE(ini_parse_key(NULL, key, sizeof(key), NULL));
}



TEST(fuzzing, parse_key_null_dest)
{
    const char *line = "key = value";
    ASSERT_TRUE(ini_parse_key(line, NULL, INI_MAX_STRING_SIZE, NULL));
}



TEST(fuzzing, parse_key_null_dest_and_zero_size)
{
    const char *line = "key = value";
    ASSERT_TRUE(ini_parse_key(line, NULL, 0, NULL));
}



TEST(fuzzing, parse_value_null_line)
{
    char val[INI_MAX_STRING_SIZE];
    ASSERT_FALSE(ini_parse_value(NULL, val, sizeof(val), NULL));
}



TEST(fuzzing, parse_value_null_dest)
{
    const char *line = "key = value";
    ASSERT_TRUE(ini_parse_value(line, NULL, INI_MAX_STRING_SIZE, NULL));
}



TEST(fuzzing, parse_value_null_dest_and_zero_size)
{
    const char *line = "key = value";
    ASSERT_TRUE(ini_parse_value(line, NULL, 0, NULL));
}



TEST(fuzzing, is_blank_line_oversized)
{
    char line[INI_MAX_LINE_SIZE + 10];
    memset(line, ' ', sizeof(line));
    line[sizeof(line) - 1] = '\0';

    ASSERT_TRUE(ini_is_blank_line(line));
}



TEST(fuzzing, parse_section_oversized_line)
{
    INISection_t section;

    char line[INI_MAX_LINE_SIZE + 10];
    memset(line, 'A', sizeof(line));
    line[sizeof(line) - 1] = '\0';

    ASSERT_FALSE(ini_parse_section(line, &section, NULL));
}



TEST(fuzzing, parse_pair_oversized_line)
{
    INIPair_t pair;

    char line[INI_MAX_LINE_SIZE + 10];
    memset(line, 'B', sizeof(line));
    line[sizeof(line) - 1] = '\0';

    ASSERT_FALSE(ini_parse_pair(line, &pair, NULL));
}



TEST(fuzzing, parse_key_oversized_line)
{
    char key[INI_MAX_STRING_SIZE];

    char line[INI_MAX_LINE_SIZE + 10];
    memset(line, 'C', sizeof(line));
    line[sizeof(line) - 1] = '\0';

    ASSERT_FALSE(ini_parse_key(line, key, sizeof(key), NULL));
}



TEST(fuzzing, parse_value_oversized_line)
{
    char val[INI_MAX_STRING_SIZE];

    char line[INI_MAX_LINE_SIZE + 10];
    memset(line, 'D', sizeof(line));
    line[sizeof(line) - 2] = '=';
    line[sizeof(line) - 1] = '\0';

    ASSERT_TRUE(ini_parse_value(line, val, sizeof(val), NULL));
    ASSERT_EQ(strlen(val), 0);
    ASSERT_TRUE(strncmp(val, line, strlen(val)) == 0);
}



TEST(fuzzing, parse_value_oversized_line_no_assignment)
{
    char val[INI_MAX_STRING_SIZE];

    char line[INI_MAX_LINE_SIZE + 10];
    memset(line, 'D', sizeof(line));
    line[sizeof(line) - 1] = '\0';

    ptrdiff_t error_offset = 0;
    ASSERT_FALSE(ini_parse_value(line, val, sizeof(val), &error_offset));
    ASSERT_EQ(error_offset, sizeof(line) - 1);
}
