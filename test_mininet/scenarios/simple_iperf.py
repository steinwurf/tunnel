#!/usr/bin/python

"""
Simple example of point-to-point link running iperf over a udp tunnel
"""
import sys
import time
import threading

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.util import pmonitor
from mininet.log import setLogLevel


if __name__ == '__main__':
    setLogLevel('info')

    argv = sys.argv

    if len(argv) != 2:
        print("Usage: {} <path-to-tunnel-binary>".format(argv[0]))
        sys.exit(0)

    tunnel_binary = argv[1]

    topo = Topo()
    topo.addHost('h1')
    topo.addHost('h2')
    topo.addLink('h1', 'h2', bw=3.0, delay='250ms', loss=0.0)

    # TCLink is needed to set the bandwidth, delay and loss constraints
    # on the link
    net = Mininet(topo=topo,
                  link=TCLink,
                  waitConnected=True)
    net.start()

    h1, h2 = net.getNodeByName('h1', 'h2')

    # We launch a separate thread for the tunnel processes, so we can monitor
    # them while iperf is running
    finished = False

    def start_tunnel():
        print("Starting udp_tunnel...")

        tun = {}
        tun[h1] = h1.popen(
            '{} --local_ip {} --remote_ip {} --tunnel_ip 10.0.1.1'
            .format(tunnel_binary, h1.IP(), h2.IP()),
            stderr=sys.stdout.fileno())

        tun[h2] = h2.popen(
            '{} --local_ip {} --remote_ip {} --tunnel_ip 10.0.1.2'
            .format(tunnel_binary, h2.IP(), h1.IP()),
            stderr=sys.stdout.fileno())

        for host, line in pmonitor(tun):
            if host:
                print("<{}>: {}".format(host.name, line.strip()))
                sys.stdout.flush()
            if finished:
                tun[h1].kill()
                tun[h2].kill()
                break
        print("udp_tunnel thread finished.")

    t1 = threading.Thread(target=start_tunnel, args=[])
    t1.start()

    # The main thread sleeps for a few seconds to allow the tunnel to start
    time.sleep(3)

    print("Starting iperf...")
    iperf = {}
    iperf[h1] = h1.popen('iperf -s -u -i 1',
                         stderr=sys.stdout.fileno())
    iperf[h2] = h2.popen('iperf -c 10.0.1.1 -u -b 2M -l 1000',
                         stderr=sys.stdout.fileno())

    for host, line in pmonitor(iperf):
        if host:
            # Output each line written as "<hX>: some output", where X
            # will be replaced by the host number i.e. 1 or 2.
            print("<{}>: {}".format(host.name, line.strip()))
            sys.stdout.flush()

        # stop the server if the client has terminated
        if h2 not in iperf:
            print("Stopping iperf server...")
            iperf[h1].kill()
            break

    finished = True
    t1.join()
    net.stop()
