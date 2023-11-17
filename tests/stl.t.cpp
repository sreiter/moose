
#include <moose/stl_serialization.h>
#include <gtest/gtest.h>

#include "to_json_and_back.h"

using namespace moose;

TEST (stl, optionalWithValue)
{
  std::optional<double> v {1.23456};
  EXPECT_EQ (v, toJsonAndBack (v));
}

TEST (stl, optionalWithoutValue)
{
  std::optional<double> v;
  EXPECT_EQ (v, toJsonAndBack (v));
}

TEST (stl, vector)
{
  std::vector<int> v {{1, 2, 3, 4, 5, 6, 7}};
  EXPECT_EQ (v, toJsonAndBack (v));
}

TEST (stl, array)
{
  std::array<int, 7> v {{1, 2, 3, 4, 5, 6, 7}};
  EXPECT_EQ (v, toJsonAndBack (v));
}

TEST (stl, set)
{
  std::set<std::string> v {{"This", "is", "a", "test"}};
  EXPECT_EQ (v, toJsonAndBack (v));
}

TEST (stl, map)
{
  std::map<int, std::string> v {{{1, "This"}, {2, "is"}, {3, "a"}, {4, "test"}}};
  EXPECT_EQ (v, toJsonAndBack (v));
}
