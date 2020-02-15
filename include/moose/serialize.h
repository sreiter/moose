#pragma once

namespace moose
{

class Archive;

template <class T>
void Serialize (Archive& archive, T& value)
{
  value.serialize (archive);
}

}// end of namespace
