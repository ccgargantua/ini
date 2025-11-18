#include "rktest.h"
#include "../ini.h"



#define n 256
static char buffer[n];



////////////////////
//  Valid Values  //
////////////////////



TEST(values, normal)
{
    const char string[] = "key=value";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");
}



TEST(values, alternative_delimiter)
{
    const char string[] = "key:value";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");
}



TEST(values, padded_single_space)
{
    const char string[] = " key = value ";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");
}



TEST(values, padded_multi_space)
{
    const char string[] = "  key  =  value  ";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");
}



TEST(values, padded_single_tab)
{
    const char string[] = "\tkey\t=\tvalue\t";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");
}



TEST(values, padded_multi_tab)
{
    const char string[] = "\t\tkey\t\t=\t\tvalue\t\t";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");
}



TEST(values, comment)
{
    const char string[] = "key=value # comment here";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");
}



TEST(values, comment_unpadded)
{
    const char string[] = "key=value# comment here";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");
}



TEST(values, comment_extra_padded)
{
    const char string[] = "key=value  \t\t# comment here";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value");
}



TEST(values, unquoted_multi)
{
    const char string[] = "key=value value";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value value");
}



TEST(values, unquoted_multi_padded)
{
    const char string[] = "key=  \tvalue value  \t";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value value");
}



TEST(values, unquoted_comment)
{
    const char string[] = "key=value value ; comment";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value value");
}



TEST(values, unquoted_comment_unpadded)
{
    const char string[] = "key=value value; comment";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value value");
}



TEST(values, unquoted_comment_extra_padded)
{
    const char string[] = "key=value value  \t; comment";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "value value");
}



TEST(values, numeric)
{
    const char string[] = "key = 0123456789";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "0123456789");
}



TEST(values, special)
{
    const char string[] = "key=~!@$%^&*()_+-{}|\\:'<>?,./";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "~!@$%^&*()_+-{}|\\:'<>?,./");
}



TEST(values, equation)
{
    const char string[] = "value = 2 + 2 = 4";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "2 + 2 = 4");
}



TEST(values, ip_address)
{
    const char string[] = "key=192.168.0.1";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "192.168.0.1");
}



TEST(values, unix_path)
{
    const char string[] = "key=/home/gargantua/Documents/document.txt";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "/home/gargantua/Documents/document.txt");
}



TEST(values, windows_path)
{
    const char string[] = "key=C:\\Users\\gargantua\\Documents\\document.txt";
    ASSERT_TRUE(ini_parse_value(string, buffer, n, NULL));
    ASSERT_STREQ(buffer, "C:\\Users\\gargantua\\Documents\\document.txt");
}



//////////////////////
//  Invalid Values  //
//////////////////////



static ptrdiff_t error_offset = 0;



TEST(values, unquoted_multispace)
{
    const char string[] = "key=value  value";
    ASSERT_FALSE(ini_parse_value(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 11);
}



TEST(values, quoted_extra_quotes)
{
    const char string[] = "key=\"string right \"here\"\"";
    ASSERT_FALSE(ini_parse_value(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 19);
}



TEST(values, quoted_unclosed)
{
    const char string[] = "key= \"value";
    ASSERT_FALSE(ini_parse_value(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 11);
}



TEST(values, quoted_unopened)
{
    const char string[] = "key= value\"";
    ASSERT_FALSE(ini_parse_value(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 10);
}



TEST(values, forbidden_prefix)
{
    const char string[] = "key=[value";
    ASSERT_FALSE(ini_parse_value(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 4);
}



TEST(values, forbidden_suffix)
{
    const char string[] = "key=value]";
    ASSERT_FALSE(ini_parse_value(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 9);
}



TEST(value, forbidden_infix)
{
    const char string[] = "key=va\nlue";
    ASSERT_FALSE(ini_parse_value(string, buffer, n, &error_offset));
    ASSERT_EQ(error_offset, 7);
}
