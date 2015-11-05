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

#include <QApplication>
#include <QTranslator>

#include "grdap.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("GR-dap");

	Funciones fGrl;
	QTranslator translator;
	stGrlDir grlDir;
	grlDir.Home = fGrl.GRlidaHomePath();

	QSettings settings(grlDir.Home +"GR-dap.conf", QSettings::IniFormat);
	settings.beginGroup("GR-dap");
		QString IdiomaSelect = settings.value("IdiomaSelect", "es_ES").toString();
	settings.endGroup();

	if( !translator.load(grlDir.Home +"idiomas/gr-dap_"+ IdiomaSelect +".qm") )
		translator.load(":/idiomas/gr-dap_"+ IdiomaSelect +".qm");
	app.installTranslator(&translator);

	QString filename = "";
	QStringList args(app.arguments());
	args.takeFirst(); // elimina el nombre del ejecutable

	GrDap w;

	if( !args.isEmpty() && args.count() > 0 )
	{
		filename = args.join(" ");
		if( QFile::exists(filename) )
			w.cargarArchivo(filename);
	}

	w.show();

	return app.exec();
}
