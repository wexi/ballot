/* Voting SQLite loader, Enoch */

#include <errno.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef __MINGW32__
/* 
 * public domain strtok_r() by Charlie Gordon
 *
 *   from comp.lang.c  9/14/2007
 *
 *      http://groups.google.com/group/comp.lang.c/msg/2ab1ecbb86646684
 *
 *     (Declaration that it's public domain):
 *      http://groups.google.com/group/comp.lang.c/msg/7c7b39328fefab9c
 */

char *strtok_r(char *str, const char *delim, char **nextp)
{
  char *ret;

  if (str == NULL) {
    str = *nextp;
  }
  str += strspn(str, delim);
  if (*str == '\0') {
    return NULL;
  }
  ret = str;
  str += strcspn(str, delim);
  if (*str) {
    *str++ = '\0';
  }
  *nextp = str;
  return ret;
}
#endif

#define SEAS 7			/* max no. of seats */
#define CANS 21			/* max no. of candidates */
#define NCOLS 23		/* max number of db columns */

int Report[NCOLS];
int DBReport(void *mess, int ncols, char **values, char **headers)
{
  int i;
  char *value;

  if (mess != NULL && ncols > 0)
    printf("%s: ", (const char *) mess);
  for (i = 0; i < ncols; i++) {
    value = values[i];
    if (value == NULL) {
      value = "N/A";
      Report[i] = 0;
    }
    else
      sscanf(value, "%i", Report + i);
    if (mess != NULL)
      printf("%s=%s ", headers[i], value);
  }
  if (mess != NULL && ncols > 0)
    printf("\n");
  return 0;
}

sqlite3 *DB;
char *DBerr = "";
void Guard(int rc)
{
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", *DBerr ? DBerr : sqlite3_errmsg(DB));
    sqlite3_close(DB);
    exit(EXIT_FAILURE);
  }
}

int change = 0, quit = 0;
void bye()
{
  if (quit)
    printf("EOF\n");
  if (change)
    printf(DBNAME ".db changed. \n");
#ifdef __MINGW32__
  if (change || !quit) {
    printf("Press Enter to end...");
    fflush(stdout);
    while (getchar() != '\n');
  }
#endif
}

