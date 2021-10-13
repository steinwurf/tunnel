#! /usr/bin/env python
# encoding: utf-8

import os
import sys
import waflib

APPNAME = 'tunnel'
VERSION = "5.1.0"


def options(opt):
    if not opt.is_toplevel():
        return

    opt.add_option(
        '--run_mininet_tests', default=None, dest='run_mininet_tests',
        action='store_true', help='Run the mininet tests')

    opt.add_option(
        '--pytest_temp', default='pytest_temp',
        help='Set the path where pytest executes the tests')


def configure(conf):

    if not conf.is_toplevel():
        return

    # Check if we have vagrant installed, needed to run the mininet
    # tests
    conf.find_program('vagrant', mandatory=False)

    # Check if we have VirtualBox installed, needed to run the mininet
    # tests
    conf.find_program('VBoxManage', mandatory=False)


def build(bld):

    bld.env.append_unique(
        'DEFINES_STEINWURF_VERSION',
        'STEINWURF_TUNNEL_VERSION="{}"'.format(
            VERSION))

    # Only build for linux platforms
    if not bld.is_mkspec_platform('linux'):
        return

    # This library doesn't work with openwrt
    if 'openwrt' in bld.env['CXX_NAME']:
        return;

    bld.stlib(
        features='cxx',
        source=bld.path.ant_glob('src/**/*.cpp'),
        target='tunnel',
        use=['platform_includes'],
        export_includes=['src']
    )

    if bld.is_toplevel():

        # Only build tests when executed from the top-level wscript,
        # i.e. not when included as a dependency
        bld.recurse('test')
        bld.recurse('examples')

        bld.add_post_fun(run_mininet_tests)


def run_mininet_tests(bld):

    if not bld.options.run_mininet_tests:
        return

    if not bld.env.VAGRANT:
        bld.fatal("Please install vagrant before running this test.")

    if not bld.env.VBOXMANAGE:
        bld.fatal("Please install virtualbox before running this test.")

    venv = bld.create_virtualenv(cwd=bld.path.abspath())

    venv.pip_install(
        ['pytest', 'pytest-testdirectory',
         'git+https://github.com/steinwurf/pytest-vagrant.git@b8465f8'])

    # We override the pytest  folder with the basetemp option,
    # so the test folders will be available at the specified location
    # on all platforms. The default location is the "pytest" local folder.
    pytest_temp = os.path.abspath(os.path.expanduser(
        bld.options.pytest_temp))

    # We need to manually remove the previously created basetemp folder,
    # because pytest uses os.listdir in the removal process, and that fails
    # if there are any broken symlinks in that folder.
    if os.path.exists(pytest_temp):
        waflib.extras.wurf.directory.remove_directory(path=pytest_temp)

    os.makedirs(name=pytest_temp)

    testdir = bld.path.find_node('test_mininet').abspath()

    # Make python not write any .pyc files. These may linger around
    # in the file system and make some tests pass although their .py
    # counter-part has been e.g. deleted
    command = 'python -B -m pytest {} --basetemp {} --vagrantfile {}'.format(
        testdir, pytest_temp, testdir)

    if bld.options.verbose:
        command += " --capture=no"

    venv.run(command, cwd=bld.path.abspath())
