#pragma once

#include <memory>
#include <string>

class TestObject;
struct CallbackWeakRef
{
    std::string callbackID;
    void* objPtr;
};