int main(int argc, char *argv[])
{
  const char *del = "., \n";
  char blob, *p, line[1024], *q, text[1024];
  int init, opt, seas, cans;
  int i, j, cnt, num, ix[CANS];
  int cycle;
  FILE *fp;

  printf(DBNAME ".db loader. Exit by Ctrl-Z (EOF). EW " __DATE__ ".\n");

  strcpy(line, argv[0]);
  strcpy(text, argv[0]);
#ifdef __MINGW32__
  p = strrchr(line, '\\');
  q = strrchr(text, '\\');
#else
  p = strrchr(line, '/');
  q = strrchr(text, '/');
#endif
  if (p != NULL)
    p++;
  else
    p = line;
  strcpy(p, "vote.ini");
  if (q != NULL)
    q++;
  else
    q = text;
  strcpy(q, DBNAME ".db");

  atexit(bye);

  init = 0; blob = '-';
  while ((opt = getopt(argc, argv, "i:")) != -1) {
    switch (opt) {
    case 'i':
      num = sscanf(optarg, "%d:%d:%c", &seas, &cans, &blob);
      if (2 <= num && num <= 3 &&
	  1 <= seas && seas <= SEAS &&
	  1 <= cans && cans <= CANS &&
	  (num == 2 || blob == '+')) {
	  init = 1;
	  break;
      }
      goto note;
      
    case '?':
    note:
      fprintf(stderr, "vote -i <#Seats>:<#Candidates>[:+]\n");
      exit(EXIT_FAILURE);
    }
  }

  if (init) {
    if ((fp = fopen(line, "wt")) == NULL ||
	fprintf(fp, "%d:%d:%c\n", seas, cans, blob) < 0 ||
	fclose(fp) == EOF) {
      perror(line);
      exit(EXIT_FAILURE);
    }
    Guard(sqlite3_open_v2(text, &DB, SQLITE_OPEN_READWRITE, NULL));
    Guard(sqlite3_exec
	  (DB, blob == '+' ?
	   "UPDATE shares SET code = randomblob(2)" :
	   "UPDATE shares SET code = NULL", NULL, NULL, NULL));
    Guard(sqlite3_exec
	  (DB, "SELECT count(*) AS apts, sum(share) AS shares FROM shares",
	   DBReport, DBNAME, &DBerr));
    sqlite3_exec(DB, "DROP TABLE votes", NULL, NULL, NULL);
    change = 1;
    strcpy(text,
	   "CREATE TABLE votes (apt INTEGER PRIMAY KEY, vtime INTEGER, ");
    for (i = 0; i < cans; i++) {
      p = text + strlen(text);
      sprintf(p, "can%d INTEGER DEFAULT 0, ", i + 1);
    }
    p = text + strlen(text) - 2;
    strcpy(p, ")");
    Guard(sqlite3_exec(DB, text, NULL, NULL, &DBerr));
  }
  else {
      if ((fp = fopen(line, "rt")) == NULL ||
	fscanf(fp, "%d:%d:%c\n", &seas, &cans, &blob) != 3 ||
	fclose(fp) == EOF) {
      perror(line);
      exit(EXIT_FAILURE);
    }
    Guard(sqlite3_open_v2(text, &DB, SQLITE_OPEN_READWRITE, NULL));
  }

  printf("Enter negative apt to delete an apt vote.\n"
	 "Seats = %d, Candidates = %d\n", seas, cans);

  while (1) {
    cycle = 0;

    /* report current vote */

    strcpy(text, "SELECT ");
    for (i = 0; i < cans; i++) {
      p = text + strlen(text);
      sprintf(p, "sum(can%d), ", i + 1);
    }
    p = text + strlen(text) - 2;
    strcpy(p, " FROM votes");
    Guard(sqlite3_exec(DB, text, DBReport, NULL, &DBerr));

    strcpy(text, "SELECT count(*) AS apts, sum(shares.share) AS shares, ");
    for (i = 0; i <= seas; i++) {
      num = -1;
      for (j = 0; j < cans; j++) {
	if (Report[j] > num) {
	  cnt = j;
	  num = Report[j];
	}
      }
      Report[cnt] = -1;
      p = text + strlen(text);
      sprintf(p, "sum(votes.can%d) AS c%d, ", cnt + 1, cnt + 1);
    }
    p = text + strlen(text) - 2;
    strcpy(p, " FROM votes inner join shares on votes.apt = shares.apt");
    Guard(sqlite3_exec(DB, text, DBReport, "\nVoted", &DBerr));

    /* enter new vote */

    printf("Apt.Can1.Can2... ");
    fflush(stdout);
    p = fgets(line, sizeof(line), stdin);
    if (p == NULL) {
      quit = 1;
      break;
    }

    p = strtok_r(p, del, &q);
    if (p == NULL || sscanf(p, "%d", &cnt) != 1) {
      printf("NO INPUT\n");
      continue;
    }

    sprintf(text, "SELECT count(*) FROM shares WHERE apt = %d", abs(cnt));
    sqlite3_exec(DB, text, DBReport, NULL, &DBerr);
    if (Report[0] != 1) {
      printf("APT UNKNOWN\n");
      continue;
    }

    if (cnt < 0) {
      sprintf(text, "DELETE FROM votes WHERE apt = %d", abs(cnt));
      sqlite3_exec(DB, text, NULL, NULL, &DBerr);
      change = 1;
      printf("DELETED APT NO %d\n", abs(cnt));
      continue;
    }

    sprintf(text, "SELECT count(*) FROM votes WHERE apt = %d", cnt);
    sqlite3_exec(DB, text, DBReport, NULL, &DBerr);
    if (Report[0] != 0) {
      printf("APT ALREADY VOTED\n");
      continue;
    }

    sprintf(text, "SELECT share %s FROM shares WHERE apt = %d",
	    (blob == '+' ? ", '0x' || hex(code) AS code" : ""), cnt);
    sqlite3_exec(DB, text, DBReport, "Ballot", &DBerr);
    num = Report[0];

    memset(ix, 0, sizeof(int) * cans);
    p = strtok_r(NULL, del, &q);
    for (i = 0; i < seas; i++) {
      if (p == NULL)
	break;
      if (sscanf(p, "%d", &j) != 1 || j < 1 || j > cans || ix[j - 1] > 0) {
	printf("ILL CAN\n");
	cycle = 1;
	break;
      }
      ix[j - 1] = num;
      p = strtok_r(NULL, del, &q);
    }
    if (cycle)
      continue;

    if (p != NULL) {
      printf("TOO MANY VOTES\n");
      continue;
    }

    strcpy(text, "INSERT INTO votes (apt, vtime, ");
    for (i = 0; i < cans; i++) {
      if (ix[i] > 0) {
	p = text + strlen(text);
	sprintf(p, "can%d, ", i + 1);
      }
    }
    p = text + strlen(text) - 2;
    sprintf(p, ") VALUES (%d, %ld,", cnt, time(NULL));
    for (i = 0; i < cans; i++)
      if (ix[i] > 0) {
	p = text + strlen(text);
	sprintf(p, "%d, ", ix[i]);
      }
    p = text + strlen(text) - 2;
    strcpy(p, ")");
    sqlite3_exec(DB, text, NULL, NULL, &DBerr);
    change = 1;
  }
  sqlite3_close(DB);

  exit(EXIT_SUCCESS);
}
