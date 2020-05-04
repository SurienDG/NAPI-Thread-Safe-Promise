
#ifndef PROMISEWRAPPER_H
#define PROMISEWRAPPER_H

#include <napi.h>
#include <functional>

// promise function types
typedef std::function<void(const std::string args)> resolveFunc;
typedef std::function<void(const std::string args)> rejectFunc;

// type of function type which will be passed promise functions
typedef std::function<void(resolveFunc, rejectFunc)> PromiseFunc;

// this will give the promise function (promFunc) the resolve and reject functions
// the promise function can then be used to do the main work
void promiseFuncWrapper(const Napi::Function &resolveInput,
                        const Napi::Function &rejectInput,
                        const PromiseFunc &promFunc);

#define PROMISE(info, funcBody)                                                                                      \
    if (info.Length() < 2)                                                                                 \
    {                                                                                                      \
        throw Napi::Error::New(info.Env(), "requires at least two function arguments");                    \
    }                                                                                                      \
promiseFuncWrapper(info[info.Length()-2].As<Napi::Function>(), info[info.Length()-1].As<Napi::Function>(), \
                      [&info](resolveFunc resolve, rejectFunc reject) funcBody)

#endif
