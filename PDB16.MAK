#
# Sample make file for OS/2 16-bit compiler
#
# The following is for MSC 6.0a

pdb16.exe: pdb16.obj
  link /pm:vio /st:8192 /exepack pdb16.obj;

pdb16.obj: pdb.c
  cl /c /W3 /AL /Oaxz /G2s /Fopdb16.obj pdb.c

