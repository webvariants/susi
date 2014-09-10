module.exports = function(grunt) {

  grunt.config.merge({
    watch: {
      files: [
        '<%= pkg.project.directories.src %>',
      ],
      tasks: ['build'],
      options: {
        spawn: false,
      },
    },

    clean: {
      build: ['<%= pkg.project.directories.bin %>']
    },

    //@TODO automate copy for bin folders
    copy: {
      // susi: {
      //   files: [{
      //     expand: true,
      //     cwd: '<%= pkg.project.directories.src %>susi/bin/',
      //     src: ['**'],
      //     dest: '<%= pkg.project.directories.bin %>susi/'
      //   }]
      // }
    },

    externalcomponents: {
      build: {
        options: {
          cmd: 'build'
        }
      }
    }
  });

  //@TODO refactor and create as plugin
  var gruntRemote = function (cmd, cwd, done) {
    var spawn  = require('child_process').spawn,
        remote = spawn('grunt', [cmd], {cwd: cwd});

    remote.stdout.on('data', function (data) {
      grunt.log.write(data);
    });
    remote.stderr.on('data', function (data) {
      grunt.fail.fatal(data);
    });
    remote.on('close', function (code) {
      grunt.log.ok(cmd+' '+cwd+' finished ('+code+')');
      done();
    });
  };

  grunt.registerMultiTask('externalcomponents', '', function () {
    var terminate = this.async();
    var cmd = this.options().cmd;
    var j = 0;
    var done = function () {
      j--;
      if (j === 0) terminate();
    };
    var components = grunt.config('pkg.project.components');
    for (var c in components) {
      j++;
      gruntRemote(cmd, components[c], done);
    }
  });

  grunt.registerTask('build', '', [
      'clean:build',
      'externalcomponents:build',
      // @TODO copy components tasks
      'copy:bridge',
      'copy:susi',
  ]);
};
