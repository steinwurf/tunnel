#! /usr/bin/env python
# encoding: utf-8

import os
import sys

try:
    from single_switch_network import SingleSwitchNetwork
    from mininet.log import setLogLevel

except ImportError as err:
    sys.exit("Error: A module was not found ({})".format(err))


def main(tunnel_binary):

    # Create the network
    net = SingleSwitchNetwork()

    # Use the broadcast address, because mininet cannot multicast
    broadcast_addr = "10.255.255.255"
    # The number of receivers in the network
    receivers = 2

    # Sender commands

    # The sender is connected to the central switch with an error-free link,
    # so we don't introduce any correlated packet erasures
    linkopts = dict(bw=5.0, delay='100ms', loss=0)
    sender_ip = "10.0.0.1"
    net.add_host('h0', addr=sender_ip, linkopts=linkopts)
    # The sender will transmit broadcast packets and it will receive normal
    # unicast packets on its local_ip
    net.add_command(
        'h0', "{} --local_ip {} --remote_ip {} --tunnel_ip 10.0.1.1"
        .format(tunnel_binary, sender_ip, broadcast_addr),
        timeout=6)
    # The iperf sender will send multicast packets over the *tunnel interface*
    # (note that iperf cannot send broadcast packets)
    net.add_command(
        'h0', 'iperf -c 224.0.67.67 -u -t 3 -i 1 -b 3M -B 10.0.1.1',
        start_time=1.0, timeout=5.0)

    # Receiver commands
    for i in range(1, receivers + 1):
        node = 'h{}'.format(i)
        node_ip = "10.0.0.{}".format(i+10)
        tunnel_ip = "10.0.1.{}".format(i+10)
        # The receivers are connected to the central switch with a lossy link,
        # so their losses should be independent
        linkopts = dict(bw=5.0, delay='200ms', loss=3)
        net.add_host(node, addr=node_ip, linkopts=linkopts)
        net.add_command(
            node, "{} --local_ip {} --remote_ip {} --tunnel_ip {}"
            .format(tunnel_binary, broadcast_addr, sender_ip, tunnel_ip),
            timeout=6)
        # Add a route that forces iperf to listen for multicast traffic on the
        # tunnel interface (iperf listens on the interface associated with
        # the default route, and there is no option to override that)
        net.add_command(
            node, 'sudo route add 224.0.67.67 dev tunwurf',
            start_time=0.3, timeout=0.2)
        # Add iperf server listening on a multicast address
        net.add_command(
            node, 'iperf -s -u -B 224.0.67.67 -i 1',
            start_time=0.5, timeout=5)

    net.run()

    results = net.results()
    failed_processes = 0

    nodes = receivers + 1
    for n in range(0, nodes):
        node = 'h{}'.format(n)
        print("\nResults for {}:\n".format(node))
        for res in results[node]:
            print("Command: {}".format(res["command"]))
            # All processes should finish with return code 0 or -9
            if res['returncode'] not in [0, -9]:
                failed_processes += 1

            status = "killed" if res["killed"] else "completed"
            print("Exit code: {} ({})".format(res["returncode"], status))
            if res["output"][0]:
                print("stdout:\n{}".format(res["output"][0]))
            if res["output"][1]:
                print("stderr:\n{}".format(res["output"][1]))

    print("Failed processes: {}\n".format(failed_processes))

    if failed_processes > 0:
        sys.exit(failed_processes)


if __name__ == '__main__':
    setLogLevel('info')

    argv = sys.argv

    if len(argv) != 2:
        print("Usage: {} <path-to-tunnel-binary>".format(argv[0]))
        sys.exit(0)

    tunnel_binary = argv[1]

    main(tunnel_binary)
