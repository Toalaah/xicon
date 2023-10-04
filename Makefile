# Copyright © 2023 Samuel Kunst <samuel at kunst.me>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.

.POSIX:

PREFIX    = /usr/local
BINPREFIX = $(PREFIX)/bin
MANPREFIX = $(PREFIX)/share/man/man1

OUT = xicon
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

CC = gcc

VERSION := $(shell git describe --tags --dirty --abbrev=7 2> /dev/null || cat version)

CFLAGS    = -std=c99 -pedantic -Wall -Wextra -Werror
CFLAGS   += -DXICON_VERSION=\"$(VERSION)\"

LDLIBS    = -lX11 -lpng16

all: $(OUT)

$(OBJ): Makefile

install:
	mkdir -p $(BINPREFIX)
	cp -p $(OUT) $(BINPREFIX)
	mkdir -p $(MANPREFIX)
	sed "s/VERSION/$(VERSION)/g" < $(OUT).1 | gzip > $(MANPREFIX)/$(OUT).1.gz

uninstall:
	rm -f $(BINPREFIX)/$(OUT)
	rm -f $(MANPREFIX)/$(OUT).1

clean:
	rm -rf $(OUT) $(OBJ)

.PHONY: all install uninstall clean
