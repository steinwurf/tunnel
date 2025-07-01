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


def options(ctx):
    ctx.load("cmake")

def configure(ctx):

    ctx.load("cmake")

    if ctx.is_toplevel():
        ctx.cmake_configure()


def build(ctx):

    ctx.load("cmake")

    if ctx.is_toplevel():
        ctx.cmake_build()
