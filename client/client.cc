
// Client side C/C++ program to demonstrate Socket programming
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

namespace cRPC
{
    class Client
    {
    private:
        char* addr;
    public:
        Client(char* addr_): addr(addr_) {}

        std::string getFile(std::string name)
        {
            std::ifstream ifs("./programs/" + name);
            std::string s;
            getline(ifs, s, (char)ifs.eof());

            s = "\n" + s;
            s = name + s;
            s = "// " + s;
            return s;
        }

        int run()
        {
            int sock = 0, valread;
            struct sockaddr_in serv_addr;

            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                return -1;
            }

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(PORT);

            // Convert IPv4 and IPv6 addresses from text to binary form
            if(inet_pton(AF_INET, addr, &serv_addr.sin_addr)<=0)
            {
                return -1;
            }

            if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            {
                return -1;
            }

            while(true)
            {
                char buffer[1024] = {0};
                std::cout<<"Enter\n1. To send a file to server\n2. To execute a particular file\n3. Shutdown the server\nEnter your choice: ";
                int ch;
                std::string megtoserver;

                std::cin>>ch;
                if(ch == 1)
                {
                    std::string filename;
                    std::cout<<"Enter the file name: ";
                    std::cin>>filename;

                    megtoserver = getFile(filename);
                }
                else if(ch == 2)
                {
                    std::string filename;
                    std::cout<<"Enter the file name: ";
                    std::cin>>filename;

                    megtoserver = "$" + filename;
                }
                else if(ch == 3)
                {
                    megtoserver = "$STOP";
                }
                else
                {
                    std::cout<<"Invalid choice! Please try again\n";
                    continue;
                }
                
                const char* msg = megtoserver.c_str();
                send(sock , msg , strlen(msg) , 0 );
                std::cout<<"Client: Message Sent\n";
                valread = read(sock, buffer, 1024);
                std::cout<<"Server Response: "<<buffer<<std::endl;
                
                if(megtoserver == "$STOP")
                    break;
                
                std::system("read");
                std::system("clear");
            }

            close(sock);
            return 0;
        }
    };
}

int main(int argc, char const *argv[])
{
    if(argc != 2)
        return 1;

    cRPC::Client cl((char*)argv[1]);
    cl.run();
    return 0;
}
