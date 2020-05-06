# NAPI-Thread-Safe-Promise

This npm package is for use in the C++ code to faciliate threadsafe promise functions in the C++ code.

## Examples


### C++ Async Promise Example (made easier with macro)
```C++
Napi::Promise promiseFuncJS(const CallbackInfo& info)
{
  // macro takes in info variable as input
  return PROMISE(info, {
      // now we can write our code here with access to the resolve and reject functions

      // we can now take in resolve and reject function to our threaded function because they are thread safe
      // note other variables we declared here may not be thread safe (ex. info variable is not thread safe) 
      std::thread([resolve, reject]() {
          // we can pass strings to our resolve and reject functions
          resolve("test");

          // note secondary calls to resolve or reject will have no effect
           resolve("test123");
      }).detach();

  });
}
```
**Note:** if we wish to pass json data to our resolve function we just pass in the json string to the function and parse it in the javascript code
> We can use a library like https://github.com/nlohmann/json and use the dump function to accomplish this

### Async Promise Example (without using macro)
```C++
Napi::Promise promiseFuncJS(const CallbackInfo& info)
{  

    return promiseFuncWrapper(info.Env(),
                       [&info](resolveFunc resolve, rejectFunc reject) { // this is a function which we will pass our thread safe resolve and reject functions too

                            // here we can write our threaded code
                           std::string arg1 = info[0].As<Napi::String>();
                           std::thread([resolve, reject, arg1]() {
                               reject(arg1);
                           }).detach();
                       });
}
```

## Corresponding Javascript code for the code examples examples above by could look like this

```Javascript

 promiseFuncJS(test).then(output => {
      console.log(output);
 }).catch(err => {
      console.error(err);
 });

```


## Usage

  1. Add a dependency on this package to `package.json`: 
   ```
   npm i napi_thread_safe_promise
   ```
  2. Reference this package's include directory in `binding.gyp`:
```gyp
  'include_dirs': ["<!@(node -p \"require('napi_thread_safe_promise').include\")"],
   "libraries": ["<!@(node -p \"require('napi_thread_safe_promise').libraries\")" ],
```
  3. Include the header in your code:
```C++
#include "promiseWrapper.h"
```

### Exception handling

If we wish to have the ability for exceptions add the following to  `binding.gyp`:
```gyp
  'cflags!': [ '-fno-exceptions' ],
  'cflags_cc!': [ '-fno-exceptions' ],
  'xcode_settings': {
    'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
    'CLANG_CXX_LIBRARY': 'libc++',
    'MACOSX_DEPLOYMENT_TARGET': '10.7',
  },
  'msvs_settings': {
    'VCCLCompilerTool': { 'ExceptionHandling': 1 },
  },
  'conditions': [
    ['OS=="win"', { 'defines': [ '_HAS_EXCEPTIONS=1' ] }]
  ]
```
