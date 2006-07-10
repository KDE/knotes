/*******************************************************************
 KNotesIface.h  --  This file defines the DCOP interface for KNotes.

 Copyright (C) 2004 by Michael Brade <brade@kde.org>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

 In addition, as a special exception, the copyright holders give
 permission to link the code of this program with any edition of
 the Qt library by Trolltech AS, Norway (or with modified versions
 of Qt that use the same license as Qt), and distribute linked
 combinations including the two.  You must obey the GNU General
 Public License in all respects for all of the code used other than
 Qt.  If you modify this file, you may extend this exception to
 your version of the file, but you are not obligated to do so.  If
 you do not wish to do so, delete this exception statement from
 your version.
*******************************************************************/

#ifndef __KNotesAppIface_h__
#define __KNotesAppIface_h__

#include <qstring.h>
#include <qmap.h>

#include <dcopobject.h>


class KNotesAppIface : virtual public DCOPObject
{
    K_DCOP
k_dcop:
    /**
     * Create a new note.
     * @param name the name (title) of the new note, if it is empty,
     *        KNotes will choose an appropriate name
     * @param text the body of the new note
     * @return the new notes' id
     */
    virtual QString newNote( const QString& name = QString::null,
                             const QString& text = QString::null ) = 0;

    /**
     * Create a new note and inserts the current text in the clipboard
     * as text.
     *
     * @param name the name (title) of the new note, if it is empty,
     *        KNotes will choose an appropriate name
     * @return the new notes' id
     */
    virtual QString newNoteFromClipboard( const QString& name = QString::null ) = 0;

    /**
     * Deletes a note forever.
     * @param noteId the id of the note to kill
     */
    virtual ASYNC killNote( const QString& noteId ) = 0;

    /**
     * Deletes a note forever.
     * @param noteId the id of the note to kill
     * @param force do not request confirmation
     */
    virtual ASYNC killNote( const QString& noteId, bool force ) = 0;

    /**
     * Get all the notes including their ids.
     * @return a QMap that maps the id of a note to its name
     */
    virtual QMap<QString,QString> notes() const = 0;

    /**
     * Changes the title/name of a note.
     * @param noteId the id of the note to be modified
     * @param newName the new title
     */
    virtual ASYNC setName( const QString& noteId, const QString& newName ) = 0;

    /**
     * Sets the text of a note. This will delete the old text!
     * @param noteId the id of the note
     * @param newText the new text for the note
     */
    virtual ASYNC setText( const QString& noteId, const QString& newText ) = 0;

    /**
     * Returns the foreground/text color of a note.
     * @param noteId the id of the note in question
     * @return the foreground/text color as a QString
     */
    virtual QString fgColor( const QString& noteId ) const = 0;

    /**
     * Returns the background color of a note.
     * @param noteId the id of the note in question
     * @return the background color as a QString
     */
    virtual QString bgColor( const QString& noteId ) const = 0;

    /**
     * Sets the color (foreground and background color) of a note.
     * @param noteId the id of the note
     * @param fgColor the new text color for the note
     * @param bgColor the new background color for the note
     */
    virtual ASYNC setColor( const QString& noteId, const QString& fgColor,
                                                   const QString& bgColor ) = 0;

    /**
     * Returns the title/name of a note.
     * @param noteId the id of the note in question
     * @return the name as a QString
     */
    virtual QString name( const QString& noteId ) const = 0;

    /**
     * Returns the text of a note.
     * @param noteId the id of the note in question
     * @return the body as a QString
     */
    virtual QString text( const QString& noteId ) const = 0;


    /******** HERE STARTS THE KNotesAppIface DCOP INTERFACE EXTENSION ********/

    /**
     * Show a note as if it had been selected from the "notes" menu.
     * @param noteId the id of the note to show
     */
    virtual ASYNC showNote( const QString& noteId ) const = 0;

    /**
     * Hide a note.
     * @param noteId the id of the note to hide
     */
    virtual ASYNC hideNote( const QString& noteId ) const = 0;

    /**
     * Show all notes on their respective desktops.
     */
    virtual ASYNC showAllNotes() const = 0;

    /**
     * Hide all notes.
     */
    virtual ASYNC hideAllNotes() const = 0;

    /**
     * Returns the width of a note.
     * @param noteId the id of the note in question
     * @return the width as a uint
     */
    virtual int width( const QString& noteId ) const = 0;

    /**
     * Returns the height of a note.
     * @param noteId the id of the note in question
     * @return the height as a uint
     */
    virtual int height( const QString& noteId ) const = 0;

    /**
     * Repositions a note.
     * @param noteId the id of the note to be moved
     * @param x the new x-coordinate of the note
     * @param y the new y-coordinate of the note
     */
    virtual ASYNC move( const QString& noteId, int x, int y ) const = 0;

    /**
     * Changes the size of a note.
     * @param noteId the id of the note to be resized
     * @param width the new width of the note
     * @param height the new height of the note
     */
    virtual ASYNC resize( const QString& noteId, int width, int height ) const = 0;

    /**
     * This tells KNotes that a specific app has synchronized with all the notes.
     * @param app the app that has synced with KNotes
     */
    virtual ASYNC sync( const QString& app ) = 0;

    /**
     * Test if a note was created new after the last sync.
     * @param app the app that wants to get the status since the last sync
     * @param noteId the id of the note
     * @return true if the note is new, false if not or if the note does
     *         not exist
     */
    virtual bool isNew( const QString& app, const QString& noteId ) const = 0;

    /**
     * Test if a note was modified since the last sync.
     * @param app the app that wants to get the status since the last sync
     * @param noteId the id of the note
     * @return true if modified (note that this will return true if the note is
     *         new as well!) and false if the note is not modified or doesn't exist
     */
    virtual bool isModified( const QString& app, const QString& noteId ) const = 0;
};

#endif
