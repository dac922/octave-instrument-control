function outs=ser_readto2(fd,readto,timeout,flush)

if (nargin < 4), flush=0; end

outs = [];
a=tic;
while ( (tic-a) < (timeout * 1000) )
    outs = [outs char(ser_read(fd,100000,100))];
    if (~isempty(findstr(outs,readto))), break; end;
end

if (flush == 1)
  maxbuf=100000;
  while (length(ser_read(fd,maxbuf,0))==maxbuf)
  end
end

