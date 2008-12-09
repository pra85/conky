/* MOC Conky integration
 *
 * Please see COPYING for details
 *
 * Copyright (c) 2008, Henri Häkkinen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id$ */

#include "conky.h"
#include "moc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_moc(struct moc_s *moc)
{
	moc->state = NULL;
	moc->file = NULL;
	moc->title = NULL;
	moc->artist = NULL;
	moc->song = NULL;
	moc->album = NULL;
	moc->totaltime = NULL;
	moc->timeleft = NULL;
	moc->curtime = NULL;
	moc->bitrate = NULL;
	moc->rate = NULL;
}

static void update_infos(struct moc_s *moc)
{
	FILE *fp;

	free_moc(moc);
	fp = popen("mocp -i", "r");
	if (!fp) {
		moc->state = strndup("Can't run 'mocp -i'", text_buffer_size);
		return;
	}

	while (1) {
		char line[100];
		char *p;

		/* Read a line from the pipe and strip the possible '\n'. */
		if (!fgets(line, 100, fp))
			break;
		if ((p = strrchr(line, '\n')))
			*p = '\0';

		/* Parse infos. */
		if (strncmp(line, "State:", 6) == 0)
			moc->state = strndup(line + 7, text_buffer_size);
		else if (strncmp(line, "File:", 5) == 0)
			moc->file = strndup(line + 6, text_buffer_size);
		else if (strncmp(line, "Title:", 6) == 0)
			moc->title = strndup(line + 7, text_buffer_size);
		else if (strncmp(line, "Artist:", 7) == 0)
			moc->artist = strndup(line + 8, text_buffer_size);
		else if (strncmp(line, "SongTitle:", 10) == 0)
			moc->song = strndup(line + 11, text_buffer_size);
		else if (strncmp(line, "Album:", 6) == 0)
			moc->album = strndup(line + 7, text_buffer_size);
		else if (strncmp(line, "TotalTime:", 10) == 0)
			moc->totaltime = strndup(line + 11, text_buffer_size);
		else if (strncmp(line, "TimeLeft:", 9) == 0)
			moc->timeleft = strndup(line + 10, text_buffer_size);
		else if (strncmp(line, "CurrentTime:", 12) == 0)
			moc->curtime = strndup(line + 13, text_buffer_size);
		else if (strncmp(line, "Bitrate:", 8) == 0)
			moc->bitrate = strndup(line + 9, text_buffer_size);
		else if (strncmp(line, "Rate:", 5) == 0)
			moc->rate = strndup(line + 6, text_buffer_size);
	}

	pclose(fp);
}

void *update_moc(void *arg)
{
	struct moc_s *moc;

	if (arg == NULL) {
		CRIT_ERR("update_moc called with a null argument!");
	}

	moc = (struct moc_s *) arg;

	while (1) {
		timed_thread_lock(moc->timed_thread);
		update_infos(moc);
		timed_thread_unlock(moc->timed_thread);
		if (timed_thread_test(moc->timed_thread, 0)) {
			timed_thread_exit(moc->timed_thread);
		}
	}
	/* never reached */
}

void free_moc(struct moc_s *moc)
{
	free(moc->state);
	free(moc->file);
	free(moc->title);
	free(moc->artist);
	free(moc->song);
	free(moc->album);
	free(moc->totaltime);
	free(moc->timeleft);
	free(moc->curtime);
	free(moc->bitrate);
	free(moc->rate);

	init_moc(moc);
}
