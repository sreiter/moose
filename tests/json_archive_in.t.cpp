#include <array>
#include <moose/moose.h>

#include <gtest/gtest.h>

using namespace moose;

auto const jsonInputData = R"""(
  {
    "array": [100, 101],
    "double": 123.321,
    "string": "test"
  })""";

class JSONArchiveInFixture : public ::testing::Test
{
public:
  JSONArchiveInFixture ()
    : archive {JSONReader::fromString (jsonInputData)}
  {}
protected:
  Archive archive;
};

TEST_F (JSONArchiveInFixture, readDouble)
{
  double d;
  archive ("double", d);
  EXPECT_EQ (d, 123.321);
}

TEST_F (JSONArchiveInFixture, readString)
{
  std::string s;
  archive ("string", s);
  EXPECT_EQ (s, "test");
}

TEST_F (JSONArchiveInFixture, readArray)
{
  std::array<int, 2> a;
  archive ("array", a);
  std::array<int, 2> const expectedA {100, 101};
  EXPECT_EQ (a, expectedA);
}
