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
#include <QJsonArray>
#include <QMediaPlayer>
#include <QUuid>
#include <QUrlQuery>

#include "Windows.h"
#include "WinUser.h"

AzerothOnTapeSettingsDialog::AzerothOnTapeSettingsDialog() :
    settings("DungFu", "Azeroth On Tape")
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    player = new QMediaPlayer;

    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    createGeneralGroupBox();

    createActions();
    createTrayIcon();

    connect(apiKeyEdit, &QLineEdit::textChanged, this, &AzerothOnTapeSettingsDialog::apiKeyChanged);
    connect(voiceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(voiceChanged(int)));
    connect(readHotkeyEdit, &QKeySequenceEdit::editingFinished, this, &AzerothOnTapeSettingsDialog::readHotkeyChanged);
    connect(speedSpinBox, SIGNAL(valueChanged(double)), this, SLOT(speedChanged(double)));
    connect(speedSlider, SIGNAL(valueChanged(int)), this, SLOT(speedChanged(int)));
    connect(trayIcon, &QSystemTrayIcon::activated, this, &AzerothOnTapeSettingsDialog::iconActivated);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(generalGroupBox);
    setLayout(mainLayout);

    trayIcon->setIcon(QIcon(":/images/quest.ico"));
    trayIcon->setToolTip(QString("Azeroth On Tape"));
    trayIcon->show();

    setWindowTitle(tr("Azeroth On Tape"));

    QUrl url("https://texttospeech.googleapis.com/v1/voices");
    QUrlQuery query;
    query.addQueryItem("key", settings.value("apiKey", "").toString());
    query.addQueryItem("languageCode", "en-US");
    url.setQuery(query);
    QJsonObject json;
    FileDownloader *voicesDownloader = new FileDownloader(RequestType::GET, url, json, this);
    connect(voicesDownloader, SIGNAL (downloaded()), this, SLOT (updateVoices()));
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

    QClipboard *clipboard = QApplication::clipboard();
    QString oldClipboardValue = clipboard->text();

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

    QString jsonString = clipboard->text();

    QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();

    clipboard->setText(oldClipboardValue);

    QUrl url("https://texttospeech.googleapis.com/v1/text:synthesize");
    QUrlQuery query;
    query.addQueryItem("key", settings.value("apiKey", "").toString());
    url.setQuery(query);
    QJsonObject input;
    input.insert("ssml", getSsmlString(jsonObject));
    QJsonObject voice;
    voice.insert("languageCode", settings.value("languageCode", DEFAULT_LANGUAGE_CODE).toString());
    voice.insert("name", settings.value("voiceName", DEFAULT_VOICE_NAME).toString());
    voice.insert("ssmlGender", settings.value("ssmlGender", DEFAULT_SSML_GENDER).toString());
    QJsonObject audioConfig;
    audioConfig.insert("audioEncoding", "MP3");
    audioConfig.insert("speakingRate", settings.value("speed", DEFAULT_SPEED).toDouble());
    QJsonObject data;
    data.insert("input", input);
    data.insert("voice", voice);
    data.insert("audioConfig", audioConfig);
    FileDownloader *fileToPlay = new FileDownloader(RequestType::POST, url, data, this);
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

void AzerothOnTapeSettingsDialog::mediaStatusChanged(QMediaPlayer::MediaStatus mediaStatus)
{
    if (mediaStatus == QMediaPlayer::MediaStatus::EndOfMedia) {
        QUrl url = player->media().canonicalUrl();
        if (url.isValid()) {
            QFile file(url.path());
            file.remove();
        }
    }
    if (readyForNewMedia(mediaStatus) && !filesToPlay.empty()) {
        QUrl fileToPlay = filesToPlay.dequeue();
        player->setMedia(fileToPlay);
        player->play();
    }
}

void AzerothOnTapeSettingsDialog::apiKeyChanged()
{
    settings.setValue("apiKey", apiKeyEdit->text());
}

