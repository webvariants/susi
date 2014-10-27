module.exports = function(grunt) {

  // Project configuration.
  grunt.initConfig({
    
    pkg: grunt.file.readJSON('package.json'),

    clean: {
      bin: ['<%= pkg.project.directories.bin %>'],
      build: ['<%= pkg.project.directories.build %>']
    },

    mkdir: {
      all: {
        options: {
          create: [
            '<%= pkg.project.directories.build %>/core',
            '<%= pkg.project.directories.build %>/cppengine'
          ]
        }
      }
    },

    shell: {
      cmake_core: {
        command: 'cmake -DPOCO_INCLUDE_DIR=../../node_modules/poco/include -DSOCI_INCLUDE_DIR=../../node_modules/soci/include/soci -DCMAKE_LIBRARY_PATH=../../node_modules/ ../../<%= pkg.project.directories.core %>',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>/core'
          }
        }
      },
      cmake_cppengine: {
        command: 'cmake -DPOCO_INCLUDE_DIR=../../node_modules/poco/include -DSOCI_INCLUDE_DIR=../../node_modules/soci/include/soci -DCMAKE_LIBRARY_PATH=../../node_modules/ ../../<%= pkg.project.directories.cppengine %>',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>/cppengine'
          }
        }
      },
      make_core: {
        command: 'make -j4',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>/core'
          }
        }
      },
      make_cppengine: {
        command: 'make -j4',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>/cppengine'
          }
        }
      },
      test_core: {
        command: './susi_test',
        options: {
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>/core'
          }
        }
      }
    },

    copy: {
      core: {
        options: {
          mode: true
        },
        files: [
          {
            expand: true,
            flatten: true,
            src: [
              '<%= pkg.project.directories.build %>/core/susi',
              '<%= pkg.project.directories.build %>/core/libsusi.so'
            ],
            dest: '<%= pkg.project.directories.bin %>'
          }
        ]
      },
      cppengine: {
        options: {
          mode: true
        },
        files: [
          {
            expand: true,
            flatten: true,
            src: ['<%= pkg.project.directories.build %>/cppengine/libsusicpp.so'],
            dest: '<%= pkg.project.directories.bin %>'
          }
        ]
      }
    },
    watch: {
      sources: {
        files: ['**/*.cpp','**/*.h'],
        tasks: ['development','watch:sources']
      }
    }
  });

  grunt.loadNpmTasks('grunt-shell');
  grunt.loadNpmTasks('grunt-mkdir');
  grunt.loadNpmTasks('grunt-contrib-clean');
  grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.loadNpmTasks('grunt-newer');

  grunt.registerTask('build', 'generate a clean build of everything', [
    'clean:bin',
    'clean:build',
    'mkdir:all',
    'shell:cmake_core',
    'shell:make_core',
    'copy:core',
    'shell:cmake_cppengine',
    'shell:make_cppengine',
    'copy:cppengine'
  ]);

  grunt.registerTask('development', 'update all changes', [
    'mkdir:all',
    'shell:cmake_core',
    'shell:make_core',
    'newer:copy:core',
    'shell:cmake_cppengine',
    'shell:make_cppengine',
    'newer:copy:cppengine'
  ]);

  grunt.registerTask('test', 'run the susi tests', ['development','shell:test_core']);
  grunt.registerTask('dev', ['development']);
  grunt.registerTask('default', ['development']);

};
