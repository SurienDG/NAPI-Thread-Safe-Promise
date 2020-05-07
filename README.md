# NAPI-Thread-Safe-Promise

[![npm version](https://badge.fury.io/js/napi_thread_safe_promise.svg)](https://badge.fury.io/js/napi_thread_safe_promise)
![https://img.shields.io/maintenance/yes/2020](https://img.shields.io/maintenance/yes/2020)
![https://img.shields.io/github/license/SurienDG/NAPI-Thread-Safe-Promise](https://img.shields.io/github/license/SurienDG/NAPI-Thread-Safe-Promise)

- [NAPI-Thread-Safe-Promise](#napi-thread-safe-promise)
  - [Introduction](#introduction)
  - [Usage](#usage)
      - [Exception handling](#exception-handling)
  - [Examples](#examples)
    - [Async Promise Example (with macro)](#async-promise-example-with-macro)
    - [Async Promise Example (without using macro)](#async-promise-example-without-using-macro)
    - [JavaScript code for the code examples above](#javascript-code-for-the-code-examples-above)
  - [Contribution](#contribution)
  - [License](#license)

## Introduction

**NOTE**: This package assumes understanding of N-API

To be used in conjunction with the N-API module to call C++ code from JavaScript.  

Allows the user to call thread safe promise functions such (resolve, reject) in the C++ code and return a promise to the javascript code.

## Usage

1. Install the package using node package manager: 

```sh
npm install napi_thread_safe_promise
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

To have the ability for exceptions add the following to `binding.gyp`:

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

## Examples

### Async Promise Example (with macro)

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
Napi::Promise promiseFuncJS(const CallbackInfo& info) {
  return promiseFuncWrapper(info.Env(),
      
      [&info](resolveFunc resolve, rejectFunc reject) { // anonymous function passed to thread safe resolve and reject functions

        // here we can write our threaded code

          std::string arg1 = info[0].As<Napi::String>();
          std::thread([resolve, reject, arg1]() {
            reject(arg1);
          }).detach();
      }
  );
}
```

### JavaScript code for the code examples above

```Javascript
promiseFuncJS(test)
  .then(output => {
    console.log(output);
  })
  .catch(err => {
    console.error(err);
  });
```

## Contribution

For contribution to this package, create a pull request and the maintainer will look into it. 

## License

NAPI-Thread-Safe-Promise is licensed under BSD-3-Clause.
