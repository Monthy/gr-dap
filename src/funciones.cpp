/*
 *
 * GR-dap by Monthy
 *
 * This file is part of GR-dap is Dial-A-Protection
 * Copyright (C) 2014 Pedro A. Garcia Rosado Aka Monthy
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

#include "funciones.h"

Funciones::Funciones()
{
	stDirApp = GRlidaHomePath();
}

Funciones::~Funciones()
{
	//
}

// Convierte de número a texto
QString Funciones::IntToStr(int num)
{
	return QVariant(num).toString();
}

// Devuelve un QColor desde un QStringList o QString
QColor Funciones::getColor(QStringList color)
{
	int r = 0; int g = 0; int b = 0; int a = 255;

	if( color.isEmpty() )
		color << "0" << "0" << "0";

	if( color.size() < 4 )
	{
		int pos = color.size();
		for(int i = pos; i < 3; ++i)
			color.insert(i, "0");
		color.insert(3, "255");
	}

	r = color.at(0).toInt();
	g = color.at(1).toInt();
	b = color.at(2).toInt();
	a = color.at(3).toInt();

	return QColor(r, g, b, a);
}

QColor Funciones::getColor(QString color)
{
	return getColor( color.split(",") );
}

// Obtiene información del archivo
stFileInfo Funciones::getInfoFile(QString filename, TipoHash hash)
{
	stFileInfo info;

	if( filename.isEmpty() )
	{
		info.Drive    = "";
		info.Path     = "";
		info.FilePath = "";
		info.Name     = "";
		info.NameExt  = "";
		info.cExt     = "";
		info.Ext      = "";
		info.Size     = "";
		info.Exists   = false;
		info.isDir    = false;
		info.Crc32    = "00000000";
		info.Sha1     = "";
		info.Md5      = "";
	} else {
		QFileInfo fi( filename );
		#ifdef Q_OS_WIN32
			info.Drive = filename.left(2) +"\\";
		#else
			info.Drive = "/";
		#endif
		if( fi.absolutePath().endsWith("/") || fi.absolutePath().endsWith("\\") )
			info.Path = QDir::toNativeSeparators( fi.absolutePath() );
		else
			info.Path = QDir::toNativeSeparators( fi.absolutePath().append("/") );
		info.FilePath = info.Path + fi.fileName();
		info.Name     = fi.baseName();
		info.NameExt  = fi.fileName();
		info.cExt     = "."+ fi.completeSuffix().toLower();
		info.Ext      = "."+ fi.suffix().toLower();
		info.Size     = IntToStr( fi.size() );
		info.Exists   = fi.exists();
		info.isDir = fi.isDir();

		if( hash == hashCrc32 || hash == hashAll )
			info.Crc32 = getHashFile(filename, hashCrc32);
		else
			info.Crc32 = "00000000";

		if( hash == hashSha1 || hash == hashAll )
			info.Sha1 = getHashFile(filename, hashSha1);
		else
			info.Sha1 = "";

		if( hash == hashMd5 || hash == hashAll )
			info.Md5 = getHashFile(filename, hashMd5);
		else
			info.Md5 = "";
	}

	return info;
}

// Obtiene el Hash del archivo
QString Funciones::getHashFile(QString filename, TipoHash hash)
{
	if( hash == hashCrc32 )
	{
		Crc32Qt crc;
		return crc.getCRC32toString(filename);
	} else {
		QCryptographicHash::Algorithm tipoAlgorithm = QCryptographicHash::Sha1;
		if( hash == hashMd5 )
			tipoAlgorithm = QCryptographicHash::Md5;
		else
			tipoAlgorithm = QCryptographicHash::Sha1;

		QCryptographicHash crypto(tipoAlgorithm);
		QFile file( filename );
		file.open(QFile::ReadOnly);
		while( !file.atEnd() )
			crypto.addData(file.read(8192));
		file.close();

		return QString(crypto.result().toHex());
	}
}

// Comprueba si un directorio existe y sino lo crea
bool Funciones::comprobarDirectorio(QString nameDir, bool info)
{
	if( !nameDir.isEmpty() )
	{
		QDir directorio;
		if( directorio.exists( nameDir ) )
			return true;
		else {
			if( info )
				return false;
			else
				return directorio.mkpath( nameDir );
		}
	} else
		return false;
}

// Devuelve el directorio que usa el GR-lida
QString Funciones::GRlidaHomePath()
{
	bool isWinOrMac = false;
	stDirApp = "";

	if( QFile::exists(QDir::currentPath() +"/GR-lida.conf") )
		stDirApp = QDir::currentPath() +"/";
	else {
		#ifdef Q_OS_WIN32
			isWinOrMac = true;
		#else
			#ifdef Q_OS_MAC
				isWinOrMac = true;
			#else
				isWinOrMac = false;
			#endif
		#endif

		if( isWinOrMac )
			stDirApp = QDir::homePath() +"/GR-lida/";
		else
			stDirApp = QDir::homePath() +"/.gr-lida/";
	}

	return stDirApp;
}
