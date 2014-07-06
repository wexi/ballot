/* Quorum SQLite loader, E. Wexler */

#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <sqlite3.h>

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

int main()
{
  char *txt, text[240];
  int apt, share;

  printf("WT200 Quorum SQLite loader. Exit by Ctrl-D. EW " __DATE__ ".\n");

  Guard(sqlite3_open_v2(DBFILE, &DB, SQLITE_OPEN_READWRITE, NULL));

  while (1) {
    txt = "select count(apt) as apts, sum(share) as shares from quorum";
    Guard(sqlite3_exec(DB, txt, DBReport, "Quorum", &DBerr));

    txt = readline("Apt No... ");
    if (txt == NULL) {
      printf("EOF\n");
      break;
    }

    if (sscanf(txt, "%d", &apt) <= 0) 
      continue;

    sprintf(text, "select apt, share from shares where apt = %d", apt);
    Report[0] = 0;
    Guard(sqlite3_exec(DB, text, DBReport, "Testing", &DBerr));
    if (Report[0] <= 0) {
      printf("ILLEGAL APT No\n");
      continue;
    }
    share = Report[1];

    sprintf(text, "insert into quorum (apt, share) values (%d, %d)", apt, share);
    if (sqlite3_exec(DB, text, NULL, NULL, NULL) != SQLITE_OK) {
      printf("DUPLICATE APT No\n");
      continue;
    }
  }

  sqlite3_close(DB);
  exit(EXIT_SUCCESS);
}
