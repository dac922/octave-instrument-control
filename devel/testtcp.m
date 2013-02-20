function testtcp

ip="127.0.0.1";
port=8000;
echostr="echotest\n";

timeout=1000;
tol=0.1;                 % 10 percent tolerance

% test error
fd=tcp(ip,port);

% test connect, write and read
system ("killall socat 2>/dev/null; socat PIPE TCP4-LISTEN:8000 &",0);
sleep(1);

fd=tcp(ip,port);

% test read timeout1
start=tic;
result = tcp_read(fd,10000,1000);
timeout1 = double(tic - start)/1000
testresult1 = tol > abs(1 - timeout1/timeout);

% write to socat
tcp_write(fd,echostr);

% read 4 chars, no timeout
start=tic;
result = tcp_read(fd,4,1000);
timeout2 = double(tic - start)/1000;
testresult2 = timeout2 < timeout;

% check read result
result = char(result);
testresult3 = strcmp(echostr(1:4),result);

% close tcp
tcp_close(fd);

%system ("killall socat");


%
printf("timeout test 1:   %d (%f)\n",testresult1,timeout1);
printf("timeout test 2:   %d (%f)\n",testresult2,timeout2);
printf("timeout test 3:   %d\n",testresult3);

