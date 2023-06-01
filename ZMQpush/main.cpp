#include <iostream>
#include <QTcpSocket>
#include <zmq.hpp>
#ifndef _WIN32
    #include <unistd.h>
#else
    #include <windows.h>
    #define sleep(n)    Sleep(n)
#endif

int main( void )
{
    std::string submessage  = "Sebbe>pushed>";
    try
    {
        zmq::context_t context(1);

        //Outgoing message go out through here
        zmq::socket_t pusher( context, ZMQ_PUSH );
        zmq::socket_t subscriber( context, ZMQ_SUB );

        pusher.connect( "tcp://benternet.pxl-ea-ict.be:24041" );
        subscriber.connect( "tcp://benternet.pxl-ea-ict.be:24042" );
        subscriber.setsockopt( ZMQ_SUBSCRIBE, submessage.c_str(), submessage.length() );

        zmq::message_t * msg = new zmq::message_t();

        while( pusher.connected() )
        {
            //benternet communication
            sleep( 1000 );
            unsigned long long n=0;
            std::string longi = "", lati = "", temp ="";

            //std::cout << "hello world" << std::endl;

            subscriber.recv( msg );
            std::cout << "Subscribed : [" << std::string( (char*) msg->data(), msg->size() ) << "]" << std::endl;

            //std::cout << "message received" << std::endl;

            //std::cout << msg->data();
            temp.append((char*) msg->data());
            //std::cout << temp;


            longi.append(temp.substr(submessage.size(), 6));
            lati.append(temp.substr(submessage.size() + 6, 6));



            //pusher.send( buffer, n);

            //std::cout << "Subscribed : [" << std::string( (char*) msg->data(), msg->size() ) << "]" << std::endl;

            //HTTP communication over TCP
            std::string request = "GET /v1/forecast?latitude=&longitude=&current_weather=true HTTP/1.0 \r\nHost: api.open-meteo.com\r\n\r\n";
            std::string received ="";

            QTcpSocket* TCP = new QTcpSocket();
            int headersize = 0;
            int datalength = 0;
            char *buffer = new char[longi.size()+lati.size()+ request.size()];

            n=sprintf (buffer, "GET /v1/forecast?latitude=%s&longitude=%s&current_weather=true HTTP/1.0 \r\nHost: api.open-meteo.com\r\n\r\n", longi.c_str(),lati.c_str());

            TCP->connectToHost("202.61.229.161", 80);
            TCP->write(buffer);

            TCP->waitForReadyRead();
            received = TCP->readAll();

            //std::cout << received << std::endl;

            for(unsigned long long i = 0; i<received.length()-18; i++)
            {

                if(!strcmp(received.substr(i,18).c_str(),"current_weather\":{"))
                {
                    headersize = i+17;
                    datalength = received.size() - headersize -1;
                    break;
                }
            }
            std::string jsondata = received.substr(headersize, datalength);
            //std::cout << jsondata<< std::endl;

            int count = 0;
            int length = 0;

            for(unsigned long long i = 0; i<jsondata.length()-2; i++)
            {
                char character = jsondata.at(i);
                if(character == ':')
                {
                    count++;
                }
            }
            std::string *data = new std::string[count*2-1];
            count =0;

            std::cout << jsondata << std::endl << jsondata.length()<< std::endl<< std::endl;
            for(unsigned long long i = 1; i<jsondata.length()-2; i++)
            {
                char character = jsondata.at(i);
                if(character == '\"' && jsondata.at(i+1)!= ':'&& jsondata.at(i-1) != ':')
                {
                    //std::cout << "statement 1 "<< std::endl;
                    //std::cout << "if statement worked on i: "<< i << " \ttrigger: " << jsondata.at(i) << std::endl;
                    length = 0;

                    do{
                        length++;
                    }
                    while(jsondata.at(i+length) != '\"');

                    //std::cout << "length of variable "<< count << " is " << length << " "<< std::endl;
                    data[count] = jsondata.substr(i+1,length-1);
                    //std::cout<< data[count] <<std::endl<<std::endl<<std::endl;

                    i+=length;
                    count++;
                }

                else if (character == ':' && jsondata.at(i+1)== '\"')
                {
                    //std::cout << "statement 2 "<< std::endl;
                    //std::cout << "if statement worked on i: "<< i << " \ttrigger: " << jsondata.at(i) <<std::endl ;
                    length = 0;

                    do{
                        length++;
                    }
                    while(jsondata.at(i+length) != ','&& (i+length)<jsondata.size()-1);

                    //std::cout << "length of variable "<< count << " is " << length << " "<< std::endl;
                    data[count] = jsondata.substr(i+1+1,length-1);
                    //std::cout<< std::endl <<std::endl << data[count] <<std::endl<<std::endl<<std::endl;

                    i+=length;
                    count++;
                }

                else if (character == ':')
                {
                    //std::cout << "statement 3 "<< std::endl;
                    //std::cout << "if statement worked on i: "<< i << " \ttrigger: " << jsondata.at(i) <<std::endl ;
                    length = 0;

                    do{
                        length++;
                    }
                    while(jsondata.at(i+length) != ',' && (i+length)<jsondata.size()-1);

                    //std::cout << "length of variable "<< count << " is " << length << " ";
                    data[count] = jsondata.substr(i+1,length-1);
                    //std::cout<<std::endl << data[count] <<std::endl<<std::endl;

                    i+=length;
                    count++;
                }

                else if (character == '}')
                {
                    //std::cout << "statement 4 "<< std::endl;
                    //std::cout<< "end of data";
                    break;
                }
            }

            std::string sendmessage = "";

            for(int i=0; i<count; i+=2)
            {

                std::string sendmessage = "Sebbe>subbed>";
                sendmessage.append(data[i].c_str());
                sendmessage.append(": ");
                sendmessage.append(data[i+1].c_str());
                pusher.send( sendmessage.c_str(), sendmessage.size());

            }
        }
    }

    catch( zmq::error_t & ex )
    {
        std::cerr << "Caught an exception : " << ex.what();
    }

    return 0;
}
