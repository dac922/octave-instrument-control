function outs=ser_readto(fd,readto,timeout)

outs = [];
for l=1:timeout
    outs = [outs char(ser_read(fd,1000,100))];
    if (~isempty(findstr(outs,readto))), break; end;
end;

