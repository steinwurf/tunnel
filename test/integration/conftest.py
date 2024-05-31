# Copyright (c) 2024 Steinwurf ApS
# All Rights Reserved
#
# Distributed under the "BSD License". See the accompanying LICENSE.rst file.
import pytest
import logging

log = logging.getLogger("Integration Test")


def pytest_addoption(parser):
    parser.addoption(
        "--tunnel-app",
        action="store",
        required=True,
        help="Path tunnel app binary",
    )


@pytest.fixture(scope="session")
def tunnel_app_path(pytestconfig):
    return pytestconfig.getoption("--tunnel-app")
