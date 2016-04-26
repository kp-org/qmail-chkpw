/* Based on "GNU libc manual" */
#include <time.h>
#ifdef HAS_CRYPT_H      /* use crypt.h from glibc */
#include <crypt.h>
#endif

char *doencrypt(char *usrpw, int s)
{
  unsigned long seed[2];
  char salt[] = "$6$........";
  const char *const seedchars =
    "./0123456789ABCDEFGHIJKLMNOPQRST"
    "UVWXYZabcdefghijklmnopqrstuvwxyz";
  int i;

  /* Generate a (not very) random seed.
     You should do it better than this... */
  seed[0] = time(NULL);
  seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);

  /* Turn it into printable characters from seedchars. */
  for (i = 0; i < 8; i++)
    salt[3+i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];

  /* Read the user's password and encrypt it. */
  usrpw = (char *)crypt(usrpw, salt);

  /* Print the result if nessecary */
  if(s == 1 ) fprintf(stderr,"%s\n",usrpw);    /* pipe */

  return (char *)usrpw;		/* 0.2: cast pointer */
}
