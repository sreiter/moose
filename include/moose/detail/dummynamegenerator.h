#pragma once

#include <string>

namespace moose::detail
{
  /// Produces an ordered sequence of names.
  class DummyNameGenerator
  {
  public:
    inline auto getNext () -> std::string
    {
      return mPrefix + std::to_string (mNumber++);
    }

  private:
    inline static const std::string mPrefix {"@noname"};
    size_t mNumber {0};
  };
}
