/* ----------------------------------------------------------------------------
 * @file main.cpp
 * @brief A prototype self-driving assistant feature for lane detection and 
 *        departure warnings
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 * @resources 
 *  - Learning OpenCV3, Kaehler, Bradski
 *  - Sam Siewerts example codes
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/utility.hpp>
#include <pthread.h>
#include <sched.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>

#include "log.h"
#include "lane.h"
#include "ringbuf.h"

using namespace cv;
using namespace std;

enum {
  CAPTURE_THREAD, 
  PROCESS_THREAD,
  WRITE_THREAD,
  NUM_THREADS
};

// Pthread variables
pthread_t threads[NUM_THREADS];

typedef struct {
  int tid;
  void* payload;
} thread_params_t;

thread_params_t thread_params[NUM_THREADS];
pthread_attr_t rt_sched_attr[NUM_THREADS];
pthread_attr_t main_attr;
struct sched_param rt_param[NUM_THREADS];
struct sched_param main_param;

// lock-free SPSC ring buffers
RingBuffer<Mat> raw_buf = RingBuffer<Mat>(16); 
RingBuffer<Mat> annot_buf = RingBuffer<Mat>(16);
//RingBuffer<Mat, 8> raw_buf; //= RingBuffer<Mat>(8); 
//RingBuffer<Mat, 8> annot_buf; //= RingBuffer<Mat>(8);

// 
// interrupt handler for ctrl-c finish-up and output
//
static volatile int exit_signal_g = 0;
static void int_handler(int signum) {
  
  exit_signal_g = true;
}


/* @brief
 *
 */
void *capture_thread(void *param) {
  
  //
  // algorithm begin  
  //
  double start, end, elapsed = 0.0;
  unsigned int framecnt = 0;
  // wait time is 1 msec
  struct timespec wait_time = {0, 1*MSEC_TO_NSEC};

  thread_params_t *arg = (thread_params_t*) param;

  String *input_image = (String*) arg->payload;
  VideoCapture cap(*input_image);
  cap.set(CAP_PROP_POS_MSEC, 10000);

  Mat temp;

  while(!exit_signal_g) {

    start = get_time_msec();
    cap >> temp;
    if( temp.empty() ) {
      LOGSYS("capture_thread, cap empty, nframes: %i\n", framecnt);
      break;
    }

    while (!raw_buf.Put(temp)) { 
      if (nanosleep(&wait_time, NULL) < 0) {
        perror("capture_thread nanosleep");
      }
    }

    char user_input = waitKey(20);
    if ( user_input == 'q' ) break;

    framecnt++;
    end = get_time_msec();
    elapsed += end-start;
    if (elapsed > 100*MSEC_TO_SEC) {break;}
  }

  exit_signal_g = 1;
  cap.release();
  
  return nullptr;
}

void *process_thread(void *param) {

  Mat image, annotated;
  LaneDetector detector;
  double start, end;
  start = get_time_msec();

  thread_params_t *arg = (thread_params_t*) param;

  int *show_pipeline = (int *) arg->payload;

  while(!exit_signal_g) {

    if(raw_buf.Get(image)) {
      detector.input_image(image);
      detector.detect();
      detector.annotate();
      if (*show_pipeline) {
        detector.show();
      }
      detector.get_annot(annotated);
      while(!annot_buf.Put(annotated)) {;} 
    }

  }

  end = get_time_msec();

  unsigned int nframes = detector.get_frame_num();
  unsigned int lines = detector.get_lines_detected();
  double proc_time = detector.get_proc_elapsed();

  LOGP(
      "proc_thread: frames: %i, msec total: %6.2f, FPS: %6.2f\n", 
      nframes, end-start, nframes*1000/(end-start)
      );

  LOGP("proc_thread: lane lines detected: %i\n", lines);
  LOGP("processing time (msec): %6.2f\n", proc_time);

  return nullptr;
} 


void *write_thread(void* param) {

  Mat img;
  unsigned int i=0;
  String output_frame_path;
  stringstream ss;
  double start, end, elapsed = 0.0;

  thread_params_t *arg = (thread_params_t*) param;
  String* output_video = (String *) arg->payload;
  String output_folder = output_video->substr(0, output_video->find_last_of("/"));
  char number[20];
  number[19] = '\0';

  VideoWriter video_out(
    *output_video,
    CV_FOURCC('a','v','c','1'),
    30.0, 
    Size(1280, 720),
    true
  );

  while(!exit_signal_g) {

    start = get_time_msec();

    if (annot_buf.Get(img)) {
    
      //video_out.write(img); 
      sprintf(number, "%08d.jpg", i);
      ss << output_folder << "/" << number; 
      //cout << ss.str() << endl;
      imwrite(ss.str(), img);
      ss.str("");
      ss.clear();
      i++;
    }

    end = get_time_msec();
    elapsed += end-start;
  }
  
  LOGP("write_thread elapsed (msec): %6.2f\n", elapsed);
  LOGP("write_thread throughput (FPS): %6.2f\n", i*1000/elapsed);

  // clean up
  video_out.release();

  return nullptr;
}

//
// the main program
//
int main(int argc, char **argv) {

  // the keys for the command line arguments
  const String parser_keys =
    "{help h usage ? | | Print help message. }"
    "{input i  | input_video/clip1.avi      | Full filepath to input video.  }"
    "{output o | output_video/clip1_out.mp4 | Full filepath to output video. }"
    "{show     | 1 | Shows intermediate image pipeline steps. }"
    ;
  // variables extracted from the parser - application settings
  String input_video;
  String output_video;
  int show_pipeline;

  // 
  // command line parsing for application settings  
  //
  CommandLineParser parser(argc, argv, parser_keys);

  if (parser.has("help")) {
    parser.printMessage();
    return 0;
  }

  input_video = parser.get<String>("input");
  output_video = parser.get<String>("output");
  show_pipeline = parser.get<int>("show");

  if (show_pipeline) {
    cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("2", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("3", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("4", CV_WINDOW_AUTOSIZE);
  }
  

  signal(SIGINT, int_handler);

  // Begin pthreads setup
  int rc;

  for (int i=0; i<NUM_THREADS; i++) {
    rc = pthread_attr_init(&rt_sched_attr[i]);
    if (rc != 0) {
      perror("pthread_attr_init");
    }
  }

  // start the capture thread
  thread_params[CAPTURE_THREAD].tid = 1;
  thread_params[CAPTURE_THREAD].payload = (void*)(&input_video);

  pthread_create( &threads[CAPTURE_THREAD],
                  &rt_sched_attr[CAPTURE_THREAD],
                  capture_thread,
                  &thread_params[CAPTURE_THREAD]
                 );

  
  // start the processing thread
  thread_params[PROCESS_THREAD].tid = 2;
  thread_params[PROCESS_THREAD].payload = (void*)(&show_pipeline);

  pthread_create( &threads[PROCESS_THREAD],
                  &rt_sched_attr[PROCESS_THREAD],
                  process_thread,
                  &thread_params[PROCESS_THREAD]
                 );

  // start the video writing thread
  thread_params[WRITE_THREAD].tid = 3;
  thread_params[WRITE_THREAD].payload = (void*)(&output_video);

  pthread_create( &threads[WRITE_THREAD],
                  &rt_sched_attr[WRITE_THREAD],
                  write_thread,
                  &thread_params[WRITE_THREAD]
                 );

  
  for (int i=0; i<NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}




