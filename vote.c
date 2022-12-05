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
int DBreport(void *name, int ncols, char **values, char **headers)
{
    int i;
    char *value;

    if (name != NULL && ncols > 0)
	printf("%s: ", (const char *) name);
    for (i = 0; i < ncols; i++) {
	value = values[i];
	if (value == NULL) {
	    value = "N/A";
	    Report[i] = 0;
	}
	else
	    sscanf(value, "%i", Report + i);
	if (name != NULL)
	    printf("%s=%s ", headers[i], value);
    }
    if (name != NULL && ncols > 0)
	printf("\n");
    return 0;
}

sqlite3 *DB;
int quit;

void Exec(const char *sql, int (*callback)(void *, int, char **, char **), void *foreword)
{
    char *errmsg = NULL;
    if (sqlite3_exec(DB, sql, callback, foreword, &errmsg) != SQLITE_OK) {
	if (errmsg) {
	    fprintf(stderr, DBNAME ": %s\n", errmsg);
	    sqlite3_free(errmsg);
	}
	sqlite3_close(DB);
	exit(EXIT_FAILURE);
    }
}

void bye()
{
    if (quit)
	printf("EOF\n");
#ifdef __MINGW32__
    if (!quit) {
	printf("Press Enter to end...");
	fflush(stdout);
	while (getchar() != '\n');
    }
#endif
}

