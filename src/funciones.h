/*
 *
 * GR-dap by Monthy
 *
 * This file is part of GR-dap is Dial-A-Protection
 * Copyright (C) 2014-2020 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <QCryptographicHash>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QColor>

#include "crc32qt.h"

struct stGrlDir {
	QString Home;
	QString Datos;
	QString Temp;
	QString Iconos;
};

struct stFileInfo {
	QString Drive;
	QString Path;
	QString FilePath;
	QString Name;
	QString NameExt;
	QString cExt;
	QString Ext;
	QString Size;
	bool    Exists;
	bool    isDir;
	QString Crc32;
	QString Sha1;
	QString Md5;
};

enum TipoHash {
	hashNo    = 0,
	hashAll   = 1,
	hashCrc32 = 2,
	hashSha1  = 3,
	hashMd5   = 4
};

class Funciones : public QObject
{
	Q_OBJECT
public:
	Funciones();
	~Funciones();

// Versión del GR-dap
	QString versionGrd(){return "0.2.0";}

// Convierte de número a texto
	QString	intToStr(int num);
// Devuelve un QColor desde un QStringList o QString
	QColor getColor(QStringList color);
	QColor getColor(QString color);
// Obtiene información del archivo
	stFileInfo getInfoFile(QString filename, TipoHash hash = hashNo);
// Obtiene el Hash del archivo
	QString getHashFile(QString filename, TipoHash hash = hashSha1);
// Comprueba si un directorio existe y sino lo crea
	bool comprobarDirectorio(QString dir, bool info = false);

// Devuelve el directorio que usa el GR-lida
	QString homePath();
	QString dirApp(){ return stDirApp; }
// Devuelve el directorio del Theme a usar
	QString theme(){ return stTheme; }

private:
	QString stDirApp, stTheme, stOS;

};

#endif // FUNCIONES_H
