#include "testObject.h"

using v8::FunctionTemplate;

void InitAll(v8::Local<v8::Object> exports)
{
    TestObject::Init(exports);
}

NODE_MODULE(duktape, InitAll)