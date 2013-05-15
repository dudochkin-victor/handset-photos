include (common.pri)
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = src themes translations

# Desktop
desktop_entry.files = data/meegophotos.desktop
desktop_entry.path = $$M_INSTALL_DATA/applications
desktop_entry.CONFIG += no_check_exist

# Translation
translation.files = translations/*.qm
translation.path = /usr/share/l10n/meegotouch

INSTALLS += \
    desktop_entry translation

