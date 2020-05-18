#pragma once
#include "CallbackWeakRef.h"
#include <nan.h>

#include <memory>
#include <map>

class TestObject: public Nan::ObjectWrap
{
    public:
        static void Init(v8::Local<v8::Object> exports);

        std::map<std::string, Nan::Persistent<v8::Function>*> callbacks;

    private:
        explicit TestObject();
        static void handleCallbackDestroyed(const Nan::WeakCallbackInfo<CallbackWeakRef> &data);
        void callbackDestroyed(const std::string& callbackID);

        static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);
        static void CreateCallback(const Nan::FunctionCallbackInfo<v8::Value> &info);
        static void CallCallback(const Nan::FunctionCallbackInfo<v8::Value> &info);

        static Nan::Persistent<v8::Function> constructor;

};
