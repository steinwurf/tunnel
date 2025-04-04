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
VERSION = "18.1.0"


def options(opt):
    if not opt.is_toplevel():
        return

    opt.add_option(
        "--pytest_temp",
        default="pytest_temp",
        help="Set the path where pytest executes the tests",
    )

    opt.add_option(
        "--cmake_toolchain_file",
        default=None,
        dest="cmake_toolchain_file",
        help="Path to the cmake toolchain file. Activates cmake build.",
    )

    opt.add_option(
        "--cmake_build_type",
        dest="cmake_build_type",
        help="Build type for cmake build.",
    )

    opt.add_option(
        "--ninja",
        dest="ninja",
        help="Use Ninja as generator for cmake build.",
        action="store_true",
    )

    opt.add_option(
        "--cmake_configure_opts",
        dest="cmake_configure_opts",
        help="Full options for cmake configure other than -S and -B",
    )

    opt.add_option(
        "--cmake_build_opts",
        dest="cmake_build_opts",
        help="Full options for cmake build other than --build .",
    )


def configure(conf):
    conf.set_cxx_std(17)

    conf.find_program("cmake", mandatory=True)
    if conf.has_tool_option("ninja"):
        conf.find_program("ninja", mandatory=True)


def build(bld):
    # Build static library if this is top-level, otherwise just .o files
    cmake_folder = _cmake_configure(bld)
    _cmake_build(bld, cmake_folder)

    if bld.options.run_tests:
        ret = bld.exec_command(
            "ctest --output-on-failure",
            cwd=cmake_folder,
            stdout=bld.logger,
            stderr=bld.logger,
        )

        if ret != 0:
            bld.fatal("Failed running tests")


class Install(waflib.Context.Context):
    cmd = "install"
    fun = "install"


def install(ctx):

    cmake_dir = os.path.join(
        ctx.path.abspath(),
        "build_cmake",
    )

<<<<<<< HEAD
    ctx.exec_command(
        f"cmake --build {cmake_dir} --target install",
    )
=======
    if bld.is_toplevel():
        # Collect the source files but exclude the platform specific ones
        sources = bld.path.ant_glob(
            "test/src/**/*.cpp", excl=["test/**/platform_*/**/*.cpp"]
        ) + bld.path.ant_glob("test/src/**/*.cc")

        if platform.system() == "Linux":
            sources += bld.path.ant_glob("test/src/**/platform_linux/**/*.cpp")
        elif platform.system() == "Darwin":
            sources += bld.path.ant_glob("test/src/**/platform_macos/**/*.cpp")
        sources += bld.path.ant_glob("test/src/**/platform_unsupported/**/*.cpp")
        bld.program(
            features="cxx test",
            source=["test/tunnel_tests.cpp"] + sources,
            target="tunnel_tests",
            use=["tunnel", "gtest"],
        )

        if platform.system() == "Linux" or platform.system() == "Darwin":
            bld.recurse("examples")
            bld.recurse("apps/app/")
>>>>>>> master


class IntegrationContext(BuildContext):
    cmd = "integration_test"
    fun = "integration_test"


