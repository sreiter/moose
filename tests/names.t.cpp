#include <moose/stl_serialization.h>

#include "utils.h"

#include <gtest/gtest.h>

namespace
{
  using namespace moose;

  struct NoName
  {
    std::string mName {"NoName"};
    int mValue {0};

    auto operator <=> (NoName const&) const = default;
    void serialize (Archive& ar)
    {
      ar (mName);
      ar (mValue);
    }
  };

  struct NameWithNoName
  {
    NoName mChild;
    std::string mName {"NameWithNoName"};
    int mValue {1};

    auto operator <=> (NameWithNoName const&) const = default;

    void serialize (Archive& ar)
    {
      ar ("child", mChild);
      ar ("name", mName);
      ar ("value", mValue);
    }
  };

  struct NoNameWithNameWithNoName
  {
    NameWithNoName mChild;
    int mValue {2};

    auto operator <=> (NoNameWithNameWithNoName const&) const = default;
    void serialize (Archive& ar)
    {
      ar (mChild);
      ar (mValue);
    }
  };
}

TEST (stl, noNameWithNameWithNoName)
{
  NoNameWithNameWithNoName const v;
  EXPECT_EQ (v, toJsonAndBack (v));
  EXPECT_EQ (v, toBinaryAndBack (v));
}

TEST (stl, arrayOfNameWithNoName)
{
  std::array<NameWithNoName, 2> const v;
  EXPECT_EQ (v, toJsonAndBack (v));
  EXPECT_EQ (v, toBinaryAndBack (v));
}
