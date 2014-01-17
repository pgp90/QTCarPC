#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaService>
#include <QMediaPlaylist>
#include <QMediaMetaData>
#include <QTime>
#include <QFileInfo>
#include <QDebug>
#include <QDirIterator>
#include <QFileDialog>
//#include <QtSerialPort/QSerialPort>

#if defined(Q_OS_WIN)
#include "audio_win.h"
#elif defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_OSX)
#include "audio_mac.h"
#elif defined(Q_OS_UNIX)
#include "audio_linux.h"
#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    playPauseButton = ui->playPauseButton;
    nextButton = ui->nextButton;
    prevButton = ui->prevButton;
    muteButton = ui->muteButton;
    volUpButton = ui->volUpButton;
    volDownButton = ui->volDownButton;
    setMediaFolderButton = ui->setMediaFolderButton;
    volSlider = ui->volSlider;
    playbackSlider = ui->playbackSlider;
    titleLabel = ui->titleLabel;
    albumLabel = ui->albumLabel;
    timeLabel = ui->timeLabel;
    lengthLabel = ui->lengthLabel;

    obdDataLabel1 = ui->obdDataLabel1;
    obdValueLabel1 = ui->obdValueLabel1;
    obdDataLabel2 = ui->obdDataLabel2;
    obdValueLabel2 = ui->obdValueLabel2;
    obdDataLabel3 = ui->obdDataLabel3;
    obdValueLabel3 = ui->obdValueLabel3;
    obdDataLabel4 = ui->obdDataLabel4;
    obdValueLabel4 = ui->obdValueLabel4;
    obdDataLabel5 = ui->obdDataLabel5;
    obdValueLabel5 = ui->obdValueLabel5;
    obdDataLabel6 = ui->obdDataLabel6;
    obdValueLabel6 = ui->obdValueLabel6;


    player = new QMediaPlayer(this);
    // owned by PlaylistModel
    playlist = new QMediaPlaylist();
    player->setPlaylist(playlist);

    connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