void AzerothOnTapeSettingsDialog::voiceChanged(int voiceIndex)
{
    QString voiceName = voiceComboBox->currentText();
    if (voices.contains(voiceName)) {
        settings.setValue("voiceName", voiceName);
        Voice voice = voices.value(voiceName);
        settings.setValue("languageCode", voice.languageCode);
        settings.setValue("ssmlGender", voice.ssmlGender);
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

void AzerothOnTapeSettingsDialog::speedChanged(double speed)
{
    settings.setValue("speed", speed);
    speedSlider->setValue(round(speed * 10.0));
}

void AzerothOnTapeSettingsDialog::speedChanged(int speed)
{
    double realSpeed = speed / 10.0;
    settings.setValue("speed", realSpeed);
    speedSpinBox->setValue(realSpeed);
}

void AzerothOnTapeSettingsDialog::updateVoices()
{
    FileDownloader* voicesDownloader = qobject_cast<FileDownloader*>(sender());
    if (voicesDownloader == nullptr) {
        return;
    }
    QJsonDocument document = QJsonDocument::fromJson(voicesDownloader->downloadedData());
    QJsonObject rootObj = document.object();
    QJsonArray voicesJson = rootObj["voices"].toArray();
    QString currentVoiceName = settings.value("voiceName", DEFAULT_VOICE_NAME).toString();
    voices.clear();
    voiceComboBox->clear();
    foreach (const QJsonValue & value, voicesJson) {
        QJsonObject obj = value.toObject();
        QString languageCode = obj["languageCodes"].toArray().first().toString();
        QString voiceName = obj["name"].toString();
        QString ssmlGender = obj["ssmlGender"].toString();
        Voice voice = {
            languageCode,
            voiceName,
            ssmlGender
        };
        if (voiceName.contains("Wavenet")) {
            voices.insert(voiceName, voice);
            voiceComboBox->addItem(voiceName);
            if (voiceName == currentVoiceName) {
                voiceComboBox->setCurrentText(voiceName);
            }
        }
    }
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
    QString base64String = rootObj["audioContent"].toString();
    if (base64String.isEmpty()) {
        return;
    }
    ba.append(base64String);
    QByteArray data = QByteArray::fromBase64(ba);
    QUuid uuid = QUuid::createUuid();
    QString tempFileFullPath = QDir::toNativeSeparators(QDir::tempPath() + "/" + qApp->applicationName().replace(" ", "") + "_" + uuid.toString(QUuid::WithoutBraces) + ".mp3");
    QFile file(tempFileFullPath);
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
    delete fileDownloader;
    QUrl url = QUrl::fromLocalFile(tempFileFullPath);
    if (readyForNewMedia(player->mediaStatus())) {
        player->setMedia(url);
        player->play();
    } else {
        filesToPlay.enqueue(url);
    }
}

void AzerothOnTapeSettingsDialog::createGeneralGroupBox()
{
    generalGroupBox = new QGroupBox(tr("General Settings"));

    apiKeyLabel = new QLabel(tr("API Key:"));
    apiKeyEdit = new QLineEdit(settings.value("apiKey", "").toString());

    voiceLabel = new QLabel(tr("Voice:"));
    voiceComboBox = new QComboBox();

    readHotkeyLabel = new QLabel(tr("Read Text Hotkey:"));
    readHotkeyEdit = new QKeySequenceEdit(getReadHotkey());

    speedLabel = new QLabel(tr("Reading Speed:"));
    double speed = settings.value("speed", DEFAULT_SPEED).toDouble();
    speedSlider = new QSlider(Qt::Orientation::Horizontal);
    speedSpinBox = new QDoubleSpinBox;
    speedSpinBox->setRange(0.5, 2.0);
    speedSpinBox->setDecimals(1);
    speedSpinBox->setSingleStep(0.1);
    speedSpinBox->setValue(speed);
    speedSlider->setTickPosition(QSlider::TicksBothSides);
    speedSlider->setTickInterval(5);
    speedSlider->setMinimum(5);
    speedSlider->setMaximum(20);
    speedSlider->setSingleStep(1);
    speedSlider->setValue(speed * 10);

    auto messageLayout = new QGridLayout;

    messageLayout->addWidget(apiKeyLabel, 1, 0);
    messageLayout->addWidget(apiKeyEdit, 1, 1, 1, 4);

    messageLayout->addWidget(voiceLabel, 2, 0);
    messageLayout->addWidget(voiceComboBox, 2, 1, 1, 4);

    messageLayout->addWidget(readHotkeyLabel, 3, 0);
    messageLayout->addWidget(readHotkeyEdit, 3, 1, 1, 4);

    messageLayout->addWidget(speedLabel, 4, 0);
    messageLayout->addWidget(speedSpinBox, 4, 1);
    messageLayout->addWidget(speedSlider, 4, 2, 2, 4);

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
    QString gossipText = json["npcGossipText"].toString();

    if (!gossipText.isEmpty()) {
        return QString("<speak><p>" + escapeText(gossipText) + "</p></speak>");
    }

    QString questTitle = json["npcQuestTitle"].toString();
    QString npcProgressText = json["npcProgressText"].toString();

    if (!questTitle.isEmpty() && !npcProgressText.isEmpty()) {
        return QString(
            "<speak>" \
                "<emphasis level=\"moderate\">" + questTitle + ".</emphasis>" \
                "<p>" + escapeText(npcProgressText) + "</p>" \
            "</speak>");
    }

    QString questText = json["npcQuestText"].toString();
    QString questObjectives = json["npcQuestObjectives"].toString();

    if (questText.isEmpty()) {
        questTitle = json["questTitle"].toString();
        questText = json["questDescription"].toString();
        questObjectives = json["questObjectives"].toString();
    }

    if (questText.isEmpty()) {
        return QString();
    }
    return QString(
        "<speak>" \
            "<emphasis level=\"strong\">" + questTitle + ".</emphasis>" \
            "<p>" +  escapeText(questText) + "</p>" \
            "<emphasis level=\"moderate\">Quest Objectives.</emphasis><p>" + questObjectives + "</p>" \
        "</speak>");
}

bool AzerothOnTapeSettingsDialog::readyForNewMedia(QMediaPlayer::MediaStatus mediaStatus) {
    return mediaStatus == QMediaPlayer::MediaStatus::EndOfMedia
           || mediaStatus == QMediaPlayer::MediaStatus::NoMedia
           || mediaStatus == QMediaPlayer::MediaStatus::InvalidMedia;
}

QString AzerothOnTapeSettingsDialog::escapeText(QString text) {
    return text.replace("\n\n", "</p><p>")
               .replace("--", "<break strength=\"medium\"");
}

#endif
