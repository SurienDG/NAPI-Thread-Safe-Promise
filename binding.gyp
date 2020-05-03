{
  "targets": [
    {
      "target_name": "napi_thread_safe_promise",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": ["promiseWrapper.cc" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "<!@(node -p \"require('napi-thread-safe-callback').include\")"

      ],
     "defines": [ "NAPI_CPP_EXCEPTIONS" ],
     "xcode_settings": {
      "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
      "CLANG_CXX_LIBRARY": "libc++",
      "MACOSX_DEPLOYMENT_TARGET": "10.7",
    },
    "msvs_settings": {
      "VCCLCompilerTool": { "ExceptionHandling": 1 },
    },
    "conditions": [
      ["OS==\"win\"", { "defines": [ "_HAS_EXCEPTIONS=1" ] }]
    ]
    }
  ]
}
