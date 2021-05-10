
#ifndef PROMISEWRAPPER_H
#define PROMISEWRAPPER_H

#include <napi.h>
#include <functional>
#include <memory>
#include <thread>

// promise function types
typedef std::function<void(const std::string args)> resolveFunc;
typedef std::function<void(const std::string args)> rejectFunc;

// type of function type which will be passed promise functions
typedef std::function<void(resolveFunc, rejectFunc)> PromiseFunc;

// this will give the promise function (promFunc) the resolve and reject
// functions the promise function can then be used to do the main work
Napi::Promise promiseFuncWrapper(const Napi::Env env,
                                 const PromiseFunc &promFunc);

#define PROMISE(info, funcBody) \
    promiseFuncWrapper(         \
        info.Env(), [&](resolveFunc resolve, rejectFunc reject) funcBody)


// Note: Initially had this code (definitions) in a separate .cc file but various users experienced issues
// with node-gyp when they were on different platforms due to updates with node and node-gyp flow for installing
// packages so to streamline the installation I am now including everything in the header file (.h) instead
struct tfsnContext {
    Napi::Promise::Deferred deffered;
    std::string data;
    bool resolve;
    bool called;
    Napi::ThreadSafeFunction tsfn;
    tfsnContext(Napi::Env env)
        : deffered{Napi::Promise::Deferred::New(env)},
          data{""},
          resolve{false},
          called{false} {};
};

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
                    Napi::Error::New(env, context->data).Value());
            }
            mu->unlock();
        });

    // create resolve function
    resolveFunc resolve = [context, mu](const std::string argsInput) {
        mu->lock();
        context->data = argsInput;
        context->resolve = true;
        if (!context->called) {
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
        if (!context->called) {
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


#endif
