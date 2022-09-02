#include <moose/version.h>
#include <gtest/gtest.h>

using namespace moose;

TEST (version, stringConversion)
{
  Version v {11265, 231, 3876};
  std::string s {"11265.231.3876"};

  EXPECT_EQ (v.toString (), s);
  EXPECT_EQ (v, Version::fromString (s));
}
