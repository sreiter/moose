#include <moose/stl_serialization.h>

#include "utils.h"

#include <gtest/gtest.h>

using namespace moose;

template <class T>
struct UnpackMe
{
  auto begin () {return mData.begin ();}
  auto end () {return mData.end ();}
  auto operator <=> (UnpackMe const& other) const = default;

  std::array<T, 2> mData;
};

template <class T>
struct TypeTraits<UnpackMe<T>>
{
  static constexpr EntryType entryType = EntryType::Range;
  static constexpr bool canBeUnpacked = true;
};

template <class T>
auto getDefaultHint (UnpackMe<T> const&) -> Hint
{
  return Hint::OneLine;
}

template <class T>
auto getDefaultHint (std::vector<UnpackMe<T>> const&) -> Hint
{
  return Hint::OneLine;
}

TEST (stl, unpackData)
{
  using Data = UnpackMe<int>;
  EXPECT_EQ (canBeUnpacked<Data> (), true);
  auto const json = R"""({ "pairs": [0, 1, 2, 3, 4, 5] })""";
  auto const jsonArrays = fromJson<std::vector<Data>> ("pairs", json);
  auto const expectedArrays = std::vector<Data> {{0, 1}, {2, 3}, {4, 5}};
  EXPECT_EQ (jsonArrays, expectedArrays);

  auto const json2 = toJson ("pairs", expectedArrays);
  auto const jsonArrays2 = fromJson<std::vector<Data>> ("pairs", json);
  EXPECT_EQ (jsonArrays2, expectedArrays);

  auto const binary = toBinary (expectedArrays);
  auto const binaryArrays = fromBinary<std::vector<Data>> (binary);
  EXPECT_EQ (binaryArrays, expectedArrays);
}
