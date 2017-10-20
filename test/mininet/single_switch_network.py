#! /usr/bin/env python
# encoding: utf-8

from __future__ import print_function
import sys
import time
import threading

try:
    from mininet.topo import Topo
    from mininet.net import Mininet
    from mininet.link import TCLink
    # from mininet.node import CPULimitedHost
    # from mininet.util import dumpNodeConnections
    from mininet.log import setLogLevel


except ImportError as err:
    sys.exit("Error: A module was not found ({})".format(err))


class SingleSwitch(Topo):
    "Single switch connected to n hosts."
    def build(self):
        self.switch = self.addSwitch('s0')

    def add_host(self, name, linkopts={}):
        """
        linkopts is of type dict with optional args:
            bw          Mbps, number
            delay       string, e.g.'5ms'
            loss        percentage, number 0-100
            max_queue_size  int
            use_htb     bool
        """
        host = self.addHost(name)
        self.addLink(host, self.switch, **linkopts)


class SingleSwitchNetwork(object):
    "Creates a simple network with one switch and nodes connected to this"
    def __init__(self):
        self.topo = SingleSwitch()
        self.hosts = []
        self.addrs = []
        self.names = []

    def add_host(self, name, addr=None, linkopts={}):
        h = dict(name=name, commands=[], addr=addr,
                 linkopts=linkopts, results=[])
        self.hosts.append(h)
        self.topo.add_host(name, linkopts)
        if addr is not None and addr in self.addrs:
            raise ValueError("Address specified multiple times:", addr)
        if name in self.names:
            raise ValueError("Hostname specified multiple times:", name)
        self.addrs.append(addr)
        self.names.append(name)

    def add_command(self, hostname, command, start_time=0.0, timeout=None):
        for h in self.hosts:
            if h['name'] == hostname:
                h['commands'].append(
                    dict(command=command, start_time=start_time,
                         timeout=timeout))
                break
        else:
            raise ValueError("Hostname does not exist:", hostname)

    def run(self):
        net = Mininet(topo=self.topo, link=TCLink, waitConnected=True)

        self._setup_addresses(net)

        net.start()
        self._execute_commands(net)
        net.stop()

    def _setup_addresses(self, net):
        for host in self.hosts:
            if host['addr'] is not None:
                h = net.get(host['name'])
                addr = host['addr']
                h.setIP(addr)
                self.addrs.append(addr)

    def _run_command(self, host, command, results):
        cmd = command['command']

        # Sleep until the designated start time
        if command['start_time'] > 0.0:
            time.sleep(command['start_time'])

        # Start the background process
        print("{}: {} start cmd: {}".format(time.time(), host.name, cmd))
        proc = host.popen(cmd)

        terminated = False
        # If there is a timeout, wait for the specified period
        if command['timeout']:
            time.sleep(command['timeout'])
            # If the process did not finish, terminate it
            if proc.poll() is None:
                proc.terminate()
                print("{}: {} terminated: {}".format(time.time(), host.name, cmd))
                terminated = True

        print("{}: {} waiting: {}".format(time.time(), host.name, cmd))
        # Save results
        results['command'] = cmd
        results['terminated'] = terminated
        results['output'] = proc.communicate()
        results['returncode'] = proc.poll()
        print("{}: {} finished: {}".format(time.time(), host.name, cmd))

    def _execute_commands(self, net):

        threads = []
        for h in self.hosts:
            name = h['name']
            host = net.get(name)

            # Start a new thread for each command
            for c in h['commands']:
                res = dict()
                h['results'].append(res)
                t = threading.Thread(
                    target = self._run_command, args = (host, c, res, ))
                threads.append(t)
                t.start()

        for t in threads:
            t.join()

    def results(self):
        """
        Returns a dict with <hostname, results> key-value pairs, where results
        are a 4-tuple with (returncode, terminated, stdout, stderr) values
        """
        res = {}
        for d in self.hosts:
            res[d['name']] = d['results']
        return res


def main():
    if '--dry-run' in sys.argv:
        return

    setLogLevel('info')

    # Create the network
    net = SingleSwitchNetwork()

    nodes = 1

    # Commands of hosts will execute in the order the are added.
    # Add one type of host (the iperf client is the sender here)
    for i in range(1, nodes + 1):
        net.add_host('h{}'.format(i),
                     ["echo 'hello' | nc 10.0.0.1 12101"],
                     #["iperf -u -c 10.0.0.1 -p 13337 -x SV -b 3M"],
                     addr="10.0.0.{}".format(i+10), timeout=3)

    # Add another type of host (the iperf server functions as a receiver here)
    net.add_host('h0',
                 ["nc -l 0.0.0.0 12101"],
                  #["iperf -u -s -p 13337"],
                  addr="10.0.0.1", timeout=3)

    # Run network scenario
    net.run()

    results = net.results()

    for n in range(0, nodes + 1):
        node = 'h{}'.format(n)
        print("\n{} results:".format(node))
        res = results[node]
        print("Terminated: {}".format(res["terminated"]))
        print("Return code: {}".format(res["returncode"]))
        print("stdout:\n{}".format(res["output"][0]))
        print("stderr:\n{}".format(res["output"][1]))

if __name__ == '__main__':
    main()
