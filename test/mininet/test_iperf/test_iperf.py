def test_iperf(testdirectory):

    # This function defines a single test which will
    # run inside a mininet virtual machine.
    #
    # The first important thing to notice in this test function,
    # is the argument to it called testdirectory.
    #
    # The testdirectory object represents an actual folder, create by
    # pytest, on our computer. This directory will serve as the working
    # directory for our test.
    #
    # The files and folders we create in the testdirectory will be
    # available inside the running virtual machine (we will see this
    # later).
    #
    # To see the available methods etc. of the testdirectory object,
    # see the docs here: https://github.com/steinwurf/pytest-testdirectory
    #
    # We use the vagrant tool to manage the virtual machine. The
    # Vagrantfile defines the virtual machine we will be using.
    #
    # You can take a quick look at the Vagrantfile file and you'll
    # notice that there isn't really much to it. We can use it to
    # customize all sorts of things about the virtual machine (but
    # we will not need that here - for information on that check the
    # vagrant documentation).
    #
    # Lets start by copying the Vagrantfile to the testdirectory:

    vagrantdir = testdirectory.from_path('test/mininet')

    # testdirectory.copy_file('test/mininet/Vagrantfile')

    # This essentially covers the setup we need to perform and we
    # are now ready to spawn the virtual machine.
    #
    # To do this we first need to check the status of the virtual machine.
    # If it has already been created we can use the 'reload' command
    # otherwise we need to run the 'up' command.

    status = vagrantdir.run('vagrant status')

    # We look for the sentence 'not created' in the output from the
    # 'status' command.
    if status.stdout.match('*not created*'):

        # The virtual machine has not been created, we need to 'up' it
        vagrantdir.run('vagrant up', stdout=None, stderr=None)

    else:

        # We do this by running the 'vagrant ...' command inside the
        # testdirectory folder.

        # We use the reload funcing to make sure that vagrant will first
        # stop the mininet virtual machine if it is already running. The
        # reason we found this nessecary is that pytest will delete/create
        # the testdirectory folders everythime we invoke the tests. The
        # new folders needs to be mapped to the virtual machine, vagrant
        # will do this when starting the virtual machine (so a reload
        # ensure this).

        vagrantdir.run('vagrant reload', stdout=None, stderr=None)

    # Using mininet it is possible to define test topologies etc. in a
    # Python script. See this introduction, for more information on this
    # topic:
    # https://github.com/mininet/mininet/wiki/Introduction-to-Mininet
    #
    # Let us copy our mininet test script to the testdirectory.

    testdirectory.copy_file('build/linux*/examples/udp_tunnel')

    # make the program executeable
    testdirectory.run('chmod u+x udp_tunnel')

    testdirectory.symlink_file('test/mininet/test_iperf/simple_iperf.py')

    # The virtual machine is now running and we can execute commands
    # inside it. The easiest is to use 'vagrant ssh ...' command to do this
    # via ssh.
    #
    # The command we exectute is in two steps:
    #
    # 1. Change directory to /vagrant. This location inside the virtual
    #    machine maps to the testdirectory on our host machine. Making
    #    the test files etc. availble. Any files created during the test
    #    will therefore also be available on our host filesystem.
    #
    # 2. Run the mininet script to exectute the test.

    testdirectory.run(
        'vagrant ssh -- -t '
        '"cd /vagrant/test_temp/test_iperf0;sudo python simple_iperf.py"',
        stdout=None, stderr=None)

    # The above command will block as long as the mininet script is
    # running.
    #
    # So after running the mininet script this would be a good place to
    # add checks to see if the test should be successful e.g. were all
    # the files created etc.
