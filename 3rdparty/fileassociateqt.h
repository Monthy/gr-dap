/*
 *
 * FileAssociateQt
 *
 * Thanks Yuezhao Huang (https://github.com/yuezhao/ezviewer)
 * modified by: Pedro A. Garcia Rosado Aka Monthy (http://www.gr-lida.org)
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

#ifndef FILEASSOC_H
#define FILEASSOC_H

#include <QString>
#include <QSysInfo>
#include <QSettings>
#include <QCoreApplication>
#include <QFileInfo>
#include <QStringList>
#include <QTemporaryFile>
#include <QTextStream>
#include <QProcess>

//#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
//#define Q_WS_WIN Q_OS_WIN
//#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

#ifdef Q_OS_WIN32
#include <Shlobj.h> // for SHChangeNotify(), refresh windows explorer
#endif

namespace FileAssoc
{
#ifdef Q_OS_WIN32
	const QString REG_DEFAULT("/.");

	static QString makeProgID(const QString & extension)
	{
		// ProgID: like editor.txt
		// txt -> .txt  -> editor.txt
		return QFileInfo(qApp->applicationFilePath()).baseName() +"."+ extension;
	}

	static bool hasProgID(const QString &progID)
	{
		QSettings regCR("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
		QSettings regCU("HKEY_CURRENT_USER", QSettings::NativeFormat);

		regCU.beginGroup("Software/Classes");

		QString key = progID +"/shell/open/command/.";
		QString appPath = qApp->applicationFilePath();
		appPath.replace("/", "\\");
		QString expect = QString("\"%1\" \"%2\"").arg(appPath, "%1"); // If it is set file association by this app, the value of key @key will be like this.

		if( regCR.childGroups().contains(progID) )
			return regCR.value(key).toString() == expect;
		else if( regCU.childGroups().contains(progID) )
			return regCU.value(key).toString() == expect;
		else
			return false;
	}

	static bool setProgID(const QString &progID, const QString &typeDescription, const QString &friendlyName, const QString &iconIndex)
	{
		QString rootKeyName;
		QString classId;
		if( QSysInfo::WindowsVersion >= QSysInfo::WV_NT )
		{
			classId = "Software/Classes/"+ progID;
			rootKeyName = "HKEY_CURRENT_USER";
		} else {
			classId = progID;
			rootKeyName = "HKEY_CLASSES_ROOT"; // Windows 95/98/ME
		}

		QSettings Reg(rootKeyName, QSettings::NativeFormat);
		if( !Reg.isWritable() || Reg.status() != QSettings::NoError )
			return false;

		QString icon = iconIndex.isEmpty() ? "0" : iconIndex;
		QString appPath = qApp->applicationFilePath();
		appPath.replace("/", "\\"); //Explorer gives 'Access Denied' if we write the path with forward slashes to the registry

		if( !typeDescription.isNull() )
			Reg.setValue(classId + REG_DEFAULT, typeDescription);/// Without this, will no able to create new text document.
		if( !friendlyName.isNull() )
			Reg.setValue(classId +"/shell/open/FriendlyAppName", friendlyName);

		Reg.setValue(classId +"/shell/open/command/.", QString("\"%1\" \"%2\"").arg(appPath, "%1"));
		Reg.setValue(classId +"/DefaultIcon/.", QString("\"%1\",%2").arg(appPath).arg(icon)); /// Use the first icon of current program for default format icon.
		return true;
	}

	static bool removeUserChoice(const QString &extension)
	{
		QSettings regCU("HKEY_CURRENT_USER", QSettings::NativeFormat);
		QString fileExtsKey = QString("Software/Microsoft/Windows/CurrentVersion/Explorer/FileExts/.") + extension;

		if( QSysInfo::WindowsVersion < QSysInfo::WV_VISTA )
		{ // XP
			regCU.remove(fileExtsKey +"/Progid");
			return true;
		}

		QTemporaryFile file;
		if( file.open() )
		{
			QTextStream out(&file);
			// Unlock Registry, make user 'everyone' can access 'UserChoice' completely.
			out << "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\."+ extension +"\\UserChoice [7]";
			out.flush();
			file.flush();

			// win7 build-in tool, for user access changing. Require path argument encoded by Unicode. Chinese may cause problem.
			// Only return after execute completely.
			QProcess::execute(QString("regini.exe %1").arg(file.fileName()));

			/// Windows 7, only can remove "/UserChoice" if we own it. remove "/UserChoice/Progid" is forbidden.
			regCU.remove(fileExtsKey +"/UserChoice");
			return true;
		}
		return false;
		// The QTemporaryFile destructor removes the temporary file
		// as it goes out of scope.
	}
#endif // Q_WS_WIN

	bool checkAssociation(const QString &extension)
	{
	#ifdef Q_OS_WIN32
	/*! The HKEY_CLASSES_ROOT subtree is a view formed by merging
	 *  HKEY_CURRENT_USER\Software\Classes and
	 *  HKEY_LOCAL_MACHINE\Software\Classes
	 */
		QSettings regCR("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
		QSettings regCU("HKEY_CURRENT_USER", QSettings::NativeFormat);

		QString ext("."+ extension);
		QString progID = makeProgID(extension);

		QString fileName = QFileInfo(qApp->applicationFilePath()).fileName();
		QString fileExtsKey = QString("Software/Microsoft/Windows/CurrentVersion/Explorer/FileExts/") + ext;
		/// Windows 7: "/UserChoice/Progid"; XP: "/Progid"
		QString curClassId = (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA)
				? regCU.value(fileExtsKey +"/UserChoice/Progid").toString()
				: regCU.value(fileExtsKey +"/Progid").toString();
		QString curAppId = regCU.value(fileExtsKey +"/Application").toString(); /// Windows XP

		bool isRegistered = false;
		if( QSysInfo::WindowsVersion >= QSysInfo::WV_NT )
		{
			if (!curClassId.isEmpty())
			{
				isRegistered = (curClassId == progID) || (0 == curClassId.compare(fileName, Qt::CaseInsensitive))
						|| (0 == curClassId.compare(QString("Applications\\%1").arg(fileName), Qt::CaseInsensitive));
			} else if (!curAppId.isEmpty()) {
				// If user uses Open With..., explorer creates it's own ClassId under Application, like "smplayer.exe"
				isRegistered = (curAppId == progID) || (0 == curAppId.compare(fileName, Qt::CaseInsensitive));
			} else {
				// No classId means that no associations exists in Default Programs or Explorer
				// Check the default per-user association
				isRegistered = (regCU.value("Software/Classes/"+ ext + REG_DEFAULT).toString() == progID);
			}
		}

		// Finally, check the system-wide association
		// If curClassId or curAppId not empty, indicating that the user in the "Open" and select the other programs, high priority, overrides other settings.
		if( !isRegistered && curClassId.isEmpty() && curAppId.isEmpty() )
			isRegistered = (regCR.value(ext + REG_DEFAULT).toString() == progID);

		return isRegistered && hasProgID(progID);
	#else
		Q_UNUSED(extension);
		return false;
	#endif // Q_WS_WIN
	}

	bool setAssociation(const QString &extension, const QString &typeDescription, const QString &friendlyName, const QString &iconIndex)
	{
	#ifdef Q_OS_WIN32
		QSettings regCR("HKEY_CLASSES_ROOT", QSettings::NativeFormat); // Read only on NT+
		QSettings regCU("HKEY_CURRENT_USER", QSettings::NativeFormat);

		if( QSysInfo::WindowsVersion < QSysInfo::WV_NT && !regCR.isWritable() ) // Win98
			return false;

		QString ext("." + extension);
		QString progID = makeProgID(extension);

		// Check if ProgID exists in the registry, otherwise create it.
		if( !hasProgID(progID) && !setProgID(progID, typeDescription, friendlyName, iconIndex) )
			return false;

		if( checkAssociation(extension) )
			return true;

		// Create the associations
		if( QSysInfo::WindowsVersion >= QSysInfo::WV_NT )
		{
			regCU.setValue("Software/Classes/"+ ext + REG_DEFAULT, progID); // Extension class

			// Explorer FileExt association
			QString fileExtsKey = QString("Software/Microsoft/Windows/CurrentVersion/Explorer/FileExts/") + ext;
			removeUserChoice(extension);
			regCU.remove(fileExtsKey +"/Application"); /// Windows XP
		} else
			regCR.setValue(ext + REG_DEFAULT, progID); //Windows ME/98/95 support

		regCU.sync();
		regCR.sync();
	//	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);// Refresh Explorer cache.

		return (regCU.status() == QSettings::NoError && regCR.status() == QSettings::NoError);
	#else
		Q_UNUSED(extension);
		Q_UNUSED(typeDescription);
		Q_UNUSED(friendlyName);
		Q_UNUSED(iconIndex);
		return false;
	#endif // Q_WS_WIN
	}

	bool clearAssociation(const QString &extension)
	{
	#ifdef Q_OS_WIN32
		if( !checkAssociation(extension) )
			return true;

		QString ext("."+ extension);
		QString progID = makeProgID(extension);
		QSettings regCU("HKEY_CURRENT_USER", QSettings::NativeFormat);
		QSettings regCR("HKEY_CLASSES_ROOT", QSettings::NativeFormat);

		if( QSysInfo::WindowsVersion < QSysInfo::WV_NT && !regCR.isWritable() ) // Win98
			return false;

		QString fileName = QFileInfo(qApp->applicationFilePath()).fileName();
		QString fileExtsKey = QString("Software/Microsoft/Windows/CurrentVersion/Explorer/FileExts/") + ext;
		/// Windows 7: "/UserChoice/Progid"; XP: "/Progid"
		QString curClassId = (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA)
				? regCU.value(fileExtsKey +"/UserChoice/Progid").toString()
				: regCU.value(fileExtsKey +"/Progid").toString();
		QString curAppId = regCU.value(fileExtsKey +"/Application").toString(); /// Windows XP

		if (!curClassId.isEmpty() && (
					(curClassId == progID)
					|| (0 == curClassId.compare(fileName, Qt::CaseInsensitive))
					|| (0 == curClassId.compare(QString("Applications\\%1").arg(fileName), Qt::CaseInsensitive))
					) )
		{
			removeUserChoice(extension);
		}

		if( !curAppId.isEmpty() && ( (curAppId == progID) || (0 == curAppId.compare(fileName, Qt::CaseInsensitive)) ) )
			regCU.remove(fileExtsKey +"/Application");

		if( QSysInfo::WindowsVersion >= QSysInfo::WV_NT )
		{
			if (regCU.value("Software/Classes/"+ ext + REG_DEFAULT).toString() == progID) // Only remove if we own it
			/// This is no recommend to delete in MSDN, case it may cause other problems, such as cannot create new text document if delete '.txt' group.
			//	regCU.remove("Software/Classes/"+ ext);
				regCU.remove("Software/Classes/"+ progID);
		} else {
			// Windows 98 ==> Write to HKCR
			if( regCR.value(ext + REG_DEFAULT).toString() == progID )
			//	regCR.remove(ext);
				regCR.remove(progID);
		}

		regCU.sync();
		regCR.sync();
	//	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);// Refresh Explorer cache.

		return (regCU.status() == QSettings::NoError && regCR.status() == QSettings::NoError);
	#else
		Q_UNUSED(extension);
		return false;
	#endif // Q_WS_WIN
	}

	void refreshExplorer()
	{
	#ifdef Q_OS_WIN32
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0); // Refresh Explorer cache.
	#endif // Q_WS_WIN
	}
}

#endif // FILEASSOC_H
