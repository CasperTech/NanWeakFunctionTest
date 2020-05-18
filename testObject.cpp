#include "testObject.h"
#include "CallbackWeakRef.h"
#include <nan.h>
#include <iostream>

Nan::Persistent<v8::Function> TestObject::constructor;

TestObject::TestObject()
{

}

void TestObject::Init(v8::Local<v8::Object> exports)
{
    v8::Local<v8::Context> TestObject = exports->CreationContext();
    Nan::HandleScope scope;

    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("TestObject").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    Nan::SetPrototypeMethod(tpl, "createCallback", CreateCallback);
    Nan::SetPrototypeMethod(tpl, "callCallback", CallCallback);


    constructor.Reset(tpl->GetFunction(TestObject).ToLocalChecked());
    exports->Set(TestObject,
                 Nan::New("TestObject").ToLocalChecked(),
                 tpl->GetFunction(TestObject).ToLocalChecked());
}


void TestObject::New(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
    if (info.IsConstructCall())
    {
        auto* obj = new TestObject();
        obj->Wrap(info.This());
        info.GetReturnValue().Set(info.This());
    }
    else
    {
        v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
        info.GetReturnValue().Set(cons->NewInstance(context).ToLocalChecked());
    }
}

void TestObject::handleCallbackDestroyed(const Nan::WeakCallbackInfo<CallbackWeakRef> &data)
{
    CallbackWeakRef* ref = data.GetParameter();
    static_cast<TestObject*>(ref->objPtr)->callbackDestroyed(ref->callbackID);
    delete ref;
}

void TestObject::callbackDestroyed(const std::string &callbackID)
{
    std::cout << "Destroyed callback " << callbackID << ". Remaining: " << callbacks.size() << std::endl;
    auto it = callbacks.find(callbackID);
    if (it == callbacks.end())
    {
        return;
    }
    callbacks[callbackID]->Reset();
    delete callbacks[callbackID];
    callbacks.erase(it);
}

void TestObject::CallCallback(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    try
    {
        auto * objWrapper = ObjectWrap::Unwrap<TestObject>(info.Holder());
        if (info[0]->IsString())
        {
            std::string callbackID = *Nan::Utf8String(info[0]);
            auto it = objWrapper->callbacks.find(callbackID);
            if (it == objWrapper->callbacks.end())
            {
                return Nan::ThrowError("Callback does not exist");
            }

            Nan::Persistent<v8::Function>* cb = (*it).second;
            v8::Local<v8::Function> fn = Nan::New(*cb);
            v8::Handle<v8::Object> global = v8::Isolate::GetCurrent()->GetCurrentContext()->Global();

            std::vector<v8::Local<v8::Value>> params;
            for (int x = 1; x < info.Length(); x++)
            {
                params.push_back(info[x]);
            }

            Nan::TryCatch tryCatch;
            v8::MaybeLocal<v8::Value> retMaybe = fn->Call(v8::Isolate::GetCurrent()->GetCurrentContext(), global, static_cast<int>(params.size()), &params[0]);
            if (tryCatch.HasCaught())
            {
                v8::Local<v8::Message> msg = tryCatch.Message();
                std::string errStr = *Nan::Utf8String(msg->Get());
                return Nan::ThrowError(errStr.c_str());
            }

            v8::Local<v8::Value> ret = retMaybe.ToLocalChecked();
            info.GetReturnValue().Set(ret);
        }
    }
    catch(const std::exception& err)
    {
        Nan::ThrowError(err.what());
    }
}

void TestObject::CreateCallback(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    try
    {
        auto * objWrapper = ObjectWrap::Unwrap<TestObject>(info.Holder());
        if (info[0]->IsString() && info[1]->IsFunction())
        {
            std::string callbackID = *Nan::Utf8String(info[0]);
            v8::Local<v8::Function> callbackFunction = Nan::To<v8::Function>(info[1]).ToLocalChecked();
            auto* persistedFunc = new Nan::Persistent<v8::Function>(callbackFunction);
            CallbackWeakRef* ref = new CallbackWeakRef();
            ref->callbackID = callbackID;
            ref->objPtr = objWrapper;
            persistedFunc->SetWeak(ref, &TestObject::handleCallbackDestroyed, Nan::WeakCallbackType::kParameter);
            objWrapper->callbacks[callbackID] = persistedFunc;
            std::cout << "Stored callback " << callbackID << std::endl;
        }
        else
        {
            Nan::ThrowError("Invalid parameters");
        }
    }
    catch(const std::exception& err)
    {
        Nan::ThrowError(err.what());
    }
}

