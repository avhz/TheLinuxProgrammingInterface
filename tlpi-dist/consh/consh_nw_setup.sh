#!/bin/sh
#
# consh_nw_setup.sh
#
# (C) 2023, Michael Kerrisk
#
# Licensed under the GNU General Public License version 2 or later

if test $# -ne 4; then
cat << EOF 1>&2
Usage: $0 <consh-PID> <netns-name> <IP-addr-0> <IP-addr-1>"

where:
    consh-PID	is the PID of the container shell
    netns-name	is the name to be used for the bind mount needed by 'ip netns'
    IP-addr-0	is the network ID to be assigned to the veth end point that
		sits outside the container namespace
    IP-addr-1	is the network ID to be assigned to the veth end point that
		sits inside the container namespace

Example:
    $0 1234 consh 10.0.0.1/24 10.0.0.2/24
EOF
    exit 1
fi

consh_pid=$1
netns_name=$2
ipaddr_0=$3
ipaddr_1=$4

# Create the bind mount that is needed by 'ip netns'

linkpath=/var/run/netns/$netns_name
sudo touch $linkpath
if test $? -ne 0; then
    echo "Failed to touch bind mount file; does $linkpath already exist?" 1>&2
    exit 1
fi

sudo mount --bind /proc/$consh_pid/ns/net $linkpath

# Create a veth pair

vethname="$netns_name-$$"
sudo ip link add $vethname-0 type veth peer name $vethname-1

# Move one end of the veth pair into the container network namespace

sudo ip link set $vethname-1 netns $netns_name

# Assign addresses to the veth end points and bring them up

sudo ip addr add $ipaddr_0 dev $vethname-0
sudo ip link set $vethname-0 up

sudo ip netns exec $netns_name ip address add $ipaddr_1 dev $vethname-1
sudo ip netns exec $netns_name ip link set $vethname-1 up
