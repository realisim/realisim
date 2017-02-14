
#pragma once

class MainDialog;
class FileStreamer;

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

private:
    void setFileStreamer(FileStreamer*);
    
    FileStreamer* mpFileStreamer; 
};