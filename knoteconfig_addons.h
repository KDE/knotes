public:
KNoteConfig() : KConfigSkeleton()
{
  setCurrentGroup( QString::fromLatin1( "Display" ) );

  KConfigSkeleton::ItemColor  *itemBgColor;
  itemBgColor = new KConfigSkeleton::ItemColor( currentGroup(), QString::fromLatin1( "bgcolor" ), mBgColor, QColor( "yellow" ) );
  addItem( itemBgColor, QString::fromLatin1( "BgColor" ) );
  KConfigSkeleton::ItemColor  *itemFgColor;
  itemFgColor = new KConfigSkeleton::ItemColor( currentGroup(), QString::fromLatin1( "fgcolor" ), mFgColor, QColor( "black" ) );
  addItem( itemFgColor, QString::fromLatin1( "FgColor" ) );
  KConfigSkeleton::ItemUInt  *itemWidth;
  itemWidth = new KConfigSkeleton::ItemUInt( currentGroup(), QString::fromLatin1( "width" ), mWidth, 200 );
  addItem( itemWidth, QString::fromLatin1( "Width" ) );
  KConfigSkeleton::ItemUInt  *itemHeight;
  itemHeight = new KConfigSkeleton::ItemUInt( currentGroup(), QString::fromLatin1( "height" ), mHeight, 200 );
  addItem( itemHeight, QString::fromLatin1( "Height" ) );

  setCurrentGroup( QString::fromLatin1( "Editor" ) );

  KConfigSkeleton::ItemFont  *itemFont;
  itemFont = new KConfigSkeleton::ItemFont( currentGroup(), QString::fromLatin1( "font" ), mFont, KGlobalSettings::generalFont() );
  addItem( itemFont, QString::fromLatin1( "Font" ) );
  KConfigSkeleton::ItemFont  *itemTitleFont;
  itemTitleFont = new KConfigSkeleton::ItemFont( currentGroup(), QString::fromLatin1( "titlefont" ), mTitleFont, KGlobalSettings::windowTitleFont() );
  addItem( itemTitleFont, QString::fromLatin1( "TitleFont" ) );
  KConfigSkeleton::ItemBool  *itemAutoIndent;
  itemAutoIndent = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "autoindent" ), mAutoIndent, true );
  addItem( itemAutoIndent, QString::fromLatin1( "AutoIndent" ) );
  KConfigSkeleton::ItemBool  *itemRichText;
  itemRichText = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "richtext" ), mRichText, false );
  addItem( itemRichText, QString::fromLatin1( "RichText" ) );
  KConfigSkeleton::ItemUInt  *itemTabSize;
  itemTabSize = new KConfigSkeleton::ItemUInt( currentGroup(), QString::fromLatin1( "tabsize" ), mTabSize, 4 );
  addItem( itemTabSize, QString::fromLatin1( "TabSize" ) );

  setCurrentGroup( QString::fromLatin1( "General" ) );

  KConfigSkeleton::ItemDouble  *itemVersion;
  itemVersion = new KConfigSkeleton::ItemDouble( currentGroup(), QString::fromLatin1( "version" ), mVersion );
  addItem( itemVersion, QString::fromLatin1( "Version" ) );

  setCurrentGroup( QString::fromLatin1( "WindowDisplay" ) );

  KConfigSkeleton::ItemInt  *itemDesktop;
  itemDesktop = new KConfigSkeleton::ItemInt( currentGroup(), QString::fromLatin1( "desktop" ), mDesktop, -10 );
  addItem( itemDesktop, QString::fromLatin1( "Desktop" ) );
  KConfigSkeleton::ItemBool  *itemHideNote;
  itemHideNote = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "HideNote" ), mHideNote, false );
  addItem( itemHideNote, QString::fromLatin1( "HideNote" ) );
  KConfigSkeleton::ItemPoint  *itemPosition;
  itemPosition = new KConfigSkeleton::ItemPoint( currentGroup(), QString::fromLatin1( "position" ), mPosition, QPoint( -10000, -10000 ) );
  addItem( itemPosition, QString::fromLatin1( "Position" ) );
  KConfigSkeleton::ItemBool  *itemShowInTaskbar;
  itemShowInTaskbar = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "ShowInTaskbar" ), mShowInTaskbar, false );
  addItem( itemShowInTaskbar, QString::fromLatin1( "ShowInTaskbar" ) );
  KConfigSkeleton::ItemBool  *itemKeepAbove;
  itemKeepAbove = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "KeepAbove" ), mKeepAbove, false );
  addItem( itemKeepAbove, QString::fromLatin1( "KeepAbove" ) );
  KConfigSkeleton::ItemBool  *itemKeepBelow;
  itemKeepBelow = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "KeepBelow" ), mKeepBelow, false );
  addItem( itemKeepBelow, QString::fromLatin1( "KeepBelow" ) );
}
