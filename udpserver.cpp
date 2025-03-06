#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

int main()
{
    int udpSocket, portNum, nBytes, seqNum = 1;
    struct sockaddr_in ClientAdd;
    char ip_string[17], port_string[10], buffer[1024], ack[100], data[1024];
    char filename[255], response[100];
    socklen_t addr_size;
    char *ClientIP;
    int head, count = 0, length, index;
    bool done = false, start = false, Quit = false;

    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
    ClientAdd.sin_family = AF_INET;
    cout << "Please enter your listening port: ";
    cin.getline(port_string, 9, '\n');
    portNum = atoi(port_string);
    ClientAdd.sin_port = htons(portNum);
    ClientAdd.sin_addr.s_addr = INADDR_ANY;
    memset(ClientAdd.sin_zero, '\0', sizeof ClientAdd.sin_zero);
    bind(udpSocket, (struct sockaddr *)&ClientAdd, sizeof(ClientAdd));
    addr_size = sizeof ClientAdd;
    do
    {
        nBytes = recvfrom(udpSocket, buffer, 255, 0, (struct sockaddr *)&ClientAdd, &addr_size);
        buffer[nBytes] = 0;
        if (strncmp(buffer, "Quit!", 5) == 0) // Client failed!
        {
            Quit = true;
            break;
        }
        else
        {
            if (strncmp(buffer, "Start!", 6) != 0) // not starting to receive data
            {
                sendto(udpSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&ClientAdd, addr_size);
                start = false;
                sprintf(filename, "%lu%s", time(NULL), buffer);
            }
            else
            {
                start = true;
            }
        }
    } while (!start);
    if (start)
    {
        FILE *pFile;
        pFile = fopen(filename, "wb");
        count = 0;
        if (pFile != NULL)
        {
            done = false;
            do
            {
                nBytes = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *)&ClientAdd, &addr_size);
                head = 0;
                for (int i = 0; i < nBytes; i++)
                {
                    if (buffer[i] == ' ') //delimiter
                    {
                        head = i + 1;
                        ack[i] = 0;
                        break;
                    }
                    else
                    {
                        ack[i] = buffer[i];
                    }
                }
                if (strncmp(ack, "Quit!", 5) != 0)
                {
                    length = atoi(ack);
                    if (length == count) // This is the right packet
                    {
                        // Get data
                        index = 0;
                        for (int i = head; i < nBytes; i++, index++)
                        {
                            data[index] = buffer[i];
                        }
                        // Write to file
                        fseek(pFile, length, SEEK_SET);
                        fwrite(data, sizeof(char), index, pFile);
                        sprintf(response, "%i", length);
                        printf("Recieve: Sequence Number - %i, Data size - %i\n", seqNum, length);
                        seqNum++;
                        sendto(udpSocket, response, strlen(response), 0, (struct sockaddr *)&ClientAdd, addr_size);
                        count += index;
                    }
                    else
                    {
                        sprintf(response, "%i", count); // send back what is expected
                        printf("Dr. Lin's Demo-  Error! Requests bytes for location: %i\n", length);
                        sendto(udpSocket, response, strlen(response), 0, (struct sockaddr *)&ClientAdd, addr_size);
                    }
                }
                else
                    done = true;

                memset(buffer, '\0', sizeof buffer);
                memset(data, '\0', sizeof data);
                memset(ack, '\0', sizeof ack);
                memset(response, '\0', sizeof response);
            } while (!done);
            printf("Exit...\n");
        }
        else
        {
            printf("Cannot write to file %s!!\n", filename);
        }
        fclose(pFile);
    }
    else
    {
        printf("Client failed!\n", filename);
    }

    close(udpSocket);

    return 0;
}