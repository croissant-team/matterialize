#ifndef MATTERIALIZE_VIDEO_DEVICES_HPP
#define MATTERIALIZE_VIDEO_DEVICES_HPP

#include <string>
#include <vector>

#define MATTERIALIZE_CAM 100
#define MATTERIALIZE_PREVIEW 98

class VideoDevice {
public:
    VideoDevice(int number, const std::string &name);

public:
    const int number;
    const std::string name;
};

class VideoDevices {
public:
    static std::vector<VideoDevice> get_devices();

    static std::string exec(const char *cmd);
};


#endif
