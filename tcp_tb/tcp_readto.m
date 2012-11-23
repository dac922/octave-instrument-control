function outs=tcp_readto(fd,readto,timeout)

outs = [];
for l=1:timeout
    outs = [outs char(tcp_read(fd,1000,100))];
    if (~isempty(findstr(outs,readto))), break; end;
end;

