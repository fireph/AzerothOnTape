#include "AzerothOnTapeSettingsDialog.hpp"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QBuffer>
#include <QByteArray>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QApplication>
#include <QClipboard>
#include <QtCore/QDebug>
#include <QTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMediaPlayer>
#include <QUuid>

#include "Windows.h"
#include "WinUser.h"

AzerothOnTapeSettingsDialog::AzerothOnTapeSettingsDialog() :
    settings("DungFu", "Azeroth On Tape")
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    player = new QMediaPlayer;

    createGeneralGroupBox();

    createActions();
    createTrayIcon();

    connect(readHotkeyEdit, &QKeySequenceEdit::editingFinished, this, &AzerothOnTapeSettingsDialog::readHotkeyChanged);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &AzerothOnTapeSettingsDialog::iconActivated);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(generalGroupBox);
    setLayout(mainLayout);

    trayIcon->setIcon(QIcon(":/images/quest.ico"));
    trayIcon->setToolTip(QString("Azeroth On Tape"));
    trayIcon->show();

    setWindowTitle(tr("Azeroth On Tape"));
}

void AzerothOnTapeSettingsDialog::setVisible(bool visible)
{
    showHideAction->setText(visible ? tr("&Hide") : tr("&Show"));
    QDialog::setVisible(visible);
}

QKeySequence AzerothOnTapeSettingsDialog::getReadHotkey()
{
    return QKeySequence(settings.value("read_hotkey", DEFAULT_READ_HOTKEY).toString());
}

void AzerothOnTapeSettingsDialog::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void AzerothOnTapeSettingsDialog::readText()
{
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return;
    }
    char window_title[256];
    GetWindowText(foreground, (LPWSTR) window_title, 256);

    QString title = QString::fromWCharArray((const wchar_t *)window_title);

    if (title != QString("World of Warcraft")) {
        return;
    }

    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Press the "Shift" key
    ip.ki.wVk = VK_SHIFT;
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Press the "F11" key
    ip.ki.wVk = VK_F11;
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the "F11" key
    ip.ki.wVk = VK_F11;
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    // Release the "Shift" key
    ip.ki.wVk = VK_SHIFT;
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    QTime dieTime = QTime::currentTime().addMSecs(100);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    // Press the "Ctrl" key
    ip.ki.wVk = VK_CONTROL;
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Press the "C" key
    ip.ki.wVk = 'C';
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the "C" key
    ip.ki.wVk = 'C';
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    // Release the "Ctrl" key
    ip.ki.wVk = VK_CONTROL;
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    QTime dieTime2 = QTime::currentTime().addMSecs(100);
    while (QTime::currentTime() < dieTime2)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    QClipboard *clipboard = QApplication::clipboard();
    QString originalText = clipboard->text();

    QJsonDocument jsonResponse = QJsonDocument::fromJson(originalText.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();

    QUrl imageUrl("https://texttospeech.googleapis.com/v1/text:synthesize?key=AIzaSyBgmpvJgC76q--16On0_Bc54FQmBi42X04");
    QJsonObject input;
    input.insert("ssml", getSsmlString(jsonObject));
    QJsonObject voice;
    voice.insert("languageCode", "en-au");
    voice.insert("name", "en-AU-Wavenet-D");
    voice.insert("ssmlGender", "MALE");
    QJsonObject audioConfig;
    audioConfig.insert("audioEncoding", "MP3");
    audioConfig.insert("speakingRate", 1.2);
    QJsonObject data;
    data.insert("input", input);
    data.insert("voice", voice);
    data.insert("audioConfig", audioConfig);
    fileToPlay = new FileDownloader(imageUrl, data, this);
    connect(fileToPlay, SIGNAL (downloaded()), this, SLOT (playFile()));

    // Press the "Escape" key
    ip.ki.wVk = VK_ESCAPE;
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the "Escape" key
    ip.ki.wVk = VK_ESCAPE;
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}

void AzerothOnTapeSettingsDialog::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        toggleWindow();
        break;
    default:
        ;
    }
}

void AzerothOnTapeSettingsDialog::toggleWindow()
{
    setVisible(!isVisible());
    if (isVisible()) {
        raise();
        activateWindow();
    }
}

void AzerothOnTapeSettingsDialog::readHotkeyChanged()
{
    int value = readHotkeyEdit->keySequence()[0];
    QKeySequence shortcut(value);
    readHotkeyEdit->setKeySequence(shortcut);
    settings.setValue("read_hotkey", shortcut.toString());
    emit onReadHotkeyUpdated(shortcut);
    readHotkeyEdit->clearFocus();
}

void AzerothOnTapeSettingsDialog::playFile()
{
    FileDownloader* fileDownloader = qobject_cast<FileDownloader*>(sender());
    if (fileDownloader == nullptr) {
        return;
    }
    QJsonDocument document = QJsonDocument::fromJson(fileDownloader->downloadedData());
    QJsonObject rootObj = document.object();
    QByteArray ba;
    ba.append(rootObj["audioContent"].toString());
    QByteArray data = QByteArray::fromBase64(ba);
    QUuid uuid = QUuid::createUuid();
    QString tempFileFullPath = QDir::toNativeSeparators(QDir::tempPath() + "/" + qApp->applicationName().replace(" ", "") + "_" + uuid.toString(QUuid::WithoutBraces) + ".mp3");
    QFile file(tempFileFullPath);
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
    delete fileDownloader;
    player->setMedia(QUrl::fromLocalFile(tempFileFullPath));
    player->play();
}

void AzerothOnTapeSettingsDialog::createGeneralGroupBox()
{
    generalGroupBox = new QGroupBox(tr("General Settings"));

    readHotkeyLabel = new QLabel(tr("Read Text Hotkey:"));
    readHotkeyEdit = new QKeySequenceEdit(getReadHotkey());

    auto messageLayout = new QGridLayout;

    messageLayout->addWidget(readHotkeyLabel, 1, 0);
    messageLayout->addWidget(readHotkeyEdit, 1, 1, 1, 4);

    generalGroupBox->setLayout(messageLayout);
}

void AzerothOnTapeSettingsDialog::createActions()
{
    showHideAction = new QAction(tr("&Show"), this);
    connect(showHideAction, &QAction::triggered, this, &AzerothOnTapeSettingsDialog::toggleWindow);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void AzerothOnTapeSettingsDialog::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showHideAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

QString AzerothOnTapeSettingsDialog::getSsmlString(QJsonObject json)
{
    QString questTitle = json["npcQuestTitle"].toString();
    QString questText = json["npcQuestText"].toString();
    QString questObjectives = json["npcQuestObjectives"].toString();

    if (questTitle.isEmpty()) {
        questTitle = json["questTitle"].toString();
        questText = json["questDescription"].toString();
        questObjectives = json["questObjectives"].toString();
    }

    if (questTitle.isEmpty()) {
        QString gossipText = json["npcGossipText"].toString();
        return QString("<speak><p>" + gossipText.replace("\n\n", "</p><p>") + "</p></speak>");
    }
    return QString(
        "<speak>" \
            "<emphasis level=\"moderate\">" + questTitle + ".</emphasis>" \
            "<p>" +  questText.replace("\n\n", "</p><p>") + "</p>" \
            "<emphasis level=\"moderate\">Objectives.</emphasis><p>" + questObjectives + "</p>" \
        "</speak>");
}

#endif
