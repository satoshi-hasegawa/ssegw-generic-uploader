{
  'variables': {
    'sseutils_root': './moat-c-utils',
  },
 'includes': [
    'common.gypi',
    'config.gypi',
   './moat-c-utils/sseutils.gypi',
  ],
  'targets': [
    # your M2M/IoT application
    {
      'target_name': '<(package_name)',
      'sources': [
        '<@(sseutils_src)',
        'src/<(package_name).cc',
        'src/sensor.cc',
        'src/external_process_sensor.cc',
        'src/queue.cc',
        'src/uploader.cc',
        'src/periodic_uploader.cc',
       ],
      'product_prefix': '',
      'type': 'shared_library',
      'cflags': [ '-fPIC' ],
      'include_dirs' : [
        '<(sseutils_include)',
      ],
      'libraries': [
      ],
      'dependencies': [
      ],
    },

  ],
}
