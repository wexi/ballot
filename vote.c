/* Voting SQLite loader, Enoch */

#include <ctype.h>
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
	fprintf(stderr, "SQL error: %s\n",
		*DBerr ? DBerr : sqlite3_errmsg(DB));
	sqlite3_close(DB);
	exit(EXIT_FAILURE);
    }
}

void Exec(const char *sql, int (*callback)(void *, int, char **, char **),
	  void *arg, char **errmsg)
{
    Guard(sqlite3_exec(DB, sql, callback, arg, errmsg));
}

int change = 0, quit = 0;
void bye()
{
    if (quit)
	printf("EOF\n");
    if (change)
	printf(DBNAME ".db changed.\n");
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
    char *p, line[1024], *q, text[1024], xX;
    int init, opt, seas, cans, secret; 
    int i, j, rc, cnt, num, ix[CANS];
    FILE *fp;

    strcpy(line, argv[0]);
    strcpy(text, argv[0]);
#ifdef __MINGW32__
    p = strrchr(line, '\\');
    q = strrchr(text, '\\');
#else
    p = strrchr(line, '/');
    q = strrchr(text, '/');
#endif
    if (p)
	p++;
    else
	p = line;
    strcpy(p, "vote.ini");
    if (q)
	q++;
    else
	q = text;
    strcpy(q, DBNAME ".db");

    init = secret = 0; xX = 'O';
    while ((opt = getopt(argc, argv, "?i:")) != -1) {
	switch (opt) {
	case 'i':
	    num = sscanf(optarg, "%d:%d:%c", &seas, &cans, &xX);
	    init = (2 <= num && num <= 3 &&
		    1 <= seas && seas <= SEAS &&
		    1 <= cans && cans <= CANS &&
		    (num == 2 || (secret = toupper(xX) == 'X')));
	    if (init) break;
	    __attribute__((fallthrough));

	case '?':
	    fprintf(stderr,
		    "https://github.com/wexi/ballot -- " __DATE__ ".\n"
		    "Init:\tvote -i <#Seats>:<#Candidates>[:X]\n" "Sqlite:\t"
		    DBNAME ".db\n");
	    exit(EXIT_FAILURE);
	}
    }

    atexit(bye);

    if (init) {
	Guard(sqlite3_open_v2(text, &DB, SQLITE_OPEN_READWRITE, NULL));

	Exec("CREATE TABLE IF NOT EXISTS bans(apt INTEGER PRIMARY KEY, comment TEXT)", NULL, NULL, NULL);

	if (secret) {
	    if (xX == 'X') {
		for (cnt = 0; cnt < 10; cnt++) {
		    rc = sqlite3_exec(DB, "UPDATE shares SET code = random()",
				      NULL, NULL, NULL);
		    if (rc == SQLITE_OK
			|| sqlite3_extended_errcode(DB) !=
			SQLITE_CONSTRAINT_UNIQUE)
			break;
		}
		Guard(rc);
	    }

	    Exec("DROP TABLE IF EXISTS shuffle", NULL, NULL, NULL);
	    Exec("CREATE TABLE shuffle (apt INTEGER PRIMARY KEY AUTOINCREMENT, share INTEGER, code INTEGER)", NULL, NULL, NULL);
	    Exec("INSERT INTO shuffle (share, code) "
		 "SELECT share, code FROM shares LEFT JOIN bans USING (apt) WHERE bans.apt IS NULL ORDER BY code",
		 NULL, NULL, NULL);

	    Exec("DROP TABLE IF EXISTS anon", NULL, NULL, NULL);
	    Exec("CREATE TABLE anon AS SELECT shares.apt apt, shuffle.apt unit FROM shares JOIN shuffle USING (code) ORDER BY apt", NULL, NULL, NULL);
	}
	else {
	    Exec("DROP TABLE IF EXISTS shuffle", NULL, NULL, NULL);
	    Exec("CREATE TABLE shuffle AS "
		 "SELECT apt, share FROM shares LEFT JOIN bans USING (apt) WHERE bans.apt IS NULL ORDER BY code",
		 NULL, NULL, NULL);
	}

	Exec("SELECT COUNT(apt) AS apts, sum(share) AS shares FROM shuffle",
	     DBReport, DBNAME, &DBerr);

	Exec("DROP TABLE IF EXISTS votes", NULL, NULL, NULL);

	change = 1;

	strcpy(text,
	       "CREATE TABLE votes (apt INTEGER PRIMAY KEY, vtime INTEGER");
	for (i = 0; i < cans; i++) {
	    p = text + strlen(text);
	    sprintf(p, ", can%d INTEGER DEFAULT 0", i + 1);
	}
	strcat(text, ")");
	Exec(text, NULL, NULL, &DBerr);

	if ((fp = fopen(line, "wt")) == NULL ||
	    fprintf(fp, "%d:%d:%d\n", seas, cans, secret) < 0 ||
	    fclose(fp) == EOF) {
	    perror(line);
	    exit(EXIT_FAILURE);
	}
    }
    else {
	if ((fp = fopen(line, "rt")) == NULL ||
	    fscanf(fp, "%d:%d:%d\n", &seas, &cans, &secret) != 3 ||
	    fclose(fp) == EOF) {
	    perror(line);
	    exit(EXIT_FAILURE);
	}
	Guard(sqlite3_open_v2(text, &DB, SQLITE_OPEN_READWRITE, NULL));
    }

    printf("Enter negative APT# to delete its vote; "
#ifdef __MINGW32__
	   "Enter ctrl-Z to end program.\n"
#else
	   "Enter ctrl-D to end program.\n"
#endif
	   DBNAME ": Seats = %d, Candidates = %d, %s APT#\n", seas, cans,
	   secret ? "SECRET" : "REAL");

    while (1) {
	__label__ cycle; cycle:

	/* report current vote */

	strcpy(text, "SELECT ");
	for (i = 0; i < cans; i++) {
	    p = text + strlen(text);
	    sprintf(p, "sum(can%d), ", i + 1);
	}
	p = text + strlen(text) - 2;
	strcpy(p, " FROM votes");
	Exec(text, DBReport, NULL, &DBerr);	/* Report[i] sums shares cast for can[i] */

	strcpy(text,
	       "SELECT count(apt) AS apts, sum(shuffle.share) AS shares");
	for (i = 0; i <= seas; i++) {	/* descending sort candidates */
	    num = -1;
	    for (j = 0; j < cans; j++) {
		if (Report[j] > num) {
		    cnt = j;
		    num = Report[j];
		}
	    }
	    Report[cnt] = -1;
	    p = text + strlen(text);
	    sprintf(p, ", sum(votes.can%d) AS c%d", cnt + 1, cnt + 1);
	}
	strcat(text, " FROM votes JOIN shuffle USING (apt)");
	Exec(text, DBReport, "\nVoted", &DBerr);

	/* enter new vote */

	printf("APT#.Can1.Can2... ");
	fflush(stdout);
	p = fgets(line, sizeof(line), stdin);
	if (p == NULL) {
	    quit = 1;
	    break;
	}

	p = strtok_r(p, del, &q);
	if (p == NULL || sscanf(p, "%d", &cnt) != 1) {
	    printf("NO INPUT\n");
	    goto cycle;
	}

	sprintf(text, "SELECT count(*) FROM shuffle WHERE apt = %d",
		abs(cnt));
	Exec(text, DBReport, NULL, &DBerr);
	if (Report[0] != 1) {
	    printf("APT# UNKNOWN\n");
	    goto cycle;
	}

	if (cnt < 0) {
	    sprintf(text, "DELETE FROM votes WHERE apt = %d", abs(cnt));
	    Exec(text, NULL, NULL, &DBerr);
	    change = 1;
	    printf("DELETED APT# %d\n", abs(cnt));
	    goto cycle;
	}

	sprintf(text, "SELECT count(*) FROM votes WHERE apt = %d", cnt);
	Exec(text, DBReport, NULL, &DBerr);
	if (Report[0] != 0) {
	    printf("APT# ALREADY VOTED\n");
	    goto cycle;
	}

	sprintf(text, "SELECT share FROM shuffle WHERE apt = %d", cnt);
	Exec(text, DBReport, "Ballot", &DBerr);
	num = Report[0];

	memset(ix, 0, sizeof(int) * cans);
	p = strtok_r(NULL, del, &q);
	for (i = 0; i < seas; i++) {
	    if (p == NULL)
		break;
	    if (sscanf(p, "%d", &j) != 1 || j < 1 || j > cans
		|| ix[j - 1] > 0) {
		printf("ILL CAN\n");
		goto cycle;
	    }
	    ix[j - 1] = num;
	    p = strtok_r(NULL, del, &q);
	}

	if (p != NULL) {
	    printf("TOO MANY VOTES\n");
	    goto cycle;
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
