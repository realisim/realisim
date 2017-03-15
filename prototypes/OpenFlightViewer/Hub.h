
#pragma once

class Broker;
class FileStreamer;
class GpuStreamer;
class MainDialog;

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

    Broker& getBroker();
    FileStreamer& getFileStreamer();
    GpuStreamer& getGpuStreamer();

private:
    void setBroker(Broker*);
    void setFileStreamer(FileStreamer*);
    void setGpuStreamer(GpuStreamer*);
    
    Broker *mpBroker;
    FileStreamer *mpFileStreamer;
    GpuStreamer *mpGpuStreamer;
};