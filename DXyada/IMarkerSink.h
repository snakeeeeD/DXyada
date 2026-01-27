#pragma once
#include "Object.h"

class IMarkerSink
{
public:
    virtual ~IMarkerSink() {}
    virtual void SpawnMarkAt(const Object& src) = 0;
};
