{
  'includes': [ 'common.gypi' ],
  'targets': [
    {
      'target_name': 'protobuf2json-c',
      'type': '<(protobuf2json_c_library)',
      'include_dirs': [
        'include',
        'src'
      ],
      'sources': [
        'src/protobuf2json.c'
      ],
      'link_settings': {
        'libraries': [
          '-lprotobuf-c',
          '-ljansson'
        ]
      },
      'defines': [
        'PRINT_UNPACK_ERRORS=0'
      ]
    },
    {
      'target_name': 'run-tests',
      'type': 'executable',
      'dependencies': [ 'protobuf2json-c' ],
      'include_dirs': [
        'include'
      ],
      'sources': [
        'test/run-tests.c',
        'test/test-list.h',
        'test/test-protobuf2json.c',
        'test/test-json2protobuf.c',
        'test/runner.c',
        'test/runner.h',
        'test/task.h',
        'test/person.pb-c.c',
        'test/test.pb-c.c',
        'test/getrusage-helper.h'
      ],
      'conditions': [
        [
          'OS=="win"',
          {
            'sources': [
              'test/runner-win.c',
              'test/runner-win.h'
            ]
          }, {
            'sources': [
              'test/runner-unix.c',
              'test/runner-unix.h'
            ]
          }
        ],
        [
          'OS=="solaris"',
          {
            'cflags': [ '-pthreads' ],
            'ldflags': [ '-pthreads' ]
          }
        ],
        [
          'OS not in "solaris android"',
          {
            'cflags': [ '-pthread' ],
            'ldflags': [ '-pthread' ]
          }
        ]
      ],
      'link_settings': {
        'libraries': [
          '-lprotobuf-c',
          '-ljansson'
        ]
      }
    },
    {
      'target_name': 'run-benchmarks',
      'type': 'executable',
      'dependencies': [ 'protobuf2json-c' ],
      'include_dirs': [
        'include'
      ],
      'sources': [
        'test/run-benchmarks.c',
        'test/benchmarks-list.h',
        'test/benchmark-dummy.c',
        'test/runner.c',
        'test/runner.h',
        'test/task.h',
        'test/person.pb-c.c',
        'test/test.pb-c.c',
        'test/getrusage-helper.h'
      ],
      'conditions': [
        [
          'OS=="win"',
          {
            'sources': [
              'test/runner-win.c',
              'test/runner-win.h'
            ]
          }, {
            'sources': [
              'test/runner-unix.c',
              'test/runner-unix.h'
            ]
          }
        ],
        [
          'OS=="solaris"',
          {
            'cflags': [ '-pthreads' ],
            'ldflags': [ '-pthreads' ]
          }
        ],
        [
          'OS not in "solaris android"',
          {
            'cflags': [ '-pthread' ],
            'ldflags': [ '-pthread' ]
          }
        ]
      ],
      'link_settings': {
        'libraries': [
          '-lprotobuf-c',
          '-ljansson'
        ]
      }
    },
    {
      'target_name': 'run-tmp',
      'type': 'executable',
      'dependencies': [ 'protobuf2json-c' ],
      'include_dirs': [
        'include'
      ],
      'sources': [
        'test/run-tmp.c',
        'test/person.pb-c.c',
        'test/test.pb-c.c'
      ],
      'link_settings': {
        'libraries': [
          '-lprotobuf-c',
          '-ljansson'
        ]
      }
    }
  ]
}
