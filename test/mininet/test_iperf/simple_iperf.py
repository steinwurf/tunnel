#!/usr/bin/python

"""
Simple example of point-to-point link running iperf
"""

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.util import pmonitor
from mininet.log import setLogLevel

if __name__ == '__main__':
    setLogLevel('info')

    # The topology
    #
    #             +-----------------------+
    #             |     10Mbit/s link     |
    #             |     255ms delay         |
    #             |     0.% packet loss   |
    #             |                       |
    #             +-----------+-----------+
    #                         |
    #                         |
    #                         |
    #                         |
    # +-------------+         v        +-------------+
    # |             |                  |             |
    # | host 1 (h1) +------------------+ host 2 (h2) |
    # |             |                  |             |
    # +-------------+                  +-------------+
    #
    topo = Topo()
    topo.addHost('h1')
    topo.addHost('h2')
    topo.addLink('h1', 'h2', bw=0.800, delay='250ms', loss=0.1)

    # The TCLink is needed for use to set the bandwidth, delay and loss
    # constraints on the link
    #
    net = Mininet(topo=topo,
                  link=TCLink,
                  waitConnected=True)
    net.start()

    h1, h2 = net.getNodeByName('h1', 'h2')

    # Open iperf server and client and send 2 MB
    popens = {}

    # set up tunnel
    popens[h1] = h1.popen('udptunnel -local_ip {} -remote_ip {}\
                          -tunnel_ip 10.0.1.1 -port 42042 &'
                          .format(h1.IP(), h2.IP()))

    popens[h2] = h2.popen('udptunnel -local_ip {} -remote_ip {}\
                          -tunnel_ip 10.0.1.2 -port 42042 &'
                          .format(h2.IP(), h1.IP))

    # h1.cmd('intfs')
    # popens[h2] = h2.popen('intfs')

    popens[h1] = h1.popen('iperf -I 10.0.1.1 -s -p 7777 -i 1')
    popens[h2] = h2.popen('iperf -I 10.0.1.2 -c 10.0.1.1 -p 7777 -n 2000000')

    for host, line in pmonitor(popens):
        if host:
            # Output each line written as "<hX>: some output", where X
            # will be replaced by the host number i.e. 1 or 2.
            print("<{}>: {}".format(host.name, line.strip()))

        # stop the server if the client has terminated
        if h2 not in popens:
            popens[h1].kill()
            break

    net.stop()
