
// Socket Hpp file
// 
#ifndef Socket_hpp
#define Socket_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_MESSAGE_LENGTH 2048
#define MAX_NUMBER_OF_CONNECTIONS 5

//Server socket code
class Socket {
public:
    //Constructor
    Socket();
    
    //Destructor
    ~Socket();
    
    //Public properties
    
    //Public member functions
    
    /*!
     * A function to initialize the socket. This must be done before the socket can be used. Will throw an error if the socket cannot be opened or if the port is occupied.
     *
     * @param portNum The number of the port on the host at which clients should connect.
     */
    void setSocket(int portNum);
    
    /*!
     * A function that sends a message to a single client. An error will be thrown if the socket is not set, if the given index is out of range, or if an error occurs in sending the message.
     *
     * @param message A const char * of the message to be sent.
     * @param strSize An unsigned int representing the length of the message
     */
    void send(const unsigned char* message, unsigned int strSize);
    
private:
    //Private properties
    
    //These are "file descriptors", which store values from both the socket system call and the accept system call
    int hostSocket;
    int clientSocket;
    int portNumber; //The port nubmer where connections are accepted
    socklen_t clientAddressSize; //The size of the address of the client, for the accept system call
    
    /* struct sockaddr_in
     
     struct sockaddr_in {
         short sin_family; //Should be AF_INET, see below
         u_short sin_port;
         struct in_addr sin_addr;
         char sin_zero[8]; //Not used, should be zero, see below
     };
     
     Note: the struct in_addr contains only a single property, an unsigned long named s_addr.
     */
    
    //These are objects of the struct "sockaddr_in", which contain internet addresses
    sockaddr_in serverAddress;
    sockaddr_in clientAddress;
    
    bool setUp = false; //Represents if the socket has already been set. If not, reading and writing will cause errors
};

#endif /* Socket_hpp */
