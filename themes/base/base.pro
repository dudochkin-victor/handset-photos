load(meegotouch_defines)
include (../../common.pri)

TEMPLATE = subdirs
THEME_NAME = $$system(basename $$PWD)

OTHER_FILES += meegophotos.conf

# Theme files
theme.files = $$system(find ./* -type d)
theme.files += meegophotos.conf
theme.path = $$M_THEME_DIR/$$THEME_NAME/meegotouch/$$TARGET/
theme.CONFIG += no_check_exist

INSTALLS += \
    theme
