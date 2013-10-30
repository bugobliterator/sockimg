 #include <stdio.h>
 #include <sys/socket.h>
#include<sys/resource.h>
 #include <arpa/inet.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <netinet/in.h>
#include <opencv2/opencv.hpp>
//#include<iostream.h>



#define BUFFSIZE 32
using namespace cv;
using namespace std;

void Die(char *mess) { perror(mess); exit(1); }

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

	


            int sock;
            struct sockaddr_in echoserver;
            char buffer[BUFFSIZE];
            unsigned int echolen;
            int received = 0;

            if (argc != 2)
	    {
           	   fprintf(stderr, "USAGE: TCPecho <server_ip> <word> <port>\n");
           	   exit(1);
            }
            /* Create the TCP socket */
            if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
	    {
            	   Die("Failed to create socket");
            }
 

/*----------------------------- Construct the server sockaddr_in structure -----------------------------------------------------------------*/


            memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
            echoserver.sin_family = AF_INET;                  /* Internet/IP */
            echoserver.sin_addr.s_addr = inet_addr(argv[1]);  /* IP address */
            echoserver.sin_port = htons(2705);       /* server port */
            /* Establish connection */
            if (connect(sock,(struct sockaddr *) &echoserver,sizeof(echoserver)) < 0)
	    {
           	   Die("Failed to connect with server");
            }


 /*----------------------------- Send the word to the server------------------------------------------------------------------------------ */


            echolen = strlen("Hello");
            if (send(sock, "Hello", echolen, 0) != echolen)
	    {
              Die("Mismatch in number of sent bytes");
            }
           

 /*---------------------------- Receive the word back from the server-------------------------------------------------------------------- */

           // uchar *p;
            
            fprintf(stdout, "Received: ");
            //while (received < echolen)
		 {
              int a,size[2];
		long unsigned int bytes=0,receive=0;
	      //p=new uchar[size[0]*3*size[1]];
              if ((bytes = recv(sock, size, sizeof(int)*2, 0)) < 1) 
	      {
                Die("Failed to receive bytes from server");
              }
		else
		{
			cout<<"Image size received successfully!!!!!"<<endl<<"Height::"<<size[0]<<endl<<"Width::"<<size[1]<<endl;
		}
		IplImage *img=cvCreateImage(cvSize(size[1],size[0]),IPL_DEPTH_8U,3);
                IplImage *img1=cvCreateImage(cvSize(size[1],size[0]),IPL_DEPTH_8U,3);
                bytes=0;
		while(receive<=((size[0]*3*size[1]-1)))
		{
			if((bytes=recv(sock,img->imageData,sizeof(uchar)*size[0]*3*size[1],0))<1)
		{
			cout<<"Receiving data failed"<<endl<<"bytes::"<<bytes<<endl;
		}
		else
		{
			cout<<"Data received successfully!!!!!!!!"<<endl<<"bytes::"<<bytes<<endl;
			
		}
		
		for( int y=0; y<bytes; y++ ) {
			*(img1->imageData+receive+y)=*(img->imageData+y);
		
	        }
		receive+=bytes;
		}
                
		

        	cvSaveImage("test.jpg",img1);
		//for(int i=0;i<size[0]*3*size[1];i++)
			//cout<<img->imageData[i]<<" ";
		getchar();
								//Important to give delay b/w save and release
		cvReleaseImage(&img);
		
            }

/*--------------------------------Closing connection-------------------------------------------------------------------------------------*/	   

	fprintf(stdout, "\n");
           close(sock);
           exit(0);
       }
