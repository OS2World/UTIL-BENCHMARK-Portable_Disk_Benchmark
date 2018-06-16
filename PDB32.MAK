#
# Sample make file for 32-bit compiler
#
# The following is for IBM C Set++ 2.0

pdb32.exe: pdb32.obj
  link386 /pm:vio /align:16 /st:8192 /exepack /packc /packd pdb32.obj;

pdb32.obj: pdb.c
  icc /c /O+ /Gi+ /Gs+ /Fopdb32.obj pdb.c

