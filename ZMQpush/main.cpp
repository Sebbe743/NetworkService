#include <iostream>
#include <QTcpSocket>
#include <zmq.hpp>
#include <cstdlib>
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
            bool city =0;
            //benternet communication
            sleep( 1000 );
            std::string longi = "", lati = "", temp ="", stad ="";

            //std::cout << "hello world" << std::endl;

            subscriber.recv( msg );
            std::cout << "Subscribed: [" << std::string( (char*) msg->data(), msg->size() ) << "]" << std::endl;

            //std::cout << "message received" << std::endl;

            //std::cout << msg->data();
            temp.append(std::string( (char*) msg->data(), msg->size()));

            //std::cout << temp << std::endl;

            int count1 = 0;
            int count2 = 0;
            for(unsigned long int i = 12; i<temp.size(); i++)
            {
                if(temp.at(i)== '>')
                {
                    i++;
                    int tempint = i;
                    //std::cout << stad << std::endl;
                    if(temp.at(i) == 'C')
                    {
                        city =1;
                        i+=5;
                        tempint = i;
                        while(i<temp.size())
                        {
                            count2++;
                            i++;
                        }
                        stad = temp.substr(tempint, count2);
                        std::cout << stad << std::endl;
                    }
                    else
                    {
                        city = 0;
                        std::cout << temp.at(i) << std::endl;
                        tempint = i;
                        while(temp.at(i) != '>')
                        {
                            count1++;
                            i++;
                            std::cout << temp.at(i) << std::endl;
                        }

                        std::cout << temp.at(i) << std::endl;
                        longi.append(temp.substr(tempint, count1));
                        std::cout << longi << std::endl;

                        i++;
                        tempint = i;

                        while(i<temp.size())
                        {
                            count2++;
                            i++;
                        }
                        lati.append(temp.substr(tempint, count2));
                        std::cout << lati << std::endl;
                    }

                }
            }
            //HTTP communication over TCP

            std::cout << "Http communication starts" <<std::endl;
            //202.61.206.6
            if(city)
            {

                std::string request = "GET /v1/search?name=&count=1&format=json HTTP/1.0 \r\nHost: geocoding-api.open-meteo.com\r\n\r\n";
                std::string received ="";

                QTcpSocket* TCP = new QTcpSocket();
                int headersize = 0;
                int datalength = 0;
                char *buffer = new char[stad.size()+ request.size()];

                sprintf(buffer, "GET /v1/search?name=%s&count=1&format=json HTTP/1.0 \r\nHost: geocoding-api.open-meteo.com\r\n\r\n", stad.c_str());
                std::cout << buffer << std::endl;
                TCP->connectToHost("202.61.206.6", 80);
                TCP->write(buffer);

                TCP->waitForReadyRead();
                received = TCP->readAll();
                std::cout << received << std::endl<< std::endl;
                TCP->waitForReadyRead();
                received = TCP->readAll();
                std::cout << received << std::endl<< std::endl;

                for(unsigned long long i = 0; i<received.length()-13; i++)
                {

                    if(!strcmp(received.substr(i,13).c_str(),"{\"results\":[{"))
                    {
                        headersize = i+13;
                        datalength = received.size() - headersize -1;
                        break;
                    }
                }
                std::string jsondata = received.substr(headersize, datalength);
                //std::cout << jsondata<< std::endl;

                std::cout << jsondata << std::endl<< std::endl;
                for(unsigned long long i = 1; i<jsondata.length()-2; i++)
                {

                    //std::cout << "     data check : " << jsondata.at(i);
                    if(!strcmp(jsondata.substr(i, 10).c_str(),"\"latitude\""))
                    {
                        std::cout << "latitude entered: " << jsondata.at(i) <<std::endl;
                        //std::cout << "statement 1 "<< std::endl;
                        //std::cout << "if statement worked on i: "<< i << " \ttrigger: " << jsondata.at(i) << std::endl;
                        i+=11;
                        int length = 0;
                        while(jsondata.at(i+length) != ',')
                        {
                            length++;
                            std::cout <<jsondata.at(i)<<std::endl;

                            std::cout <<length <<std::endl;
                        //    i++;
                        }

                        lati = jsondata.substr(i,length);

                        std::cout <<lati <<std::endl;
                    }

                    if(!strcmp(jsondata.substr(i, 11).c_str(),"\"longitude\""))
                    {
                        //std::cout << "statement 1 "<< std::endl;
                        //std::cout << "if statement worked on i: "<< i << " \ttrigger: " << jsondata.at(i) << std::endl;
                        std::cout << "longitude entered: " << jsondata.at(i) <<std::endl;
                        i+=12;
                        int length = 0;
                        while(jsondata.at(i+length) != ',')
                        {
                            length++;
                        //    i++;
                        }
                        longi = jsondata.substr(i,length);
                        std::cout <<longi <<std::endl;
                    }
                }
            }










            std::cout << "weather request for longi: " <<longi << " lati: " << lati <<std::endl;

            std::string request = "GET /v1/forecast?latitude=&longitude=&current_weather=true HTTP/1.0 \r\nHost: api.open-meteo.com\r\n\r\n";
            std::string received ="";



            QTcpSocket* TCP2 = new QTcpSocket();
            int headersize = 0;
            int datalength = 0;
            char *buffer = new char[longi.size()+lati.size()+ request.size()];
            sprintf (buffer, "GET /v1/forecast?latitude=%s&longitude=%s&current_weather=true HTTP/1.0 \r\nHost: api.open-meteo.com\r\n\r\n", longi.c_str(),lati.c_str());
            std::cout << buffer;
            TCP2->connectToHost("202.61.229.161", 80);
            TCP2->write(buffer);

            TCP2->waitForReadyRead();
            received = TCP2->readAll();

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
            std::cout << jsondata<< std::endl;

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
            std::cout << jsondata << std::endl<< std::endl;
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
                    length = 0;

                    do{
                        length++;
                    }
                    while(jsondata.at(i+length) != ','&& (i+length)<jsondata.length()-3);

                    data[count] = jsondata.substr(i+1+1,length-1);

                    i+=length;
                    count++;
                }

                else if (character == ':')
                {
                    length = 0;

                    do{
                        length++;
                    }
                    while(jsondata.at(i+length) != ',' && (i+length)<jsondata.length()-3);

                    data[count] = jsondata.substr(i+1,length-1);

                    i+=length;
                    count++;
                }

                else if (character == '}')
                {
                    break;
                }
            }
            std::string sendmessage = "";

            if(city){
                char buffer[75];
                sprintf(buffer, "Sebbe>subbed>Stad: %s>longi: %s lati: %s \t",stad.c_str(), longi.c_str(), lati.c_str());
                sendmessage = buffer;

            }
            else
            sendmessage = "Sebbe>subbed>\t";

            for(int i=0; i<count; i+=2)
            {
                if(i||city)
                {
                    sendmessage.append("\t\t\t\t");
                }
                sendmessage.append(data[i].c_str());
                sendmessage.append(": ");
                sendmessage.append(data[i+1].c_str());
                sendmessage.append(" \n");
            }
            pusher.send( sendmessage.c_str(), sendmessage.size());
        }
    }

    catch( zmq::error_t & ex )
    {
        std::cerr << "Caught an exception : " << ex.what();
    }

    return 0;
}
