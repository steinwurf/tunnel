#! /usr/bin/env python
# encoding: utf-8

import os
import sys
import subprocess

APPNAME = 'tunnel'
VERSION = '1.1.0'


def options(opt):

    opt.add_option(
        '--run_mininet_tests', default=None, dest='run_mininet_tests',
        action='store_true', help='Run the mininet tests')


def configure(conf):

    # Check if we have mininet installed
    conf.find_program('mn', mandatory=False)


def build(bld):

    bld.env.append_unique(
        'DEFINES_STEINWURF_VERSION',
        'STEINWURF_TUNNEL_VERSION="{}"'.format(
            VERSION))

    # Only build for linux platforms
    if bld.is_mkspec_platform('linux'):
        bld.recurse('src/tunnel')

        if bld.is_toplevel():

            # Only build tests when executed from the top-level wscript,
            # i.e. not when included as a dependency
            bld.recurse('test')
            bld.recurse('examples')

            bld.add_post_fun(run_mininet_tests)


def run_command(args, **kwargs):
    print("Running: {}".format(args))
    sys.stdout.flush()
    subprocess.check_call(args, **kwargs)


def run_mininet_tests(bld):

    if bld.has_tool_option('run_mininet_tests'):

        if not bld.env.MN:
            bld.fatal("Please install mininet before running this test "
                      "(see README for instructions).")

        # Find the tunnel binary
        taskgen = bld.get_tgen_by_name('udp_tunnel')
        tunnel_binary = taskgen.link_task.outputs[0].abspath()
        print("Binary: " + tunnel_binary)

        # Make sure that Mininet is in a clean state (after a previous crash)
        #with open(os.devnull, 'wb') as pipe:  # silence mininet reset
        #    run_command(["sudo", "mn", "-c"], stdout=pipe, stderr=pipe)

        run_command(["sudo", "mn", "-c"])

        root = os.getcwd()
        # Run the mininet tests in the 'test/mininet' folder
        if os.path.exists('test/mininet'):
            os.chdir('./test/mininet')

            run_command(["sudo", "python", "simple_iperf.py", tunnel_binary])

        os.chdir(root)

