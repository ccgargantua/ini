#include "rktest.h"
#include "../ini.h"



TEST(blank_lines, empty_string)
{
    const char string[] = "";
    ASSERT_TRUE(ini_is_blank_line(string));
}



TEST(blank_lines, single_space)
{
    const char string[] = " ";
    ASSERT_TRUE(ini_is_blank_line(string));
}



TEST(blank_lines, multi_space)
{
    const char string[] = "   ";
    ASSERT_TRUE(ini_is_blank_line(string));
}



TEST(blank_lines, newline)
{
    const char string[] = "\n";
    ASSERT_TRUE(ini_is_blank_line(string));
}



TEST(blank_lines, carriage_return)
{
    const char string[] = "\r";
    ASSERT_TRUE(ini_is_blank_line(string));
}



TEST(blank_lines, tab)
{
    const char string[] = "\t";
    ASSERT_TRUE(ini_is_blank_line(string));
}



TEST(blank_lines, null)
{
    const char string[] = "\0";
    ASSERT_TRUE(ini_is_blank_line(string));
}



TEST(blank_lines, whitespace)
{
    const char string[] = " \t\r\n";
    ASSERT_TRUE(ini_is_blank_line(string));
}



TEST(blank_lines, comment_pound)
{
    const char string[] = "# this is a comment";
    ASSERT_TRUE(ini_is_blank_line(string));
}



TEST(blank_lines, comment_semicolon)
{
    const char string[] = "; this is a comment";
    ASSERT_TRUE(ini_is_blank_line(string));
}



TEST(blank_lines, nonblank_single_word)
{
    const char string[] = "hello";
    ASSERT_FALSE(ini_is_blank_line(string));
}



TEST(blank_lines, nonblank_multi_word)
{
    const char string[] = "hello world";
    ASSERT_FALSE(ini_is_blank_line(string));
}



TEST(blank_lines, nonblank_padded)
{
    const char string[] = "  hello   world   ";
    ASSERT_FALSE(ini_is_blank_line(string));
}



TEST(blank_lines, nonblank_comment)
{
    const char string[] = "hello world # comment here";
    ASSERT_FALSE(ini_is_blank_line(string));
}
