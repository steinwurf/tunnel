#!/bin/bash

HOSTID=""
ADDRESS=""

HOSTID_ADDED=""
LINK_SET=""
LINK=""

function finish {
    echo "Removing virtual network stack"

    # Remove veth interface
    if [[ -n "${LINK_SET}" && -n "${LINK}" ]]; then
        ip link del dev $LINK
    fi

    # Remove network namespace
    if [[ -n "${HOSTID_ADDED}" && -n "${HOSTID}" ]]; then
        ip netns del $HOSTID
    fi
}
trap finish EXIT

while getopts ":n:a:" opt; do
    case $opt in
        n)
            HOSTID=$OPTARG
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
if [ -z "${HOSTID}" ]; then
    echo "Virtual host id must be set with argument -n"
    exit 1
fi

if [ -z "${ADDRESS}" ]; then
    echo "Virtual address must be set with -a (e.g. -a 10.0.0.X)"
    exit 1
fi

LINK="nanonet-$HOSTID"
IFACE="eth-$HOSTID"
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
ip netns add $HOSTID
HOSTID_ADDED="True"

# Push interface to namespace
ip link set $IFACE netns $HOSTID

 # Set localhost to network namespace
ip netns exec $HOSTID ip link set dev lo up

ip -netns $HOSTID addr add $ADDRESS/24 dev $IFACE
ip -netns $HOSTID link set dev $IFACE up
ip -netns $HOSTID route add default via 10.0.0.1

ip netns exec $HOSTID echo "nameserver 8.8.8.8" >> /etc/resolv.conf
ip netns exec $HOSTID echo "nameserver 8.8.4.4" >> /etc/resolv.conf

echo "Opening host specific terminal. All commands will be run on the host network stack. Exit with 'exit'"
ip netns exec $HOSTID bash --rcfile <(cat ~/.bashrc ; echo "PS1=$HOSTID"-'$PS1')
