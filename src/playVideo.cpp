#include "ffmpegUtil.h"
#include "MediaProcessor.hpp"

extern "C" {
#include "SDL.h"
};

using std::cout;
using std::endl;

// SDL events
#define REFRESH_EVENT (SDL_USEREVENT + 1)
#define BREAK_EVENT (SDL_USEREVENT + 3)
#define VIDEO_FINISH (SDL_USEREVENT + 4)

void refresh(int timeInterval, bool& exitRefresh, bool& faster) {
    while (!exitRefresh) {
        SDL_Event event;
        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
        if (faster) {
            std::this_thread::sleep_for(std::chrono::milliseconds(timeInterval / 2));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(timeInterval));
        }
    }
    cout << "[THREAD] picRefresher thread finished." << endl;
}

void playSdlVideo(VideoProcessor& vProcessor, AudioProcessor* audio = nullptr) {

    auto width = vProcessor.getWidth();
    auto height = vProcessor.getHeight();

    //create window
    SDL_Window* window;
    window = SDL_CreateWindow("Player_window", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        string errMsg = "SDL_CreateWindow failed ! ";
        errMsg += SDL_GetError();
        cout << errMsg << endl;
        throw std::runtime_error(errMsg);
    }

    //create renderer
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, 0);

    //create texture
    Uint32 pixformat = SDL_PIXELFORMAT_IYUV;
    SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, width, height);

    SDL_Event event;
    auto frameRate = vProcessor.getFrameRate();

    bool exitRefresh = false;
    bool faster = false;
    std::thread refreshThread{ refresh, (int)(1000 / frameRate), std::ref(exitRefresh), std::ref(faster) };

    while (!vProcessor.isStreamFinished()) {
        SDL_WaitEvent(&event);

        if (event.type == REFRESH_EVENT) {
            if (vProcessor.isStreamFinished()) {
                exitRefresh = true;
                continue;  // skip REFRESH event.
            }

            //sync video and audio
            if (audio != nullptr) {
                auto vTs = vProcessor.getPts();
                auto aTs = audio->getPts();
                if (vTs > aTs&& vTs - aTs > 30) {
                    cout << "VIDEO FASTER ================= vTs - aTs [" << (vTs - aTs)
                        << "]ms, SKIP A EVENT" << endl;
                    // skip a REFRESH_EVENT
                    faster = false;
                    continue;
                }
                else if (vTs < aTs && aTs - vTs > 30) {
                    cout << "VIDEO SLOWER ================= aTs - vTs =[" << (aTs - vTs) << "]ms, Faster"
                        << endl;
                    faster = true;
                }
                else {
                    faster = false;
                    // cout << "=================   vTs[" << vTs << "] aPts[" << aTs << "] nothing to
                    // do." << endl;
                }
            }

            AVFrame* frame = vProcessor.getFrame();
            if (frame != nullptr) {
                SDL_UpdateYUVTexture(sdlTexture, NULL,       
                    // Y
                    frame->data[0], frame->linesize[0], 
                    // U
                    frame->data[1], frame->linesize[1], 
                    // V
                    frame->data[2], frame->linesize[2]   
                );
                SDL_RenderClear(sdlRenderer);
                SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
                SDL_RenderPresent(sdlRenderer);

                if (!vProcessor.refreshFrame()) {
                    cout << "WARN: vProcessor.refreshFrame false" << endl;
                }
            }
            else {
                cout << "WARN: getFrame failed." << endl;
            }

        }
        else if (event.type == SDL_QUIT) {
            cout << "SDL screen got a SDL_QUIT." << endl;
            exitRefresh = true;
            // close window.
            break;
        }
        else if (event.type == BREAK_EVENT) {
            break;
        }
    }

    refreshThread.join();
    cout << "[THREAD] Sdl video thread finish" << endl;
}





