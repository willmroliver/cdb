%macro define 1
    %ifdef __MACH__
      global _%1
      _%1:
    %else
      global %1
      %1:
    %endif
%endmacro

