#include "videoPlayer.hpp"
#include <thread>
#include <filesystem>
#include <chrono>

VLCVideoPlayer::VLCVideoPlayer() : m_running(true)
{
    const char* const vlc_args[] = {
        "--no-xlib",
        "--quiet",
        "--no-video-title-show",
        "--avcodec-hw=none",
        "--intf=dummy",
        "--no-audio",
        "--no-sub-autodetect-file",
        "--no-media-library",
        "--file-caching=300",
        "--no-video-deco",
        "--no-osd"
    };

    m_instance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
}

VLCVideoPlayer::~VLCVideoPlayer()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_shuttingDown = true; 
    }

    stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    if (m_mediaPlayer) {
        libvlc_media_player_release(m_mediaPlayer);
        m_mediaPlayer = nullptr;
    }

    if (m_instance) {
        libvlc_release(m_instance);
        m_instance = nullptr;
    }
}

bool VLCVideoPlayer::load(const std::string& path, int width, int height)
{
    stop();

    if (!std::filesystem::exists(path)) {
        return false;
    }

    m_videoPath = path;

    libvlc_media_t* media = libvlc_media_new_path(m_instance, path.c_str());
    if (!media) {
        return false;
    }

    // Aplicar input-repeat si está activado
    if (m_loop) {
        libvlc_media_add_option(media, "input-repeat=65535");
    }

    m_mediaPlayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    m_width = width;
    m_height = height;
    m_frameBuffer.resize(m_width * m_height * 3);

    libvlc_video_set_format(m_mediaPlayer, "RV24", width, height, width * 3);
    libvlc_video_set_callbacks(m_mediaPlayer, lockCallback, unlockCallback, displayCallback, this);

    return true;
}

void VLCVideoPlayer::play()
{
    if (m_mediaPlayer) {
        if (libvlc_media_player_play(m_mediaPlayer) < 0) {
            return;
        }
        if (libvlc_media_player_is_playing(m_mediaPlayer)) {
            return;
        }    

        auto start = std::chrono::steady_clock::now();
        while (m_frameBuffer[0] == 0 && m_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > 200) {
                break;
            }
        }
    }
}

void VLCVideoPlayer::pause()
{
    if (m_mediaPlayer) {
        libvlc_media_player_set_pause(m_mediaPlayer, 1);
    }
}

void VLCVideoPlayer::stop()
{
    if (m_mediaPlayer) {
        libvlc_media_player_stop(m_mediaPlayer);
    }
}

void VLCVideoPlayer::setLoop(bool loop)
{
    m_loop = loop;
}

const uint8_t* VLCVideoPlayer::getFrameData() const {
    return m_running ? m_frameBuffer.data() : nullptr;
}

int VLCVideoPlayer::getWidth() const {
    return m_width;
}

int VLCVideoPlayer::getHeight() const {
    return m_height;
}

void* VLCVideoPlayer::lockCallback(void* opaque, void** planes)
{
    auto* player = static_cast<VLCVideoPlayer*>(opaque);
    if (!player || !player->m_running) return nullptr;

    player->m_mutex.lock();
    *planes = player->m_frameBuffer.data();
    return nullptr;
}

void VLCVideoPlayer::unlockCallback(void* opaque, void* picture, void* const* planes)
{
    auto* player = static_cast<VLCVideoPlayer*>(opaque);
    if (!player || !player->m_running) return;
    player->m_mutex.unlock();
}

void VLCVideoPlayer::displayCallback(void* opaque, void* picture)
{
    auto* player = static_cast<VLCVideoPlayer*>(opaque);
    if (!player || !player->m_running) return;
}
