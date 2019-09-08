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
#include <QDoubleSpinBox>
#include <QString>
#include <QQueue>
#include <QMediaPlayer>
#include <QBuffer>
#include <QComboBox>
#include <QSlider>

#include "FileDownloader.hpp"

struct Voice {
    QString languageCode;
    QString name;
    QString ssmlGender;
};

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
    void mediaStatusChanged(QMediaPlayer::MediaStatus mediaStatus);
    void apiKeyChanged();
    void voiceChanged(int voiceIndex);
    void readHotkeyChanged();
    void speedChanged(double speed);
    void speedChanged(int speed);
    void updateVoices();
    void playFile();

private:
    void createGeneralGroupBox();
    void createActions();
    void createTrayIcon();
    bool readyForNewMedia(QMediaPlayer::MediaStatus mediaStatus);
    QString getSsmlString(QJsonObject json);
    QString escapeText(QString text);

    QHash<QString, Voice> voices;
    QQueue<QUrl> filesToPlay;

    QSettings settings;
    QMediaPlayer *player;
    QBuffer *mediaStream;

    QGroupBox *generalGroupBox;
    QLabel *apiKeyLabel;
    QLineEdit *apiKeyEdit;
    QLabel *voiceLabel;
    QComboBox *voiceComboBox;
    QLabel *readHotkeyLabel;
    QKeySequenceEdit *readHotkeyEdit;
    QLabel *speedLabel;
    QDoubleSpinBox *speedSpinBox;
    QSlider *speedSlider;

    QAction *showHideAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    const QSize ICON_SIZE = QSize(32, 32);
    const QString DEFAULT_LANGUAGE_CODE = "en-US";
    const QString DEFAULT_VOICE_NAME = "en-AU-Wavenet-D";
    const QString DEFAULT_SSML_GENDER = "MALE";
    const QString DEFAULT_READ_HOTKEY = "Del";
    const double DEFAULT_SPEED = 1.0;
};

#endif // QT_NO_SYSTEMTRAYICON

#endif // AZEROTHONTAPESETTINGSDIALOG_H
