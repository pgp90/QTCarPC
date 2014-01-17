#ifndef AUDIO_H
#define AUDIO_H

//#include "mainwindow.h"
//#include "ui_mainwindow.h"
//#include <QMediaService>
//#include <QMediaPlaylist>
//#include <QMediaMetaData>
//#include <QTime>
//#include <QFileInfo>
//#include <QDebug>
//#include <QDirIterator>


#if defined(Q_OS_WIN) || defined(OS_WIN)
#ifndef int8
#define int8 char
#define int16 short
#define int32 int
#define int64 long long

#define uint8 unsigned int8
#define uint16 unsigned int16
#define uint32 unsigned int32
#define uint64 unsigned int64
#endif

#ifndef u8
#define s8 int8
#define s16 int16
#define s32 int32
#define s64 int64

#define u8 uint8
#define u16 uint16
#define u32 uint32
#define u64 uint64
#endif

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);
char* GetMixerErrorDesc(MMRESULT Code);
#endif

int getMasterVolume(long* vol);
void setMasterVolume(long* vol);

#endif // AUDIO_H
