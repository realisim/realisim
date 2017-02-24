
#pragma once

class MainDialog;
class FileStreamer;
class GpuStreamer;

class Hub
{
public:
    Hub();
    Hub(const Hub&) = delete;
    Hub& operator=(const Hub&) = delete;
    ~Hub();

    // Maindialog will be responsible for setting all pointers
    // to data member
    //
    friend class MainDialog;

    FileStreamer& getFileStreamer();
    GpuStreamer& getGpuStreamer();

private:
    void setFileStreamer(FileStreamer*);
    void setGpuStreamer(GpuStreamer*);
    
    FileStreamer *mpFileStreamer;
    GpuStreamer *mpGpuStreamer;
};