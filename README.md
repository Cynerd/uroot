uroot (User's root)
===================
Tool that uses Linux namespaces and resource separation to provide root like
functionality even under unprivileged user on Linux systems. This is more commonly
known as unprivileged containers. Difference between this project and full fledge
containerization solution is that this tools tries to provide less separation to
allow more versatile uses. See example usages if you are not sure what this can
do. There is also section with limitations stating what you can't do with this
tool.

In general this tools allows you to imaginary become root. This has a lot of use
cases outside of just creating full containers. You can use it to control some bad
behaving program without fully separating it from host system.

System setup
------------
TODO (describe shadow requirements)

Limitations of this tools
-------------------------
This tool is not perfect as well as technology it uses is not perfect. There can
be bugs and there are for sure unimplemented features. This section provides you
with information about some known problems that we are unable to solve because of
limitations of used technology. Please check this list before you report problem
or even before you use tool it self.

### Block devices are no go
Unfortunately current implementation of namespaces, primarily mount points
unshare, does not support usage of block devices subsystem. That is kernel
subsystem handling access to storage devices. Most of kernel file system drivers
are implemented on top of block devices and because of that non of those file
systems can be used. This means that you can modify (mount) only already mounted
file systems or system file systems such as tmpfs or procfs. Allowing user access
to `/dev` device is not enough to fix this issue. This also means that you are not
able to use FUSE file systems.

Example usages
--------------
On top of making you look cool that you are able to get root on system you should
not (those hacking skills) this tool also have some real live uses. Some of them
can be clear cut but some usages might not be immediately clear. That is the main
reason why this section exists. It also should give you hints to common traps.

### chroot
TODO

### Single killable process
TODO

### Network isolation
TODO
