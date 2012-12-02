% do some communication with my adsl modem via serial line
fd = ser_open("/dev/ttyS0",115200);

% something available?
char(ser_read(fd,200,2000))
%    ans =
%

% write command
ser_write(fd,["uptime" char(10)]);
char(ser_read(fd,2000,2000))
%    ans = uptime
%
%     19:00:50 up 2 min, load average: 0.03, 0.04, 0.01
%
%    / #
ser_close(fd)

