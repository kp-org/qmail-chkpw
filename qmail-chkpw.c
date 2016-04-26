#include "global.h"
#include "md5.h"
#include "hmac_md5.h"
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>

#define LINE_MAX 256
#define SASLUSERFILE "QMAILHOME/users/smtpauth"
#define READONLY "r"
char up[513];
int uplen;
static char hextab[]="0123456789abcdef";

#include "pwcrypt.c"
#include "pwmknew.c"

char *clearpw;

int doit(const char *testlogin, const char *response, unsigned char *challenge)
{
  static char line[LINE_MAX + 1];
  int found_user= 0;
  unsigned char *password = NULL;	/* 0.2: orig was 'const char */
  unsigned char digest[16];
  unsigned char digascii[33];		/* 0.2: orig was 'const char */
  unsigned char h;
  FILE *fp;
  int j;
  char *linepnt;

  int i;
  char authtype[4];

  if ((fp = fopen(SASLUSERFILE, READONLY)) == NULL) _exit(2);
  while (fgets(line, LINE_MAX, fp) != NULL) {
	if ((linepnt = strchr(line, '\n')) != NULL) {
	  *linepnt = 0;
    }
    if ((linepnt = strtok(line, ":")) == NULL) _exit(2);
    if (strcmp(linepnt, testlogin) == 0) {
	if ((linepnt = strtok(NULL, ":")) == NULL) _exit(2);
	  password = linepnt;	/* get password from file */
  	  clearpw = crypt(clearpw,linepnt);
    found_user = 1;
    break;
    }
  }
  fclose(fp);

  if (!found_user) return(1);

  /* AUTH with encrypted password */
  for (i=0;i<3;i++) { authtype[i]=password[i]; }
  authtype[4] = '\0';
  if( (authtype[0] == '$') && (authtype[0] == authtype[2]) && (authtype[1] < 1 || authtype[1] > 6) ) {
	/* password is encrypted --> no CRAM-MD5 auth */
	return(strcmp(password,clearpw)); // && strcmp(password,response));
  }
  /* do not allow plain text passwords with AUTH LOGIN/PLAIN (hard to debug!) */
  if((strcmp(clearpw,password) != 0) && ((strcmp(password,clearpw) && strcmp(password,response)) == 0)) {
	return(1);
  }

  hmac_md5( challenge, strlen(challenge), password, strlen(password), digest);

  digascii[32]=0;
  for (j=0;j<16;j++){
	h=digest[j] >> 4;
    digascii[2*j]=hextab[h];
    h=digest[j] & 0x0f;
	digascii[(2*j)+1]=hextab[h];
  }
  /* CRAM-MD5 check */
  return(strcmp(digascii,response) && strcmp(password,response));
}
/* ***** begin main ******************************************* */
int main(int argc,char **argv)
{
  char *login;
  char *response;
  char *challenge;
  int r;
  int i;
  int accepted;

  if (!argv[1]) _exit(2);

  /* process the '-e' option (interactive) */
  if(strcmp(argv[1],"-e") == 0) {
    process_e();
    _exit(0);
  }

  /* process the '-r' option (read from stdin) */
  if(strcmp(argv[1],"-r") == 0) {
	process_r();
    _exit(0);
  }

/* ********************************************************** */
  /* check for input on fd3 (the real password check)*/
  uplen = 0;
  for (;;) {
    do
      r = read(3,up + uplen,sizeof(up) - uplen);
    while ((r == -1) && (errno == EINTR));
    if (r == -1) _exit(111);
    if (r == 0) break;
    uplen += r;
    if (uplen >= sizeof(up)) _exit(1);
  }
  close(3);

  i = 0;
  login = up + i;
  while (up[i++]) if (i == uplen) _exit(2);
  response = up + i;
  if (i == uplen) _exit(2);
  while (up[i++]) if (i == uplen) _exit(2);
  challenge = up + i;

  clearpw=response; /* save original response unencrypted */

  /* some stuff for debug on command line */
/*
  printf("Input: \n");
  printf("login: %s\n",login);
  printf("resp.: %s\n",response);
  printf("chall: %s\n\n",challenge);
*/
  doencrypt(clearpw,0);		/* encrypt the response */

  accepted = doit(login,response,challenge);
  /* some stuff for debug on command line */
  printf("accepted: %i\n",accepted);	// return value of 'doit'

  for (i = 0;i < sizeof(up);++i) up[i] = 0;

  if (accepted)  _exit(1);
    execvp(argv[1],argv + 1);
    _exit(111);
}
