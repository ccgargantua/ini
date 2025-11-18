#include "rktest.h"
#include "../ini.h"



INISection_t section;



//////////////////////
//  Valid Sections  //
//////////////////////



TEST(sections, normal)
{
    const char string[] = "[section]";
    ASSERT_TRUE(ini_parse_section(string, &section, NULL));
    ASSERT_STREQ(section.name, "section");
}



TEST(sections, padding)
{
    const char string[] = "[  section  ]";
    ASSERT_TRUE(ini_parse_section(string, &section, NULL));
    ASSERT_STREQ(section.name, "section");
}



TEST(sections, comment)
{
    const char string[] = "[section] ; section";
    ASSERT_TRUE(ini_parse_section(string, &section, NULL));
    ASSERT_STREQ(section.name, "section");
}



TEST(sections, multiple_words)
{
    const char string[] = "[This is a section]";
    ASSERT_TRUE(ini_parse_section(string, &section, NULL));
    ASSERT_STREQ(section.name, "This is a section");
}



TEST(sections, underscores)
{
    const char string[] = "[_section_here_ ]";
    ASSERT_TRUE(ini_parse_section(string, &section, NULL));
    ASSERT_STREQ(section.name, "_section_here_");
}



////////////////////////
//  Invalid Sections  //
////////////////////////



static ptrdiff_t error_offset = 0;



TEST(sections, no_brackets)
{
    const char string[] = "section";
    ASSERT_FALSE(ini_parse_section(string, &section, &error_offset));
    ASSERT_EQ(error_offset, 0);
}



TEST(sections, too_many_spaces)
{
    const char string[] = "[Too  many]";
    ASSERT_FALSE(ini_parse_section(string, &section, &error_offset));
    ASSERT_EQ(error_offset, 6);
}



TEST(sections, empty)
{
    const char string[] = "";
    ASSERT_FALSE(ini_parse_section(string, &section, &error_offset));
    ASSERT_EQ(error_offset, 0);
}



TEST(sections, early_comment)
{
    const char string[] = "# [This is a section]";
    ASSERT_FALSE(ini_parse_section(string, &section, &error_offset));
    ASSERT_EQ(error_offset, 21);
}



TEST(sections, numeric_start)
{
    const char string[] = "[1section]";
    ASSERT_FALSE(ini_parse_section(string, &section, &error_offset));
    ASSERT_EQ(error_offset, 1);
}



TEST(sections, special)
{
    const char string[] = "[This is a-section]";
    ASSERT_FALSE(ini_parse_section(string, &section, &error_offset));
    ASSERT_EQ(error_offset, 10);
}
