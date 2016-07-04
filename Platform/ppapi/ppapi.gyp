{
  'includes': [
    'filenames.gypi'
  ],
  'variables': {
    'nacl_sdk_dir': './vendor/nacl-sdk/pepper_49',
    'ppapi_include_dir': '<(nacl_sdk_dir)/include'
  },
  'target_defaults': {
    'include_dirs': [
      './include'
    ],
    'cflags': [
      '-w',
      '-fPIC', 
      '-fvisibility=hidden',
      #'-m32',
      #'-D_GLIBCXX_USE_CXX11_ABI=0'
    ],
    'cflags_cc': [
      '-std=c++11',
      '-fpermissive'
    ],
    'defines': [
      'BUILDING_EPUB3'
    ]
  },
  'targets': [
    {
      'target_name': 'readium',
      'type': 'executable',
      'include_dirs': [
        '<(ppapi_include_dir)'
      ],

      'ldflags': [
        # '-m32',
      ],
     
      'dependencies': [
        'epub3',
        'ppapi',
        'libxml2'
      ],
      'sources': [
        'src/readium.cc'
      ]
    },
    {
      'target_name': 'ppapi',
      'type': 'static_library',
      'include_dirs': [
        '<(ppapi_include_dir)'
      ],
      'sources': [
        '<@(ppapi_sources)'
      ]
    },
    {
      'target_name': 'libxml2',
      'type': 'static_library',
      'include_dirs': [
        '<(libxml2_include_dir)'
      ],
      'sources': [
        '<@(libxml2_sources)'
      ]
    },
    {
      'target_name': 'epub3',
      'type': 'static_library',
      'dependencies': [
        'libzip',
        'google_url',
      ],
      'include_dirs': [
        './include/ePub3',
        './include/ePub3/utilities'
      ],
      'sources': [
        '<@(epub3_sources)'
      ]
    },
    {
      'target_name': 'libzip',
      'type': 'static_library',
      'sources': [
        '<@(libzip_sources)'
      ]
    },
    {
      'target_name': 'google_url',
      'type': 'static_library',
      'sources': [
        '<@(google_url_sources)'
      ]
    },
  ],
  'conditions': [
    ['OS=="linux"', {
        'variables': {
          'libxml2_include_dir': '/usr/include/libxml2',
        },
        'target_defaults': {
          'include_dirs': [
            '<(libxml2_include_dir)'
          ],
          'link_settings': {
            'libraries': [
              '-lxml2',
              '-lz',
              '-lpthread',
              '-lcrypto'
            ]
          }
        }
    }],
    ['OS=="win"', {
        'variables': {
          'win_platform_dir': '../Windows/ReadiumSDK/Prebuilt',
          'win_platform_include_dir': '<(win_platform_dir)/Include',
          'win_platform_lib_dir': '<(win_platform_dir)/Lib/x86',
          'libxml2_include_dir': './vendor/libxml2/include'
        },
        'target_defaults': {
          'defines': [
            'MSVS_VERSION_2015',
            'NDEBUG',
            'NOMINMAX',
            'WIN32',
            'XML_BUFFER_ALLOC_BOUNDED=6'
          ],
            'msvs_disabled_warnings': [
              4530,
              4577,
              4028,
              4090
            ],
            'msvs_settings': {
              'VCCLCompilerTool': {
                'WarnAsError': 'false',
              },
            },
          'cflags': [
            '/EHsc'
          ],
          'include_dirs': [
            '<(win_platform_include_dir)'
          ],
          'link_settings': {
            'library_dirs': [
              '<(win_platform_lib_dir)'
            ],
            'libraries': [
              '-lzlib.lib',
              '-lws2_32.lib',
              '-lmsvcrt.lib',
              '-llegacy_stdio_definitions.lib',
              '-ladvapi32.lib'
            ]
          }
        }
    }],
  ]
}