#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "pifacedigital.h"

int main( int argc, char *argv[] ) {
    uint8_t inputs;    /**< Input bits (pins 0-7) */
    uint8_t pin = 0x7f;     /** Pin 7 is connected to the flowmeter */
    int hw_addr = 0;   /**< PiFaceDigital hardware address  */
    int intenable = 1; /**< Whether or not interrupts are enabled  */
    int count = 0;
    int lastcount = 0;
    int seccount = 0;
    int litres = 1000;
    int maxflow = 1000; // maximum flow checking
    int flowexceeded = 0;
    int flowmaxtime = 10;
    char nameBuff[32];
    FILE* liveflow; //Temp file to write live flow rate
    int index;
    int c;
    int ppl = 500; /* Pulses per litre */
    time_t timeout = 60; /** Default 60 seconds timeout */
    time_t start;
    time_t lastmin;
    time_t now;
    time_t lastsec = 0;
    char * piface;
    opterr = 0;
   
    memset(nameBuff,0,sizeof(nameBuff));
    strncpy(nameBuff,"/tmp/flowtemp-XXXXXX",21); 
    while ((c = getopt (argc, argv, "l:t:p:m:")) != -1){
        switch (c) {
            case 't':
            timeout = atoi(optarg);
            break;
            case 'l':
            litres = atoi(optarg);
            break;
            case 'p':
            ppl = atoi(optarg);
            break;
            case 'm':
            maxflow = atoi(optarg);
            break;
            case '?':
            if (optopt == 't')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
               fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
            fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
            return 1;
            default:
            abort ();
        }
    }

    for (index = optind; index < argc; index++)
        fprintf (stderr,"Non-option argument %s\n", argv[index]);

    start = time(NULL); 
    if ( (piface = getenv("PIFACE")) != NULL  ) {
        hw_addr = atoi(piface);
    }
    if ( (piface = getenv("PPL")) != NULL  ) {
        ppl = atoi(piface);
    }

    /** 
    * Read command line value for flow period
    */
    if (timeout)
        timeout += start;

    /* if the total litres are set, convert the litres to pulses */
    if (litres)
     litres = litres * ppl;

    /**
    * Open piface digital SPI connection(s)
    */
    pifacedigital_open(hw_addr);

    // Open temp file for live flow updates
    int fd = mkstemp(nameBuff);
    liveflow = fdopen(fd,"wb+");
    fprintf(stderr,"TEMPNAME=%s\n",nameBuff);


    /**
    * Enable interrupt processing (only required for all
    * blocking/interrupt methods)
    */
    intenable = pifacedigital_enable_interrupts();
    if ( intenable == 0) 
        fprintf(stderr,"Interrupts enabled.\n");
    else 
        fprintf(stderr,"Could not enable interrupts.  Try running using sudo to enable PiFaceDigital interrupts.\n");


    /**
    * Wait for input change interrupt
    */
    lastmin=time(NULL);
    if( intenable ) {
        fprintf(stderr,"Interrupts disabled, skipping interrupt tests.\n");
    }   else {
        /** printf("Waiting for input (press any button on the PiFaceDigital)\n"); */
        while (1) {
            int ret = pifacedigital_wait_for_input(&inputs, 20000, hw_addr);
            if ( !ret ) {
                fprintf(stderr,"Timeout, %d pulses\n there is no flow\n",count );
//                break;
            } else {
                if ( inputs == pin ) {
                    count++;
                    pifacedigital_digital_write(7,!pifacedigital_read_bit(7, OUTPUT, 0));
                    now = time(NULL);
                    if (now > lastsec + 10) {
                        if (liveflow != NULL)
                        	fprintf(liveflow,"%f\n",((float)(count-seccount)/ppl)*6);
                        lastsec = now;
                        seccount = count;
                        rewind(liveflow);
                    }
                    if ( now > lastmin + 60 ) {
			float flow;
			flow  = (float)(count-lastcount)/(float)ppl;
                        fprintf(stderr,"%-10.2f %d %d %d %3.1f %d\n",
                                (float)(time(NULL)-start)/60, ppl, count, (count / ppl),flow,flowexceeded); /* Print the number of litres counted to stderr */
                        lastmin = time(NULL);
                        lastcount = count ;
			// increment the high flow count so that we can abort after 5 minutes of high flow
			if ( flow > (float)maxflow) {
				flowexceeded++;
			} else {
				flowexceeded = 0;
				flowmaxtime = 4; //reset the allowed high flow time to a lower bar
			}

                    }

                }
            }
/*            if ( time(NULL) > timeout || count > litres || flowexceeded > 4) 
                break; */
	    if ( time(NULL) > timeout ) {
		fprintf(stderr,"Time Exceeded\n");
		break;
	    }
	    if ( count > litres ) {
                fprintf(stderr,"Target litres reached\n");
                break;
            }
	    if ( flowexceeded > flowmaxtime ) {
                fprintf(stderr,"Flow rate exceeded\n");
                break;
            }

        }
        printf("TIME=%-10.2f\nPULSES=%d\nTOTAL=%-10.2f\nLPM=%-10.2f\nPPL=%d\n",(float)(time(NULL)-start)/60,count,(float)count/ppl,((float)count/ppl)/((float)(time(NULL)-start)/60),ppl);
        pifacedigital_digital_write(7,0);
    }

    /**
    * Close the connection to the PiFace Digital
    */
    pifacedigital_close(hw_addr);
    if (liveflow != NULL) unlink(nameBuff);
    
    return(0);
}
