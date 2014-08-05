#-------------------------------------------------
#
# Project created by QtCreator 2014-01-10T16:51:20
#
#-------------------------------------------------

# Output filename, directory
win32|mac {
	TARGET = GR-dap
} else {
	TARGET = gr-dap
}
TEMPLATE = app
DESTDIR += bin

# Output directory
UI_DIR += src
MOC_DIR += build
RCC_DIR += build
OBJECTS_DIR += build

# Compiler, tools and options
QT += core gui
contains(QT_CONFIG, opengl):QT += opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Files
INCLUDEPATH += . \
	src \
	ui \
	3rdparty

DEPENDPATH += . \
	src \
	res \
	ui

SOURCES += src/main.cpp \
	src/grdap.cpp \
	src/grdapview.cpp \
	src/funciones.cpp \
	src/qtzip.cpp

HEADERS  += src/grdap.h \
	src/grdapview.h \
	src/funciones.h \
	src/qtzip.h \
	3rdparty/fileassociateqt.h

FORMS += ui/grdap.ui

# 3rdparty Files
HEADERS += 3rdparty/osdab-zip/unzip.h \
	3rdparty/osdab-zip/unzip_p.h \
	3rdparty/osdab-zip/zip.h \
	3rdparty/osdab-zip/zipentry_p.h \
	3rdparty/osdab-zip/zipglobal.h \
	3rdparty/osdab-zip/zip_p.h

SOURCES += 3rdparty/osdab-zip/unzip.cpp \
	3rdparty/osdab-zip/zip.cpp \
	3rdparty/osdab-zip/zipglobal.cpp

LIBS += -lz

HEADERS += 3rdparty/crc32qt.h
# FIN 3rdparty

TRANSLATIONS += res/idiomas/gr-dap_es_ES.ts \
	res/idiomas/gr-dap_en_EN.ts

RESOURCES += res/gr-dap.qrc
win32 {
	ICON = gr-dap.ico
	RC_FILE += gr-dap.rc
}
mac:ICON += gr-dap.icns

#CONFIG += mobility
#MOBILITY =
