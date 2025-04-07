// # Student Name:  Jacob Hamacher
// # Program Name:  AN Final Client
// # Creation Date:  11/21/22
// # Last Modified Date:  4/7/25(Adding comments)
// # CSCI Course:  CSCI 452
// # Grade Received:  100/100 (Pending grade)
// # Design Comments: This is a C++ program that connects
//                    to a C++ server and uses UDP to transfer
//                    a file from one computer to another on a 
//                    local network

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

using namespace std;

int main()
{
    int udpSocket, portNum, nBytes;
    struct sockaddr_in ServerAdd;
    char ip_string[17], port_string[10], buffer[1024], filename[1024], ack[1024];
    socklen_t addr_size;
    int broadcastPermission = 1;
    bool resend = false;
    int readSize;
    int count;
    char data[1000];
    int head;
    int tmp;

    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
    setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission, sizeof(broadcastPermission));
    ServerAdd.sin_family = AF_INET;
    cout << "Please enter your server Address:";
    cin.getline(ip_string, 16, '\n');
    cout << "Please enter your server port:";
    cin.getline(port_string, 9, '\n');
    portNum = atoi(port_string);
    ServerAdd.sin_port = htons(portNum);
    ServerAdd.sin_addr.s_addr = inet_addr(ip_string);
    memset(ServerAdd.sin_zero, '\0', sizeof ServerAdd.sin_zero);
    addr_size = sizeof ServerAdd;
    cout << "Please enter your file name:";
    cin.getline(filename, 1024, '\n');
    FILE *pFile;
    pFile = fopen(filename, "rb");

    if (pFile != NULL)
    {
        // Send filename to server
        nBytes = strlen(filename);
        resend = false;
        do
        {
            sendto(udpSocket, filename, nBytes, 0, (struct sockaddr *)&ServerAdd, addr_size);
            nBytes = recvfrom(udpSocket, ack, 1024, 0, (struct sockaddr *)&ServerAdd, &addr_size);
            ack[nBytes] = 0;
            if (strncmp(filename, ack, strlen(ack)) != 0)
            {
                resend = true;
            }
            else // Filename received by server successfully
            {
                strcpy(filename, "Start!");
                printf("Start!\n");
                sendto(udpSocket, filename, nBytes, 0, (struct sockaddr *)&ServerAdd, addr_size);
            }
        } while (resend);
        // Begin read 1000 bytes at a time
        count = 0;
        tmp = 0;
        int read_size;
        do
        {
            do
            {
                // file starting location
                resend = false;
                sprintf(buffer, "%i ", count);
                // Go to the asking position in the file
                fseek(pFile, count, SEEK_SET);
                // Read 1000 bytes
                read_size = fread(data, 1, 1000, pFile);
                head = strlen(buffer); // starting data index
                for (int i = 0; i < read_size; i++, head++)
                {
                    buffer[head] = data[i];
                }
                nBytes = head;
                sendto(udpSocket, buffer, nBytes, 0, (struct sockaddr *)&ServerAdd, addr_size);        // send 1000bytes
                nBytes = recvfrom(udpSocket, ack, 1024, 0, (struct sockaddr *)&ServerAdd, &addr_size); //wait for acknowledge
                ack[nBytes] = 0;
                tmp = atoi(ack);
                if (tmp != count) // server and client has different view of current file location
                {
                    count = tmp;
                    resend = true;
                }

            } while (resend);
            count += 1000;
            memset(buffer, '\0', sizeof buffer);
        } while (!feof(pFile));
        printf("Done\n");
        fclose(pFile);
    }
    else
    {
        printf("Error opening file!\n");
    }
    sprintf(buffer, "Quit! ", 6);
    sendto(udpSocket, buffer, 6, 0, (struct sockaddr *)&ServerAdd, addr_size);
    close(udpSocket);

    return 0;
}
