// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "screenshotsaver.h"
#include "src/core/controller.h"
#include "src/utils/confighandler.h"
#include "src/utils/filenamehandler.h"
#include "src/utils/systemnotification.h"
#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QFileDialog>
#include <QImageWriter>
#include <QMessageBox>
#include <QMimeData>
#if defined(Q_OS_MACOS)
#include "src/widgets/capture/capturewidget.h"
#endif
#include <fstream>
ScreenshotSaver::ScreenshotSaver()
  : m_id(0)
{}

ScreenshotSaver::ScreenshotSaver(const unsigned id)
  : m_id(id)
{}

// TODO: If data is saved to the clipboard before the notification is sent via
// dbus, the application freezes.
void ScreenshotSaver::saveToClipboard(const QPixmap& capture)
{
    // If we are able to properly save the file, save the file and copy to
    // clipboard.
    if ((ConfigHandler().saveAfterCopyValue()) &&
        (!ConfigHandler().savePath().isEmpty())) {
        saveToFilesystem(capture,
                         ConfigHandler().savePath(),
                         QObject::tr("Capture saved to clipboard."));
        QApplication::clipboard()->setPixmap(capture);
    }
    // Otherwise only save to clipboard
    else {
        if (ConfigHandler().useJpgForClipboard()) {
            // FIXME - it doesn't work on MacOS
            QByteArray array;
            QBuffer buffer{ &array };
            QImageWriter imageWriter{ &buffer, "JPEG" };
            imageWriter.write(capture.toImage());

            QPixmap jpgPixmap;
            bool isLoaded = jpgPixmap.loadFromData(
              reinterpret_cast<uchar*>(array.data()), array.size(), "JPEG");
            if (isLoaded) {
                // Need to send message before copying to clipboard
                SystemNotification().sendMessage(
                  QObject::tr("Capture saved to clipboard"));

                QMimeData* mimeData = new QMimeData;
                mimeData->setData("image/jpeg", array);
                QApplication::clipboard()->setMimeData(mimeData);
            } else {
                SystemNotification().sendMessage(
                  QObject::tr("Error while saving to clipboard"));
                return;
            }
        } else {
            // Need to send message before copying to clipboard
            SystemNotification().sendMessage(
              QObject::tr("Capture saved to clipboard"));
            QApplication::clipboard()->setPixmap(capture);
        }
    }
}

bool ScreenshotSaver::saveToFilesystem(const QPixmap& capture,
                                       const QString& path,
                                       const QString& messagePrefix)
{
    QString completePath = FileNameHandler().generateAbsolutePath(path);
    completePath += QLatin1String(".png");
    bool ok = capture.save(completePath);
    QString saveMessage = messagePrefix;
    QString notificationPath = completePath;
    if (!saveMessage.isEmpty()) {
        saveMessage += " ";
    }

    if (ok) {
        ConfigHandler().setSavePath(path);
        saveMessage += QObject::tr("Capture saved as ") + completePath;
        Controller::getInstance()->sendCaptureSaved(
          m_id, QFileInfo(completePath).canonicalFilePath());
    } else {
        saveMessage += QObject::tr("Error trying to save as ") + completePath;
        notificationPath = "";
    }

    SystemNotification().sendMessage(saveMessage, notificationPath);
    return ok;
}


QString ShowSaveFileDialog(QWidget *parent,
                           const QString &title,
                           const QString &directory,
                           const QString &filter) {
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
  return QFileDialog::getSaveFileName(parent,
                                      title,
                                      directory,
                                      filter);
#else

  QFileDialog dialog(parent, title, directory, filter);
  if (parent) {
    dialog.setWindowModality(Qt::WindowModal);
  }

  QRegExp filter_regex(QLatin1String("(?:^\\*\\.(?!.*\\()|\\(\\*\\.)(\\w+)"));
  QStringList filters = filter.split(QLatin1String(";;"));

  dialog.setAcceptMode(QFileDialog::AcceptSave);


  if (dialog.exec() == QDialog::Accepted) {

    QString file_name = dialog.selectedFiles().first();
    QFileInfo info(file_name);
    if (info.suffix().isEmpty()){
		if ( !dialog.selectedNameFilter().isEmpty()) {
			  if (filter_regex.indexIn(dialog.selectedNameFilter()) != -1) {
				QString extension = filter_regex.cap(1);
				file_name += QLatin1String(".") + extension;
			  }
		}
    }

    return file_name;
  } else {

    return QString();
  }
#endif  // Q_WS_MAC || Q_WS_WIN
}


bool ScreenshotSaver::saveToFilesystemGUI(const QPixmap& capture)
{
    bool ok = false;
    ConfigHandler config;
    QString savePath = FileNameHandler().absoluteSavePath();
#if defined(Q_OS_MACOS)
    for (QWidget* widget : qApp->topLevelWidgets()) {
        QString className(widget->metaObject()->className());
        if (0 ==
            className.compare(CaptureWidget::staticMetaObject.className())) {
            widget->showNormal();
            widget->hide();
            break;
        }
    }
#endif
    if (!config.savePathFixed()) {
//        savePath = QFileDialog::getSaveFileName(
//          nullptr,
//          QObject::tr("Save screenshot"),
//          FileNameHandler().absoluteSavePath(),
//          QLatin1String("Portable Network Graphic file (PNG) (*.png);;BMP "
//                        "file (*.bmp);;JPEG file (*.jpg);;By extension"), &selectedFilter);


    	savePath = ShowSaveFileDialog(nullptr,  QObject::tr("Save screenshot"), FileNameHandler().absoluteSavePath(),QLatin1String("Portable Network Graphic file (PNG) (*.png);;BMP "
    			                        "file (*.bmp);;JPEG file (*.jpg);;By extension"));

    }
    if (!savePath.endsWith(QLatin1String(".png"), Qt::CaseInsensitive) &&
        !savePath.endsWith(QLatin1String(".bmp"), Qt::CaseInsensitive) &&
        !savePath.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive)) {
        savePath += QLatin1String(".png");
    }
    ok = capture.save(savePath);

    if (ok) {
        QString pathNoFile =
          savePath.left(savePath.lastIndexOf(QLatin1String("/")));
        ConfigHandler().setSavePath(pathNoFile);
        QString msg = QObject::tr("Capture saved as ") + savePath;
        if (config.copyPathAfterSaveEnabled()) {
            QApplication::clipboard()->setText(savePath);
            msg =
              QObject::tr("Capture is saved and copied to the clipboard as ") +
              savePath;
        }
        SystemNotification().sendMessage(msg, savePath);
        Controller::getInstance()->sendCaptureSaved(
          m_id, QFileInfo(savePath).canonicalFilePath());
    } else {
        QString msg = QObject::tr("Error trying to save as ") + savePath;
        QMessageBox saveErrBox(
          QMessageBox::Warning, QObject::tr("Save Error"), msg);
        saveErrBox.setWindowIcon(QIcon(":img/app/flameshot.svg"));
        saveErrBox.exec();
    }

    return ok;
}
