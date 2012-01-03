###############################################################################
# Makefile
#
# Build rules for liborsp.
#
# ---------------------------------------------------------------------------
# liborsp - Ojete RSP library.
#   (C) 2008-2012 Gerardo García Peña <killabytenow@gmail.com>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the Free
#   Software Foundation; either version 2 of the License, or (at your option)
#   any later version.
#
#   This program is distributed in the hope that it will be useful, but WITHOUT
#   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#   more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program; if not, write to the Free Software Foundation, Inc., 51
#   Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
###############################################################################

PACKAGE_VERSION=1.0.0
PACKAGE_NAME=liborsp
PACKAGE_TARNAME=$(PACKAGE_NAME)-$(PACKAGE_VERSION)

# configure your installation here
DOCDIR=/usr/share/doc/$(PACKAGE_NAME)
LIBDIR=/usr/lib

# filez
TARGETS=liborsp.a liborsp.so test stest

# library modules
ST_OBJS=encoding.o decoding.o rspfd_fd.o msgparse.o rspmsg.o

all : $(TARGETS)

%.o : %.c
	gcc \
	  -DPACKAGE_NAME='"$(PACKAGE_NAME)"' \
	  -DPACKAGE_VERSION='"$(PACKAGE_VERSION)"' \
	  -DPTRACE_ENABLED='"$(PTRACE_ENABLED)"' \
	  -Wall -fPIC -c -o $@ $<

lib%.a : %.o
	ar r $@ $^

lib%.so : %.o
	gcc -Wall -shared -fPIC -o $@ $^

%.so : %.o
	gcc -Wall -shared -fPIC -o $@ $^

encoding.o : encoding.c encoding.h buffer.h
decoding.o : decoding.c decoding.h buffer.h
rspfd_fd.o : rspfd_fd.c rspfd.h
rspmsg.o   : rspmsg.c rspmsg.h buffer.h
msgparse.o : msgparse.c
liborsp.so : $(ST_OBJS)
liborsp.a  : $(ST_OBJS)

###############################################################################
# PHONY TARGETS
#
.PHONY : clean install tar stacktrace

clean :
	rm -f stacktrace.o ptrace.o stacktrace_dso.o \
	      libstacktrace.a                        \
	      stacktrace.so libstacktrace.so         \
	      test stest

install : all
	install -m 0644 -D BUGS             $(DOCDIR)/BUGS
	install -m 0644 -D ChangeLog        $(DOCDIR)/ChangeLog
	install -m 0644 -D COPYING          $(DOCDIR)/COPYING
	install -m 0644 -D README           $(DOCDIR)/README
	install -m 0644 -D THANKS           $(DOCDIR)/THANKS
	install -m 0755 -D stacktrace       $(BINDIR)/stacktrace
	install -m 0644 -D stacktrace.h     $(INCDIR)/stacktrace.h
	install -m 0755 -D stacktrace.so    $(LIBDIR)/stacktrace.so
	install -m 0755 -D libstacktrace.so $(LIBDIR)/libstacktrace.so
	install -m 0755 -D libstacktrace.a  $(LIBDIR)/libstacktrace.a

$(PACKAGE_TARNAME).tar.gz :                  \
  BUGS COPYING ChangeLog README THANKS       \
  Makefile                                   \
  msgparse.c .h stacktrace_dso.c \
  run_test.sh test.c
	mkdir $(PACKAGE_TARNAME)
	cp $^ $(PACKAGE_TARNAME)
	tar cfz $@ $(PACKAGE_TARNAME)
	rm -rf -- $(PACKAGE_TARNAME)

tar : $(PACKAGE_TARNAME).tar.gz

