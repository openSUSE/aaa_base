/* Copyright (C) 2000-2002 SuSE Linux AG, Nuremberg.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

static inline int my_is_alnum_punct(char c)
{
  return isdigit(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') 
    || c == '.' || c == ',' || c == '-' || c == '_' || c == '+';
}

int
main (int argc, char *argv[])
{
  FILE *fp;
#define MAX_VERSION_LENGTH 80
  char buffer[4096 + MAX_VERSION_LENGTH]; /* buffer + sizeof ("Linux version .....") */
  char command[512] = "";
  int found = 0;

  if (argc != 2)
    {
      char msg[] = "usage: get_kernel_version <kernel_image>\n";
      write (2, msg, sizeof (msg));
      return 1;
    }

  /* check if file exist and is compressed */
  {
#define DETECT_SIZE 6
    unsigned char  buf [DETECT_SIZE];
    int fd = open (argv[1], O_RDONLY | O_CLOEXEC);
    if (fd == -1)
      {
	fprintf (stderr, "Cannot open kernel image \"%s\"\n", argv[1]);
	return 1;
      }

    if (read (fd, buf, DETECT_SIZE) != DETECT_SIZE)
      {
	fprintf (stderr, "Short read\n");
	close (fd);
	return 1;
      }

    /* \xFD7zXZ\x00 */
    if ((buf [0] == 0xfd) &&
	(buf [1] == '7') &&
	(buf [2] == 'z') &&
	(buf [3] == 'X') &&
	(buf [4] == 'Z') &&
	(buf [5] == 0))
      {
	snprintf (command, sizeof (command), "/usr/bin/xz -dc %s 2>/dev/null", argv[1]);
      }

    if (buf [0] == 037 && (buf [1] == 0213 || buf [1] == 0236))
      {
	snprintf (command, sizeof (command), "/bin/gzip -dc %s 2>/dev/null", argv[1]);
      }

    if (*command) {
	fp = popen (command, "re");
	if (fp == NULL)
	  {
	    fprintf (stderr, "%s: faild\n", command);
	    close (fd);
	    return 1;
	  }
      }
    else
      {
	fp = fopen (argv[1],"re");
      }
    close (fd);
  }

  memset (buffer, 0, sizeof (buffer));


  while (!found)
    {
      ssize_t in;
      int i;

      in = fread (&buffer[MAX_VERSION_LENGTH],
		  1, sizeof (buffer) - MAX_VERSION_LENGTH, fp);

      if (in <= 0)
	break;
      for (i = 0; i < in; i++)
	if (buffer[i] == 'L' && buffer[i+1] == 'i' &&
	    buffer[i+2] == 'n' && buffer[i+3] == 'u' &&
	    buffer[i+4] == 'x' && buffer[i+5] == ' ' &&
	    buffer[i+6] == 'v' && buffer[i+7] == 'e' &&
	    buffer[i+8] == 'r' && buffer[i+9] == 's' &&
	    buffer[i+10] == 'i' && buffer[i+11] == 'o' &&
	    buffer[i+12] == 'n' && buffer[i+13] == ' ' &&
	    /* current s390 images have that string without version: */
	    buffer[i+14] != 0 && buffer[i+17] != 0)
          {
	    int j = i+14;
	    int number_dots = 0;

	    /* check if we really found a version */
	    for (j = j+1; buffer[j] != ' '; j++)
	      {
		char c = buffer[j];

		if (c == '.') {
		  number_dots++;
		  continue;
		}

		if (((number_dots < 2) && !isdigit(c)) ||
		    ((number_dots >= 2) && !my_is_alnum_punct(c)))
		  {
		    fprintf(stderr, "invalid char %c\n", c);
		    break;
		  }
	      }
	    /* Use the result anyway */
	    found = 1;
	    break;
          }

      if (found)
	{
	  int j;
	  for (j = i+14; buffer[j] != ' '; j++);
	  buffer[j] = '\0';
	  printf ("%s\n", &buffer[i+14]);
	}
      else
	{
	  if (in < (ssize_t)(sizeof (buffer) - MAX_VERSION_LENGTH))
	    break;
	  memcpy (&buffer[0], &buffer[sizeof (buffer) - MAX_VERSION_LENGTH],
		  MAX_VERSION_LENGTH);
	  memset (&buffer[MAX_VERSION_LENGTH], 0,
		  sizeof (buffer) - MAX_VERSION_LENGTH);
	}
    }

  if(!found) {
    /* ia32 kernel */
    if(
      !fseek(fp, 0x202, SEEK_SET) &&
      fread(buffer, 1, 4, fp) == 4 &&
      buffer[0] == 0x48 && buffer[1] == 0x64 &&
      buffer[2] == 0x72 && buffer[3] == 0x53 &&
      !fseek(fp, 0x20e, SEEK_SET) &&
      fread(buffer, 1, 2, fp) == 2
    ) {
      unsigned ofs = 0x200 + ((unsigned char *) buffer)[0] + (((unsigned char *) buffer)[1] << 8);

      if(
        !fseek(fp, ofs, SEEK_SET) &&
        fread(buffer, 1, MAX_VERSION_LENGTH, fp) == MAX_VERSION_LENGTH
      ) {
        char *s = buffer;

        for(s[MAX_VERSION_LENGTH] = 0; *s; s++) if(*s == ' ') { *s = 0; break; }
        if(*buffer) {
          found = 1;
          printf("%s\n", buffer);
        }
      }
    }
  }
  if(!found) {
    /* Compressed S390x kernel */
    /* Compare with vanilla kernel git commit 6abe28197024f */
    if(
      !fseek(fp, 0x10008, SEEK_SET) &&
      fread(buffer, 1, 6, fp) == 6 &&
      buffer[0] == 'S' && buffer[1] == '3' &&
      buffer[2] == '9' && buffer[3] == '0' &&
      buffer[4] == 'E' && buffer[5] == 'P' &&
      !fseek(fp, 0x10428, SEEK_SET) &&
      fread(buffer, 1, 8, fp) == 8
    ) {
      unsigned long long version_string_offset =
	((unsigned char *) buffer)[7] +
	(((unsigned char *) buffer)[6] << 8) +
	(((unsigned char *) buffer)[5] << 16) +
	(((unsigned char *) buffer)[4] << 24) +
	((unsigned long long) ((unsigned char *) buffer)[3] << 32) +
	((unsigned long long) ((unsigned char *) buffer)[2] << 40) +
	((unsigned long long) ((unsigned char *) buffer)[1] << 48) +
	((unsigned long long) ((unsigned char *) buffer)[0] << 56);
      if(
        !fseek(fp, version_string_offset, SEEK_SET) &&
        fread(buffer, 1, MAX_VERSION_LENGTH, fp) == MAX_VERSION_LENGTH
      ) {
        char *s = buffer;

        for(s[MAX_VERSION_LENGTH] = 0; *s; s++) if(*s == ' ') { *s = 0; break; }
        if(*buffer) {
          found = 1;
          printf("%s\n", buffer);
        }
      }
    }
  }

  if (command[0] != '\0')
    pclose (fp);
  else
    fclose (fp);

  if (found)
    return 0;
  else
    return 1;

  return 0;
}

/* vim: set sw=4 ts=8 sts=4 noet: */
