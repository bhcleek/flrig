// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef __DIALOGS_H__
#define __DIALOGS_H__

#include <string>

#include "font_browser.h"
#include "support.h"

extern Fl_Double_Window *dlgDisplayConfig;
extern Fl_Double_Window *dlgXcvrConfig;
extern Fl_Double_Window *dlgMemoryDialog;
extern Fl_Double_Window *dlgControls;

extern Font_Browser     *fntbrowser;

extern void init_port_combos();

extern bool editing;

extern void sort_by_call();
extern void sort_by_datetime();
extern void sort_by_nbr();
extern void sort_by_freq();

extern void clear_qso();
extern void save_qso();

extern void edit_entry();
extern void delete_entry();
extern void view_log();

extern void log_close();
extern void log_open();
extern void log_new();

extern void log_load();
extern void log_save();
extern void log_save_as();

extern void log_export_adif();
extern void log_import_adif();

#endif // __DIALOGS_H__
