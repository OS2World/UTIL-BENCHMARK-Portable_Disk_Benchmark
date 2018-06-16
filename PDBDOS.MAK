#
# Sample make file for DOS 16-bit compiler
#
# The following is for MSC 6.0a

pdbdos.exe: pdbdos.obj
  link /st:8192 /exepack pdbdos.obj;

pdbdos.obj: pdb.c
  cl /c /W3 /AL /Oaxz /G2s /Fopdbdos.obj pdb.c

