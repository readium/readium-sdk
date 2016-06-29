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
      '-DBUILDING_EPUB3',
      #'-D_GLIBCXX_USE_CXX11_ABI=0'
    ],
    'cflags_cc': [
      '-std=c++11',
      '-fpermissive'
    ]
  },
  'targets': [
    {
      'target_name': 'readium',
      'type': 'shared_library',
      'include_dirs': [
        '<(ppapi_include_dir)'
      ],

      'ldflags': [
        # '-m32',
      ],
      'libraries': [
        '-lxml2',
        '-lz',
        '-lpthread',
        '-lcrypto'
      ],
      'dependencies': [
        'epub3',
        'ppapi'
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
          ]
        }
    }],
  ]
}