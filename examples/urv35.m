function [power]=urv35(init=0,serialport="/dev/ttyS0",baud=9600)
% [power]=urv35(init=0,serialport="/dev/ttyS0",baud=9600)
%
%     get power from R&S URV35
%

if (init==1)
  initurv35(serialport,baud);
end

fds=ser_open(serialport,baud);

ser_write(fds,sprintf("DM -100\n")); %tcp_read(fds,10000,100);  % set defined reference value
ser_write(fds,sprintf("X2\n")); %tcp_read(fds,10000,100);  % trigger and save to ref (measurement time?)
ser_read(fds,10000,200); % clear buffer

for k=1:50 % 50 * 200ms = 10 seconds timeout
  ser_write(fds,sprintf("Z0\n")); %outs = char(tcp_read(fds,17,2000)), length(outs)  % read ref  
  outs = ser_readto(fds,"\r\n",50);  
  out=sscanf(outs(7:end),"%f")(1);
  if (out != -100), break; else sleep(1); end;
end

ser_write(fds,sprintf("L0\n")); %tcp_read(fds,10000,100);  % local mode

ser_close(fds);

end


function initurv35(serialport,baud)

  fds=ser_open(serialport,baud);
  ser_write(fds,sprintf("C1\n")); %tcp_read(fds,10000,100);   % reset to default?
  ser_write(fds,sprintf("A0\n")); %tcp_read(fds,10000,100);   % something with display?
  ser_write(fds,sprintf("X0\n")); %tcp_read(fds,10000,100);
  ser_write(fds,sprintf("U1\n")); %tcp_read(fds,10000,100);   % U1 set dBm
  ser_write(fds,sprintf("KF0\n")); %tcp_read(fds,10000,100);  % freq correction off
  ser_write(fds,sprintf("KA0\n")); %tcp_read(fds,10000,100);  % atten correction off
  ser_write(fds,sprintf("SC0\n")); %tcp_read(fds,10000,100);  % scl off (0=dB, 1=0-10 ?)
  ser_write(fds,sprintf("N0\n")); %tcp_read(fds,10000,100);   % prefix on
  ser_write(fds,sprintf("R4\n")); %tcp_read(fds,10000,100);  % precision high
  ser_write(fds,sprintf("W3\n")); %tcp_read(fds,10000,100);  % format \r \n
  ser_close(fds);

end

