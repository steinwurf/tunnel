
import logging
import dummynet
import time
import os
import json

log = logging.getLogger(__name__)

def run_test(tunnel_app_path, tunnel_mode):
    port = 1234
    test_duration = 10

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
        
        demo0.run_async()



        while process_monitor.run():
            if time.time() >= end_time:
                log.error("Test timeout")
                process_monitor.stop()
                raise Exception("Test timeout")


    finally:
        net.cleanup()



def test_tun_tunnel(tunnel_app_path):
    run_test(tunnel_app_path, "tun")
def test_tap_tunnel(tunnel_app_path):
    run_test(tunnel_app_path, "tap")
