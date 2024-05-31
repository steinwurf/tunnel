# Copyright (c) 2024 Steinwurf ApS
# All Rights Reserved

# Distributed under the "BSD License". See the accompanying LICENSE.rst file.

import logging
import dummynet
import time
import os
import json

log = logging.getLogger(__name__)

def run_test(tunnel_app_path, tunnel_mode):
    ip2 = "10.0.1.2:12345"
    tun0 =  "10.0.0.1"
    tun1 =  "10.0.0.2"
    process_monitor = dummynet.ProcessMonitor(log=log)
    sudo = os.getuid() != 0
    log.info(f"sudo: {sudo}")
    shell = dummynet.HostShell(log=log, process_monitor=process_monitor, sudo=sudo)
    net = dummynet.DummyNet(shell=shell)

    try:
        demo0 = net.netns_add("demo0")
        demo1 = net.netns_add("demo1")
        net.link_veth_add(p1_name="demo0-eth0", p2_name="demo1-eth0")

        net.link_set(namespace="demo0", interface="demo0-eth0")
        net.link_set(namespace="demo1", interface="demo1-eth0")

        demo0.addr_add(ip="10.0.1.1/24", interface="demo0-eth0")
        demo1.addr_add(ip="10.0.1.2/24", interface="demo1-eth0")
        demo0.up(interface="demo0-eth0")
        demo1.up(interface="demo1-eth0")
        log.info(f"Running: {tunnel_app_path} {tunnel_mode}")
        
        tunnel_process2 = demo1.run_async(f"{tunnel_app_path} -m {tunnel_mode} -a {tun1} -l {ip2}", daemon=True)
        time.sleep(1)
        tunnel_process1 = demo0.run_async(f"{tunnel_app_path} -m {tunnel_mode} -a {tun0} -r {ip2}", daemon=True)
        time.sleep(1)

        ping_process =demo0.run(f"ping -I {tunnel_mode}0 {tun1} -i 0.5 -c 10")
        log.info(f"ping status: {ping_process.stdout} {ping_process.stderr}")
        
        assert "icmp_seq=10" in ping_process.stdout
    finally:
        net.cleanup()



def test_tun_tunnel(tunnel_app_path):
    run_test(tunnel_app_path, "tun")

def test_tap_tunnel(tunnel_app_path):
    run_test(tunnel_app_path, "tap")

