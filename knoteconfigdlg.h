/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2005, The KNotes Developers

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
*******************************************************************/

#ifndef KNOTECONFIGDLG_H
#define KNOTECONFIGDLG_H

#include <kconfigdialog.h>
#include <kcmodule.h>
#include <kcmultidialog.h>
#include "knotes_export.h"
class QString;

class KNoteConfig;

class KNOTES_EXPORT KNoteConfigDlg
  : public KCMultiDialog
{
  Q_OBJECT
  public:
    explicit KNoteConfigDlg( const QString &title,
                    QWidget *parent );
    ~KNoteConfigDlg();

public slots:
    void slotOk();
signals:
    void configWrote();
};


class KNoteDisplayConfig : public KCModule
{
    Q_OBJECT
public:
    KNoteDisplayConfig( const KComponentData &inst, QWidget *parent );
    /** Reimplemented from KCModule. */
    virtual void load();
    virtual void save();
};

class KNoteEditorConfig : public KCModule
{
    Q_OBJECT
public:
    KNoteEditorConfig( const KComponentData &inst, QWidget *parent );
    /** Reimplemented from KCModule. */
    virtual void load();
    virtual void save();
};


class KNoteActionConfig : public KCModule
{
    Q_OBJECT
public:
    KNoteActionConfig( const KComponentData &inst, QWidget *parent );
    /** Reimplemented from KCModule. */
    virtual void load();
    virtual void save();

private Q_SLOTS:
    void slotHelpLinkClicked(const QString &);
};

class KNoteNetworkConfig : public KCModule
{
    Q_OBJECT
public:
    KNoteNetworkConfig( const KComponentData &inst, QWidget *parent );
    /** Reimplemented from KCModule. */
    virtual void load();
    virtual void save();
};

class KNoteDisplayConfigWidget :public QWidget
{
    Q_OBJECT
public:
    explicit KNoteDisplayConfigWidget( bool defaults );
};

class KNoteEditorConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KNoteEditorConfigWidget( bool defaults );
};

#endif
