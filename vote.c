/* Voting SQLite loader, E. Wexler */

#include <setjmp.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <sqlite3.h>

/* max 7 seats and candidates */

const char *INITF = "vote.ini";
const char *DBFILE = "wt200.db";

#define NCOLS 25		/* max number of db columns */
int Report[NCOLS];
int DBReport(void* mess, int ncols, char** values, char** headers)
{
  int i;
  char *value;

  if (mess != NULL && ncols > 0)
    printf("%s: ", (const char*)mess);
  for (i = 0; i < NCOLS; i++) {
    value = values[i];
    if (value == NULL)
      value = "N/A";
    if (mess != NULL && i < ncols)
      printf("%s=%s ", headers[i], value);
    if (i >= ncols || sscanf(value, "%d", Report+i) != 1)
      Report[i] = -1;
  }
  if (mess != NULL && ncols > 0)
    printf("\n");
  return 0;
}

sqlite3 *DB;
char *DBerr = "CANNOT OPEN DATABASE";
void Guard(int rc)
{
  if(rc != SQLITE_OK) {
    if (DBerr) {
      fprintf(stderr, "SQL error: %s\n", DBerr);
      sqlite3_close(DB);
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char *argv[])
{
  char *txt, text[240];
  int init, opt, seas, cans; 
  int i, j, cnt, num, jump;
  int share, ix[7], vote[7];
  jmp_buf jmp;
  FILE *fp;

  printf("WT200 Votes to SQLite loader. Exit by Ctrl-D. EW " __DATE__ ".\n");

  init = 0;
  seas = 0;
  cans = 0;
  while ((opt = getopt(argc, argv, "i:")) != -1) {
    switch (opt) {
    case 'i':
      init = 1;
      sscanf(optarg, "%d:%d", &seas, &cans);
      seas = atoi(optarg);

      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s [-i #seats:#candidates]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (init) {
    if ((fp = fopen(INITF, "wt")) == NULL || fprintf(fp, "%d:%d\n", seas, cans) < 0 || fclose(fp) == EOF) {
      perror(INITF);
      exit(EXIT_FAILURE);
    }
  }
  else {
    if ((fp = fopen(INITF, "rt")) == NULL || fscanf(fp, "%d:%d\n", &seas, &cans) < 0 || fclose(fp) == EOF) {
      perror(INITF);
      exit(EXIT_FAILURE);
    }
  }  

  printf("Seats = %d, Candidates = %d\n", seas, cans);
  if (seas < 1 || seas > 7 || cans < 1 || cans > 7) {
    fprintf(stderr, "Check arguments\n");
    exit(EXIT_FAILURE);
  }

  Guard(sqlite3_open_v2(DBFILE, &DB, SQLITE_OPEN_READWRITE, NULL));

  jump = setjmp(jmp);
  while (jump >= 0) {
    txt = "select count(*) as apts, sum(vote1) as v1, sum(vote2) as v2, sum(vote3) as v3, sum(vote4) as v4, sum(vote5) as v5, sum(vote6) as v6, sum(vote7) as v7 from votes";
    Guard(sqlite3_exec(DB, txt, DBReport, "Voting", &DBerr));

    txt = readline("Share.Vote.Vote... ");
    if (txt == NULL) {
      printf("EOF\n");
      longjmp(jmp, -1);
    }

    num = sscanf(txt, "%d.%d.%d.%d.%d.%d.%d.%d", &share, ix+0, ix+1, ix+2, ix+3, ix+4, ix+5, ix+6);
    if (num <= 0)
      longjmp(jmp, 1);

    num--;
    if (seas < num) {
      printf("TOO MANY VOTES\n");
      longjmp(jmp, 1);
    }

    for (i = 0; i < num; i++)
      if (ix[i] < 1 || cans < ix[i]) {
	printf("UNKNOWN CANDIDATE\n");
	longjmp(jmp, 1);
      }

    for (i = 0; i < num-1; i++)
      for (j = i + 1; j < num; j++)
	if (ix[i] == ix[j]) {
	  printf("DUPLICATE CANDIDATE\n");
	  longjmp(jmp, 1);
	}

    sprintf(text, "select count(*) from shares where share = %d", share);
    Guard(sqlite3_exec(DB, text, DBReport, NULL, &DBerr));
    if (Report[0] <= 0) {
      printf("ILLEGAL SHARE\n");
      longjmp(jmp, 1);
    }
    cnt = Report[0];

    sprintf(text, "select count(*) from votes where share = %d", share);
    Guard(sqlite3_exec(DB, text, DBReport, NULL, &DBerr));
    if (Report[0] >= cnt) {
      printf("EXHAUSTED SHARE\n");
      longjmp(jmp, 1);
    }

    memset(vote, 0, sizeof(vote));
    for (i = 0; i < num; i++)
      vote[ix[i] - 1] = share;
    sprintf(text, "insert into votes (share, vote1, vote2, vote3, vote4, vote5, vote6, vote7) values (%d, %d, %d, %d, %d, %d, %d, %d)",
	    share, vote[0], vote[1], vote[2], vote[3], vote[4], vote[5], vote[6]);
    Guard(sqlite3_exec(DB, text, NULL, NULL, &DBerr));
  }

  sqlite3_close(DB);
  exit(EXIT_SUCCESS);
}
