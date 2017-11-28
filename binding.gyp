{
  "targets": [
    {
        "target_name": "brocken",
        "sources": [ 
            "src/main.cpp",
            "src/Utils.cpp"
        ],

        "libraries": [
            "<!@(node utils/find-opencv.js --libs)"
        ],

        "include_dirs": [
            "<!@(node utils/find-opencv.js --cflags)",
            "<!(node -e \"require('nan')\")"
        ],

        "cflags!": ["-fno-exceptions"],
        "cflags_cc!": ["-fno-rtti",  "-fno-exceptions"],

        "conditions": [
            [ "OS==\"linux\" or OS==\"freebsd\" or OS==\"openbsd\" or OS==\"solaris\" or OS==\"aix\"", {
                "cflags": [
                "<!@(node utils/find-opencv.js --cflags)",
                "-Wall"
                ]
            }],
            [ "OS==\"win\"", {
                "cflags": [
                    "-Wall"
                ],
                "defines": [
                    "WIN"
                ],
                "msvs_settings": {
                "VCCLCompilerTool": {
                    "ExceptionHandling": "2",
                    "DisableSpecificWarnings": [ "4530", "4506", "4244" ],
                },
                }
            }],
            [ # cflags on OS X are stupid and have to be defined like this
            "OS==\"mac\"", {
                "xcode_settings": {
                "OTHER_CFLAGS": [
                    "-mmacosx-version-min=10.7",
                    "-std=c++11",
                    "-stdlib=libc++",
                    "<!@(node utils/find-opencv.js --cflags)",
                ],
                "GCC_ENABLE_CPP_RTTI": "YES",
                "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
                }
            }
            ]
        ],

        "configurations": {
            # This is used for generating code coverage with the `--debug` argument
            "Debug": {
            "conditions": [
                ['OS=="linux"', {
                "cflags": ["-coverage"],
                "ldflags": ["-coverage"]
                }],
                ['OS=="mac"', {
                "xcode_settings": {
                    "OTHER_CFLAGS": [
                    "-fprofile-arcs -ftest-coverage",
                    ],
                    "OTHER_LDFLAGS": [
                    "--coverage"
                    ]
                }
                }]

            ]
            },
        }
    }
  ]
}