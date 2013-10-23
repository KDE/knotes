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

#ifndef RESOURCELOCALCONFIG_H
#define RESOURCELOCALCONFIG_H

#include <kresources/configwidget.h>
#include "knotes_export.h"
class KUrlRequester;


class KNOTES_EXPORT ResourceLocalConfig : public KRES::ConfigWidget
{
    Q_OBJECT
public:
    explicit ResourceLocalConfig( QWidget *parent = 0, const char *name = 0 );
    virtual ~ResourceLocalConfig();
    
public slots:
    virtual void loadSettings( KRES::Resource *resource );
    virtual void saveSettings( KRES::Resource *resource );
    
private:
    KUrlRequester *mURL;
};

#endif
