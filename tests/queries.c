#include "rktest.h"
#include "../ini.h"



/////////////////////
//  Valid Queries  //
/////////////////////



TEST(queries, add_section)
{
    INIData_t *data = ini_create_data();
    const INISection_t *section = ini_add_section(data, "section");
    ASSERT_TRUE(section != NULL);
    ASSERT_TRUE(section == ini_has_section(data, "section"));
    ini_free_data(data);
}



TEST(queries, add_pair)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    const INIPair_t *pair = ini_add_pair(data, "section", (INIPair_t){"key", "value"});
    ASSERT_TRUE(pair != NULL);
    ASSERT_STREQ(pair->key, "key");
    ASSERT_STREQ(pair->value, "value");
    ini_free_data(data);
}



TEST(queries, add_pair_to_section)
{
    INIData_t *data = ini_create_data();
    INISection_t *section = ini_add_section(data, "section");
    const INIPair_t *pair = ini_add_pair_to_section(section, (INIPair_t){"key", "value"});
    ASSERT_TRUE(pair != NULL);
    ASSERT_STREQ(pair->key, "key");
    ASSERT_STREQ(pair->value, "value");
    ini_free_data(data);
}



TEST(queries, get_value)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "value"});
    const char *val = ini_get_value(data, "section", "key");
    ASSERT_TRUE(val != NULL);
    ASSERT_STREQ(val, "value");
    ini_free_data(data);
}



TEST(queries, get_string)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "string value!"});
    const char *val = ini_get_string(data, "section", "key", "");
    ASSERT_STREQ(val, "string value!");
    ini_free_data(data);
}



TEST(queries, get_unsigned)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "1234567890"});
    const unsigned long val = (unsigned long)ini_get_unsigned(data, "section", "key", 0);
    ASSERT_LONG_EQ(val, 1234567890);
    ini_free_data(data);
}



TEST(queries, get_signed)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "-1"});
    const long val = (long)ini_get_signed(data, "section", "key", 0);
    ASSERT_LONG_EQ(val, -1);
    ini_free_data(data);
}



TEST(queries, get_hex_upper)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "0xDEADBEEF"});
    const long val = (long)ini_get_hex(data, "section", "key", 0);
    ASSERT_LONG_EQ(val, 0xdeadbeef);
    ini_free_data(data);
}



TEST(queries, get_hex_lower)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "0xdeadbeef"});
    const long val = (long)ini_get_hex(data, "section", "key", 0);
    ASSERT_LONG_EQ(val, 0xdeadbeef);
    ini_free_data(data);
}



TEST(queries, get_float)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "3.14"});
    const float val = (float)ini_get_float(data, "section", "key", 0);
    ASSERT_LONG_EQ(val, 3.14);
    ini_free_data(data);
}



TEST(queries, get_bool_true)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "true"});
    ASSERT_TRUE(ini_get_bool(data, "section", "key", false));
    ini_free_data(data);
}



TEST(queries, get_bool_false)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "false"});
    ASSERT_FALSE(ini_get_bool(data, "section", "key", true));
    ini_free_data(data);
}



///////////////////////
//  Invalid Queries  //
///////////////////////



TEST(queries, empty_data)
{
    INIData_t *data = ini_create_data();
    ASSERT_TRUE(ini_has_section(data, "section") == NULL);
    ini_free_data(data);
}



TEST(queries, missing_section)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ASSERT_TRUE(ini_has_section(data, "other section") == NULL);
    ini_free_data(data);
}



TEST(queries, duplicate_section)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ASSERT_TRUE(ini_add_section(data, "section") == NULL);
    ini_free_data(data);
}



TEST(queries, add_pair_to_nonexistent_section)
{
    INIData_t *data = ini_create_data();
    const INIPair_t *pair = ini_add_pair(data, "section", (INIPair_t){"key", "value"});
    ASSERT_TRUE(pair == NULL);
    ini_free_data(data);
}



TEST(queries, get_missing_value)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    const char *val = ini_get_value(data, "section", "key");
    ASSERT_TRUE(val == NULL);
    ini_free_data(data);
}



TEST(queries, get_missing_value_from_missing_section)
{
    INIData_t *data = ini_create_data();
    const char *val = ini_get_value(data, "section", "key");
    ASSERT_TRUE(val == NULL);
    ini_free_data(data);
}



TEST(queries, get_missing_string)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    const char *val = ini_get_string(data, "section", "key", "default");
    ASSERT_TRUE(val != NULL);
    ASSERT_STREQ(val, "default");
    ini_free_data(data);
}



TEST(queries, get_missing_unsigned)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    const int val = (int)ini_get_unsigned(data, "section", "key", -1);
    ASSERT_EQ(val, -1);
    ini_free_data(data);
}



TEST(queries, get_invalid_unsigned)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "not unsigned"});
    const int val = (int)ini_get_unsigned(data, "section", "key", -1);
    ASSERT_EQ(val, -1);
    ini_free_data(data);
}



#include <limits.h>



TEST(queries, get_missing_signed)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    const unsigned val = (unsigned)ini_get_signed(data, "section", "key", UINT_MAX);
    ASSERT_EQ(val, UINT_MAX);
    ini_free_data(data);
}



TEST(queries, get_invalid_signed)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "not signed"});
    const unsigned val = (unsigned)ini_get_signed(data, "section", "key", UINT_MAX);
    ASSERT_EQ(val, UINT_MAX);
    ini_free_data(data);
}



TEST(queries, get_missing_hex)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    const unsigned val = (unsigned)ini_get_hex(data, "section", "key", 0xDEADBEEF);
    ASSERT_EQ(val, 0xDEADBEEF);
    ini_free_data(data);
}



TEST(queries, get_invalid_hex)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "not hex"});
    const unsigned val = (unsigned)ini_get_hex(data, "section", "key", 0xDEADBEEF);
    ASSERT_EQ(val, 0xDEADBEEF);
    ini_free_data(data);
}



TEST(queries, get_missing_float)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    const float val = (float)ini_get_float(data, "section", "key", 9.18f);
    ASSERT_EQ(val, 9.18f);
    ini_free_data(data);
}



TEST(queries, get_invalid_float)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "not float"});
    const float val = (float)ini_get_float(data, "section", "key", 9.18f);
    ASSERT_EQ(val, 9.18f);
    ini_free_data(data);
}



TEST(queries, get_missing_bool_true)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    const bool val = ini_get_bool(data, "section", "key", true);
    ASSERT_TRUE(val);
    ini_free_data(data);
}



TEST(queries, get_invalid_bool_true)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "not bool"});
    const bool val = ini_get_bool(data, "section", "key", true);
    ASSERT_TRUE(val);
    ini_free_data(data);
}



TEST(queries, get_missing_bool_false)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    const bool val = ini_get_bool(data, "section", "key", false);
    ASSERT_FALSE(val);
    ini_free_data(data);
}



TEST(queries, get_invalid_bool_false)
{
    INIData_t *data = ini_create_data();
    ini_add_section(data, "section");
    ini_add_pair(data, "section", (INIPair_t){"key", "not bool"});
    const bool val = ini_get_bool(data, "section", "key", false);
    ASSERT_FALSE(val);
    ini_free_data(data);
}
