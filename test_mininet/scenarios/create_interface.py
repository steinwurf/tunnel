#!/usr/bin/python

"""
Create TUN interface on two hosts
"""

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.log import setLogLevel

from mininet_test import TestMonitor
from mininet_test import TestMonitorHost


if __name__ == '__main__':
    setLogLevel('info')

    topo = Topo()
    topo.addHost('host')


    # TCLink is needed to set the bandwidth, delay and loss constraints
    # on the link
    net = Mininet(topo=topo,
                  link=TCLink,
                  waitConnected=True,
                  host=TestMonitorHost)
    net.start()

    test_monitor = TestMonitor()

    host = net.getNodeByName('host')

    host.set_test_monitor(test_monitor=test_monitor)


    # Create the interface
    host.pexec("./interface", cwd=None)

    res = host.pexec('ip link show', cwd=None)
    res.match(stdout='* tuniface: * mtu 1000 *')

    res = host.pexec('ip addr show', cwd=None)
    res.match(stdout='* inet 10.0.0.1/24 *')

    res = host.pexec('route', cwd=None)
    res.match(stdout='*10.0.0.0 * 255.255.255.0 * tuniface')

    net.stop()
