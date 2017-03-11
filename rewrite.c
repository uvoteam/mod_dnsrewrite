
/*
 *  DNS rewrite preload library
 *
 * Allows to replace hostname in getaddrinfo() calls with something other.
 * Reads list of mappings from file, specified in the DNSREWRITE_CONFIG
 * environment variable.
 * */

#include "config.h"

#define _GNU_SOURCE
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef int (*getaddrinfo_type) (const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);

typedef struct dnsrewrite_tuple_struct dnsrewrite_tuple_t;
struct dnsrewrite_tuple_struct {
	char *from;
	char *to;
	dnsrewrite_tuple_t *next;
};

static dnsrewrite_tuple_t *dnsrewrite_list = NULL;
static getaddrinfo_type super_getaddrinfo = NULL;
static int dnsrewrite_initialized = 0;

static int iseol (const char c)
{
	return c == '\n' || c == '\x0' || c == '#';
}

static void dnsrewrite_read_file (const char *filename)
{
	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t rlen = 0;
	dnsrewrite_tuple_t *end = NULL;

	fp = fopen (filename, "r");

	if (!fp) {
		D("getaddrinfo: error opening file %s.\n", filename);
		return;
	}

	while ((rlen = getline (&line, &len, fp)) != -1) {
		const char *fs, *fe, *ts, *te, *le;
		dnsrewrite_tuple_t *tuple;

		for (fs = line; isspace (*fs); fs++) { }
		if (iseol (*fs))
			continue;
		for (fe = fs; !isspace (*fe) && !iseol (*fe); fe++) { }
		if (iseol (*fe))
			continue;
		for (ts = fe; isspace (*ts); ts++) { }
		if (iseol (*ts))
			continue;
		for (te = ts; !isspace (*te) && !iseol (*te); te++) { }
		for (le = te; isspace (*le); le++) { }
		if (!iseol (*le))
			continue;

		tuple = (dnsrewrite_tuple_t*) malloc(sizeof(dnsrewrite_tuple_t));

		tuple->from = (char*) malloc (fe - fs + 1);
		strncpy (tuple->from, fs, fe - fs);
		tuple->from[fe - fs] = '\0';

		tuple->to = (char*) malloc (te - ts + 1);
		strncpy (tuple->to, ts, te - ts);
		tuple->to[te - ts] = '\0';

		tuple->next = NULL;

		D("getaddrinfo: got mapping (%s -> %s)\n", tuple->from, tuple->to);

		if (end)
			end->next = tuple;
		else
			end = dnsrewrite_list = tuple;
	}

	if (line)
		free (line);
	fclose (fp);

}

static void dnsrewrite_initialize ()
{
	const char *filename = getenv (DNSREWRITE_CONFIG_ENV);

	D("getaddrinfo: initializing (%s)\n", filename ? filename : "NULL");

	if (filename && *filename)
		dnsrewrite_read_file (filename);

	super_getaddrinfo = (getaddrinfo_type) dlsym (RTLD_NEXT, "getaddrinfo");

	D("getaddrinfo: initialization done.\n");

	dnsrewrite_initialized = 1;
}

int getaddrinfo (const char *node, const char*service, const struct addrinfo *hints, struct addrinfo **res)
{
	D("getaddrinfo: called (%s, %s)\n", node ? node : "NULL", service ? service : "NULL");

	if (!dnsrewrite_initialized)
		dnsrewrite_initialize ();

	if (dnsrewrite_list) {
		for (dnsrewrite_tuple_t *tuple = dnsrewrite_list; tuple != NULL; tuple = tuple->next) {
			if (!strcmp (tuple->from, node)) {
				D("getaddrinfo: rewriting %s -> %s\n", node, tuple->to);
				node = tuple->to;
				break;
			}
		}
	}

	return super_getaddrinfo (node, service, hints, res);
}

/* The End */
