/*******************************************************************
    Copyright (c) 2005, The KNotes Developers

    This file is part of KNotes.

    KNotes is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KNotes is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with KNotes; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*******************************************************************/

#include <qlabel.h>
#include <qlayout.h>

#include <kdebug.h>
#include <klocale.h>
#include <kresources/configwidget.h>
#include <kurlrequester.h>

#include "resourcelocal.h"
#include "resourcelocalconfig.h"

ResourceLocalConfig::ResourceLocalConfig( QWidget *parent,  const char *name )
    : KRES::ConfigWidget( parent, name )
{
    QHBoxLayout *layout = new QHBoxLayout( this );

    QLabel *label = new QLabel( i18n( "Location:" ), this );
    mURL = new KURLRequester( this );
    KFile::Mode mode = static_cast<KFile::Mode>( KFile::File |
                                                 KFile::LocalOnly );
    mURL->setMode( mode );
    layout->addWidget( label );
    layout->addWidget( mURL );
}

ResourceLocalConfig::~ResourceLocalConfig()
{
}

void ResourceLocalConfig::loadSettings( KRES::Resource *resource )
{
    ResourceLocal *res = dynamic_cast<ResourceLocal *>( resource );
    if ( res )
        mURL->setURL( res->url().prettyURL() );
    else
        kdDebug() << "ERROR: ResourceLocalConfig::loadSettings(): no ResourceLocal, cast failed" << endl;
}

void ResourceLocalConfig::saveSettings( KRES::Resource *resource )
{
    ResourceLocal *res = dynamic_cast<ResourceLocal *>( resource );
    if ( res )
        res->setURL( mURL->url() );
    else
        kdDebug() << "ERROR: ResourceLocalConfig::saveSettings(): no ResourceLocal, cast failed" << endl;
}

#include "resourcelocalconfig.moc"