int main(int argc, char *argv[])
{
    const char *del = "., \n";
    char *p, line[1024], *q, text[1024], xX, *sally, *sunit;
    int init, opt, seas, cans, secret, anons;
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

    init = secret = 0;
    xX = 'O';
    while ((opt = getopt(argc, argv, "?i:")) != -1) {
	switch (opt) {
	case 'i':
	    num = sscanf(optarg, "%d:%d:%c", &seas, &cans, &xX);
	    init = (2 <= num && num <= 3 &&
		    1 <= seas && seas <= SEAS &&
		    1 <= cans && cans <= CANS &&
		    (num == 2 || (secret = toupper(xX) == 'X')));
	    if (init)
		break;
	    __attribute__((fallthrough));

	case '?':
	    fprintf(stderr,
		    "https://github.com/wexi/ballot -- " __DATE__ ".\n"
		    "Init:\tvote -i <#Seats>:<#Candidates>[:X]\n"
		    "Sqlite:\t" DBNAME ".db\n");
	    exit(EXIT_FAILURE);
	}
    }

    atexit(bye);

    if (sqlite3_open_v2(text, &DB, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK) {
	fprintf(stderr, DBNAME ": %s\n", sqlite3_errmsg(DB));
	exit(EXIT_FAILURE);
    }

    if (init) {

	Exec("CREATE TABLE IF NOT EXISTS evotes(apt INTEGER PRIMARY KEY, FOREIGN KEY(apt) REFERENCES shares(apt))", NULL, NULL);
	Exec("CREATE TABLE IF NOT EXISTS arrears(apt INTEGER PRIMARY KEY, FOREIGN KEY(apt) REFERENCES shares(apt))", NULL, NULL);
	Exec("CREATE TEMP TABLE bans AS SELECT apt FROM evotes UNION SELECT apt FROM arrears", NULL, NULL);

	if (secret) {
	    if (xX == 'X') {
		for (cnt = 0; cnt < 10; cnt++) {
		    rc = sqlite3_exec(DB, "UPDATE shares SET code = random()", NULL, NULL, NULL);
		    if (rc == SQLITE_OK || rc != SQLITE_CONSTRAINT)
			break;
		}
		if (rc != SQLITE_OK) {
		    fprintf(stderr, DBNAME ": %s\n", sqlite3_errmsg(DB));
		    sqlite3_close(DB);
		    exit(EXIT_FAILURE);
		}
	    }
	    
	    Exec("SELECT count(apt) FROM shares WHERE code IS NULL", DBreport, NULL);
	    if (Report[0]) {
		fprintf(stderr, DBNAME ": shares.code is uninitialized, use :X\n");
		exit(EXIT_FAILURE);
	    }

	    Exec("DROP TABLE IF EXISTS shuffle", NULL, NULL);
	    Exec("CREATE TABLE shuffle (apt INTEGER PRIMARY KEY AUTOINCREMENT, share INTEGER, code INTEGER UNIQUE, FOREIGN KEY(apt) REFERENCES shares(apt))", NULL, NULL);
	    Exec("INSERT INTO shuffle (share, code) SELECT share, code FROM shares LEFT JOIN bans USING (apt) WHERE bans.apt IS NULL ORDER BY code", NULL, NULL);

	    Exec("DROP TABLE IF EXISTS anons", NULL, NULL);
	    Exec("CREATE TABLE anons (apt INTEGER PRIMARY KEY, unit INTEGER UNIQUE, FOREIGN KEY(apt) REFERENCES shares(apt))", NULL, NULL);
	    Exec("INSERT INTO anons SELECT shares.apt apt, shuffle.apt unit FROM shares JOIN shuffle USING (code) ORDER BY apt", NULL, NULL);
	}
	else {
	    Exec("DROP TABLE IF EXISTS shuffle", NULL, NULL);
	    Exec("CREATE TABLE shuffle (apt INTEGER PRIMARY KEY, share INTEGER, FOREIGN KEY(apt) REFERENCES shares(apt))", NULL, NULL);
	    Exec("INSERT INTO shuffle SELECT apt, share FROM shares LEFT JOIN bans USING (apt) WHERE bans.apt IS NULL ORDER BY apt", NULL, NULL);
	}

	Exec("SELECT COUNT(apt) AS 'Eligible_Apartments', sum(share) AS 'Eligible_Shares' FROM shuffle", DBreport, DBNAME);

	Exec("DROP TABLE IF EXISTS votes", NULL, NULL);
	strcpy(text, "CREATE TABLE votes (apt INTEGER PRIMAY KEY, vtime INTEGER");
	for (i = 0; i < cans; i++) {
	    p = text + strlen(text);
	    sprintf(p, ", can%d INTEGER DEFAULT 0", i + 1);
	}
	strcat(text, ", FOREIGN KEY(apt) REFERENCES shares(apt))");
	Exec(text, NULL, NULL);

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
	Exec("SELECT count(*) FROM votes", NULL, NULL);
    }

    anons = sqlite3_exec(DB, "SELECT count(*) FROM anons", NULL, NULL, NULL) == SQLITE_OK;
    sunit = secret ? "Unit#" : "Apt#";
    sally = secret ? "Anon" : "Open";

    printf("Enter negative %s to delete its vote; "
#ifdef __MINGW32__
	   "Enter ctrl-Z to end program.\n"
#else
	   "Enter ctrl-D to end program.\n"
#endif
	   , sunit);
    printf("%s Vote: Seats = %d, Candidates = %d\n", sally, seas, cans);

    while (1) {
	__label__ cycle;
      cycle:

	/* report current vote */

	strcpy(text, "SELECT ");
	for (i = 0; i < cans; i++) {
	    p = text + strlen(text);
	    sprintf(p, "sum(can%d), ", i + 1);
	}
	p = text + strlen(text) - 2;
	strcpy(p, " FROM votes");
	Exec(text, DBreport, NULL); /* Report[i] sums shares cast for can[i] */

	strcpy(text, "SELECT count(apt) 'ballots', sum(shuffle.share) AS 'shares'");
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
	Exec(text, DBreport, "\nVoted");

	/* enter vote */

	printf("%s.Can1.Can2... ", sunit);
	fflush(stdout);
	p = fgets(line, sizeof(line), stdin);
	if (p == NULL) {
	    quit = 1;
	    sqlite3_close(DB);
	    exit(EXIT_SUCCESS);
	}

	p = strtok_r(p, del, &q);
	if (p == NULL || sscanf(p, "%d", &cnt) != 1) {
	    printf("NO INPUT\n");
	    goto cycle;
	}

	sprintf(text, "SELECT count(*) FROM shuffle WHERE apt = %d", abs(cnt));
	Exec(text, DBreport, NULL);
	if (Report[0] != 1) {
	    printf("%s UNKNOWN\n", sunit);
	    goto cycle;
	}

	if (secret || !anons)
	    sprintf(text, "SELECT apt '%s', share 'Share' FROM shuffle WHERE apt = %d", sunit, cnt);
	else
	    sprintf(text, "SELECT apt 'Apt#', share 'Share', unit 'Unit#' FROM shuffle JOIN anons USING (apt) WHERE apt = %d", cnt);
	Exec(text, DBreport, "Member");
	num = Report[1];

	if (cnt < 0) {
	    sprintf(text, "DELETE FROM votes WHERE apt = %d", abs(cnt));
	    Exec(text, NULL, NULL);
	    printf("DELETED %s %d\n", sunit, abs(cnt));
	    goto cycle;
	}

	sprintf(text, "SELECT count(*) FROM votes WHERE apt = %d", cnt);
	Exec(text, DBreport, NULL);
	if (Report[0] != 0) {
	    printf("%s ALREADY VOTED\n", sunit);
	    goto cycle;
	}

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
	Exec(text, NULL, NULL);
    }
}
