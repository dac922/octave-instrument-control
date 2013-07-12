function [out] = spoll (obj)
# function [out] = spoll (obj)
#

if (nargin < 1)
  print_usage();
end

if iscell (obj) && numel (obj) > 0
  if ~all (cellfun (@(x) isa (x,'octave_gpib'),obj))
    error ("obj contains wrong elements");
  end
  
  out = {};
  for i = 1:numel (obj)
    if ~gpib_spoll (obj{i})
      out{end+1} = obj{i};
    end
  end
  
  return
  
elseif (!isa (obj,'octave_gpib'))
  error ('spoll: need octave_gpib object');
end

out = [];
if ~gpib_spoll (obj)
  out = obj;
end
