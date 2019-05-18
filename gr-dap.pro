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


# Output directory
DESTDIR += bin
UI_DIR += build/ui
MOC_DIR += build/moc
RCC_DIR += build/rcc
OBJECTS_DIR += build/obj

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
	src/grdap_acercad.cpp

HEADERS  += src/grdap.h \
	src/grdapview.h \
	src/funciones.h \
	src/grdap_acercad.h

FORMS += ui/grdap.ui \
	ui/grdap_acercad.ui

# 3rdparty Files
HEADERS += 3rdparty/crc32qt.h \
	3rdparty/fileassociateqt.h

include(3rdparty/Qt7zip/Qt7zip.pri)
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
