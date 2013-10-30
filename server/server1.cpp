 #include <stdio.h>
 #include <sys/socket.h>
//#include<iostream.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include<highgui.h>
#include <opencv2/opencv.hpp>
#include<sys/resource.h>
#include <cv.h>
//#include"cvaux.h"
//#include"cvcore.h"
#define MAXPENDING 5    /* Max connection requests */
#define BUFFSIZE 32

using namespace cv;
using namespace std;

void Die(char *mess) { perror(mess); exit(1); }

void HandleClient(int sock,IplImage *img)
{
	
/*-------------------------------------------------sending image size----------------------------------------------------------------------------------*/
	int *size;
	size=new int[2];
	size[0]=img->height;
	size[1]=img->width;
        if (send(sock, size, sizeof(int)*2, 0) <0)
	{
		Die("Failed to send bytes to client");
	}
	else
	{
		cout<<"Sending size successful!!!!"<<endl;
	}
	
/*-------------------------------------------------sending image-----------------------------------------------------------------------------------*/
	
//	char *data;
//	data=new char[size[0]*size[1]];
	signed int bytes=0;
	while(bytes==0)
	{	
		if(bytes=send(sock,img->imageData,sizeof(char)*size[0]*3*size[1],0)<0)
		{
			cout<<"Failed to send data bytes to client"<<endl<<"bytes::"<<bytes<<endl;
		}
		else
		{
			cout<<"Sending image completed!!!!"<<endl<<"bytes::"<<bytes<<endl;
		}
	}        
	close(sock);
}

int main(int argc, char *argv[])
{

const rlim_t kStackSize = 64L * 1024L * 1024L;   // min stack size = 64 Mb
		struct rlimit rl;
		int result;
		result = getrlimit(RLIMIT_STACK, &rl);
    
		if (result == 0)
		{
			if (rl.rlim_cur < kStackSize)
		        {
		            rl.rlim_cur = kStackSize;
		            result = setrlimit(RLIMIT_STACK, &rl);
		            if (result != 0)
		            {
		                fprintf(stderr, "setrlimit returned result = %d\n", result);
		            }
	        }
	    }





            int serversock, clientsock;
            IplImage* img=cvLoadImage("14.png");
	    //cvNamedWindow("test");
	    //cvShowImage("test",img);
	    //cvWaitKey(0);
	    //cvDestroyWindow("test");

		/*for(int i=0;i<img->width*3*img->height;i++)
			cout<<img->imageData[i]<<" ";*/


            struct sockaddr_in echoserver, echoclient;

            if (argc != 2)
	    {
              fprintf(stderr, "USAGE: echoserver <port>\n");
              exit(1);
            }
            

/* ----------------------------------------------------Create the TCP socket----------------------------------------------------------------- */


            if ((serversock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
	   {
              Die("Failed to create socket");
            }
            

/*-------------------------------------------------- Construct the server sockaddr_in structure ---------------------------------------------*/


            memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
            echoserver.sin_family = AF_INET;                  /* Internet/IP */
            echoserver.sin_addr.s_addr = inet_addr(argv[1]);   /* Incoming addr */
            echoserver.sin_port = htons(2705);       /* server port */

 /*------------------------------------------------- Bind the server socket----------------------------------------------------------------- */
     

          if (bind(serversock, (struct sockaddr *) &echoserver,sizeof(echoserver)) < 0)
	 {
            Die("Failed to bind the server socket");
          }
          

/*------------------------------------------------ Listen on the server socket---------------------------------------------------------------- */


          if (listen(serversock, MAXPENDING) < 0)
	 {
            Die("Failed to listen on server socket");
          }

  /*--------------------------------------------------- Run until cancelled -------------------------------------------------------------------*/
            //while (1)
	   {
              unsigned int clientlen = sizeof(echoclient);
              /* Wait for client connection */
              if ((clientsock =accept(serversock, (struct sockaddr *) &echoclient,&clientlen)) < 0)
	      {
             	   Die("Failed to accept client connection");
              }
              fprintf(stdout, "Client connected: %s\n",inet_ntoa(echoclient.sin_addr));
              HandleClient(clientsock,img);
            }
		cvReleaseImage(&img);
}
