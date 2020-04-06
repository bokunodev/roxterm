/*
    roxterm - VTE/GTK terminal emulator with tabs
    Copyright (C) 2015-2020 Tony Houghton <h@realh.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef ROXTERM_DIRS_H
#define ROXTERM_DIRS_H

#include "defns.h"

G_BEGIN_DECLS

extern char *roxterm_app_dir;
extern char *roxterm_bin_dir;

void roxterm_init_app_dir(int argc, char **argv);

void roxterm_init_bin_dir(const char *argv0);

G_END_DECLS

#endif /* ROXTERM_DIRS_H */
