#!/usr/bin/python

"""
Simple example of point-to-point link running iperf over a udp tunnel
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

    # set up tunnel
    tun1 = h1.popen('./udp_tunnel --local_ip {} --remote_ip {}\
                    --tunnel_ip 10.0.1.1 --port 42042'
                    .format(h1.IP(), h2.IP()))

    if tun1.poll() is None:
        print("<h1> " + tun1.stdout.readline())
    else:
        print("<h1> Error: " + tun1.stderr.readline())

    print("<h2> Startin tunnel")
    tun2 = h2.popen('./udp_tunnel --local_ip {} --remote_ip {}\
                    --tunnel_ip 10.0.1.2 --port 42042'
                    .format(h2.IP(), h1.IP()))

    if tun2.poll() is None:
        print("<h2> " + tun2.stdout.readline())
    else:
        print("<h2> Error: " + tun2.stderr.readline())

    iperf = {}
    iperf[h1] = h1.popen('iperf -I 10.0.1.1 -s -p 7777 -i 1')
    iperf[h2] = h2.popen('iperf -I 10.0.1.2 -c 10.0.1.1 -p 7777 -n 2000000')

    for host, line in pmonitor(iperf):
        if host:
            # Output each line written as "<hX>: some output", where X
            # will be replaced by the host number i.e. 1 or 2.
            print("<{}>: {}".format(host.name, line.strip()))

        # stop the server if the client has terminated
        if h2 not in iperf:
            iperf[h1].kill()
            break

    net.stop()
