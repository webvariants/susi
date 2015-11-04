'use strict';

module.exports = function(grunt) {

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
            '<%= pkg.project.directories.build %>'
          ]
        }
      }
    },

    shell: {
      cmakeDebug: {
        command: 'cmake -DCMAKE_BUILD_TYPE=Debug ..',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>'
          }
        }
      },
      cmakeRelease: {
        command: 'cmake -DCMAKE_BUILD_TYPE=Release ..',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>'
          }
        }
      },
      make: {
        command: 'make -j8',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>'
          }
        }
      },
      makeInstall: {
        command: 'sudo make -j8 install',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>'
          }
        }
      },
      makePackage: {
        command: 'make -j8 package',
        options: {
          stderr: false,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>'
          }
        }
      },
      strip: {
        command: 'strip bin/* lib/*',
        options: {
          stderr: true,
          execOptions: {
            cwd: '<%= pkg.project.directories.build %>'
          }
        }
      }
    },

    copy: {
      package: {
        options: {
          mode: true
        },
        files: [{
          expand: true,
          flatten: true,
          src: [
            '<%= pkg.project.directories.build %>/susi-*.deb',
          ],
          dest: '<%= pkg.project.directories.build %>..'
        }]
      },
      core: {
        options: {
          mode: true
        },
        files: [{
          expand: true,
          flatten: true,
          src: [
            '<%= pkg.project.directories.build %>/bin/susi-*',
            '<%= pkg.project.directories.build %>/lib/lib*.so'
          ],
          dest: '<%= pkg.project.directories.bin %>'
        }]
      }
    },
    watch: {
      sources: {
        files: ['**/*.cpp', '**/*.h'],
        tasks: ['development', 'watch:sources']
      }
    },
  });

  grunt.loadNpmTasks('grunt-shell');
  grunt.loadNpmTasks('grunt-mkdir');
  grunt.loadNpmTasks('grunt-contrib-clean');
  grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.loadNpmTasks('grunt-newer');

  grunt.registerTask('build', 'build everything', [
    'clean',
    'mkdir',
    'shell:cmakeDebug',
    'shell:make',
    'copy'
  ]);

  grunt.registerTask('development', 'build everything, but do not clean directories before', [
    'mkdir',
    'shell:cmakeDebug',
    'shell:make',
    'copy'
  ]);


  grunt.registerTask('release', 'build everything, but do not clean directories before', [
    'clean',
    'mkdir',
    'shell:cmakeRelease',
    'shell:makePackage',
    'copy:package'
  ]);

  grunt.registerTask('install', ['shell:makeInstall']);

  grunt.registerTask('default', ['development']);

};
