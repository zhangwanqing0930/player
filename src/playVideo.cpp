#include "ffmpegUtil.h"
#include "MediaProcessor.hpp"

extern "C" {
#include "SDL.h"
};

using std::cout;
using std::endl;

// Refresh Event
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

    SDL_Window* screen;
    // SDL 2.0 Support for multiple windows
    screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, width / 2, height / 2,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!screen) {
        string errMsg = "SDL create window failed ! ";
        errMsg += SDL_GetError();
        cout << errMsg << endl;
        throw std::runtime_error(errMsg);
    }

    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

    // IYUV: Y + U + V  (3 planes)
    // YV12: Y + V + U  (3 planes)
    Uint32 pixformat = SDL_PIXELFORMAT_IYUV;

    SDL_Texture* sdlTexture =
        SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, width, height);
    // Use this function to update a rectangle within a planar
    // YV12 or IYUV texture with new pixel data.
    SDL_Event event;
    auto frameRate = vProcessor.getFrameRate();
    cout << "frame rate [" << frameRate << "]" << endl;

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

            // Use this function to update a rectangle within a planar
            // YV12 or IYUV texture with new pixel data.
            AVFrame* frame = vProcessor.getFrame();

            if (frame != nullptr) {
                SDL_UpdateYUVTexture(sdlTexture,  // the texture to update
                    NULL,        // a pointer to the rectangle of pixels to update, or
                                 // NULL to update the entire texture
                    frame->data[0],      // the raw pixel data for the Y plane
                    frame->linesize[0],  // the number of bytes between rows of pixel
                                       // data for the Y plane
                    frame->data[1],      // the raw pixel data for the U plane
                    frame->linesize[1],  // the number of bytes between rows of pixel
                                       // data for the U plane
                    frame->data[2],      // the raw pixel data for the V plane
                    frame->linesize[2]   // the number of bytes between rows of pixel
                                       // data for the V plane
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





