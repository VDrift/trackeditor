import os, sys, errno, SCons
from time import gmtime, strftime

#---------------#
# Build Options #
#---------------#
opts = Options('vdrift.conf')
#opts.Add('settings', 'Directory under user\'s home dir where settings will be stored', '.vdrift')
#opts.Add('prefix', 'Path prefix where VDrift data will be installed', '/usr/share/games/vdrift')
#opts.Add(PathOption('bin', 'Path where vdrift binary executable will be installed', '/usr/games'))
#opts.Add(BoolOption('release', 'Turn off debug option during build', 0))

#--------------------------#
# Create Build Environment #
#--------------------------#
env = Environment(ENV = os.environ,
                  CPPPATH = ['#include'],
		  CCFLAGS = ['-Wall', '-Wno-non-virtual-dtor'],
		  LIBPATH = ['.', '#lib'],
		  options = opts)

#--------------#
# Save Options #
#--------------#
opts.Update(env)
opts.Save('vdrift.conf', env)

#--------------------------------------------#
# distcopy and tarballer functions           #
# by Paul Davis <paul@linuxaudiosystems.com> #
#--------------------------------------------#

def distcopy (target, source, env):
    treedir = str (target[0])

    try:
        os.mkdir (treedir)
    except OSError, (errnum, strerror):
        if errnum != errno.EEXIST:
            print 'mkdir ', treedir, ':', strerror

    cmd = 'tar cf - '
    #
    # we don't know what characters might be in the file names
    # so quote them all before passing them to the shell
    #
    all_files = ([ str(s) for s in source ])
    cmd += " ".join ([ "'%s'" % quoted for quoted in all_files])
    cmd += ' | (cd ' + treedir + ' && tar xf -)'
    p = os.popen (cmd)
    return p.close ();

def tarballer (target, source, env):            
    cmd = 'tar -jcf ' + str (target[0]) +  ' ' + str(source[0]) + "  --exclude '*~'"
    print 'running ', cmd, ' ... '
    p = os.popen (cmd)
    return p.close ()

dist_bld = Builder (action = distcopy,
                    target_factory = SCons.Node.FS.default_fs.Entry,
                    source_factory = SCons.Node.FS.default_fs.Entry,
                    multi = 1)

tarball_bld = Builder (action = tarballer,
                       target_factory = SCons.Node.FS.default_fs.Entry,
                       source_factory = SCons.Node.FS.default_fs.Entry)

env.Append (BUILDERS = {'Distribute' : dist_bld})
env.Append (BUILDERS = {'Tarball' : tarball_bld})

#--------------------------------------------------#
# Builder for untarring something into another dir #
#--------------------------------------------------#
def tarcopy(target, source, env):
    cmd = 'tar zxf ' + source + ' -C ' + target
    p = os.popen(cmd)
    return p.close()

copy_tar_dir = Builder (action = tarcopy,
                        target_factory = SCons.Node.FS.default_fs.Entry,
                        source_factory = SCons.Node.FS.default_fs.Entry)

env.Append (BUILDERS = {'TarCopy' : copy_tar_dir})

#-----------------#
# Parse Arguments #
#-----------------#
#fmod = ARGUMENTS.get('fmod', 0)
#bin = ARGUMENTS.get('bin', '/usr/games')
#prefix = ARGUMENTS.get('prefix', '/usr/share/games/vdrift')
#settings = ARGUMENTS.get('settings', '.vdrift')
#release = ARGUMENTS.get('release', 0)

#if int(fmod):
#    sound = 'fmod-3.74'
#else:
#    sound = 'openal'

#------------#
# Build Help #
#------------#
Help("""
Type: 'scons' to compile with the default options.
%s """ % opts.GenerateHelpText(env))

#--------------------------#
# Check for Libs & Headers #
#--------------------------#
conf = Configure(env)

#for lib in check_libs:
#    if not conf.CheckLib(lib):
#        print 'You do not have the %s library installed. Exiting.' % lib
#        Exit(1)

SDL_headers = ['GL/gl.h', 'GL/glu.h', 'SDL/SDL.h', 'SDL/SDL_image.h']

for header in SDL_headers:
    if not conf.CheckCXXHeader(header):
        print 'You do not have the %s headers installed. Exiting.' % header
        Exit(1)

env = conf.Finish()

#---------------#
# Debug/release #
#---------------#
#if not int(release):
env.Append(CCFLAGS = ['-g', '-O2'])

#---------#
# Version #
#---------#
version = strftime("%Y-%m-%d")
env.Append(CCFLAGS = ['-DVERSION=\\"%s\\"' % version])
#-----------------#
# Create Archives #
#-----------------#
src_dir_name = 'build/vdrift-%s-src' % version
bin_dir_name = 'build/vdrift-%s-bin' % version
# scons_local_tarball = 'tools/scons-local-0.96.1.tar.gz'

src_dir = env.Distribute(src_dir_name, ['SConstruct', 'lib/libfmod-3.74.so'])
bin_dir = env.Distribute(bin_dir_name, ['build/vdrift', 'tools/scripts/vdrift-install.sh'])

# env.TarCopy('build/scons-local/', scons_local_tarball)

src_archive = env.Tarball('%s.tar.bz2' % src_dir_name, src_dir)
bin_archive = env.Tarball('%s.tar.bz2' % bin_dir_name, bin_dir)

#----------------#
# Target Aliases #
#----------------#
#env.Alias(target = 'install', source = [prefix, bin])
env.Alias(target = 'src-package', source = src_archive)
env.Alias(target = 'bin-package', source = bin_archive)

#----------------#
# Subdirectories #
#----------------#
Export(['env', 'version', 'src_dir', 'bin_dir'])

#SConscript('data/SConscript')
#SConscript('docs/SConscript')
SConscript('include/SConscript')
#SConscript('tools/SConscript')
SConscript('src/SConscript', build_dir='build', duplicate = 0)
