% simple telnet login

fd = tcp_open("192.168.100.1",23);

char(tcp_read(fd,1000,1000))
%   ans = !
%   ADSL Modem login:

tcp_write(fd,["root" char(10)]);
char(tcp_read(fd,1000,1000))
%   ans = root
%   Password:

tcp_write(fd,["password" char(10)]);
char(tcp_read(fd,1000,1000))
%   ans =
%
%
%   BusyBox v0.60.4 (2006.06.10-17:22+0000) Built-in shell (msh)
%   Enter 'help' for a list of built-in commands.
%
%
%
%   #

tcp_close(fd);
