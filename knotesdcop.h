/* knotesdcop.h
**
** Copyright (C) 2000 by Adriaan de Groot
**
** This file defines a DCOP interface for KNotes.
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 675 Mass Ave, Cambridge,
** MA 02139, USA.
*/

#ifndef KNOTESDCOP_H
#define KNOTESDCOP_H

#include <dcopobject.h>

class KNotesDCOP : virtual public DCOPObject
{
	K_DCOP
k_dcop:
	/**
	* Show the n-th note in the list, as if it had been
	* selected from the "notes" menu.
	*/
	virtual ASYNC showNote(QString name) = 0;

	/**
	* Re-read the directory(ies) containing KNotes
	* and add any new notes to the notes managed by
	* KNotes.
	*/
	virtual ASYNC rereadNotesDir() = 0;

	/**
	* Add a note -- with complete text, title, and pilot id --
	* to the list of notes managed by KNotes.
	*/
	virtual ASYNC addNote(QString title, 
		QString body, 
		unsigned long pilotid) = 0;
} ;


// $Log$
// Revision 1.1  2000/12/19 18:00:49  adridg
// Added DCOP calls to manipulate notes (intended for KPilot)
//
#endif
