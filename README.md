# edison-dfrobot


# Performing UDP tunneling through an SSH connection
# REFERENCE: http://www.qcnetwork.com/vince/doc/divers/udp_over_ssh_tunnel.html

local$ mkfifo /tmp/fifo
local$ sudo nc -l -u -p 8888 < /tmp/fifo | nc ssh.forwarder.com 9013 > /tmp/fifo

After the above, whatever is sent to UDP port 8888, can transform into the TCP
stream on ssh-forwarder port 9013.

For the remote server-side, the opposite has to receive the above...

remote$ mkfifo /tmp/fifo
remote$ nc -l -p 9013 or 8083 < /tmp/fifo | nc -u localhost 8888 > /tmp/fifo

Of course, a server awaiting on port 8888 must be run first before the above
command is run.

