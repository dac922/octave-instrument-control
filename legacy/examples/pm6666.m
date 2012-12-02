function [freq,tmp]=pm6666(init=0,gpibaddress=6)
% function [freq,tmp]=pm6666(gpibaddress=6)
%
%

if (init == 1)
  pm6666_init(gpibaddress);
end

gpib_write(gpibaddress,'FRUN OFF');
gpib_write(gpibaddress,'OUTM 2');
gpib_write(gpibaddress,'X');
sleep(3);
tmp=char(gpib_read(gpibaddress,10000));
freq=sscanf(tmp,'FREQ %f');


gpib_write(gpibaddress,'D');
gpib_write(gpibaddress,'MTIME 1');
gpib_write(gpibaddress,'FRUN ON');

end


function pm6666_init(gpibaddress)
  gpib_write(gpibaddress,'D'); % power on reset
  gpib_write(gpibaddress,'FREQ A');
  gpib_write(gpibaddress,'MTIME 1');
  gpib_write(gpibaddress,'INPA');
  gpib_write(gpibaddress,'COUPL DC');
end
