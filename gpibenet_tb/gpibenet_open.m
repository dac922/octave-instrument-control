function [ret,ret2] = gpibenet_open(ip="192.168.10.4",port=44516)

% gpib_open: fid = ibdev(minor=0, id, sad=0, GPIBsession[id].timeout, send_eoi=1, eos_mode=0);
% gpib_write: gperr = ibwrt(GPIBsession[id].fid,cdata,datlen)) & ERR
% gpib_read: gperr = ibrd(GPIBsession[id].fid,cdata,readbytes);
% gpib close: ibonl(GPIBsession[i].fid,0);

datafmt=13;
freqfmt=15;
numbers=401;
maxreaddata=datafmt*numbers+1;
maxreadfreq=freqfmt*numbers+1;


  fd=gpibenet_ibdev(16)
  gpibenet_ibwrt(fd,"FMT1");
  gpibenet_ibwrt(fd,"A?");
  ret=gpibenet_ibrd(fd);
  gpibenet_ibwrt(fd,"X?");
  ret2=gpibenet_ibrd(fd);
  %ret=gpibenet_ibclr(fd);
  %gpibenet_ibwrt(fd,"ID?");
  %ret=gpibenet_ibrd(fd);
  gpibenet_ibonl(fd)
  %gpibenet_ibrsp;
end

function [ibsta,iberr,ibcntl] = getibsta(in)

  if length(in) == 12
    ibsta  = in(1)  * 256 + in(2);
    iberr  = in(3)  * 256 + in(4);
    ibcntl = in(11) * 256 + in(12);
  else
    warning(sprintf("wrong format_ len = %d",length(in)));
  end
end


function [ret,len,info] = gpibenet_sresp(fd)
%function [ret,len,info] = gpibenet_sresp(fd)
  % format of response:
  %   00 00 00 0c   c1 00 00 06  00 00 00 00 00 00 00 00
  %         -----   ----- -----                    -----
  %         length  ibsta iberr                    ibcntl

  len = char(tcp_read(fd,4,1000));

  if length(len) == 4
    info = eth2int(len(1:2),16);
    len  = eth2int(len(3:4),16);
    ret  = char(tcp_read(fd,len,1000));
  else
    len = length(len);
    ret = [];
  end

end



function [ibsta,iberr,ibcntl] = gpibenet_scmd(fd,args,writestr=[])

  tmp = char([args(:);zeros(12-length(args),1)]);
  %tcp_write(fd,tmp(end:-1:1)');
  tcp_write(fd,tmp');

  if length(writestr)>0
    tcp_write(fd,writestr);
  end

  ret = gpibenet_sresp(fd);
  [ibsta,iberr,ibcntl] = getibsta(ret);

end

% ibonl 
%   58 00 01 00 00 00 00 00 00 00 00 00
function gpibenet_ibonl(fd,val=0)
  gpibenet_scmd(fd,[0x58 0x00 0x01]);
  tcp_close(fd);
end


% ibdev
%  07 02 00 01 10 00 00 00  0d 00 04 00             ........ ....
%  50 05 00 00 00 00 00 00  00 00 00 00             P....... ....
%  07 00 18 01 00 00 00 00  0d 00 00 00             ........ ....
%  50 10 01 00 00 00 00 00  00 00 00 00             P....... ....
%  50 15 0b 00 00 00 00 00  00 00 00 00             P....... ....
%  58 01 01 00 00 00 00 00  00 00 00 00             X....... ....
%  4e 01 00 00 00 00 00 00  00 00 00 00             N....... ....
function fd=gpibenet_ibdev(pad,sad=0,tmo=13,eot=1,eos=0)

  ip="192.168.152.233";
  port=5000;
  fd = tcp_open(ip,port);

  gpibenet_scmd(fd,[0x07 0x02 0x00 0x01 pad  0x00 0x00 0x00 tmo 0x00 0x04 0x00]);
  gpibenet_scmd(fd,[0x07 0x00 0x18 0x01 0x00 0x00 0x00 0x00 tmo 0x00 0x00 0x00]);
  gpibenet_scmd(fd,[0x07  1     0  bitor(0x40,eot) pad sad eos 0 tmo]);
  gpibenet_scmd(fd,[0x07  0  0x5c  bitor(0x40,1)     0   0   0 0  13]);
  %gpibenet_ibonl(1);
end

function gpibenet_ibrsp(fd)
  gpibenet_scmd(fd,[0x19]);
end

% ibwrt (char) 0x62, (char) 0x0[3], (int) length, (int) 0x0, (char) *string
%   62 00 00 00 00 00 00 03 00 00 00 00 31 32 33
function gpibenet_ibwrt(fd,value)
  gpibenet_scmd(fd,[0x62 0x00 0x00 0x00 int2eth(length(value),32) int2eth(0)],char(value));
  sleep(1);
end

% ibrd  (char) 0x16, (char) 0x0[3], (int) length, (int) 0x0
%   16 00 00 00 00 00 07 d0 00 00 00 00
function out=gpibenet_ibrd(fd)

  ibstaend=0x2000;  % end
  ibsta=0;
  
  maxreadlen=2000;
  
  out=[];
  
  while bitand(ibsta,ibstaend) == 0

    [ibsta,iberr,ibcntl]=gpibenet_scmd(fd,[0x16 0x00 0x00 0x00 int2eth(maxreadlen) int2eth(0)]);

    out = [ out gpibenet_sresp(fd)];
    
    [ret,len,info] = gpibenet_sresp(fd);
    % response 00 01 00 00 on end of read request, don't know why
    if (len==0) && (info == 1)
      [ret,len,info] = gpibenet_sresp(fd);
      [ibsta,iberr,ibcntl] = getibsta(ret);
    end
  end

end

% ibclr
%   40 00 00 00 00 00 00 00 00 00 00 00
function ret=gpibenet_ibclr(fd)
  ret=gpibenet_scmd(fd,[0x40]);
end


function ret = int2eth(val,bits=32)
  ret = typecast(swapbytes(int32(val)),'char');
  %ret = [char(val&0xFF) char(val&0xFF00) char(val&0xFF0000) char(val&0xFF000000)];
end

function ret = eth2int(val,bits=32)
  %ret = typecast(val(4:-1:1),'int32');
  if (bits==32)
    ret = val(1) * 2.**24 + val(2) * 2.**16 + val(3) * 2.**8 + val(4);
  elseif (bits == 16)
    ret = val(1) * 2.**8 + val(2);
  else
    ret = val(1);
  end
  
end

