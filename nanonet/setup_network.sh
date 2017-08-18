#!/bin/bash

FORWARD=""
FORWARD_SET=""
BRIDGE_SET=""
IPTABLES_SET=""
RP_FILTER_SET=""
RP_FILTER=""
# Define function to revert all settings
function finish {

    echo "Tearing down network"
    # Remove added rules to ip tables
    if [ -n "${IPTABLES_SET}" ]; then
        echo "Reverting routing tables"
        iptables -t nat -D POSTROUTING -o "nanonet-bridge" -j MASQUERADE
        iptables -t nat -D POSTROUTING -o eno1 -j MASQUERADE
    fi

    # Remove bridge interface
    if [ -n "${BRIDGE_SET}" ]; then
        echo "Removing bridge interface"
        ip link del dev "nanonet-bridge"
    fi

    # Reset IP forward to original setting
    if [ -n "${FORWARD_SET}" && -n "{$FORWARD}" ]; then
        echo "Reverting IP forwarding"
        echo $FORWARD > /proc/sys/net/ipv4/ip_forward
    fi

    # Revert rp_filter default value
    if [ -n "${RP_FILTER_SET}" && -n "${RP_FILTER}" ]; then
        echo "Reverting reverse path filter"
        echo $RP_FILTER > /proc/sys/net/ipv4/conf/default/rp_filter
    fi
}
trap finish EXIT

# Save IP Forward setting
FORWARD="${cat /proc/sys/net/ipv4/ip_forward}"

# Enable IP forwarding
echo "Setting up IP forwarding"
echo 1 > /proc/sys/net/ipv4/ip_forward
FORWARD_SET="True"

# Save rp_filter setting
RP_FILTER="${cat /proc/sys/net/ipv4/conf/default/rp_filter}"
echo "Setting reverse path routing filter to \"loose\""
echo 2 > /proc/sys/net/ipv4/conf/default/rp_filter
RP_FILTER_SET="True"

# Create needed interfaces
echo "Creating bridge interface \"nanonet-bridge\" with IP 10.0.0.1/24"
ip link add "nanonet-bridge" type bridge
ip addr add 10.0.0.1/24 dev "nanonet-bridge"
ip link set "nanonet-bridge" up
BRIDGE_SET="True"

echo "Setting up routing tables for nanonet-bridge and $eth interfaces"
# Set NATing on this host
iptables -t nat -A POSTROUTING -o "nanonet-bridge" -j MASQUERADE
iptables -t nat -A POSTROUTING -o eno1 -j MASQUERADE
IPTABLES_SET="True"

### Block program here ###
cat
