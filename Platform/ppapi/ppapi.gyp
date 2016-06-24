{
  'includes': [
    'filenames.gypi'
  ],
  'variables': {
    'NACL_SDK_ROOT': './vendor/nacl-sdk/pepper_49',
    'PPAPI_INCLUDE_DIR': '<(NACL_SDK_ROOT)/include',
    'SYS_INCLUDE_DIR': '/usr/include/x86_64-linux-gnu/',
    'LIBXML2_INCLUDE_DIR': '/usr/include/libxml2',
    'SYS_LIBRARY_DIR': '/usr/lib/i386-linux-gnu',
    'ROOT_DIR': '../../ePub3',
    'THIRD_PARTY_DIR': '<(ROOT_DIR)/ThirdParty',
    'ZIP_DIR': '<(THIRD_PARTY_DIR)/libzip'
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
      #'-D_GLIBCXX_USE_CXX11_ABI=0',
      '-g'
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
        '<(PPAPI_INCLUDE_DIR)',
        '<(LIBXML2_INCLUDE_DIR)',
      ],

      'ldflags': [
        #'-L<(SYS_LIBRARY_DIR)',
        #'-L../../<(NACL_SDK_ROOT)/lib/linux_host/Release'
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
        '<(PPAPI_INCLUDE_DIR)'
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
        '<(LIBXML2_INCLUDE_DIR)',
        '<(SYS_INCLUDE_DIR)',
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
      'include_dirs': [
        '<(libzip_dir)',
        '<(SYS_INCLUDE_DIR)'
      ],
      'sources': [
        '<@(libzip_sources)'
      ]
    },
    {
      'target_name': 'google_url',
      'type': 'static_library',
      'include_dirs': [
        '<(LIBXML2_INCLUDE_DIR)'
      ],
      'sources': [
        '<@(google_url_sources)'
      ]
    },
  ],
}