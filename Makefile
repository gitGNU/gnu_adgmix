#
# Copyright (C) 2002, Reynaldo H. Verdejo Pinochet 
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# 
#
# This Makefile is a mess, is on the very TOP of the TODO list :-)
#
# Choose the compiler
#   #CC = CC
CC=gcc
VPATH = man
# Compiler options
# 
CCOPTS=-Wall -O2 `pkg-config --libs --cflags gtk+-2.0`
# Header files
HEADERS=logo.h
#
adgmix:adgmix.c $(HEADERS) manpage
	$(CC) $(CCOPTS) oss_functions.c adgmix.c -o adgmix
adgmix.1.gz:manpage
	gzip -v --best -c ./man/manpage > ./man/adgmix.1.gz
#install
# root only
install:adgmix adgmix.1.gz
	install -d $(DESTDIR)/usr/bin/
	install -d $(DESTDIR)/usr/share/man/man1/
	install adgmix $(DESTDIR)/usr/bin/
	install ./man/adgmix.1.gz $(DESTDIR)/usr/share/man/man1/
# Clean
# root only
.PHONY: clean
clean:
	-rm adgmix 
	-rm ./man/adgmix.1.gz
.PHONY: uninstall
uninstall:
	-rm $(DESTDIR)/usr/bin/adgmix
	-rm $(DESTDIR)/usr/share/man/man1/adgmix.1.gz
