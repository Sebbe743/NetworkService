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
    srand(time(nullptr));
    try
    {
        zmq::context_t context(1);

        //Outgoing message go out through here
        zmq::socket_t ventilator( context, ZMQ_PUSH );
//		ventilator.connect( "tcp://192.168.1.8:24041" );
//		ventilator.connect( "tcp://localhost:24041" );
        ventilator.connect( "tcp://benternet.pxl-ea-ict.be:24041" );

        while( ventilator.connected() )
        {
            sleep( 1000 );
            char buffer [50];
            int n, a=rand()%9+1, b=rand()%10;
            n=sprintf (buffer, "Sebbe>subbed>%d%d", a, b);
            ventilator.send( buffer, n);
            std::cout << "Pushed : " << buffer << std::endl;
        }
    }
    catch( zmq::error_t & ex )
    {
        std::cerr << "Caught an exception : " << ex.what();
    }

    return 0;
}
