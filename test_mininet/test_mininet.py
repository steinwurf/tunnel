
def test_simple_iperf(sshdirectory):
    sshdirectory.put_file('build_current/examples/sample_tunnel')
    sshdirectory.put_file('test_mininet/scenarios/simple_iperf.py')
    sshdirectory.run("sudo python simple_iperf.py ./sample_tunnel")


def test_multicast(sshdirectory):
    sshdirectory.put_file('build_current/examples/sample_tunnel')
    sshdirectory.put_file('test_mininet/scenarios/single_switch_network.py')
    sshdirectory.put_file('test_mininet/scenarios/multicast.py')
    sshdirectory.run('sudo python multicast.py ./sample_tunnel')

def test_create_interface(sshdirectory):
    sshdirectory.put_file('build_current/examples/interface')
    sshdirectory.put_file('test_mininet/scenarios/create_interface.py')
    sshdirectory.run('sudo python create_interface.py')