//    connect(playlist, SIGNAL(currentIndexChanged(int)), SLOT(playlistPositionChanged(int)));
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
//    connect(player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));
//    connect(player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));

    //connect control button signals
    connect(playPauseButton, SIGNAL(clicked()), this, SLOT(playClicked()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(prevClicked()));
    connect(muteButton, SIGNAL(clicked()), this, SLOT(muteClicked()));
    connect(volUpButton, SIGNAL(clicked()), this, SLOT(volUpClicked()));
    connect(volDownButton, SIGNAL(clicked()), this, SLOT(volDownClicked()));
    connect(volSlider, SIGNAL(sliderMoved(int)), this, SLOT(setVolume(int)));
    connect(volSlider, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
    connect(playbackSlider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
    connect(playbackSlider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
//    connect(muteButton, SIGNAL(clicked()), this, SLOT(muteClicked()));
    connect(setMediaFolderButton, SIGNAL(clicked()), this, SLOT(open()));

    timeLabel->setText(timeToString(0));
    lengthLabel->setText(timeToString(0));

    playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    prevButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    volUpButton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    volDownButton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
//    volSlider->setValue(player->volume());
    long vol = -1;
    getMasterVolume(&vol);
    volSlider->setValue(vol);


qDebug()<<"loading files";
    addToPlaylist("/Users/jpalnick/Music/iTunes/iTunes Media/Music/Daft Punk");
qDebug()<<"done loading files";

//    addToPlaylist("/Users/jpalnick/Music/iTunes/iTunes Media/Music/Daft Punk/Human After All/Technologic.mp3");
//    addToPlaylist("/Users/jpalnick/Music/iTunes/iTunes Media/Music/Daft Punk/Human After All/01 Human After All.mp3");
    playlist->setPlaybackMode(QMediaPlaylist::Random);
    playlist->next();

    setObdDataLabels(1);
    setObdValueLabels(1);

    ui->mediaGroupBox->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setObdDataLabels(int page) {
    switch (page) {
    case 1:
        obdDataLabel1->setText("Coolant Temp");
        obdDataLabel2->setText("Timing Adv.");
        obdDataLabel3->setText("Long-Term Fuel Trim");
        obdDataLabel4->setText("Short-Term Fuel Trim");
        obdDataLabel5->setText("MAF");
        obdDataLabel6->setText("RPM");
        break;
    default:
        break;
    }
}

void parseResult1(QString str, int* a) {
    bool ok;
    str.replace(" ", "");
    QString str2 = str.right(1*2);
    *a = str2.toInt(&ok, 16);
}

void parseResult2(QString str, int* a, int* b) {
    bool ok;
    str.replace(" ", "");
    QString str2 = str.right(1*2);
    *a = str2.mid(0, 2).toInt(&ok, 16);
    *b = str2.mid(2, 2).toInt(&ok, 16);
}

void parseResult3(QString str, int* a, int* b, int* c) {
    bool ok;
    str.replace(" ", "");
    QString str2 = str.right(1*2);
    *a = str2.mid(0, 2).toInt(&ok, 16);
    *b = str2.mid(2, 2).toInt(&ok, 16);
    *c = str2.mid(4, 2).toInt(&ok, 16);
}

void parseResult4(QString str, int* a, int* b, int* c, int* d) {
    bool ok;
    str.replace(" ", "");
    QString str2 = str.right(1*2);
    *a = str2.mid(0, 2).toInt(&ok, 16);
    *b = str2.mid(2, 2).toInt(&ok, 16);
    *c = str2.mid(4, 2).toInt(&ok, 16);
    *d = str2.mid(6, 2).toInt(&ok, 16);
}

QString makeRequest(QString req) {
//    QByteArray readData;
//    serial.writeData(str.toLocal8Bit());
//    serial.readLine(readData);
//    QString out = QString(readData);
//    return out;
    return "";
}

void MainWindow::setObdValueLabels(int page) {
//    QSerialPort serial;

    QString str1;
    QString str2;
    QString str3;
    QString str4;
    QString str5;
    QString str6;

    int value1A;
    int value1B;
    int value1C;
    int value1D;
    int value2A;
    int value2B;
    int value2C;
    int value2D;
    int value3A;
    int value3B;
    int value3C;
    int value3D;
    int value4A;
    int value4B;
    int value4C;
    int value4D;
    int value5A;
    int value5B;
    int value5C;
    int value5D;
    int value6A;
    int value6B;
    int value6C;
    int value6D;

    switch (page) {
    default:
        break;
    case 1:
        str1 = "41 05 5E";
        str2 = "41 0E 80";
        str3 = "41 06 83";
        str4 = "41 07 7D";
        str5 = "41 10 9C 40";
        str6 = "41 0C 4D 20";

    //    str1 = makeRequest("0105");
    //    str2 = makeRequest("010E");
    //    str3 = makeRequest("0106");
    //    str4 = makeRequest("0107");
    //    str5 = makeRequest("0110");
    //    str6 = makeRequest("010C");

        parseResult1(str1, &value1A);
        parseResult1(str2, &value2A);
        parseResult1(str3, &value3A);
        parseResult1(str4, &value4A);
        parseResult2(str5, &value5A, &value5B);
        parseResult2(str6, &value6A, &value6B);

        int value1 = value1A - 40;
        float value2 = ((float)(value2A - 128)) / 2.0;
        float value3 = ((float)value3A-128.0) * 100.0/128.0;
        float value4 = ((float)value4A-128.0) * 100.0/128.0;
        float value5 = ((((float)value5A)*256.0)+((float)value5B)) / 100.0;
        float value6 = ((((float)value6A)*256.0)+((float)value6B)) / 4.0;

        obdValueLabel1->setText(QString("%1 ºF").arg(value1));
        obdValueLabel2->setText(QString("%1 º").arg(value2));
        obdValueLabel3->setText(QString("%1 \%").arg(value3));
        obdValueLabel4->setText(QString("%1 \%").arg(value4));
        obdValueLabel5->setText(QString("%1").arg(value5));
        obdValueLabel6->setText(QString("%1").arg(value6));
        break;
    }
}

void MainWindow::open()
{
    qDebug()<<"open()";
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Files"));

//    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Files"));
//    foreach (QString const &fileName, fileNames) {
        addToPlaylist(dir);
//    }
}

void MainWindow::durationChanged(qint64 duration) {
    lengthLabel->setText(timeToString(duration));
    playbackSlider->setMaximum(duration);
}

void MainWindow::positionChanged(qint64 progress) {
//    if (!slider->isSliderDown()) {
//        slider->setValue(progress);
//    }

    timeLabel->setText(timeToString(progress));
    pos = progress / 1000;
    if (!playbackSlider->isSliderDown()) {
        playbackSlider->setValue(progress);
    }
}

void MainWindow::metaDataChanged() {
    qDebug()<<"metaDataChanged()";
    if (player->isMetaDataAvailable()) {
        titleLabel->setText(player->metaData(QMediaMetaData::Title).toString());
        albumLabel->setText(player->metaData(QMediaMetaData::AlbumArtist).toString());
//        setTrackInfo(QString("%1 - %2")
//                .arg(player->metaData(QMediaMetaData::AlbumArtist).toString())
//                .arg(player->metaData(QMediaMetaData::Title).toString()));

//        if (coverLabel) {
//            QUrl url = player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

//            coverLabel->setPixmap(!url.isEmpty()
//                    ? QPixmap(url.toString())
//                    : QPixmap());
//        }
    } else {
        titleLabel->setText("-");
        albumLabel->setText("-");
    }
}

void MainWindow::playClicked() {
    qDebug()<<"playClicked()";
    switch (player->state()) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        player->play();
        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    case QMediaPlayer::PlayingState:
        player->pause();
        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void MainWindow::nextClicked() {
    qDebug()<<"nextClicked()";
    playlist->next();
}

void MainWindow::prevClicked() {
    qDebug()<<"prevClicked()";
    if(player->position() <= 5000)
        playlist->previous();
    else
        player->setPosition(0);

}

void MainWindow::muteClicked() {
    qDebug()<<"muteClicked()";
}

void MainWindow::volUpClicked() {
    qDebug()<<"volUpClicked()";
    long vol = -1;
    getMasterVolume(&vol);
    vol += 10;
    if (vol > 100)
        vol = 100;
    setMasterVolume(&vol);
    volSlider->setValue(vol);

//    QString str = QString::fromStdString(exec("osascript -e \"output volume of (get volume settings)\""));
//    str.replace("\"", "");
//    str.replace("\n", "");

//    qDebug()<<str;
}

void MainWindow::volDownClicked() {
    qDebug()<<"volDownClicked()";
    long vol = -1;
    getMasterVolume(&vol);
    vol -= 10;
    if (vol < 0)
        vol = 0;
    setMasterVolume(&vol);
    volSlider->setValue(vol);
//    qDebug()<<QString::fromStdString(exec("osascript -e \"output volume of (get volume settings)\""));
}

void MainWindow::seek(int seconds) {
    if (seconds/1000 != pos) {
        qDebug()<<"seek()";
        player->setPosition(seconds);
    }
}

//void MainWindow::playlistPositionChanged(int currentItem) {
//    qDebug()<<"seek()";
//}

void MainWindow::statusChanged(QMediaPlayer::MediaStatus status) {
    qDebug()<<"statusChanged()";
    switch (status) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
//        play();
        player->play();
        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    case QMediaPlayer::PlayingState:
//        pause();
        player->pause();
        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    default:
        break;
    }
}

void MainWindow::setState(QMediaPlayer::State state) {
    qDebug()<<"setState()";
}


void MainWindow::setVolume(int volume) {
    qDebug()<<"setVolume()";
    long vol = volume;
    if (vol < 0)
        vol = 0;
    else if (vol > 100)
        vol = 100;
    setMasterVolume(&vol);
    if (!volSlider->isSliderDown()) {
        volSlider->setValue(vol);
    }
}

//qint32 MainWindow::getVolume() {
//    qDebug()<<"getVolume()";
//}

void MainWindow::setMuted(bool muted) {
    qDebug()<<"setMuted()";
}

QString MainWindow::timeToString(qint64 t) {
    QTime currentTime((t/3600000)%60, (t/60000)%60, (t/1000)%60, t%1000);
    QString format = "mm:ss";
    QString out = currentTime.toString(format);
    return out;
}

void MainWindow::addToPlaylist(QString filename) {
    qDebug()<<filename;
    QFileInfo fileInfo(filename);
    if (fileInfo.exists()) {
        if (fileInfo.isFile()) {
            QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
            if (fileInfo.suffix().toLower() == QLatin1String("m3u")) {
                playlist->load(url);
            } else
                playlist->addMedia(url);
        } else {
            QDirIterator it(filename, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                QString str = it.next();
                if (!str.contains("/.", Qt::CaseInsensitive)) {
                    addToPlaylist(str);
                }
            }
        }
    } else {
        QUrl url(filename);
        if (url.isValid()) {
            playlist->addMedia(url);
        }
    }
}

