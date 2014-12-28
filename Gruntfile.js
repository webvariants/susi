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
            '<%= pkg.project.directories.build %>/watchdog',
            '<%= pkg.project.directories.build %>/jsengine'
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
      cmake_jsengine: {
        command: 'cmake -DPOCO_INCLUDE_DIR=../../node_modules/poco/include -DSOCI_INCLUDE_DIR=../../node_modules/soci/include/soci -DCMAKE_LIBRARY_PATH=../../node_modules/ ../../<%= pkg.project.directories.jsengine %>',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>/jsengine'
          }
        }
      },
      cmake_watchdog: {
        command: 'cmake ../../<%= pkg.project.directories.watchdog %>',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>/watchdog'
          }
        }
      },
      make_core: {
        command: 'make -j4 susi',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>/core'
          }
        }
      },
      make_jsengine: {
        command: 'make -j4',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>/jsengine'
          }
        }
      },
      make_watchdog: {
        command: 'make -j4',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>/watchdog'
          }
        }
      },
      test_core: {
        command: './susi_test --gtest_filter="'+(grunt.option("filter")||"*")+'"',
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
      jsengine: {
        options: {
          mode: true
        },
        files: [
          {
            expand: true,
            flatten: true,
            src: ['<%= pkg.project.directories.build %>/jsengine/susi-jsengine'],
            dest: '<%= pkg.project.directories.bin %>'
          }
        ]
      },
      watchdog: {
        options: {
          mode: true
        },
        files: [
          {
            expand: true,
            flatten: true,
            src: ['<%= pkg.project.directories.build %>/watchdog/watchdog'],
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
    },
  });

  grunt.loadNpmTasks('grunt-shell');
  grunt.loadNpmTasks('grunt-mkdir');
  grunt.loadNpmTasks('grunt-contrib-clean');
  grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.loadNpmTasks('grunt-newer');
  grunt.loadNpmTasks('grunt-debian-package');

  var register = function(name){
    grunt.registerTask(name,'build '+name,[
      'mkdir',
      'shell:cmake_'+name,
      'shell:make_'+name,
      'newer:copy:'+name
    ]);
  };

  register('core');
  register('jsengine');
  register('watchdog');

  grunt.registerTask('test', 'run the susi tests', ['development','shell:test_core']);
  
  grunt.registerTask('development', ['core','jsengine','watchdog']);
  grunt.registerTask('build', ['clean','core','jsengine','watchdog']);

  grunt.registerTask('dev',['development']);  
  grunt.registerTask('default', ['development']);

};