def integration_test(ctx):
    # Test only for linux
    if not ctx.is_mkspec_platform("linux"):
        return

    tunnel_app_binary = os.path.relpath(
        os.path.join("build_cmake", "apps", "app", "tunnel_test_app")
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


def _cmake_configure_command(bld):
    rely_dir = str(bld.path.abspath())

    cmake_folder = str(bld.out_dir)

    if bld.has_tool_option("cmake_configure_opts"):
        cmake_folder += (
            f"/cmake_custom_{str(hash(bld.get_tool_option('cmake_configure_opts')))}"
        )
        return ["cmake", "-S", rely_dir, "-B", cmake_folder] + bld.get_tool_option(
            "cmake_configure_opts"
        ).split(" "), cmake_folder

    cmake_toolchain_file = None
    if bld.has_tool_option("cmake_toolchain_file"):
        cmake_toolchain_file = bld.get_tool_option("cmake_toolchain_file")

    rely_dir = str(bld.path.abspath())

    cmake_folder = str(bld.out_dir)
    if not cmake_toolchain_file:
        cmake_folder += "/cmake_default"

    else:
        cmake_folder += "/{}".format(pathlib.Path(cmake_toolchain_file).stem)

    cmake_build_type = "Debug"
    if bld.has_tool_option("cmake_build_type"):
        cmake_build_type = bld.get_tool_option("cmake_build_type")
    cmake_cmd = [
        "cmake",
        "-S",
        rely_dir,
        "-B",
        cmake_folder,
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        "-DSTEINWURF_RESOLVE={}".format(
            (str(bld.path.abspath()).replace("\\", "/") + "/resolve_symlinks")
        ),
        "-DSTEINWURF_TOP_NAME=tunnel",
        "-DCMAKE_BUILD_TYPE={}".format(cmake_build_type),
    ]
    if os.path.exists("build_cmake"):
        os.remove("build_cmake")
    os.symlink(cmake_folder, "build_cmake", target_is_directory=True)

    if cmake_toolchain_file:
        cmake_cmd += ["-DCMAKE_TOOLCHAIN_FILE={}".format(cmake_toolchain_file)]

    if bld.has_tool_option("ninja"):
        cmake_cmd += [f"-GNinja"]
    return cmake_cmd, cmake_folder


def limit_jobs_based_on_memory(default_jobs=1):
    """
    Calculate the number of jobs based on total system memory (Linux only).
    If unable to determine memory or not on Linux, return the default value.
    """
    if platform.system() != "Linux":
        # Not on Linux, return the default value
        return default_jobs

    meminfo_path = "/proc/meminfo"
    if not os.path.exists(meminfo_path):
        # /proc/meminfo not found, return the default value
        return default_jobs

    try:
        with open(meminfo_path, "r") as meminfo:
            for line in meminfo:
                if line.startswith("MemTotal"):
                    # MemTotal is in kB, convert to GB
                    total_memory_kb = int(line.split()[1])
                    total_memory_gb = total_memory_kb / (1024**2)
                    # Return jobs calculated as total_memory_gb / 2, minimum 1
                    return max(1, int(total_memory_gb / 2))
    except Exception as e:
        # Fallback to default if any error occurs
        print(f"Error reading memory info: {e}")
        return default_jobs


def _cmake_build_command(bld, cmake_folder):
    # Base command with the cmake build folder
    cmake_cmd = ["cmake", "--build", cmake_folder]

    # Add any additional build options if provided
    if bld.has_tool_option("cmake_build_opts"):
        cmake_cmd += bld.get_tool_option("cmake_build_opts").split(" ")

    if platform.system() != "Windows":
        # Use the number of jobs specified by Options.options.jobs
        num_jobs = (
            waflib.Options.options.jobs or 1
        )  # Default to 1 if jobs option is not set

        # Limit the number of jobs based on total system memory
        num_jobs = limit_jobs_based_on_memory(default_jobs=num_jobs)

        cmake_cmd.append(f"-j{num_jobs}")
    else:
        # Parallelize MSBuild https://devblogs.microsoft.com/cppblog/improved-parallelism-in-msbuild/
        cmake_cmd.append("--parallel")
        cmake_cmd.append("--")
        cmake_cmd.append("/p:UseMultiToolTask=true")
        cmake_cmd.append("/p:EnforceProcessCountAcrossBuilds=true")
    return cmake_cmd


def _cmake_configure(bld):
    cmake_cmd, cmake_folder = _cmake_configure_command(bld)
    # Execute the cmake command within Waf's context
    ret = bld.exec_command(
        cmake_cmd, cwd=bld.out_dir, stdout=bld.logger, stderr=bld.logger
    )

    if ret != 0:
        bld.fatal("CMake configuration failed")

    return cmake_folder


def _cmake_build(bld, cmake_folder):
    # Get the build command
    cmake_build_cmd = _cmake_build_command(bld, cmake_folder)

    # Execute the cmake build command within Waf's context
    ret = bld.exec_command(
        cmake_build_cmd, cwd=cmake_folder, stdout=bld.logger, stderr=bld.logger
    )

    # Check for any build errors
    if ret != 0:
        bld.fatal(f"CMake build failed: {ret}")


class CMakeCleanContext(BuildContext):
    cmd = "cmake_clean"
    fun = "cmake_clean"


class Clean(waflib.Context.Context):
    cmd = "clean"
    fun = "clean"


def clean(ctx):
    ctx.logger = waflib.Logs.make_logger("/tmp/waf_clean.log", "cfg")

    build_dir = os.path.join(ctx.path.abspath(), "build")
    build_symlink = os.path.join(ctx.path.abspath(), "build_current")
    cmake_symlink = os.path.join(ctx.path.abspath(), "build_cmake")

    # Remove the "build" folder if it exists, with start and end messages
    ctx.start_msg("\nChecking and removing build directory")
    if os.path.isdir(build_dir):
        shutil.rmtree(build_dir)
        ctx.end_msg("Removed")
    else:
        ctx.end_msg("Not found", color="YELLOW")

    # Remove the "build_current" symlink if it exists, with start and end messages
    ctx.start_msg("Checking and removing build_current symlink")
    if os.path.islink(build_symlink):
        os.unlink(build_symlink)
        ctx.end_msg("Removed")
    else:
        ctx.end_msg("Not found", color="YELLOW")
    # Remove the "build_cmake" symlink if it exists, with start and end messages
    ctx.start_msg("Checking and removing build_cmake symlink")
    if os.path.islink(cmake_symlink):
        os.unlink(cmake_symlink)
        ctx.end_msg("Removed")
    else:
        ctx.end_msg("Not found", color="YELLOW")
