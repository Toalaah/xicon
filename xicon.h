/**
 * Copyright © 2023 Samuel Kunst <samuel at kunst.me>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef XICON_H
#define XICON_H

#define XICON_PROGRAM_NAME "xicon"
#ifndef XICON_VERSION
#define XICON_VERSION "unknown"
#endif

struct xicon_opts {
  char *win_id;
  const char *out_path;
  int dimension;
};

static unsigned char flags;
#define MODE_HELP (1 << 0)
#define MODE_VERSION (1 << 1)
#define MODE_HEX (1 << 2)
#define MODE_JSON (1 << 3)

#define set(mode) flags |= mode
#define is_set(mode) flags &mode

void parse_opts(int argc, char **argv, struct xicon_opts *opts);
void die(const char *msg, ...);
void save_png(const char *filename, unsigned int width, unsigned long height, unsigned long *raw_data);
void usage(void);

#endif // XICON_H
