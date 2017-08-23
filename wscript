#! /usr/bin/env python
# encoding: utf-8

import os
import waflib
from waflib.Build import BuildContext

APPNAME = 'tunnel'
VERSION = '1.0.0'


def build(bld):

    bld.env.append_unique(
        'DEFINES_STEINWURF_VERSION',
        'STEINWURF_TUNNEL_VERSION="{}"'.format(
            VERSION))

    bld.recurse('src/tunnel')

    if bld.is_toplevel():

        # Only build tests when executed from the top-level wscript,
        # i.e. not when included as a dependency
        bld.recurse('test')
        bld.recurse('examples')


def resolve(ctx):

    # Testing dependencies
    ctx.add_dependency(
        name='virtualenv',
        recurse=False,
        optional=False,
        resolver='git',
        method='checkout',
        checkout='15.1.0',
        sources=['github.com/pypa/virtualenv.git'])


def options(opt):

    # In default mode Vagrant stores information about provisioned machines
    # next to the Vagrantfile, since creating/deleting these folders
    # between  unit tests - that would mean that we would lose this
    # information. Causing Vagrant to recreate the virtual machine for
    # each run (which would be time consuming).
    #
    # Instead we would like to store this information in a per-project
    # global location. We do this in the user's home directory:
    home_path = os.path.abspath(os.path.expanduser("~"))
    vagrant_machine_path = os.path.join(
        home_path, ".vagrant_machines", APPNAME)

    opt.add_option(
        '--vagrant_machines', default=vagrant_machine_path,
        help='Path to where Vagrant will store information about the '
             'created virtual machines.')

    opt.add_option(
        '--mininet_test_filter', default=None, action='store',
        help='Runs all tests that include the substring specified.'
             'Wildcards not allowed. (Used with --run_tests)')

    opt.add_option(
        '--no_pep8', default=False, action='store_true',
        help='Skip pep8 check of python files')

    opt.add_option(
        '--no_pyflakes', default=False, action='store_true',
        help='Skip pyflakes check of python files')


def configure(conf):

    # Check if we have vagrant installed, needed to run the mininet
    # tests
    conf.find_program('vagrant', mandatory=False)

    # Check if we have VirtualBox installed, needed to run the mininet
    # tests
    conf.find_program('VBoxManage', mandatory=False)


# Mininet tests
class MininetContext(BuildContext):
        cmd = 'mininet'
        fun = 'mininet'


def mininet(bld):
    """ Run the mininet tests.

    Will fail hard if the Vagrant dependencies are not there.

    Basic usage:

        Running the tests: ./waf mininet
        Print output from the vm: ./waf mininet -v
    """

    # Create a log file for the output
    path = os.path.join(bld.bldnode.abspath(), 'mininet.log')
    bld.logger = waflib.Logs.make_logger(path, 'cfg')

    if not bld.env.VAGRANT:

        bld.fatal("Cannot run mininet tests without vagrant installed "
                  "(see README for instructions).")

    if not bld.env.VBOXMANAGE:

        bld.fatal("Cannot run mininet tests without VirtualBox installed "
                  "(see README for instructions).")

    _run_mininet(bld=bld)


def _run_mininet(bld):

    _ensure_vagrant_box_exists(ctx=bld, box='comnets/mininet')

    # Create the virtualenv in the build folder to make sure we run
    # isolated from the sources
    venv = _create_virtualenv(cwd=bld.path.abspath(), ctx=bld)

    with venv:
        venv.pip_install('pytest', 'pytest-testdirectory', 'pep8',
                         'pyflakes')

        # Run PEP8 check
        if bld.options.no_pep8 is False:
            bld.msg("Running", "pep8")
            venv.run('python -m pep8 --filename=*.py,wscript '
                     'test wscript buildbot.py')

        # Run pyflakes
        if bld.options.no_pyflakes is False:
            bld.msg("Running", "pyflakes")
            venv.run('python -m pyflakes test')

        # Added our systems path to the virtualenv (otherwise we cannot
        # find vagrant)
        venv.env['PATH'] = os.path.pathsep.join(
            [venv.env['PATH'], os.environ['PATH']])

        # Store the Vagrant virtual mahcine information here.
        venv.env['VAGRANT_DOTFILE_PATH'] = bld.options.vagrant_machines

        # We override the pytest temp folder with the basetemp option, so
        # the test folders will be available at the specified location on
        # all platforms. The default location is the "pytest" local folder.
        basetemp = os.path.abspath(os.path.expanduser(
            'test/mininet/test_temp'))

        # We need to manually remove the previously created basetemp
        # folder, because pytest uses os.listdir in the removal process,
        # and that fails if there are any broken symlinks in that folder.
        if os.path.exists(basetemp):
            waflib.extras.wurf.directory.remove_directory(path=basetemp)

        testdir = bld.path.find_node('test')

        # Make python not write any .pyc files. These may linger around
        # in the file system and make some tests pass although their .py
        # counter-part has been e.g. deleted
        command = 'python -B -m pytest {} --basetemp {}'.format(
            testdir.abspath(), basetemp)

        if bld.options.verbose:
            command += " --capture=no"

        # Adds the test filter if specified
        if bld.options.mininet_test_filter:
            command += ' -k "{}"'.format(bld.options.mininet_test_filter)

        venv.run(command)


def _has_vagrant_box(ctx, box):
    """ Checks whether the Vagrant box has already been downloaded.

    :param ctx: A Waf Context instance.
    :param box: The box name as a string.
    :return: True if the box already exists, otherwise False
    """

    output = ctx.cmd_and_log("vagrant box list")

    boxes = output.split('\n')
    for b in boxes:
        if b.startswith(box):
            return True
    return False


def _download_vagrant_box(ctx, box):
    """ Download the Vagrant box.

    :param ctx: A Waf Context instance.
    :param box: The box name as a string.
    """

    ret = ctx.exec_command("vagrant box add {}".format(box), stdout=None,
                           stderr=None)

    if ret != 0:
        ctx.fatal('Could not fetch comnets/mininet box')


def _ensure_vagrant_box_exists(ctx, box):
    """ Ensure that the Vagrant box exists on the machine.

    :param ctx: A Waf Context instance.
    :param box: The box name as a string.
    """

    if not _has_vagrant_box(ctx=ctx, box=box):
        _download_vagrant_box(ctx=ctx, box=box)
    else:
        ctx.msg("Vagrant box {}".format(box), "exists")


def _create_virtualenv(ctx, cwd):
    """ Create a new virtualenv

    :param ctx: A Waf Context instance.
    :param cwd: The working directory, as a string, where the virtualenv
        will be created and where the commands will run.
    """

    # Make sure the virtualenv Python module is in path
    venv_path = ctx.dependency_path('virtualenv')

    env = dict(os.environ)
    env.update({'PYTHONPATH': os.path.pathsep.join([venv_path])})

    from waflib.extras.wurf.virtualenv import VirtualEnv
    return VirtualEnv.create(
        cwd=cwd, env=env, name=None, ctx=ctx,
        pip_packages_path=os.path.join(ctx.path.abspath(), 'pip_packages'))
