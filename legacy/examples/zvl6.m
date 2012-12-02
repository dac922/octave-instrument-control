function varargout = zvl6(varargin)
% function [data1, freq1, data2, freq2, ..., errors] = zvl6(ip|fd,trace1,trace2,...)
%
%    datax, freqx        data and frequency points of tracex
%                           tracex is 'Trc1" in NWA mode
%                           tracex is 'CH1DATA' in SPA mode
%
%    ip|fd               ip = ip address of ZVL6 or
%                        fd = file descriptor of open vxi11 session
%

    ntraces=nargin-1;

    % pre checks
    if (nargin < 1), error("zvl6_nwatracedata: no ip address"); end
    if (nargin < 2), error("zvl6_nwatracedata: no trace"); end
    if (nargout != ntraces*2) && (nargout != ntraces*2 + 1)
        error("zvl6_nwatracedata: wrong number of output elements");
    end

    % change function pointers if first arg seems to be a file descriptor
    if length(varargin{1}) == 1      % assume it's a file descriptor if length == 1
        vxi11_open = @(x) x;         % don't open new descriptor, use parameter as fd
        vxi11_close = @() [];        % don't close descriptor
    end

    fd = vxi11_open(varargin{1});

    % check if NWA or SPA is active
    vxi11_write(fd,sprintf("INST:SEL?\n"));
    if (isempty(findstr(char(vxi11_read(fd,100000,1000)),"NWA")))
        getdatazvl6_tracedata = @getdatazvl6_spectracedata;
        hwerror=getdatazvl6_specerror(fd);
    else
        getdatazvl6_tracedata = @getdatazvl6_nwatracedata;
        hwerror=getdatazvl6_nwaerror(fd);
    end

    % get data
    for trace = 0:ntraces-1
        [varargout{trace*2+1},varargout{trace*2+2}] = getdatazvl6_tracedata(fd,varargin{trace+2});
    end

    % error value is always the last output argument
    varargout{trace*2+3} = hwerror;

    vxi11_close(fd);

endfunction


function hwerror = getdatazvl6_nwaerror(fd)
% get error status in NWA mode
    vxi11_write(fd,"STAT:QUES:INT:HARD?\n");
    hwerror=str2num(char(vxi11_read(fd,1000000,1000)))
    if bitand(hwerror,2^3), warning("RF overload"); end
    if bitand(hwerror,2^4), warning("IF overload"); end
    if bitand(hwerror,2^5), warning("LO unlocked"); end
    if bitand(hwerror,2^1), warning("ExtRef unlocked"); end
    if bitand(hwerror,2^8), warning("OCXO temp too low"); end
endfunction


function hwerror = getdatazvl6_specerror(fd)
% get error status in SPA mode
    vxi11_write(fd,"STAT:QUES:POW?\n");
    hwerror=str2num(char(vxi11_read(fd,10000,1000)));
    if (hwerror==1), warning("RF overload"); end
endfunction


function [data,freq] = getdatazvl6_nwatracedata(fd,trace)
% get trace data in NWA mode

    vxi11_write(fd,sprintf("FORMAT:DATA ASCII\n"));
    vxi11_write(fd,sprintf("CALC:PAR:SEL '%s'\n",trace));
  
    %vxi11_write(fd,sprintf("TRACE:DATA:RESPONSE? CH1DATA\n"));
    vxi11_write(fd,sprintf("CALC:DATA? SDAT\n"));
    tmp=char(vxi11_read(fd,1000000,1000));
    tmp(tmp==',')=10; tmp=sscanf(tmp,"%f"); tmp=reshape(tmp,2,length(tmp)/2)';
    data=tmp(:,1)+i*tmp(:,2);
  
    %vxi11_write(fd,sprintf("TRACE:DATA:STIMULUS? CH1DATA\n"));
    vxi11_write(fd,sprintf("CALC:DATA:STIM?\n"));
    tmp=char(vxi11_read(fd,1000000,1000));
    tmp(tmp==',')=10;
    freq=sscanf(tmp,"%f");

endfunction


function [data,freq] = getdatazvl6_spectracedata(fd,trace)
% get trace data in SPA mode

    %vxi11_write(fd,sprintf("FORMAT REAL,32\n"));                               % reset
    vxi11_write(fd,sprintf("FORMAT ASCII\n"));                               % reset
    vxi11_write(fd,sprintf("TRAC? %s\n",trace));                               % reset
    tmp=char(vxi11_read(fd,1000000,1000));
    data=str2num(tmp);

    vxi11_write(fd,sprintf("SENS:SWEEP:POINTS?\n"));
    points=str2num(char(vxi11_read(fd,10000000,1000)));
    vxi11_write(fd,sprintf("FREQ:START?\n"));
    fstart=str2num(char(vxi11_read(fd,10000000,1000)));
    vxi11_write(fd,sprintf("FREQ:STOP?\n"));
    fstop=str2num(char(vxi11_read(fd,10000000,1000)));
    freq=linspace(fstart,fstop,points);

end
