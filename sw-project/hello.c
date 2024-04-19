#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include "fbputchar.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "usbkeyboard.h"
#include <pthread.h>
#include <time.h>
#include "vga_ball.h"
#include <sys/mman.h>
#include <dirent.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <math.h>

/*
#include "hwlib.h"
#include "socal/hps.h"
#include "socal/socal.h"
#include "hps_0.h"
#include "audio_control.h"
#include "pcm.h"*/


#define SERVER_HOST "128.59.19.114"
#define SERVER_PORT 42000
#define BUFFER_SIZE 128

#define HW_REGS_BASE (ALT_STM_OFST)
#define HW_REGS_SPAN (0x04000000)
#define HW_REGS_MASK (HW_REGS_SPAN - 1)

//base addr
static volatile unsigned long *h2p_lw_axi_addr = NULL;

volatile unsigned long *oc_i2c_audio_addr = NULL;
volatile unsigned long *audio_addr = NULL;

int vga_ball_fd;
int sockfd; /* Socket file descriptor */
struct libusb_device_handle *keyboard;
uint8_t endpoint_address;
pthread_t network_thread;
void *network_thread_f(void *);
clock_t start, end;
double cpu_time_used;


void update_array(int* arr, int size){
    for (int i = 0; i < size; i++) {
        if (arr[i] < 1500){
            arr[i]++;
            arr[i]++;
        }
        if (arr[i] == 1294 || arr[i] == 1293){
            arr[i] = 1500;
        }
        //printf("%d ", arr[i]);
    }
}

void set_ball_position(const note_position_t *c)  //------------------add------------------
{
  note_pos_t bpvla;
  bpvla.position = *c;
  if (ioctl(vga_ball_fd, VGA_BALL_WRITE_POSITION, &bpvla)) {
      perror("ioctl(VGA_BALL_WRITE_POSITION) failed");
      return;
  }
}

void set_ball_status(const node_status_t *c)  //------------------add------------------
{
  node_sta_t status_new;
  status_new.status = *c;
  if (ioctl(vga_ball_fd, VGA_BALL_WRITE_STATUS, &status_new)) {
      perror("ioctl(VGA_BALL_WRITE_STATUS) failed");
      return;
  }
}

void set_background(const background_t *c)  //------------------add------------------
{
  back_t background_new;
  background_new.background = *c;
  if (ioctl(vga_ball_fd, VGA_BALL_WRITE_BACKGROUND, &background_new)) {
      perror("ioctl(VGA_BALL_WRITE_BACKGROUND) failed");
      return;
  }
}

void set_ball_score(const total_score_t *c)  //------------------add------------------
{
  total_sc_t score_new;
  score_new.total_score = *c;
  if (ioctl(vga_ball_fd, VGA_BALL_WRITE_SCORE, &score_new)) {
      perror("ioctl(VGA_BALL_WRITE_SCORE) failed");
      return;
  }
}

/*void set_node_status(const node_status_t *v)
{
  note_arg_t note;
  note.status = *v; 
  if (ioctl(vga_ball_fd, NODE_WRITE_STATUS, &note)) {
      perror("ioctl( NODE_WRITE_STATUS) failed");
      return;
  }
}*/

