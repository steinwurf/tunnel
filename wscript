#! /usr/bin/env python
# encoding: utf-8

APPNAME = 'tunnel'
VERSION = '0.0.0'


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
