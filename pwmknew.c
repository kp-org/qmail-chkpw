/* process the '-e' or '-r' option (read from stdin) */

void process_e()
{
  char *newpw, *cmppw;
  newpw = strdup(getpass("   New password: "));
  if(strlen(newpw) < 1) { puts("Empty password!"); _exit(111); }
  /* check password (min/max) lenght */
  if(strlen(newpw) < 6) { puts("Minimum Password length is 6 characters."); _exit(111); }
  if(strlen(newpw) > 128) { puts("Maximum password lenght is 128 characters."); _exit(111); }
  cmppw = strdup(getpass("Repeat password: "));
  if (strcmp(newpw,cmppw) == 0) { newpw = doencrypt(newpw,1); } 
    else { printf("Passwords don't match!\n"); }
  _exit(0);
}

void process_r()
{
  char buf[129];
  char *input;
  int pw;
  int pwlen;

  for (;;) {
    do
      pw = read(0,buf + pwlen,sizeof(buf) - pwlen);
    while (( pw == -1)  && (errno == EINTR));
    if (pw == -1) _exit(111);
    if (pw == 0) break;
    pwlen += pw;
    if (pwlen >= sizeof(buf)) _exit(1);
  }
  close(0);
  if (pwlen < 1) _exit(1);
  input = buf;
  strchr(input, '\n');
//printf("In: %s\n",input);
//printf("len: %i\n",pwlen);
  input = doencrypt(input,1);
  _exit(0);
}
