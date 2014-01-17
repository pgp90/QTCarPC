#include "audio.h"

#if defined(Q_OS_UNIX) || defined(OS_UNIX)
#include <unistd.h>
#include <fcntl.h>
#ifdef OSSCONTROL
#define MIXER_DEV "/dev/dsp"
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <stdio.h>
#else
#include <alsa/asoundlib.h>
#endif

int getMasterVolume(long* outvol) {
#ifdef OSSCONTROL
    int ret = 0;
    int fd, devs;

    if ((fd = open(MIXER_DEV, O_WRONLY)) > 0)
    {
        ioctl(fd, SOUND_MIXER_READ_VOLUME, outvol);
        close(fd);
        *outvol = *outvol & 0xff;
        return outvol;
    }
    return -1;
#else
    snd_mixer_t* handle;
    snd_mixer_elem_t* elem;
    snd_mixer_selem_id_t* sid;

    static const char* mix_name = "Master";
    static const char* card = "default";
    static int mix_index = 0;

    long pmin, pmax;
    long get_vol, set_vol;
    float f_multi;

    snd_mixer_selem_id_alloca(&sid);

    //sets simple-mixer index and name
    snd_mixer_selem_id_set_index(sid, mix_index);
    snd_mixer_selem_id_set_name(sid, mix_name);

        if ((snd_mixer_open(&handle, 0)) < 0)
        return -1;
    if ((snd_mixer_attach(handle, card)) < 0) {
        snd_mixer_close(handle);
        return -2;
    }
    if ((snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
        snd_mixer_close(handle);
        return -3;
    }
    ret = snd_mixer_load(handle);
    if (ret < 0) {
        snd_mixer_close(handle);
        return -4;
    }
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        snd_mixer_close(handle);
        return -5;
    }

    long minv, maxv;

    snd_mixer_selem_get_playback_volume_range (elem, &minv, &maxv);
    fprintf(stderr, "Volume range <%i,%i>\n", minv, maxv);

    if(snd_mixer_selem_get_playback_volume(elem, 0, outvol) < 0) {
        snd_mixer_close(handle);
        return -6;
    }

    fprintf(stderr, "Get volume %i with status %i\n", *outvol, ret);
    /* make the value bound to 100 */
    *outvol -= minv;
    maxv -= minv;
    minv = 0;
    *outvol = 100 * (*outvol) / maxv; // make the value bound from 0 to 100
    snd_mixer_close(handle);
    return 0;
#endif

}

void setMasterVolume(long* outvol) {
#ifdef OSSCONTROL
    int ret = 0;
    int fd, devs;

    if ((fd = open(MIXER_DEV, O_WRONLY)) > 0)
    {
        if(*outvol < 0 || *outvol > 100)
            return -2;
        *outvol = (*outvol << 8) | *outvol;
        ioctl(fd, SOUND_MIXER_WRITE_VOLUME, outvol);
        close(fd);
        return 0;
    }
    return -1;;
#else
    snd_mixer_t* handle;
    snd_mixer_elem_t* elem;
    snd_mixer_selem_id_t* sid;

    static const char* mix_name = "Master";
    static const char* card = "default";
    static int mix_index = 0;

    long pmin, pmax;
    long get_vol, set_vol;
    float f_multi;

    snd_mixer_selem_id_alloca(&sid);

    //sets simple-mixer index and name
    snd_mixer_selem_id_set_index(sid, mix_index);
    snd_mixer_selem_id_set_name(sid, mix_name);

        if ((snd_mixer_open(&handle, 0)) < 0)
        return -1;
    if ((snd_mixer_attach(handle, card)) < 0) {
        snd_mixer_close(handle);
        return -2;
    }
    if ((snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
        snd_mixer_close(handle);
        return -3;
    }
    ret = snd_mixer_load(handle);
    if (ret < 0) {
        snd_mixer_close(handle);
        return -4;
    }
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        snd_mixer_close(handle);
        return -5;
    }

    long minv, maxv;

    snd_mixer_selem_get_playback_volume_range (elem, &minv, &maxv);
    fprintf(stderr, "Volume range <%i,%i>\n", minv, maxv);

    if(*outvol < 0 || *outvol > VOLUME_BOUND) // out of bounds
        return -7;
    *outvol = (*outvol * (maxv - minv) / (100-1)) + minv;

    if(snd_mixer_selem_set_playback_volume(elem, 0, *outvol) < 0) {
        snd_mixer_close(handle);
        return -8;
    }
    if(snd_mixer_selem_set_playback_volume(elem, 1, *outvol) < 0) {
        snd_mixer_close(handle);
        return -9;
    }
    fprintf(stderr, "Set volume %i with status %i\n", *outvol, ret);

    snd_mixer_close(handle);
    return 0;
#endif

}

//int main(void)
//{
//    long vol = -1;
//    printf("Ret %i\n", getMasterVolume(&vol));
//    printf("Master volume is %i\n", vol);

//    vol = 100;
//    printf("Ret %i\n", setMasterVolume(&vol));

