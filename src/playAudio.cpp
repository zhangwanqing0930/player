#include "ffmpegUtil.h"
#include "MediaProcessor.hpp"

extern "C" {
#include "SDL.h"
};

using std::cout;
using std::endl;

void sdlAudioCallback(void* userdata, Uint8* stream, int len) {
    AudioProcessor* receiver = (AudioProcessor*)userdata;
    receiver->writeAudioData(stream, len);
}

void playSdlAudio(SDL_AudioDeviceID& audioDeviceID, AudioProcessor& aProcessor) {
    //--------------------- GET SDL audio READY -------------------

    // audio specs containers
    SDL_AudioSpec wanted_specs;
    SDL_AudioSpec specs;

    cout << "aProcessor.getSampleFormat() = " << aProcessor.getSampleFormat() << endl;
    cout << "aProcessor.getSampleRate() = " << aProcessor.getOutSampleRate() << endl;
    cout << "aProcessor.getChannels() = " << aProcessor.getOutChannels() << endl;
    cout << "++" << endl;

    int samples = -1;
    while (true) {
        cout << "getting audio samples." << endl;
        samples = aProcessor.getSamples();
        if (samples <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else {
            cout << "get audio samples:" << samples << endl;
            break;
        }
    }

    // set audio settings from codec info
    wanted_specs.freq = aProcessor.getOutSampleRate();
    wanted_specs.format = AUDIO_S16SYS;
    wanted_specs.channels = aProcessor.getOutChannels();
    wanted_specs.samples = samples;
    wanted_specs.callback = sdlAudioCallback;
    wanted_specs.userdata = &aProcessor;

    // open audio device
    audioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &wanted_specs, &specs, 0);

    // SDL_OpenAudioDevice returns a valid device ID that is > 0 on success or 0 on failure
    if (audioDeviceID == 0) {
        string errMsg = "Failed to open audio device:";
        errMsg += SDL_GetError();
        cout << errMsg << endl;
        throw std::runtime_error(errMsg);
    }

    cout << "wanted_specs.freq:" << wanted_specs.freq << endl;
    // cout << "wanted_specs.format:" << wanted_specs.format << endl;
    std::printf("wanted_specs.format: Ox%X\n", wanted_specs.format);
    cout << "wanted_specs.channels:" << (int)wanted_specs.channels << endl;
    cout << "wanted_specs.samples:" << (int)wanted_specs.samples << endl;

    cout << "------------------------------------------------" << endl;

    cout << "specs.freq:" << specs.freq << endl;
    // cout << "specs.format:" << specs.format << endl;
    std::printf("specs.format: Ox%X\n", specs.format);
    cout << "specs.channels:" << (int)specs.channels << endl;
    cout << "specs.silence:" << (int)specs.silence << endl;
    cout << "specs.samples:" << (int)specs.samples << endl;


    SDL_PauseAudioDevice(audioDeviceID, 0);
    cout << "[THREAD] audio start thread finish." << endl;
}



