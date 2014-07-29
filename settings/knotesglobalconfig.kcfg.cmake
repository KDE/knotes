<?xml version="1.0" encoding="UTF-8"?>
<kcfg xmlns="http://www.kde.org/standards/kcfg/1.0"
      xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
      xsi:schemaLocation="http://www.kde.org/standards/kcfg/1.0
      http://www.kde.org/standards/kcfg/1.0/kcfg.xsd" >
  <include>QFontDatabase</include>
  <group name="General">
    <entry name="AutoCreateResourceOnStart" type="Bool">
      <default>true</default>
    </entry>
  </group>

  <group name="Misc">
    <entry name="SystemTrayShowNotes" type="Bool">
      <default>true</default>
    </entry>
  </group>


  <group name="Printing">
     <entry name="Theme" type="String">
        <default>${DATA_INSTALL_DIR}/knotes/print/themes/default/</default>
     </entry>
  </group>

  <group name="Display">
    <entry name="BgColor" type="Color" key="bgcolor">
      <default code="true">Qt::yellow</default>
    </entry>

    <entry name="FgColor" type="Color" key="fgcolor">
      <default code="true">Qt::black</default>
    </entry>

    <entry name="Width" type="UInt" key="width">
      <default>300</default>
    </entry>

    <entry name="Height" type="UInt" key="height">
      <default>300</default>
    </entry>

    <entry name="RememberDesktop" type="Bool">
      <default>true</default>
    </entry>
  </group>

  <group name="Editor">
    <entry name="Font" type="Font" key="font">
      <default code="true">QFontDatabase::systemFont(QFontDatabase::GeneralFont)</default>
    </entry>

    <entry name="TitleFont" type="Font" key="titlefont">
      <default code="true">QFontDatabase::systemFont(QFontDatabase::TitleFont)</default>
    </entry>

    <entry name="AutoIndent" type="Bool" key="autoindent">
      <default>true</default>
    </entry>

    <entry name="RichText" type="Bool" key="richtext">
      <default>false</default>
    </entry>

    <entry name="TabSize" type="UInt" key="tabsize">
      <default>4</default>
    </entry>

    <entry name="ReadOnly" type="Bool">
      <default>false</default>
    </entry>
  </group>

  <group name="WindowDisplay">
    <entry name="Desktop" type="Int" key="desktop">
      <default>-10</default>
    </entry>

    <entry name="HideNote" type="Bool">
      <default>false</default>
    </entry>

    <entry name="Position" type="Point" key="position">
      <default code="true">QPoint( -10000, -10000 )</default>
    </entry>

    <entry name="ShowInTaskbar" type="Bool">
      <default>false</default>
    </entry>

    <entry name="KeepAbove" type="Bool">
      <default>false</default>
    </entry>

    <entry name="KeepBelow" type="Bool">
      <default>false</default>
    </entry>
  </group>

</kcfg>
