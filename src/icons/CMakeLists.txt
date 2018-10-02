file(GLOB knotes_icons "*-apps-knotes.*")
ecm_install_icons(ICONS
    ${knotes_icons}
    16-actions-knotes_alarm.png
    16-actions-knotes_close.png
    16-actions-knotes_date.png
    16-actions-knotes_delete.png
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)
