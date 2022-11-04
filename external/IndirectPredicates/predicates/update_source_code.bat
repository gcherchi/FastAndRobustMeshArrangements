DEL indirect_predicates.h
DEL indirect_predicates.cpp
FOR %%f in (..\JPCK\predicates\direct\*.txt) DO ..\JPCK\x64\Release\converter.exe %%f -p
FOR %%f in (..\JPCK\predicates\indirect\*.txt) DO ..\JPCK\x64\Release\converter.exe %%f -p
