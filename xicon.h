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

  int flag_help;
  int flag_version;
  int flag_hex;
  int flag_json;
};

void parse_opts(int argc, char **argv, struct xicon_opts *opts);
void die(const char *msg, ...);
void save_png(const char *filename, unsigned int width, unsigned long height, unsigned long *raw_data);
void usage(void);

#endif // XICON_H
