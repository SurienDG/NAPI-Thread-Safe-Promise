#include "promiseWrapper.h"

#include <functional>
#include <memory>
#include <thread>

struct tfsnContext {
    Napi::Promise::Deferred deffered;
    std::string data;
    bool resolve;
    bool called;
    Napi::ThreadSafeFunction tsfn;
    tfsnContext(Napi::Env env)
        : deffered{Napi::Promise::Deferred::New(env)},
          data{""},
          resolve{false}, called{false} {};
};

void FinalizerCallback(Napi::Env env, void *finalizeData, void *context) {}

Napi::Promise promiseFuncWrapper(const Napi::Env env,
                                 const PromiseFunc &promFunc) {
    std::shared_ptr<tfsnContext> context = std::make_shared<tfsnContext>(env);
    std::shared_ptr<std::mutex> mu = std::make_shared<std::mutex>();
    context->tsfn = Napi::ThreadSafeFunction::New(
        env, Napi::Function::New(env, [](const Napi::CallbackInfo &info) {}),
        "TSFN", 0, 1, [context, mu](Napi::Env env) {
            mu->lock();
            if (context->resolve) {
                context->deffered.Resolve(
                    Napi::String::New(env, context->data));
            } else {
                context->deffered.Reject(
                    Napi::String::New(env, context->data));
            }
            mu->unlock();
        });

    // create resolve function
    resolveFunc resolve = [context, mu](const std::string argsInput) {
        mu->lock();
        context->data = argsInput;
        context->resolve = true;
        if (!context->called){
            context->called = true;
            context->tsfn.Release();
        }
        mu->unlock();
    };
    // create reject function
    rejectFunc reject = [context, mu](const std::string &msg) {
        mu->lock();
        context->data = msg;
        context->resolve = false;
        if (!context->called){
            context->called = true;
            context->tsfn.Release();
        }
        context->tsfn.Release();
        mu->unlock();
    };

    // call our function to do work and either resolve or reject a resolution
    promFunc(resolve, reject);

    return context->deffered.Promise();
}
