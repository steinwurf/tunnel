#!/bin/bash

HOSTNAME=""
ADDRESS=""

HOSTNAME_ADDED=""
LINK_SET=""
LINK=""

function finish {
    echo "Removing virtual network stack"

    # Remove veth interface
    if [[ -n "${LINK_SET}" && -n "${LINK}" ]]; then
        ip link del dev $LINK
    fi

    # Remove network namespace
    if [[ -n "${HOSTNAME_ADDED}" && -n "${HOSTNAME}" ]]; then
        ip netns del $HOSTNAME
    fi
}
trap finish EXIT

while getopts ":n:a:" opt; do
    case $opt in
        n)
            HOSTNAME=$OPTARG
            ;;
        a)
            ADDRESS=$OPTARG
            ;;
        \?)
            echo "Invalid option: $OPTARG"
            exit 1
            ;;
        :)
            echo "Option $OPTARG requires an argument."
            exit 1
            ;;
    esac
done

# Fail if host id is not given
if [ -z "${HOSTNAME}" ]; then
    echo "Virtual host id must be set with argument -n"
    exit 1
fi

if [ -z "${ADDRESS}" ]; then
    echo "Virtual address must be set with -a (e.g. -a 10.0.0.X)"
    exit 1
fi

LINK="nanonet-$HOSTNAME"
IFACE="eth-$HOSTNAME"
BRIDGE="nanonet-bridge"

# Create needed interfaces
ip link add dev $LINK type veth peer name $IFACE
LINK_SET="True"
ip link set dev $LINK up

# Set interfaces to bridge
ip link set $LINK master $BRIDGE

# Set IP addresses
ip addr add 10.0.0.2/24 dev $IFACE

# Set interfaces up
ip link set $IFACE up

# Add network namespace
ip netns add $HOSTNAME
HOSTNAME_ADDED="True"

# Push interface to namespace
ip link set $IFACE netns $HOSTNAME

 # Set localhost to network namespace
ip netns exec $HOSTNAME ip link set dev lo up

ip -netns $HOSTNAME addr add $ADDRESS/24 dev $IFACE
ip -netns $HOSTNAME link set dev $IFACE up

ip netns exec $HOSTNAME echo "nameserver 8.8.8.8" >> /etc/resolv.conf
ip netns exec $HOSTNAME echo "nameserver 8.8.4.4" >> /etc/resolv.conf

ip netns exec $HOSTNAME xterm