int main(){


    static const char filename[] = "/dev/vga_ball";
    printf("VGA ball Userspace program started\n");
    if ( (vga_ball_fd = open(filename, O_RDWR)) == -1) {
        fprintf(stderr, "could not open %s\n", filename);
        return -1;
    }
    printf("initial state: ");


    if ( (keyboard = openkeyboard(&endpoint_address)) == NULL ) {
            fprintf(stderr, "Did not find a keyboard\n");
            exit(1);
        }
    //location variable
    float track_1[92] = {9.24, 11.38, 14.54, 17.69, 19.83, 22.94, 24.33, 25.91, 27.49, 29.07, 31.16, 32.51, 35.16, 37.76, 40.68, 41.98, 43.98, 45.46, 46.53, 49.92, 51.78, 53.13, 54.66, 56.8, 58.37, 60.74, 61.86, 64.18, 65.76, 67.34, 68.68, 70.5, 71.56, 74.21, 76.07, 78.72, 80.57, 82.94, 84.01, 85.59, 87.68, 88.75, 91.39, 93.48, 94.74, 96.46, 97.99, 99.89, 101.19, 103.14, 105.28, 107.0, 108.86, 110.62, 111.64, 113.36, 114.52, 115.96, 118.56, 120.14, 123.72, 127.01, 128.87, 131.75, 133.1, 134.91, 137.6, 140.76, 143.36, 146.52, 149.72, 152.88, 154.97, 158.17, 160.26, 161.89, 163.98, 167.14, 170.71, 172.48, 175.59, 178.24, 180.14, 182.46, 184.88, 187.25, 189.06, 190.87, 193.28, 195.65, 197.79};
    float track_2[184] = {6.08, 7.15, 9.01, 9.66, 10.87, 11.94, 14.02, 15.09, 17.46, 18.11, 19.32, 20.85, 22.76, 22.99, 24.29, 24.57, 25.63, 26.15, 27.21, 27.72, 28.79, 29.3, 30.93, 31.21, 32.46, 33.02, 34.09, 36.18, 37.24, 38.31, 40.45, 40.91, 41.8, 42.26, 43.61, 44.03, 45.42, 45.7, 46.49, 47.28, 49.69, 50.43, 51.5, 52.06, 52.85, 53.59, 54.2, 54.94, 56.52, 56.98, 58.14, 58.42, 60.37, 60.79, 61.81, 62.04, 63.11, 64.69, 65.53, 66.04, 67.11, 67.62, 68.41, 69.2, 70.26, 70.77, 71.33, 72.12, 73.47, 74.72, 75.79, 76.86, 78.44, 78.95, 80.02, 81.08, 82.66, 83.17, 83.96, 84.24, 85.31, 85.87, 87.4, 87.72, 88.47, 88.79, 91.12, 91.63, 92.69, 93.53, 94.69, 95.06, 96.41, 96.92, 97.62, 98.5, 99.47, 100.08, 101.15, 102.17, 102.77, 103.19, 104.86, 105.33, 106.95, 107.51, 108.81, 109.09, 110.39, 110.67, 111.6, 111.97, 113.31, 113.82, 114.34, 114.85, 115.68, 116.15, 118.1, 118.84, 119.91, 120.7, 123.3, 123.86, 125.43, 127.52, 128.59, 128.92, 131.24, 132.17, 132.82, 133.14, 134.44, 135.47, 137.56, 138.07, 140.2, 141.27, 142.85, 143.92, 146.01, 147.08, 149.21, 150.23, 152.37, 153.39, 154.74, 155.02, 157.11, 158.69, 160.03, 160.54, 161.75, 162.26, 163.7, 164.4, 166.63, 168.21, 170.34, 170.76, 172.43, 172.94, 174.57, 176.1, 177.96, 178.75, 179.54, 180.33, 181.67, 182.97, 184.55, 185.11, 187.2, 187.62, 188.78, 189.34, 190.36, 190.91, 193.0, 193.33, 195.14, 195.7, 197.23, 198.3}; 
    float track_3[183] = { 7.71, 8.73, 9.71, 10.31, 12.4, 13.47, 15.6, 17.18, 18.25, 19.27, 21.41, 22.48, 23.5, 24.06, 24.61, 25.12, 26.42, 26.98, 28.0, 28.28, 29.35, 30.37, 31.44, 32.23, 33.07, 33.85, 36.46, 36.73, 38.82, 40.4, 41.19, 41.75, 42.54, 43.33, 44.35, 45.19, 46.07, 46.16, 47.83, 49.13, 50.48, 51.27, 52.43, 52.52, 53.64, 54.15, 55.12, 56.29, 57.35, 58.1, 58.89, 59.95, 60.98, 61.53, 62.6, 62.88, 64.97, 65.25, 66.27, 67.06, 67.9, 68.17, 69.43, 69.98, 70.82, 71.29, 72.63, 73.42, 75.28, 75.6, 77.14, 77.88, 78.99, 79.78, 82.11, 82.43, 83.45, 83.73, 84.52, 85.03, 86.24, 86.61, 87.96, 88.42, 89.54, 90.09, 91.9, 92.18, 93.76, 94.32, 95.11, 96.13, 97.2, 97.43, 98.55, 99.24, 100.36, 100.64, 102.21, 102.73, 103.47, 104.58, 105.65, 106.72, 108.02, 108.53, 109.27, 109.64, 110.94, 111.22, 112.01, 113.03, 114.06, 114.29, 114.89, 115.45, 117.26, 118.05, 119.07, 119.63, 121.21, 122.79, 124.37, 125.16, 128.08, 128.5, 129.15, 129.66, 132.31, 132.59, 133.38, 133.89, 135.98, 137.04, 138.62, 139.69, 141.78, 142.34, 144.43, 145.5, 147.59, 148.65, 150.74, 151.81, 153.44, 154.46, 155.53, 156.6, 158.96, 159.75, 160.82, 161.61, 162.31, 163.47, 164.77, 166.12, 168.72, 169.78, 171.13, 172.15, 173.22, 174.52, 176.43, 177.17, 178.79, 179.21, 180.88, 181.39, 183.02, 184.04, 185.62, 186.69, 187.66, 188.04, 189.85, 190.12, 191.43, 192.49, 193.56, 194.86, 196.07, 196.44, 199.37};
    float track_4[92] = { 8.22, 9.8, 12.96, 16.67, 18.76, 21.92, 23.55, 25.08, 26.94, 28.24, 29.86, 31.95, 33.53, 36.69, 39.89, 41.47, 43.05, 45.14, 46.11, 48.34, 51.22, 52.48, 53.87, 55.73, 57.82, 58.93, 61.02, 62.83, 65.02, 66.83, 68.13, 69.47, 71.05, 73.14, 75.56, 77.37, 79.74, 82.38, 83.5, 84.57, 86.29, 88.38, 90.05, 91.95, 94.04, 95.85, 97.38, 98.78, 100.4, 102.49, 104.3, 106.44, 108.3, 109.6, 111.18, 112.76, 114.1, 115.4, 117.54, 119.49, 121.49, 124.92, 128.36, 129.38, 132.54, 133.61, 136.25, 139.13, 142.29, 144.99, 148.14, 151.3, 153.95, 156.04, 159.2, 161.33, 163.42, 165.56, 169.27, 171.87, 174.01, 176.66, 179.17, 181.3, 183.48, 186.41, 187.99, 190.08, 191.47, 194.07, 196.12, 199.92};
    int score = 18496;
    while(true){

    int location_1[4] ={1500,1500,1500,1500};
    int location_2[4] ={1500,1500,1500,1500};
    int location_3[4] ={1500,1500,1500,1500};
    int location_4[4] ={1500,1500,1500,1500};

    int size_1 = sizeof(track_1)/sizeof(track_1[0]);
    int size_2 = sizeof(track_2)/sizeof(track_2[0]);
    int size_3 = sizeof(track_3)/sizeof(track_3[0]);
    int size_4 = sizeof(track_4)/sizeof(track_4[0]);

    int last_status = 0;
    //keyboard varibale
    struct usb_keyboard_packet packet;
    int transferred;
    char keystate[12];
    int time_arr[4] = {0,0,0,0};
    int status_arr[4] = {0,0,0,0};

    float total_score = 0.0;

    float note_score = 1000/551;

    total_score_t d ={score};
    set_ball_score(&d);

    /*if ( (keyboard = openkeyboard(&endpoint_address)) == NULL ) {
            fprintf(stderr, "Did not find a keyboard\n");
            exit(1);
        }*/
    /*void *virtual_base;
  	int fd;

  	if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1)
  	{
  		fprintf(stderr, "[ERROR] Fail to open \"/dev/mem\"...\n");
  		return 1;
  	}

  	virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
  	if (virtual_base == MAP_FAILED)
  	{
  		fprintf(stderr, "[ERROR] mmap() failed...\n");
  		close(fd);
  		return 1;
  	}

  	h2p_lw_axi_addr = virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST) & (unsigned long)(HW_REGS_MASK));
  	oc_i2c_audio_addr = virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + OC_I2C_MASTER_0_BASE) & (unsigned long)(HW_REGS_MASK));
  	audio_addr = virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + AUDIO_IF_0_BASE) & (unsigned long)(HW_REGS_MASK));

  	printf("[INFO] i2c_audio_addr:	%04Xh\n", (unsigned int)oc_i2c_audio_addr);
  	printf("[INFO] audio_addr:		%04Xh\n", (unsigned int)audio_addr);

  	oc_i2c_audio_init();
  	init_audio();

  	usleep(500 * 1000); ///// !!!!!!! note. this delay is necessary

  	char AudioName[128] = {"TheDawn.mp3"};
  	if (argv[0] == 2 && argv[1])
  		strncpy(AudioName, argv[1], 128);

  	//TODO: Check if .pcm exists.

  	char PCMName[128 + 4];
  	strncpy(PCMName, AudioName, 128);
  	strncat(PCMName, ".pcm", 4);

  	AUDIO_SetSampleRate(RATE_ADC32K_DAC32K);

  	FILE *is_exist;
  	if (is_exist = fopen(PCMName, "r"), is_exist == NULL)
  	{
  		printf("[INFO] Converting PCM...\n");
  		char cmd[512];
  		sprintf(cmd, "ffmpeg -i \"%s\" -f s16le -ar 32000 -ac 2 -acodec pcm_s16le \"%s\" -y", AudioName, PCMName);
  		if (system(cmd) != 0)
  		{
  			fprintf(stderr, "[ERROR] Convert PCM failed...");
  			munmap(virtual_base, HW_REGS_SPAN);
  			close(fd);
  			return 1;
  		}
  	}
    */
    //while(true){
    //char PCMName[128 + 4];
    //pthread_create(&network_thread, NULL, network_thread_f, (void *) PCMName);

    start = clock(); // get the start time
    int initial = 0;

    for(int i = 0; i < 600000; i++){
        //location part
        while (true){
            libusb_interrupt_transfer(keyboard, endpoint_address,
			      (unsigned char *) &packet, 5,
			      &transferred, 0);
            if (transferred == 5) {
                sprintf(keystate, "%04x %04x %04x %04x %04x", packet.a, packet.b, packet.c, packet.d, packet.e);
                //printf("%s\n", keystate);
            }
            background_t c ={initial};
            set_background(&c);
            if(initial == 1){
                break;
            }

            if(packet.b == 1){
                initial = 1;
            }
            usleep(50000);
            //input inital to hardware 
        }

        if(i == 0){
            char PCMName[128 + 4];
            usleep(3000);
            //if audio need delay add usleep here
            pthread_create(&network_thread, NULL, network_thread_f, (void *) PCMName);
        }
        if(i == 0){
            //if logic need delay add usleep here
        }
        float timer = (float)i;
        float true_time = (timer+552.0)/200;
        if (i % 200 == 0){
            printf("%f\n",true_time);
        }

        //printf("%f\n",true_time);
        int remain_v = 0;
        for (int i1 = 0; i1 < size_1; i1++){
            if (true_time == track_1[i1]){
                for(int j1 = 0; j1 <4; j1++){
                    if(location_1[j1] == 1500){
                        location_1[j1] = 0;
                        break;
                    }
                }
            }
        }

        for (int i2 = 0; i2 < size_2; i2++){
            if (true_time == track_2[i2]){
                for(int j2 = 0; j2 <4; j2++){
                    if(location_2[j2] == 1500){
                        location_2[j2] = 0;
                        break;
                    }
                }
            }
        }

        for (int i3 = 0; i3 < size_3; i3++){
            if (true_time == track_3[i3]){
                for(int j3 = 0; j3 <4; j3++){
                    if(location_3[j3] == 1500){
                        location_3[j3] = 0;
                        break;
                    }
                }
            }
        }

        for (int i4 = 0; i4 < size_4; i4++){
            if (true_time == track_4[i4]){
                for(int j4 = 0; j4 <4; j4++){
                    if(location_4[j4] == 1500){
                        location_4[j4] = 0;
                        break;
                    }
                }
            }
        }
        
        //keyboard prt
        /*libusb_interrupt_transfer(keyboard, endpoint_address,
			      (unsigned char *) &packet, 5,
			      &transferred, 0);
        if (transferred == 5) {
            sprintf(keystate, "%04x %04x %04x %04x %04x", packet.a, packet.b, packet.c, packet.d, packet.e);
            printf("%s\n", keystate);
        }*/

        // boom 
        if (last_status != packet.b){
            if(packet.b - 16 >= 0){
                remain_v = packet.b - 16;
                if(status_arr[3] != 1){
                    status_arr[3] = 1;
                    for (int i = 0; i<4; i++){
                        if(location_4[i]>=  1072&& location_4[i]<= 1138){
                            printf("hit in track 4");
                            status_arr[3] = 2;
                            location_4[i] = 1500;
                            total_score += note_score;
                        }
                        if ((location_4[i]>= 1052 && location_4[i]<1072)){
                            status_arr[3] = 3;
                            location_4[i] = 1500;
                        }
                    
                  }
              }
          }
          else{
                remain_v = packet.b;
                status_arr[3] = 0;
                
          }
          if(remain_v - 8 >= 0){
              remain_v = remain_v - 8;
              if(status_arr[2] != 1){
                  status_arr[2] = 1;
                  for (int i = 0; i<4; i++){
                      if(location_3[i]>= 1072 && location_3[i]<= 1138){
                          printf("hit in track 3");
                          status_arr[2] = 2;
                          location_3[i] = 1500;
                          total_score += note_score;
                      }
                      if ((location_3[i]>=1052 && location_3[i]<1072)){
                          printf("miss in track 3");
                          status_arr[2] = 3;
                          location_3[i] = 1500;
                      }
                  }
              }    
          }
          else{
            status_arr[2] = 0;
          }
            


          if(remain_v - 4 >= 0){
              remain_v = remain_v - 4;
              if(status_arr[1] != 1){
                  status_arr[1] = 1;
                  for (int i = 0; i<4; i++){
                      if(location_2[i]>=1072 && location_2[i]<=1138){
                          printf("hit in track 2");
                          status_arr[1] = 2;
                          location_2[i] = 1500;
                          total_score += note_score;
                      }
                      if ((location_2[i]>=1052 && location_2[i]<1072)){
                          printf("miss in track 2");
                          status_arr[1] = 3;
                          location_2[i] = 1500;


                      }
                  }
              }    
          }
          else{
            status_arr[1] = 0;
          }
          if(remain_v - 2 >= 0){
              remain_v = remain_v - 8;
              if(status_arr[0] != 1){
                  status_arr[0] = 1;
                  for (int i = 0; i<4; i++){
                      if(location_1[i]>=1072 && location_1[i]<=1138){
                          printf("hit in track 1");
                          status_arr[0] = 2;
                          location_1[i] = 1500;
                          total_score += note_score;
                      }
                      if ((location_1[i]>=1052 && location_1[i]<1072)){
                          printf("miss in track 1");
                          status_arr[0] = 3;
                          location_1[i] = 1500;
                      }
                  }
              }    
          }
          else{
            status_arr[0] = 0;
          }
        }
        
        for (int i = 0; i<4; i++){
            if (location_1[i] >= 1138 && location_1[i] <= 1154 && status_arr[0] == 0){
                status_arr[0] = 4;
                time_arr[0] = 1;
            }
            if (location_2[i] >= 1138 && location_2[i] <= 1154 && status_arr[1] == 0){
                status_arr[1] = 4;
                time_arr[1] = 1;
            }
            
            if (location_3[i] >= 1138 && location_3[i] <= 1154 && status_arr[2] == 0){
                status_arr[2] = 4;
                time_arr[2] = 1;
            }

            if (location_4[i] >= 1138 && location_4[i] <= 1154 && status_arr[3] == 0){
                status_arr[3] = 4;
                time_arr[3] = 1;
            }
        }
        //printf("%d %d %d %d\n", status_arr[0],status_arr[1],status_arr[2],status_arr[3]);

        last_status = packet.b;
        
        for(int i =0 ; i <4; i++){
            if (time_arr[i] == 20){
                time_arr[i] = 0;
                /*if (status_arr[i] == 4){
                   status_arr[i] = 0; 
                }*/
                status_arr[i] = 0; 
            }
            if (time_arr[i] > 0 ){
                time_arr[i]++;
            }
        }

        note_position_t a ={location_4[0],location_3[0],location_2[0],location_1[0],\
                                location_4[1],location_3[1],location_2[1],location_1[1],\
                                location_4[2],location_3[2],location_2[2],location_1[2],\
                                location_4[3],location_3[3],location_2[3],location_1[3]};

        node_status_t b ={status_arr[0],status_arr[1],status_arr[2],status_arr[3]};
        

        set_ball_position(&a);
        set_ball_status(&b);



        //printf("%d\n",i);
        update_array(location_1,4);
        //printf("======");
        update_array(location_2,4);
        //printf("======");
        update_array(location_3,4);
        //printf("=======");
        update_array(location_4,4);
        //printf("\n");
        
  
        end = clock(); // get the end time
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

        if (cpu_time_used < 0.005) {
            // wait for the remaining time
            double remaining_time = 0.0045 - cpu_time_used;
            
        
            usleep(remaining_time * 1000000);
        }

        start = clock(); // update the start time for the next iteration
        if(i > 42200){
            pthread_cancel(network_thread);
            //pthread_join(network_thread,NULL);

            break;
        }
        
    }
    pthread_join(network_thread,NULL);
    score = (int)roundf(total_score);
    printf("%d\n",score);
    int first_decimal = score / 1000 << 12;
    int second_decimal = (score % 1000 ) / 100  << 8;
    int third_decimal = (score % 100 ) / 10 <<4;
    int fourth_decimal =(score % 10);
    score = first_decimal + second_decimal + third_decimal + fourth_decimal;
    printf("%d\n",score);
    //usleep(10000000);
    }
    return 0;
}

void *network_thread_f(void *arg)
{
  /*char *PCMName = (char *) arg;
  printf("[INFO] Now playing ...\n");
  play_PCM(PCMName);
  printf("[INFO] Music over, quitting...\n");*/
  char cmd[512];
  sprintf(cmd, "./MyPlayer nggyu.MP3");
  if(system(cmd) !=0 )
  {
    return 1;
  }
  for(int i = 0; i <10000; i++){
    if (i %200 == 0){
        printf("%d",i);
    }
  }

  return NULL;
}
