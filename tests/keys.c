#include "rktest.h"
#include "../ini.h"



#define n 256
static char buffer[n];



//////////////////
//  Valid Keys  //
//////////////////



TEST(keys, normal)
{
    const char string[] = "key=value";
    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key");
}



TEST(keys, padded_single_space)
{
    const char string[] = " key = value ";
    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key");
}



TEST(keys, padded_multi_space)
{
    const char string[] = "  key  =  value  ";
    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key");
}



TEST(keys, padded_single_tab)
{
    const char string[] = "\tkey\t=\tvalue\t";
    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key");
}



TEST(keys, padded_multi_tab)
{
    const char string[] = "\t\tkey\t\t=\t\tvalue\t\t";
    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key");
}



TEST(keys, comment)
{
    const char string[] = "key=value # comment";
    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key");
}



TEST(keys, alpha_numeric)
{
    const char string[] = "key1=value";
    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key1");
}



TEST(keys, underscore_prefix)
{
    const char string[] = "_key=value";
    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "_key");
}



TEST(keys, underscore_suffix)
{
    const char string[] = "key_=value";
    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "key_");
}



TEST(keys, underscore_infix)
{
    const char string[] = "ke_y=value";
    ASSERT_TRUE(ini_parse_key(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "ke_y");
}



////////////////////
//  Invalid Keys  //
////////////////////



static ptrdiff_t error_offset = 0;



TEST(keys, numeric_prefix)
{
    const char string[] = "1key=value";
    ASSERT_FALSE(ini_parse_key(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 0);
}



TEST(keys, special_prefix)
{
    const char string[] = "-key=value";
    ASSERT_FALSE(ini_parse_key(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 0);
}



TEST(keys, special_suffix)
{
    const char string[] = "key$=value";
    ASSERT_FALSE(ini_parse_key(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 3);
}



TEST(keys, special_infix)
{
    const char string[] = "ke(y=value";
    ASSERT_FALSE(ini_parse_key(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 2);
}



TEST(keys, space)
{
    const char string[] = "key key=value";
    ASSERT_FALSE(ini_parse_key(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 4);
}



TEST(keys, tab)
{
    const char string[] = "key\tkey=value";
    ASSERT_FALSE(ini_parse_key(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 4);
}



TEST(keys, lonely)
{
    const char string[] = "key";
    ASSERT_FALSE(ini_parse_key(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 3);
}