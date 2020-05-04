
#include "promiseWrapper.h"

#include <functional>
#include <memory>
#include <thread>

#include "napi-thread-safe-callback.hpp"

Napi::Promise promiseFuncWrapper(const Napi::Env &env,
                                 const PromiseFunc &promFunc) {
    Napi::Promise::Deferred promise = Napi::Promise::Deferred::New(env);
    Napi::Function resolveInput =
        Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
            Napi::HandleScope scope(info.Env());
        });
    Napi::Function rejectInput =
       Napi::Function::New(env, [promise](const Napi::CallbackInfo& info) {
                promise.Reject(info[0].As<Napi::String>());
        });
    std::shared_ptr<ThreadSafeCallback> resolveInputPtr =
        std::make_shared<ThreadSafeCallback>(resolveInput);
    std::shared_ptr<ThreadSafeCallback> rejectInputPtr =
        std::make_shared<ThreadSafeCallback>(rejectInput);

    // create resolve function
    resolveFunc resolve = [resolveInputPtr, promise](const std::string argsInput) {
        resolveInputPtr->call(
            [&argsInput, &promise](Napi::Env env, std::vector<napi_value> &args) {
              //  Napi::HandleScope scope (env);
              //  promise.Resolve(Napi::String::New(env, argsInput));
                // This will run in main thread and needs to construct the
                // arguments for the call
                args = {Napi::String::New(env, argsInput)};
            });
    };
    // create reject function
    rejectFunc reject = [rejectInputPtr](const std::string &msg) {
        rejectInputPtr->callError(msg);
    };
    // call our function to do work and either resolve or reject a resolution
    promFunc(resolve, reject);

    return promise.Promise();
}
