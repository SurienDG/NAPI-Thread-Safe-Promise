
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
void promiseFuncWrapper(const Napi::Function &resolveInput, const PromiseFunc &promFunc);

#endif
