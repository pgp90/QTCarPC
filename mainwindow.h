#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class QAbstractButton;
class QAbstractSlider;
//class QComboBox;
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QMediaPlaylist;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void open();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void metaDataChanged();
    void prevClicked();
    void nextClicked();
    void playClicked();
    void muteClicked();
    void volUpClicked();
    void volDownClicked();
//void playlistPositionChanged(int);
    void seek(int seconds);
    void statusChanged(QMediaPlayer::MediaStatus status);
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

//    QMediaPlayer::State playerState;
//    bool playerMuted;
    QAbstractButton *playPauseButton;
    QAbstractButton *nextButton;
    QAbstractButton *prevButton;
    QAbstractButton *muteButton;
    QAbstractButton *volUpButton;
    QAbstractButton *volDownButton;
    QAbstractButton *setMediaFolderButton;
    QAbstractSlider *volSlider;
    QAbstractSlider *playbackSlider;
    QLabel *titleLabel;
    QLabel *albumLabel;
    QLabel *timeLabel;
    QLabel *lengthLabel;
    QMediaPlayer *player;
    QMediaPlaylist *playlist;

    void setObdDataLabels(int page);
    void setObdValueLabels(int page);

    qint64 pos;

    QString timeToString(qint64 t);
    void addToPlaylist(QString filename);

    QLabel *obdDataLabel1;
    QLabel *obdValueLabel1;
    QLabel *obdDataLabel2;
    QLabel *obdValueLabel2;
    QLabel *obdDataLabel3;
    QLabel *obdValueLabel3;
    QLabel *obdDataLabel4;
    QLabel *obdValueLabel4;
    QLabel *obdDataLabel5;
    QLabel *obdValueLabel5;
    QLabel *obdDataLabel6;
    QLabel *obdValueLabel6;
};

#endif // MAINWINDOW_H
