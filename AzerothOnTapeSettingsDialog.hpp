#ifndef AZEROTHONTAPESETTINGSDIALOG_H
#define AZEROTHONTAPESETTINGSDIALOG_H

#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QAction>
#include <QDialog>
#include <QGroupBox>
#include <QKeySequenceEdit>
#include <QHash>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QString>
#include <QQueue>
#include <QMediaPlayer>
#include <QBuffer>

#include "FileDownloader.hpp"

class AzerothOnTapeSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    AzerothOnTapeSettingsDialog();

    void setVisible(bool visible) override;
    QKeySequence getReadHotkey();

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void onReadHotkeyUpdated(QKeySequence readHotkey);

public slots:
    void readText();

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void toggleWindow();
    void readHotkeyChanged();
    void playFile();

private:
    void createGeneralGroupBox();
    void createActions();
    void createTrayIcon();
    QString getSsmlString(QJsonObject json);

    QSettings settings;
    FileDownloader *fileToPlay;
    QMediaPlayer *player;
    QBuffer *mediaStream;

    QGroupBox *generalGroupBox;
    QLabel *readHotkeyLabel;
    QKeySequenceEdit *readHotkeyEdit;

    QAction *showHideAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    const QSize ICON_SIZE = QSize(32, 32);
    const QString DEFAULT_READ_HOTKEY = "Del";
};

#endif // QT_NO_SYSTEMTRAYICON

#endif // AZEROTHONTAPESETTINGSDIALOG_H
