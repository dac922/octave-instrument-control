function [itemp,ihum,err,tmp]=hc4057(temp=[],hum=[],addr=0,com="/dev/ttyS0",baud=9600)
% function [itemp,ihum,err,tmp]=hc4057(temp,hum,addr,com,baud)
%
%   get and set temperature and humidity of climatic chamber
%
%      [itemp,ihum,err,tmp]=hc4057
%          get actual temperature and humidity
%
%      [itemp,ihum,err,tmp]=hc4057([])
%          stop climatic chamber
%
%      [itemp,ihum,err,tmp]=hc4057(temp, hum, addr, com, baud)
%          temp            set target temperature
%          hum             set target humidity, default = []
%                          if 'hum' is empty, humidity is switched off
%          addr            address of chamber, default = 0
%          com             serial port used, default = "/dev/ttyS0"
%          baud            baudrate, default = 9600
%

fd=ser_open(com,baud);

% format: $aaI   ($ = start, aa = address, I = read out)
ser_write(fd,sprintf("$%02dI\r",addr));
tmp=char(ser_read(fd,1000,1000));

% answer: (1) target temperature, (2) actual temperature
%         (3) target humidity,    (4) actual humidity
%         (5) ... (14)
%         format: xxxxx.x
%
%         (0)       0                 
%         (1)       START
%         (2)       error
%         (3)       temperature
%         (4)       humidity
%         (5) ... (23)
%         format: b   (0 or 1)
%
itemp=sscanf(tmp((1:6)+7*1),"%f");   % actual temperature
ihum=sscanf(tmp((1:6)+7*3),"%f");    % actual humidity
err=sscanf(tmp(7*14+1+2),"%d");      % error status


% if there are input arguments or chamber reports error
if ((nargin>0) || (err!=0) )

  if (isempty(temp) || err == 1)
    % turn off chamber on error or if requested
    temp=20;
    atemp=0;
    hum=20;
    ahum=0;
    start=0;
  else
    % start chamber and activate temperature
    start=1;
    atemp=1;
  end

  if (isempty(hum))
    % disable humidity on request
    hum=20;
    ahum=0;
  else
    % enable humidity
    ahum=1;
  end

  % write to chamber
  ser_write(fd,
	sprintf("$%02dE %06.1f %06.1f 0020.0 0020.0 0020.0 0020.0 0020.0 0%d0%d%d00000000000\r",
		addr,temp,hum,start,atemp,ahum));

  char(ser_read(fd,1000,1000))

  % reprot error
  if (err != 0)
    warning("climatic chamber error - paused - fix error press enter");
    pause
  end

end

ser_close(fd);
