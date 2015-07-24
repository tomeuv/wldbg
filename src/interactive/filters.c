/*
 * Copyright (c) 2015 Marek Chalupa
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "wayland/wayland-private.h"

#include "wldbg.h"
#include "interactive.h"
#include "print.h"

static struct print_filter *
create_filter(const char *pattern)
{
	struct print_filter *pf;

	pf = malloc(sizeof *pf);
	if (!pf) {
		fprintf(stderr, "No memory\n");
		return NULL;
	}

	pf->filter = strdup(pattern);
	if (!pf->filter) {
		fprintf(stderr, "No memory\n");
		return NULL;
	}

	if (regcomp(&pf->regex, pattern, REG_EXTENDED) != 0) {
		fprintf(stderr, "Failed compiling regular expression\n");
		free(pf->filter);
		free(pf);
		return NULL;
	}

	return pf;
}

static int
cmd_create_filter(struct wldbg_interactive *wldbgi,
		  char *buf, int show_only)
{
	struct print_filter *pf;
	char filter[128];

	sscanf(buf, "%s", filter);

	pf = create_filter(filter);
	if (!pf)
		return CMD_CONTINUE_QUERY;

	pf->show_only = show_only;
	wl_list_insert(wldbgi->print_filters.next, &pf->link);

	printf("Filtering messages: %s%s\n",
	       show_only ? "" : "hide ", filter);

	return CMD_CONTINUE_QUERY;
}

int
cmd_hide(struct wldbg_interactive *wldbgi,
	 struct message *message,
	 char *buf)
{
	(void) message;
	return cmd_create_filter(wldbgi, buf, 0);
}

void
cmd_hide_help(int oneline)
{
	if (oneline)
		printf("Hide particular messages");
	else
		printf("Hide messages matching given extended regular expression\n\n"
		       "hide REGEXP\n");
}

int
cmd_showonly(struct wldbg_interactive *wldbgi,
	      struct message *message,
	      char *buf)
{
	(void) message;
	return cmd_create_filter(wldbgi, buf, 1);
}

void
cmd_showonly_help(int oneline)
{
	if (oneline)
		printf("Show only particular messages");
	else
		printf("Show only messages matching given extended regular expression.\n"
		       "Filters are accumulated, so the message is shown if\n"
		       "it matches any of showonly commands\n\n"
		       "showonly REGEXP\n");
}