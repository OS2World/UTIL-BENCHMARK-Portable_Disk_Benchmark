/*----------------------------------------------------------------------*
 | Portable Disk Benchmark                                              |
 |                                                                      |
 | Written by Albert J. Shan.                                           |
 |   CompuServe: 70730,401                                              |
 |   Internet:   70730.401@compuserve.com                               |
 |                                                                      |
 | A portable C benchmark program for testing disk cache and file       |
 | I/O efficiency.                                                      |
 |                                                                      |
 | Version 0.01  03/09/94                                               |
 | - Initial release.  Currently, it is limited to 127 512-byte blocks  |
 |   (65024 bytes) due to the requirement of 16-bit operating systems   |
 |   such as DOS and OS/2 1.x.  These OSes do not provide a file system |
 |   call to perform write to the disk with >= 64K bytes.  The C        |
 |   compiler must support timeb data structures in order to provide    |
 |   reasonably accurate result.  If you're recompiling with a 16-bit   |
 |   compiler, make sure you invoke the Large Memory Model.  It should  |
 |   be compiled with the most aggressive optimization switches.        |
 *----------------------------------------------------------------------*/

typedef  unsigned long ULONG;
#include <io.h>
#include <fcntl.h>
#include <sys\timeb.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>


/* ANSI Standard pseudo-random number generator.
   For eliminating C Library random number generator dependency. */

static unsigned long int next = 1;

int pdb_rand (void)
{
  next = next * 1103515245L + 12345L;
  return ((int) (next / 0x10000L) & 0x7FFF);
}

void pdb_srand (unsigned int seed)
{
  next = seed;
}



/*----------------------------*
 | Calculate time difference. |
 *----------------------------*/

long DiffTime (struct timeb tStop, struct timeb tStart)
{
  return ((tStop.time - tStart.time) * 1000L + (long)((long)tStop.millitm - (long)tStart.millitm));
}



/*--------------------------------------------------------------*
 | 6-pass Disk Benchmark routine.                               |
 |   1. Sequential Write                                        |
 |   2. Random Read                                             |
 |   3. Random Write (same data)                                |
 |   4  Sequential Read                                         |
 |   5. Random Read                                             |
 |   6. Random Write (changed data)                             |
 |                                                              |
 | Only unbuffered file I/O calls are used to avoid compiler    |
 | efficiency in buffered I/O calls.  It is all up to the file  |
 | system to provide the caching.                               |
 *--------------------------------------------------------------*/

