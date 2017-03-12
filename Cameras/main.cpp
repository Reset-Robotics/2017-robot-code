// The code below must be added into ahoy22.cpp if using UDP Streaming

#define QUALITY 5
#define PORT1 5440
#define PORT2 5441
#define ENDCHAR 'e'


struct CameraStream {
    VideoCapture cam;
    Socket socket;
    vector<int> config;

    CameraStream(int cameraNumber, int port) {
        cam.open(cameraNumber);
        socket.setSocket(port);

        if(!cam.open(0))
            throw 1;

        config.push_back(CV_IMWRITE_JPEG_QUALITY);
        config.push_back(QUALITY);
    }

    void sendFrame() {
        Mat frame;
        cam >> frame;

        Mat compressedMat;
        vector<uchar> buff;
        imencode(".jpg", frame, buff, config);
        buff.push_back(ENDCHAR);

        unsigned char * data = &buff[0];
        socket.send(data, (unsigned int)buff.size()+1);
    }
};

// in main do CameraStream stream(0,5806) // camera number and port
