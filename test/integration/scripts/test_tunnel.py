
import logging
import dummynet
import time
import os
import json

log = logging.getLogger(__name__)

def run_test(tunnel_app_path, tunnel_mode):
    port = 1234
    test_duration = 5 

    iperf_server_cmd = " ".join(
        ["iperf3", "--server 0.0.0.0 -i 0.1", f"--port {port}", "-1"]
    )
    iperf_client_cmd = " ".join(
        [
            "iperf3",
            "--client 11.11.11.22",
            f"--time {test_duration}",
            f"--port {port}",
        ]
    )

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
        ip2 = "10.0.1.2:12345"
        tun0 =  "10.0.0.1"
        tun1 =  "10.0.0.2"
        demo0.up(interface="demo0-eth0")
        demo1.up(interface="demo1-eth0")
        log.info(f"Running: {tunnel_app_path} {tunnel_mode}")
        
        tunnel_process2 = demo1.run_async(f"{tunnel_app_path} -m {tunnel_mode} -a {tun1} -l {ip2}", daemon=False)
        time.sleep(1)
        tunnel_process1 = demo0.run_async(f"{tunnel_app_path} -m {tunnel_mode} -a {tun0} -r {ip2}", daemon=False)
        time.sleep(1)
        # while True:
        #     time.sleep(1)

        ping_process =demo0.run_async("ping -I tun0 10.0.0.2 -c 2")


        end_time = 2*test_duration
        while process_monitor.run():
            if time.time() >= end_time:
                log.error("Test timeout")
                process_monitor.stop()
                log.info(ping_process.stdout)
                raise Exception("Test timeout")


    finally:
        net.cleanup()



def test_tun_tunnel(tunnel_app_path):
    run_test(tunnel_app_path, "tun")
def test_tap_tunnel(tunnel_app_path):
    run_test(tunnel_app_path, "tap")