int DiskBench (long FileSize, long BlockSize)
{
  long BlkLoc, tTotal;
  int  hFile, i, Limit, NumRW;
  long tWrite, tRead, tRandomWrite, tRandomRead;
  long tRandomWrite2, tRandomRead2;
  struct timeb tStart, tStop;
  char *buf;

  buf = malloc ((unsigned int)BlockSize);
  Limit = (int)(FileSize * 1024L * 1024L / BlockSize);

  /* Limit randow read/write to 1MB data regardless of BlockSize */
  NumRW = (int)(1024L * 1024L / BlockSize);

  /* File create */
  hFile = open ("PDBTEST.$$$", O_CREAT | O_RDWR | O_BINARY, S_IREAD | S_IWRITE);
  if (hFile == -1)
  {
    printf ("Can't open file\n");
    return (1);
  }

  /* Initialize buffer block */
  memset (buf, 'A', (unsigned int)BlockSize);

  /* Sequential write */
  ftime (&tStart);
  for (i = 0; i < Limit; i++)
    write (hFile, buf, (unsigned int)BlockSize);
  ftime (&tStop);
  tWrite = DiffTime (tStop, tStart);
  printf ("Sequential write: %3ld.%02ld secs\n", tWrite / 1000, (tWrite % 1000) / 10);

  pdb_srand (12345);

  /* Random read */
  lseek (hFile, 0L, SEEK_SET);
  ftime (&tStart);
  for (i = 0; i < NumRW; i++)
  {
    BlkLoc = (long)(pdb_rand () % Limit) * BlockSize;
    lseek (hFile, BlkLoc, SEEK_SET);
    read (hFile, buf, (unsigned int)BlockSize);
  }
  ftime (&tStop);
  tRandomRead = DiffTime (tStop, tStart);
  printf ("     Random read: %3ld.%02ld secs\n", tRandomRead / 1000, (tRandomRead % 1000) / 10);

  pdb_srand (54321U);

  /* Random write with SAME data */
  lseek (hFile, 0L, SEEK_SET);
  ftime (&tStart);
  for (i = 0; i < NumRW; i++)
  {
    BlkLoc = (long)(pdb_rand () % Limit) * BlockSize;
    lseek (hFile, BlkLoc, SEEK_SET);
    write (hFile, buf, (unsigned int)BlockSize);
  }
  ftime (&tStop);
  tRandomWrite = DiffTime (tStop, tStart);
  printf ("    Random write: %3ld.%02ld secs\n", tRandomWrite / 1000, (tRandomWrite % 1000) / 10);

  /* Sequential read */
  lseek (hFile, 0L, SEEK_SET);
  ftime (&tStart);
  for (i = 0; i < Limit; i++)
    read (hFile, buf, (unsigned int)BlockSize);
  ftime (&tStop);
  tRead = DiffTime (tStop, tStart);
  printf (" Sequential read: %3ld.%02ld secs\n", tRead / 1000, (tRead % 1000) / 10);

  pdb_srand (12345);

  /* Random read pass 2 with same random seed */
  lseek (hFile, 0L, SEEK_SET);
  ftime (&tStart);
  for (i = 0; i < NumRW; i++)
  {
    BlkLoc = (long)(pdb_rand () % Limit) * BlockSize;
    lseek (hFile, BlkLoc, SEEK_SET);
    read (hFile, buf, (unsigned int)BlockSize);
  }
  ftime (&tStop);
  tRandomRead2 = DiffTime (tStop, tStart);
  printf ("     Random read: %3ld.%02ld secs\n", tRandomRead2 / 1000, (tRandomRead2 % 1000) / 10);

  pdb_srand (54321U);

  /* Random write pass 2 with changed data */
  memset (buf, 'B', (unsigned int)BlockSize);
  lseek (hFile, 0L, SEEK_SET);
  ftime (&tStart);
  for (i = 0; i < NumRW; i++)
  {
    BlkLoc = (long)(pdb_rand () % Limit) * BlockSize;
    lseek (hFile, BlkLoc, SEEK_SET);
    write (hFile, buf, (unsigned int)BlockSize);
  }
  ftime (&tStop);
  tRandomWrite2 = DiffTime (tStop, tStart);
  printf ("    Random write: %3ld.%02ld secs\n", tRandomWrite2 / 1000, (tRandomWrite2 % 1000) / 10);

  tTotal = (long)tWrite + (long)tRead + (long)tRandomWrite +
           (long)tRandomRead + (long)tRandomRead2 + (long)tRandomWrite2;
  printf ("           Total: %3ld.%02ld secs\n", tTotal / 1000L, (tTotal % 1000L) / 10);

  close (hFile);
  unlink ("PDBTEST.$$$");
  free (buf);

  return (0);
}



/*---------------*
 | Main program. |
 *---------------*/

int main (int argc, char *argv[])
{
  long FileSize, BlockSize;

  if (argc != 3)
  {
    printf ("\nPortable Disk Benchmark  V0.01  03/09/94\n\n");
    printf ("usage: PDB [File size] [Blocks]\n");
    printf ("  File size - File size in MB.  Allowable range: 1 to 32MB.\n");
    printf ("  Blocks    - Number of blocks to read/write per I/O call.\n");
    printf ("              Each block is 512 bytes.  Allowable range: 1 to 127.\n\n");
    printf ("example: pdb 2 4\n");
    return (1);
  }

  FileSize = max (1L, atoi (argv[1]));
  FileSize = min (32L, FileSize);
  BlockSize = max (1L, atol (argv[2]));
  BlockSize = min (127L, BlockSize);
  BlockSize *= 512L;

  printf ("File size: %2ld MB  Block size: %5ld bytes\n", FileSize, BlockSize);

  return (DiskBench (FileSize, BlockSize));
}

