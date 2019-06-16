// Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
//
// This file is part of Flameshot.
//
//     Flameshot is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     Flameshot is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with Flameshot.  If not, see <http://www.gnu.org/licenses/>.

#include "screenshotsaver.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "src/utils/systemnotification.h"
#include "src/utils/filenamehandler.h"
#include "src/utils/confighandler.h"
#include <QClipboard>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QImageWriter>

ScreenshotSaver::ScreenshotSaver() {
}

void ScreenshotSaver::saveToClipboard(const QPixmap &capture) {
    char tmp_file[] = "/tmp/clip.XXXXXX.png";
    mkstemps(tmp_file, 4);
    bool success = capture.save(QString(tmp_file), 0, -1);

    if (!success) {
        SystemNotification().sendMessage(
                QObject::tr("Saved to QT clipboard (Disappears after closing instance)"));
        QApplication::clipboard()->setPixmap(capture);
    } else {
        char save[100 + sizeof(tmp_file)];
        snprintf(save, sizeof(save),"cat %s | xclip -selection clipboard -target image/png -i", tmp_file);
        system(save);

        char rm[100 + sizeof(tmp_file)];
        snprintf(rm, sizeof(rm),"rm -f %s", tmp_file);
        system(rm);

        SystemNotification().sendMessage(
                QObject::tr("Saved to global clipboard"));
    }

}

bool ScreenshotSaver::saveToFilesystem(const QPixmap &capture,
                                       const QString &path)
{
    QString completePath = FileNameHandler().generateAbsolutePath(path);
    completePath += QLatin1String(".png");
    bool ok = capture.save(completePath);
    QString saveMessage;
    QString notificationPath = completePath;

    if (ok) {
        ConfigHandler().setSavePath(path);
        saveMessage = QObject::tr("Capture saved as ") + completePath;
    } else {
        saveMessage = QObject::tr("Error trying to save as ") + completePath;
        notificationPath = "";
    }

    SystemNotification().sendMessage(saveMessage, notificationPath);
    return ok;
}

bool ScreenshotSaver::saveToFilesystemGUI(const QPixmap &capture) {
    bool ok = false;

    while (!ok) {
        QString savePath = QFileDialog::getSaveFileName(
                    nullptr,
                    QString(),
                    FileNameHandler().absoluteSavePath() + ".png",
					QLatin1String("Portable Network Graphic file (PNG) (*.png);;BMP file (*.bmp);;JPEG file (*.jpg)"));

        if (savePath.isNull()) {
            break;
        }

	if (!savePath.endsWith(QLatin1String(".png"), Qt::CaseInsensitive) &&
	    !savePath.endsWith(QLatin1String(".bmp"), Qt::CaseInsensitive) &&
	    !savePath.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive)) {

	    savePath += QLatin1String(".png");
	}

        ok = capture.save(savePath);

        if (ok) {
            QString pathNoFile = savePath.left(savePath.lastIndexOf(QLatin1String("/")));
            ConfigHandler().setSavePath(pathNoFile);
            QString msg = QObject::tr("Capture saved as ") + savePath;
            SystemNotification().sendMessage(msg, savePath);
        } else {
            QString msg = QObject::tr("Error trying to save as ") + savePath;
            QMessageBox saveErrBox(
                        QMessageBox::Warning,
                        QObject::tr("Save Error"),
                        msg);
            saveErrBox.setWindowIcon(QIcon(":img/app/flameshot.svg"));
            saveErrBox.exec();
        }
    }
    return ok;
}
