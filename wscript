#! /usr/bin/env python
# encoding: utf-8

import shutil
import os
import platform
import waflib
import hashlib
import os.path
from waflib.Build import BuildContext

APPNAME = "tunnel"
VERSION = "14.0.0"


def options(opt):
    if not opt.is_toplevel():
        return

    opt.add_option(
        "--pytest_temp",
        default="pytest_temp",
        help="Set the path where pytest executes the tests",
    )


def configure(conf):
    conf.set_cxx_std(17)

    if not conf.is_toplevel():
        return


def build(bld):
    bld.stlib(
        features="cxx",
        source=bld.path.ant_glob("src/**/*.cpp"),
        target="tunnel",
        use=["platform_includes", "poke"],
        export_includes=["src"],
    )

    if bld.is_toplevel() and bld.is_mkspec_platform("linux"):
        # Only build tests when executed from the top-level wscript,
        # i.e. not when included as a dependency
        bld.recurse("test")
        bld.recurse("examples")
        bld.recurse("apps/app/")


class IntegrationContext(BuildContext):
    cmd = "integration_test"
    fun = "integration_test"


def integration_test(ctx):
    # Test only for linux platforms
    if not ctx.is_mkspec_platform("linux"):
        return

    tunnel_app_binary = os.path.relpath(
        os.path.join(ctx.out_dir, "apps", "app", "tunnel_test_app")
    )
    if not os.path.exists(tunnel_app_binary):
        ctx.fatal(
            f"Cannot find tunnel binary in {tunnel_app_binary}, did you run 'waf build'?"
        )

    venv = ctx.create_virtualenv(name="pytest-venv", overwrite=True)

    # To update the requirements.txt just delete it - a fresh one
    # will be generated from test/integration/requirements.in
    if not os.path.isfile("test/integration/requirements.txt"):
        venv.run("python -m pip install pip-tools")
        venv.run("pip-compile test/integration/requirements.in")

    cmd_options = ""

    if ctx.has_tool_option("filter"):
        cmd_options += f"-k '{ctx.get_tool_option('filter')}'"

    venv.run("python -m pip install -r test/integration/requirements.txt")
    venv.run(
        f"pytest -xrA --tunnel-app={tunnel_app_binary} {cmd_options} test/integration"
    )
