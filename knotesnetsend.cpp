/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 2003, Daniel Martin <daniel.martin@pirack.com>
               2004, Michael Brade <brade@kde.org>

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
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

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

#include <klocale.h>
#include <kmessagebox.h>
#include <ksockaddr.h>

#include "knotesnetsend.h"

// This comes in seconds!
#define CONNECT_TIMEOUT 10


KNotesNetworkSender::KNotesNetworkSender( const QString& hostname, int port )
  : KExtendedSocket( hostname, port )
{
    enableRead( false );
    enableWrite( false );
    setBlockingMode( true );    // Recommended by documentation.
    setTimeout( CONNECT_TIMEOUT );

    QObject::connect( this, SIGNAL(connectionSuccess()), SLOT(slotConnected()) );
    QObject::connect( this, SIGNAL(connectionFailed( int )), SLOT(slotError( int )) );
    QObject::connect( this, SIGNAL(closed( int )), SLOT(slotClosed( int )) );
    QObject::connect( this, SIGNAL(readyWrite()), SLOT(slotReadyWrite()) );
}

void KNotesNetworkSender::setSenderId( const QString& sender )
{
    m_sender = sender.ascii();
}

void KNotesNetworkSender::setNote( const QString& title, const QString& text )
{
    // TODO: support for unicode and richtext.
    // Mmmmmm... how to behave with such heterogeneous environment?
    // AFAIK, ATnotes does not allow UNICODE.
    m_title = title.ascii();
    m_note = text.ascii();
}

void KNotesNetworkSender::slotConnected()
{
    if ( m_sender.isEmpty() )
        m_note.prepend( m_title + "\n");
    else
        m_note.prepend( m_title + " (" + m_sender + ")\n" );

    enableWrite( true );
}

void KNotesNetworkSender::slotReadyWrite()
{
    m_index += writeBlock( m_note.data() + m_index, m_note.length() - m_index );

    // If end of text reached, close connection
    if ( m_index == m_note.length() )
        closeNow();
}

void KNotesNetworkSender::slotError( int errorCode )
{
    KMessageBox::sorry( 0, i18n("Communication error: %1").arg(
                                strError( status(), errorCode ) ) );
    slotClosed( 0 );
}

void KNotesNetworkSender::slotClosed( int /*state*/ )
{
    delete this;
}

#include "knotesnetsend.moc"