//    return 0;
//}
#elif defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_OSX) || defined(OS_MAC)

#include <string>
#include <iostream>
#include <stdio.h>

std::string exec(char* cmd) {
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
    exec(str.toStdString().c_str());
}



#elif defined(Q_OS_WIN) || defined(OS_WIN)

/*
Entry point
*/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   u32 NumDevices, NumControls;
   HMIXER hMixer = NULL;
   MMRESULT Result;
   MIXERLINE Line;
   MIXERCONTROL Control;
   MIXERLINECONTROLS LineControls;
   MIXERCONTROLDETAILS Details;
   MIXERCONTROLDETAILS_UNSIGNED Value;
   //MIXERCONTROLDETAILS_LISTTEXT Text;

   //Get # of devices
   NumDevices = mixerGetNumDevs();
   printf("# devices: %d\n", NumDevices);

   for(u32 DeviceID = 0; DeviceID < NumDevices; DeviceID++)
   {
      printf("Device #%u\n", DeviceID);

      //Open the device
      Result = mixerOpen(&hMixer, DeviceID, 0, 0, MIXER_OBJECTF_MIXER);
      if(Result == MMSYSERR_NOERROR)
         printf("Opened OK\n");
      else
      {
         printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
         break;
      }


      //Get the number of controls, channels, device name, etc
      Line.cbStruct = sizeof(MIXERLINE);
      Line.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
      Result = mixerGetLineInfo((HMIXEROBJ)hMixer, &Line, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
      if(Result == MMSYSERR_NOERROR)
         printf("Prod. Name: \"%s\"\nShort Name: \"%s\"\nFull Name: \"%s\"\n%d channels, %d controls\n", Line.Target.szPname, Line.szShortName, Line.szName, (u32)Line.cChannels, (u32)Line.cControls);
      else
      {
         printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
         mixerClose(hMixer);
         break;
      }

      //Get the value and text of each control
      NumControls = Line.cControls;
      for(u32 ControlID = 1; ControlID < NumControls + 1; ControlID++) //IDs are NOT zero-based
      {
         printf("Control #%u:\n", ControlID);

         Control.cbStruct = sizeof(MIXERCONTROL);
         LineControls.cbStruct = sizeof(MIXERLINECONTROLS);
         LineControls.dwControlID = ControlID;
         LineControls.cControls = 1;
         LineControls.cbmxctrl = sizeof(MIXERCONTROL);
         LineControls.pamxctrl = &Control;
         Result = mixerGetLineControls((HMIXEROBJ)hMixer, &LineControls, MIXER_GETLINECONTROLSF_ONEBYID);
         if(Result == MMSYSERR_NOERROR)
            printf("\tShort Name: \"%s\"\n\tFull Name: \"%s\"\n\tItems: %d\n\tRange: %u to %u\n\tSteps: %d\n", Control.szShortName, Control.szName, (u32)Control.cMultipleItems, (u32)Control.Bounds.dwMinimum, (u32)Control.Bounds.dwMaximum, (u32)Control.Metrics.cSteps);
         else
         {
            printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
            break;
         }


         //Get value
         Details.cbStruct = sizeof(MIXERCONTROLDETAILS);
         Details.dwControlID = ControlID;
         Details.cChannels = 1; //All channels at the same time
         Details.cMultipleItems = 0;
         Details.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
         Details.paDetails = &Value;
         Result = mixerGetControlDetails((HMIXEROBJ)hMixer, &Details, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
         if(Result == MMSYSERR_NOERROR)
            printf("\tValue: %u\n", (u32)Value.dwValue);
         else
         {
            printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
            break;
         }


         //Get text, doesn't work, dunno what it even actually does
         //Details.cbStruct = sizeof(MIXERCONTROLDETAILS);
         //Details.dwControlID = ControlID;
         //Details.cChannels = 1; //All channels at the same time
         //Details.cMultipleItems = Control.cMultipleItems;
         //Details.cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
         //Details.paDetails = &Text;
         //Result = mixerGetControlDetails((HMIXEROBJ)hMixer, &Details, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_LISTTEXT);
         //if(Result == MMSYSERR_NOERROR)
         //   printf("\tText: \"%s\"\n", Text.szName);
         //else
         //{
         //   printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
         //   break;
         //}
      }

      mixerClose(hMixer);
      printf("\n");
   }
   printf("Done\n");

   return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   u32 NumDevices, NumControls;
   HMIXER hMixer = NULL;
   MMRESULT Result;
   MIXERLINE Line;
   MIXERCONTROL Control;
   MIXERLINECONTROLS LineControls;
   MIXERCONTROLDETAILS Details;
   MIXERCONTROLDETAILS_UNSIGNED Value;
   //MIXERCONTROLDETAILS_LISTTEXT Text;

   //Get # of devices
   NumDevices = mixerGetNumDevs();
   printf("# devices: %d\n", NumDevices);

   for(u32 DeviceID = 0; DeviceID < NumDevices; DeviceID++)
   {
      printf("Device #%u\n", DeviceID);

      //Open the device
      Result = mixerOpen(&hMixer, DeviceID, 0, 0, MIXER_OBJECTF_MIXER);
      if(Result == MMSYSERR_NOERROR)
         printf("Opened OK\n");
      else
      {
         printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
         break;
      }


      //Get the number of controls, channels, device name, etc
      Line.cbStruct = sizeof(MIXERLINE);
      Line.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
      Result = mixerGetLineInfo((HMIXEROBJ)hMixer, &Line, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
      if(Result == MMSYSERR_NOERROR)
         printf("Prod. Name: \"%s\"\nShort Name: \"%s\"\nFull Name: \"%s\"\n%d channels, %d controls\n", Line.Target.szPname, Line.szShortName, Line.szName, (u32)Line.cChannels, (u32)Line.cControls);
      else
      {
         printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
         mixerClose(hMixer);
         break;
      }

      //Get the value and text of each control
      NumControls = Line.cControls;
      for(u32 ControlID = 1; ControlID < NumControls + 1; ControlID++) //IDs are NOT zero-based
      {
         printf("Control #%u:\n", ControlID);

         Control.cbStruct = sizeof(MIXERCONTROL);
         LineControls.cbStruct = sizeof(MIXERLINECONTROLS);
         LineControls.dwControlID = ControlID;
         LineControls.cControls = 1;
         LineControls.cbmxctrl = sizeof(MIXERCONTROL);
         LineControls.pamxctrl = &Control;
         Result = mixerGetLineControls((HMIXEROBJ)hMixer, &LineControls, MIXER_GETLINECONTROLSF_ONEBYID);
         if(Result == MMSYSERR_NOERROR)
            printf("\tShort Name: \"%s\"\n\tFull Name: \"%s\"\n\tItems: %d\n\tRange: %u to %u\n\tSteps: %d\n", Control.szShortName, Control.szName, (u32)Control.cMultipleItems, (u32)Control.Bounds.dwMinimum, (u32)Control.Bounds.dwMaximum, (u32)Control.Metrics.cSteps);
         else
         {
            printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
            break;
         }


         //Get value
         Details.cbStruct = sizeof(MIXERCONTROLDETAILS);
         Details.dwControlID = ControlID;
         Details.cChannels = 1; //All channels at the same time
         Details.cMultipleItems = 0;
         Details.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
         Details.paDetails = &Value;
         Result = mixerGetControlDetails((HMIXEROBJ)hMixer, &Details, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
         if(Result == MMSYSERR_NOERROR)
            printf("\tValue: %u\n", (u32)Value.dwValue);
         else
         {
            printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
            break;
         }


         //Get text, doesn't work, dunno what it even actually does
         //Details.cbStruct = sizeof(MIXERCONTROLDETAILS);
         //Details.dwControlID = ControlID;
         //Details.cChannels = 1; //All channels at the same time
         //Details.cMultipleItems = Control.cMultipleItems;
         //Details.cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
         //Details.paDetails = &Text;
         //Result = mixerGetControlDetails((HMIXEROBJ)hMixer, &Details, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_LISTTEXT);
         //if(Result == MMSYSERR_NOERROR)
         //   printf("\tText: \"%s\"\n", Text.szName);
         //else
         //{
         //   printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
         //   break;
         //}
      }

      mixerClose(hMixer);
      printf("\n");
   }
   printf("Done\n");

   return 0;
}


//Disable muting
         Details.cbStruct = sizeof(MIXERCONTROLDETAILS);
         Details.dwControlID = 2; //Master Mute
         Details.cChannels = 1;
         Details.cMultipleItems = 0;
         Details.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
         Details.paDetails = &Value;
         Value.dwValue = 0;
         Result = mixerSetControlDetails((HMIXEROBJ)hMixer, &Details, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
         if(Result != MMSYSERR_NOERROR)
         {
            printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
            break;
         }

         //Set volume level
         Details.cbStruct = sizeof(MIXERCONTROLDETAILS);
         Details.dwControlID = 1; //Master Volume
         Details.cChannels = 1;
         Details.cMultipleItems = 0;
         Details.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
         Details.paDetails = &Value;
         Value.dwValue = 5000;
         Result = mixerSetControlDetails((HMIXEROBJ)hMixer, &Details, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
         if(Result != MMSYSERR_NOERROR)
         {
            printf("%u Failed: %s\n", __LINE__, GetMixerErrorDesc(Result));
            break;
         }
char* GetMixerErrorDesc(MMRESULT Code)
{
   switch(Code)
   {
      case MMSYSERR_ALLOCATED:
         return "Already allocated by max # of clients";

      case MMSYSERR_BADDEVICEID:
         return "Invalid device ID";

      case MMSYSERR_INVALFLAG:
         return "Invalid flag";

      case MMSYSERR_INVALHANDLE:
         return "Invalid handle";

      case MMSYSERR_INVALPARAM:
         return "Invalid parameter";

      case MMSYSERR_NODRIVER:
         return "No device available";

      case MMSYSERR_NOMEM:
         return "Not enough memory";

      default:
         return "Unknown error";
   }
}
#endif
