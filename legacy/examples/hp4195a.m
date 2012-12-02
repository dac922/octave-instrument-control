function [data,freq] = hp4195a(register='A',gpibaddress=17)
% function [data,freq] = hp4195a(register='A',gpibaddress=17)
%
%   get data from HP 4195A
%
%         data        values of 'register'
%         freq        frequency points
%
%         data        hpgl binary data if register = "HPGL"
%

% get hardcopy
if strcmp(register,"HPGL")==1
  data = hp4195a_getplot(gpibaddress);
  freq = [];
  return;
end

gpib_write(gpibaddress,'FMT1'); % set format
gpib_read(gpibaddress,10000);   % dummy read

% for 401 points only
datafmt=13;
freqfmt=15;
numbers=401;
maxreaddata=datafmt*numbers+1;
maxreadfreq=freqfmt*numbers+1;

% read values
gpib_write(gpibaddress,[register '?']);
tmp=char(gpib_read(gpibaddress,maxreaddata))(1:maxreaddata-1);
data=str2num(reshape(tmp,datafmt,numbers)');

% read frequency
gpib_write(gpibaddress,'X?');
tmp=char(gpib_read(gpibaddress,maxreadfreq))(1:maxreadfreq-1);
freq=str2num(reshape(tmp,freqfmt,numbers)');

end


function hpgl = hp4195a_getplot(gpibaddress)
% function hpgl = hp4195a_getplot(gpibaddress)
%
%   get hpgl data from HP 4195A

  gpib_write(gpibaddress,'CPYM1');
  gpib_write(gpibaddress,'COPY');
  a=char(gpib_read(gpibaddress,1000000));

end
