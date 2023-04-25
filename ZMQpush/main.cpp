#include <iostream>
#include <zmq.hpp>
#ifndef _WIN32
    #include <unistd.h>
#else
    #include <windows.h>
    #define sleep(n)    Sleep(n)
#endif

int main( void )
{
    std::string submessage  = "Sebbe>pushed";
    srand(time(nullptr));
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
            sleep( 1000 );
            char buffer [50];
            int n, a=rand()%9+1, b=rand()%10;
            n=sprintf (buffer, "Sebbe>subbed>%d%d", a, b);
            pusher.send( buffer, n);
            std::cout << "Pushed : " << buffer << std::endl;

            for(int i=0; i>5; i++)
            {
                pusher.send( buffer, n);
                std::cout << "Pushed : " << buffer << std::endl;
            }

            subscriber.recv( msg );
            std::cout << "Subscribed : [" << std::string( (char*) msg->data(), msg->size() ) << "]" << std::endl;
        }
    }
    catch( zmq::error_t & ex )
    {
        std::cerr << "Caught an exception : " << ex.what();
    }

    return 0;
}
