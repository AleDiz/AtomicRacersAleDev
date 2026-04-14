#pragma once

#include <string>
#include <cstdint>
#include <vlc/vlc.h>
#include <vector>
#include <mutex>
#include <atomic>

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

class VLCVideoPlayer {
public:
    VLCVideoPlayer();
    ~VLCVideoPlayer();

    bool load(const std::string& path, int width, int height);
    void play();
    void pause();
    void stop();

    void setLoop(bool loop);

    const uint8_t* getFrameData() const;
    int getWidth() const;
    int getHeight() const;

private:
    static void* lockCallback(void* opaque, void** planes);
    static void unlockCallback(void* opaque, void* picture, void* const* planes);
    static void displayCallback(void* opaque, void* picture);

    libvlc_instance_t* m_instance = nullptr;
    libvlc_media_player_t* m_mediaPlayer = nullptr;
    std::string m_videoPath;

    std::vector<uint8_t> m_frameBuffer;
    int m_width = 0;
    int m_height = 0;

    bool m_loop = false;
    bool m_running = true;
    bool m_shuttingDown = false;

    std::mutex m_mutex; 
};

#endif
