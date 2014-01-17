#include "audio_mac.h"

#include <QString>

#include <string>
#include <iostream>
#include <stdio.h>

std::string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

int getMasterVolume(long* vol) {
    QString str = QString::fromStdString(exec("osascript -e \"output volume of (get volume settings)\""));
    str.replace("\"", "");
    str.replace("\n", "");

    bool ok;
    *vol = str.toLong(&ok, 10);
    return ok;
}

void setMasterVolume(long* vol) {
    QString str = QString("osascript -e \"set volume output volume %1\"").arg(*vol);
    const char *c_str2 = str.toLocal8Bit().data();
//    char* cstr = str.toStdString().c_str();
    std::string s = exec(c_str2);
}
