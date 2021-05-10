{
  "targets": [
    {
      "target_name": "napi_thread_safe_promise",
      "type": "static_library",
      "cflags!": [
        "-fno-exceptions"
      ],
      "cflags_cc!": [
        "-fno-exceptions"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include_dir\")"
      ],
      "defines": [
        "NAPI_CPP_EXCEPTIONS"
      ],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "CLANG_CXX_LIBRARY": "libc++",
        "MACOSX_DEPLOYMENT_TARGET": "10.7"
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1
        }
      },
      "conditions": [
        [
          "OS==\"win\"",
          {
            "defines": [
              "_HAS_EXCEPTIONS=1"
            ]
          }
        ],
        [
          "OS==\"mac\"",
          {
            "cflags+": [
              "-fvisibility=hidden"
            ],
            "xcode_settings": {
              "GCC_SYMBOLS_PRIVATE_EXTERN": "YES"
            }
          }
        ]
      ]
    }
  ]
}