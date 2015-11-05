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
#include <QMessageBox>

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
		bool PrimeraVez = settings.value("Primeravez", true).toBool();
	settings.endGroup();

	if( !translator.load(grlDir.Home +"idiomas/gr-dap_"+ IdiomaSelect +".qm") )
		translator.load(":/idiomas/gr-dap_"+ IdiomaSelect +".qm");
	app.installTranslator(&translator);

	QString filename = "";
	QStringList args(app.arguments());
	args.takeFirst(); // elimina el nombre del ejecutable

	GrDap w;

	bool isOkReg = false;
	if (PrimeraVez)
	{
		QMessageBox msgBox(0);
		msgBox.setIcon(QMessageBox::Question);
		msgBox.setWindowTitle(QObject::tr("¿Registrar extensión?"));
		msgBox.setText(QObject::tr("¿Deseas registrar las extensiones .dap y dapz?\nDe esta forma podras abir dichos archivos al hacer doble clic sobre ellos."));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::NoToAll);
		msgBox.setDefaultButton(QMessageBox::Save);
		msgBox.setButtonText(QMessageBox::Yes, QObject::tr("Si"));
		msgBox.setButtonText(QMessageBox::No, QObject::tr("No"));
		msgBox.setButtonText(QMessageBox::NoToAll, QObject::tr("No, recordar decision"));

		switch (msgBox.exec())
		{
			case QMessageBox::Yes:
				isOkReg = true;
			break;
			case QMessageBox::No:
				isOkReg = false;
			break;
			case QMessageBox::NoToAll:
			{
				isOkReg = false;
				PrimeraVez = false;
			}
			break;
		}

		settings.beginGroup("GR-dap");
			settings.setValue("Primeravez", PrimeraVez);
		settings.endGroup();
	}

	if (args.contains("--regdap") || isOkReg)
	{
		w.setAssociation("dap" , "Dial-A-Protection"    , "1");
		w.setAssociation("dapz", "Dial-A-Protection Zip", "2");
		args.removeAt(args.indexOf("--regdap"));
	}

	if (args.contains("--unregdap"))
	{
		w.clearAssociation("dap");
		args.removeAt(args.indexOf("--unregdap"));
	}

	if( !args.isEmpty() && args.count() > 0 )
	{
		filename = args.join(" ");
		if( QFile::exists(filename) )
			w.cargarArchivo(filename);
	}

	w.show();

	return app.exec();
}
