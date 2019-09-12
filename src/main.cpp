#include <QApplication>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QHotkey>
#include <QMessageBox>
#include <QtCore/QObject>

#include "AzerothOnTapeSettingsDialog.hpp"

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(azerothontape);

    QApplication app(argc, argv);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("Azeroth On Tape"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    auto settingsDialog = new AzerothOnTapeSettingsDialog();

    auto hotkey = new QHotkey(settingsDialog->getReadHotkey(), true, &app);
    QObject::connect(hotkey, &QHotkey::activated, settingsDialog, &AzerothOnTapeSettingsDialog::readText,
                     Qt::DirectConnection);
    QObject::connect(settingsDialog,
                     QOverload<QKeySequence>::of(&AzerothOnTapeSettingsDialog::onReadHotkeyUpdated),
                     [&](QKeySequence readHotkey) {
                         hotkey->setShortcut(readHotkey, true);
                     });

    return QApplication::exec();
}

#else

#include <QDebug>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QString text("QSystemTrayIcon is not supported on this platform");

    QLabel *label = new QLabel(text);
    label->setWordWrap(true);

    label->show();
    qDebug() << text;
    app.exec();
}

#endif
