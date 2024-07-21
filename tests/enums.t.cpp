#include "utils.h"

#include <moose/moose.h>
#include <moose/stl_serialization.h>

#include <gtest/gtest.h>

using namespace moose;

enum class Color
{
  Red,
  Green,
  Blue
};

template <>
struct moose::TypeTraits<Color> : public EnumToStringTraits<Color> {};

auto const jsonColors = R"""(
  {
    "colors": ["Blue", "Green", "Red"]
  })""";

auto const expectedColors = std::vector<Color> {Color::Blue, Color::Green, Color::Red};

TEST (enums, colorsFromJson)
{
  auto const colors = fromJson<std::vector<Color>> ("colors", jsonColors);
  EXPECT_EQ (colors, expectedColors);
}

TEST (enums, colorsWriteRead)
{
  EXPECT_EQ (expectedColors, toJsonAndBack (expectedColors));
  EXPECT_EQ (expectedColors, toBinaryAndBack (expectedColors));
}
