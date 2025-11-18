#include "rktest.h"
#include "../ini.h"



INIPair_t pair;



///////////////////
//  Valid Pairs  //
///////////////////



TEST(pairs, normal)
{
    const char string[] = "key=value";
    ASSERT_TRUE(ini_parse_pair(string, &pair, NULL));
    ASSERT_STREQ(pair.key, "key");
    ASSERT_STREQ(pair.value, "value");
}



TEST(pairs, padded)
{
    const char string[] = "  key  =  value  ";
    ASSERT_TRUE(ini_parse_pair(string, &pair, NULL));
    ASSERT_STREQ(pair.key, "key");
    ASSERT_STREQ(pair.value, "value");
}



TEST(pairs, comment)
{
    const char string[] = "key=value  \t ; comment here";
    ASSERT_TRUE(ini_parse_pair(string, &pair, NULL));
    ASSERT_STREQ(pair.key, "key");
    ASSERT_STREQ(pair.value, "value");
}



TEST(pairs, unquoted_string)
{
    const char string[] = "key=this is a value";
    ASSERT_TRUE(ini_parse_pair(string, &pair, NULL));
    ASSERT_STREQ(pair.key, "key");
    ASSERT_STREQ(pair.value, "this is a value");
}



TEST(pairs, quoted_string)
{
    const char string[] = "key=\"this is a value\"";
    ASSERT_TRUE(ini_parse_pair(string, &pair, NULL));
    ASSERT_STREQ(pair.key, "key");
    ASSERT_STREQ(pair.value, "this is a value");
}



/////////////////////
//  Invalid Pairs  //
/////////////////////



static ptrdiff_t error_offset = 0;



TEST(pairs, invalid_key)
{
    const char string[] = "1key=value";
    ASSERT_FALSE(ini_parse_pair(string, &pair, &error_offset));
    ASSERT_EQ(error_offset, 0);
}



TEST(pairs, invalid_value)
{
    const char string[] = "key=va[lue";
    ASSERT_FALSE(ini_parse_pair(string, &pair, &error_offset));
    ASSERT_EQ(error_offset, 6);
}



TEST(pairs, lone_value)
{
    const char string[] = "=value";
    ASSERT_FALSE(ini_parse_pair(string, &pair, &error_offset));
    ASSERT_EQ(error_offset, 0);
}



TEST(pairs, early_comment)
{
    const char string[] = "#key=value";
    ASSERT_FALSE(ini_parse_pair(string, &pair, &error_offset));
    ASSERT_EQ(error_offset, 10);
}



TEST(pairs, bad_string)
{
    const char string[] = "key=\"unfinished string";
    ASSERT_FALSE(ini_parse_pair(string, &pair, &error_offset));
    ASSERT_EQ(error_offset, 22);
}
