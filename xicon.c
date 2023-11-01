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
#define _POSIX_C_SOURCE 200809L

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <libpng16/png.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xicon.h"

void
die(const char *msg, ...) {
  va_list args;
  fflush(stderr);
  fprintf(stderr, "%s: ", XICON_PROGRAM_NAME);
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}

void
save_png(const char *filename, unsigned int width, unsigned long height, unsigned long *raw_data) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    die("error opening file %s for writing", filename);
  }

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    fclose(fp);
    die("error creating PNG write structure");
  }

  png_infop info = png_create_info_struct(png);
  if (!info) {
    png_destroy_write_struct(&png, NULL);
    fclose(fp);
    die("error creating PNG info structure");
  }

  if (setjmp(png_jmpbuf(png))) {
    png_destroy_write_struct(&png, &info);
    fclose(fp);
    die("error during PNG creation");
  }

  png_init_io(png, fp);
  png_set_IHDR(png, info, width, width, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png, info);

  png_bytep row = (png_bytep)malloc(4 * width * sizeof(png_byte));

  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      png_bytep pixel = &(row[x * 4]);
      unsigned long tmp = raw_data[(y * width + x)];
      // data is given in BGRA, we require RGBA
      pixel[0] = (tmp >> 16) & 0xff;
      pixel[1] = (tmp >> 8) & 0xff;
      pixel[2] = (tmp >> 0) & 0xff;
      pixel[3] = (tmp >> 24) & 0xff;
    }
    png_write_row(png, row);
  }

  png_write_end(png, NULL);
  free(row);
  png_destroy_write_struct(&png, &info);
  fclose(fp);
  return;
}

void
usage(void) {
  const char *usage =
      XICON_PROGRAM_NAME " [-h] [-v] [-x] [-o output] [-d dimension] window_id\n"
                         "    -h             print this help and exit\n"
                         "    -v             print version and exit\n"
                         "    -x             parse the window id as a hex value (default false)\n"
                         "    -o output      override the output path (default /dev/stdout)\n"
                         "    -d dimension   maximum icon dimension to query. If this value is too\n"
                         "                   small, image data may not be parsed corectly (default 128)\n";
  printf("%s", usage);
}

void
parse_opts(int argc, char *argv[], struct xicon_opts *opts) {
  // default values
  opts->dimension = 128;
  opts->out_path = "/dev/stdout";
  opts->win_id = NULL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0) {
      set(MODE_HELP);
      // early return if help or version flag is passed
      return;
    } else if (strcmp(argv[i], "-v") == 0) {
      set(MODE_VERSION);
      return;
    } else if (strcmp(argv[i], "-x") == 0) {
      set(MODE_HEX);
    } else if (strcmp(argv[i], "-o") == 0) {
      if (i + 1 >= argc)
        die("missing output path parameter");
      i++;
      opts->out_path = argv[i];
    } else if (strcmp(argv[i], "-d") == 0) {
      if (i + 1 >= argc)
        die("missing dimension parameter");
      i++;
      int tmp = atoi(argv[i]);
      if (!tmp) {
        fprintf(stderr, "could not parse dimension '%s', using default instead\n", argv[i]);
      } else {
        opts->dimension = tmp;
      }
    }
    // remaining args
    else {
      opts->win_id = argv[i];
      // TODO: don't ignore further args, maybe append to list of IDs instead?
      return;
    }
  }
}

int
main(int argc, char *argv[]) {

  Display *display;
  Window window;
  Atom atom, actual_type;
  int actual_format;
  unsigned long num_items, bytes_after;
  unsigned char *prop_data = NULL;
  struct xicon_opts *opts = malloc(sizeof(struct xicon_opts));

  parse_opts(argc, argv, opts);

  if (is_set(MODE_HELP)) {
    usage();
    return EXIT_SUCCESS;
  }

  if (is_set(MODE_VERSION)) {
    printf("%s\n", XICON_VERSION);
    return EXIT_SUCCESS;
  }

  display = XOpenDisplay(NULL);

  if (!display) {
    die("cannot open display");
  }

  // read input from stdin if not passed as a direct arg
  if (!opts->win_id) {
    FILE *fp;
    size_t len = 0;
    ssize_t read;
    fp = fopen("/dev/stdin", "r");
    if (fp == NULL) {
      die("could not open file pointer to /dev/stdin");
    }
    if ((read = getline(&(opts->win_id), &len, fp)) == -1) {
      die("failed to read from stdin");
    }
    // strip trailing newline
    size_t last_idx = strlen(opts->win_id) - 1;
    if (opts->win_id[last_idx] == '\n') {
      opts->win_id[last_idx] = '\0';
    }
  }

  window = strtol(opts->win_id, NULL, ((is_set(MODE_HEX)) ? 16 : 10));
  if (!window) {
    die("cannot parse window id '%s'", opts->win_id);
  }

  Atom cardinal = XInternAtom(display, "CARDINAL", False);
  atom = XInternAtom(display, "_NET_WM_ICON", False);

  if (XGetWindowProperty(display, window, atom, 0, (opts->dimension * opts->dimension) + 8, False, cardinal,
                         &actual_type, &actual_format, &num_items, &bytes_after, &prop_data) != Success) {
    die("could not retrieve window property for window '%ld'", window);
    return EXIT_FAILURE;
  }

  if (!prop_data) {
    XCloseDisplay(display);
    die("window '%ld' has no icon data", window);
  }

  unsigned long *raw_data = (unsigned long *)prop_data;
  unsigned long width = *raw_data;
  unsigned long height = *(raw_data + 1);
  unsigned long *icon_data = raw_data + 2;

  save_png(opts->out_path, width, height, icon_data);
  XFree(prop_data);

  XCloseDisplay(display);

  return EXIT_SUCCESS;
}
