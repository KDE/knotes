/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 2003, Daniel Martin <daniel.martin@pirack.com>
               2004, 2006, Michael Brade <brade@kde.org>

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

#include <klocale.h>
#include <kmessagebox.h>

#include "knotesnetsend.h"

#define CONNECT_TIMEOUT 10000


KNotesNetworkSender::KNotesNetworkSender( const QString& hostname, int port )
  : KBufferedSocket( hostname, QString::number( port ) ),
    m_note( 0 ), m_title( 0 ), m_sender( 0 ), m_index( 0 )
{
    enableRead( false );
    enableWrite( false );
    setTimeout( CONNECT_TIMEOUT );

    // QObject:: prefix needed, otherwise the KStreamSocket::connect()
    // mehtod is called!!!
    QObject::connect( this, SIGNAL(connected( const KResolverEntry& )), 
                            SLOT(slotConnected( const KResolverEntry& )) );
    QObject::connect( this, SIGNAL(gotError( int )), SLOT(slotError( int )) );
    QObject::connect( this, SIGNAL(closed()), SLOT(slotClosed()) );
    QObject::connect( this, SIGNAL(readyWrite()), SLOT(slotReadyWrite()) );
}

void KNotesNetworkSender::setSenderId( const QString& sender )
{
    m_sender = sender.ascii();
}

void KNotesNetworkSender::setNote( const QString& title, const QString& text )
{
    // TODO: support for unicode and rich text.
    // Mmmmmm... how to behave with such heterogeneous environment?
    // AFAIK, ATnotes does not allow UNICODE.
    m_title = title.ascii();
    m_note = text.ascii();
}

void KNotesNetworkSender::slotConnected( const KResolverEntry& )
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
        close();
}

void KNotesNetworkSender::slotError( int err )
{
    KMessageBox::sorry( 0, i18n("Communication error: %1")
           .arg( errorString( static_cast<KSocketBase::SocketError>(err) ) )
    );
    slotClosed();
}

void KNotesNetworkSender::slotClosed()
{
    deleteLater();
}

#include "knotesnetsend.moc"
