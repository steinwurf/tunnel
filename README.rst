======
tunnel
======

This is the default description, please change it.

The tunnel lib allow you to create an manipulate a tun interfaces on Linux.
A tun interface is essentially a virtual network interface on the IP (layer 3).


.. contents:: Table of Contents:
   :local:

Usage
=====



Here is how you use it...



Route a vlc stream over a tunnel
--------------------------------

Using the udp_tunnel example located in /exmaples we can do a small test.
Two machines that can access each other over a network, and each with the tunnel
project compiled and vlc installed is needed.

You also need to know the ip addresses of the two machines the "server" and the
"client", here we assume  that the server have the IP 10.10.0.1 and the client
10.10.0.2.

On the Server side start a tunnel and a vlc server:

::
    (as root)
    ./build/linux/examples/udp_tunnel --local_ip 10.10.0.1 --remote_ip 10.10.0.2 --tunnel_ip 10.0.0.13

    cvlc video_file.mp4 --sout udp:10.0.0.13:9915 --loop

This will create a tun interface "tunwurf" with ip 10.0.0.100, traffic arriving
from the remote will be routed to the tunwurf inteface, and all traffic sent on
the tunwurf interface will be routed to the remote ip .


if you run ifconfig you should be able to verify that a new interface have been created

::
    ifconfig

::
    tunwurf: flags=4305<UP,POINTOPOINT,RUNNING,NOARP,MULTICAST>  mtu 25000
        inet 10.0.0.13  netmask 255.255.255.0  destination 10.0.0.13
        inet6 fe80::84e8:aaae:9fdc:6e92  prefixlen 64  scopeid 0x20<link>
        unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 500  (UNSPEC)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 243  bytes 43632 (42.6 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0


On the Client side open a tunnel and vlc viewer

::
    (as root)
    ./build/linux/examples/udp_tunnel  --local_ip 10.10.0.2 --remote_ip 10.10.0.1 --tunnel_ip 10.0.0.42

    cvlc udp://@10.0.0.42:9915
