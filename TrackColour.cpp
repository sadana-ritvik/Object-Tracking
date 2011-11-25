#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

//definitions of ROI values
#define centerX 270
#define centerY 210
#define ROISTARTX 310
#define ROISTARTY 230
#define ROIENDX 330
#define ROIENDY 250
#define ROIHEIGHT 20
#define ROIWIDTH 20


using namespace std;

IplImage* GetThresholdedImage(IplImage* img, CvScalar hsv_min, CvScalar hsv_max)
{
  // Convert the image into an HSV image
  IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
  cvCvtColor(img, imgHSV, CV_BGR2HSV);

  IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
  cvSmooth( imgHSV, imgHSV, CV_BLUR, 27, 0, 0, 0 );
  
  
  cvInRangeS(imgHSV, hsv_min, hsv_max, imgThreshed);

  cvReleaseImage(&imgHSV);

  return imgThreshed;
}


int main()
{
  CvCapture* capture = 0;
  capture = cvCaptureFromCAM(0);	

  int framecount = 0;
  
  if(!capture)
  {
    printf("Could not initialize capturing...\n");
    return -1;
  }
  
  
  int choice;
  cout<<"What do you want to do?"<<endl;
  cout<<"1. Control the mouse."<<endl;
  cout<<"2. Control arrow keys."<<endl;
  
  while(1)
  {
    cout<<"Enter your choice: ";
    cin>>choice;
    if( choice < 1 || choice > 2 )
    {
      cout<<"Invalid output. Try again."<<endl;
    }
    else break;
  }
  sleep(1);
  if(choice == 1)
    cout<<"Starting mouse control"<<endl;
  else if(choice == 2)
    cout<<"Starting keyboard control"<<endl;
  
  //The calibration code goes here.
    cvNamedWindow("video");
    IplImage* frame = 0;
    while(true)
    {
      frame = cvQueryFrame(capture);
      //drawing the yellow rectangle affects the color values in the ROI. So, drawing the rectangle just outside the ROI
      cvRectangle(frame, cvPoint(ROISTARTX - 1, ROISTARTY - 1), cvPoint(ROIENDX + 1, ROIENDY + 1), cvScalar(0, 255, 255));
      cvShowImage("video", frame);
      int input = cvWaitKey(1);
//       cout<<input<<endl;
      if(input == 32 || input == 1048608)
	break;
    }
    
    //Now we got the frame with the object to be tracked in the rectangle. We need to get the HSV value range for the object.
    cvSetImageROI(frame, cvRect(ROISTARTX, ROISTARTY, ROIHEIGHT, ROIWIDTH));
    
    IplImage *sub_img = cvCreateImageHeader(cvSize( ROIHEIGHT, ROIWIDTH ), frame->depth, frame->nChannels);
    sub_img->origin = frame->origin;
    sub_img->widthStep = frame->widthStep;
    sub_img->imageData = frame->imageData +
			 ROISTARTY * frame->widthStep +
			 ROISTARTX * frame->nChannels;
    
    CvScalar hsv_avg, hsv_sdv;
    
    //Get the average and standard deviation of HSV values for the recognized object
    cvAvgSdv(frame, &hsv_avg, &hsv_sdv, 0);
    
    //handwritten code for getting average of HSV values
// 	    double avg[3];
// 	    double total[3];
// 	    double sd[3];
// 	    int pixelcount = 0;
// 	    avg[0] = avg[1] = avg[2] = 0;
// 	    total[0] = total[1] = total[2] = 0;
// 	    for( int y=0; y<sub_img->height; y++ ) 
// 	    {
// 	      uchar* ptr = (uchar*) ( sub_img->imageData + y * sub_img->widthStep );
// 	      for( int x=0; x<sub_img->width; x++ ) 
// 	      {
// 		avg[0] += ptr[3*x];
// 		avg[1] += ptr[3*x+1];
// 		avg[2] += ptr[3*x+2];
// 		lastY
// 		pixelcount++;
// 	      }
// 	    }

	    
// 	    double ROIavg[4];
	    int Hbuckets[256];
	    int Sbuckets[256];
	    int Vbuckets[256];
	    int Hbucketindex = 0;
	    int Sbucketindex = 0;
	    int Vbucketindex = 0;
// 	    ROIavg[0] = (double)(avg[0]/pixelcount);
// 	    ROIavg[1] = (double)(avg[1]/pixelcount);
// 	    ROIavg[2] = (double)(avg[2]/pixelcount);
// 	    ROIavg[3] = 0;

	    for(int i = 0; i < 256; i++)
	    {
	      Hbuckets[i] = 0;
	      Sbuckets[i] = 0;
	      Vbuckets[i] = 0;
	    }
	    for( int y=0; y<sub_img->height; y++ ) 
	    {
	      uchar* ptr = (uchar*) ( sub_img->imageData + y * sub_img->widthStep );
	      for( int x=0; x<sub_img->width; x++ ) 
	      {
		Hbucketindex = (int)ptr[3*x];
		Hbuckets[Hbucketindex] ++;
		Sbucketindex = (int)ptr[3*x + 1];
		Sbuckets[Sbucketindex] ++;
		Vbucketindex = (int)ptr[3*x + 2];
		Vbuckets[Vbucketindex] ++;
// 		total[0] += (ROIavg[0] - ptr[3*x]) * (ROIavg[0] - ptr[3*x]);
// 		total[1] += (ROIavg[1] - ptr[3*x + 1]) * (ROIavg[1] - ptr[3*x + 1]);
// 		total[2] += (ROIavg[2] - ptr[3*x + 2]) * (ROIavg[2] - ptr[3*x + 2]);
	      }
	    }
	    
	    int Hmaxbucket = 0;
	    int Hmaxval = 0;
	    int Smaxbucket = 0;
	    int Smaxval = 0;
	    int Vmaxbucket = 0;
	    int Vmaxval = 0;
	    for(int i = 0; i < 256; i++)
	    {
	      cout<<Hbuckets[i]<<" ";
	      if(Hbuckets[i] > Hmaxval)
	      {
		Hmaxval = Hbuckets[i];
		Hmaxbucket = i;
	      }
	    }
	    cout<<endl<<endl;
// 	    cout<<Hmaxbucket<<" : "<< Hmaxval<<endl;
	    
	    for(int i = 0; i < 256; i++)
	    {
	      cout<<Sbuckets[i]<<" ";
	      if(Sbuckets[i] > Smaxval)
	      {
		Smaxval = Sbuckets[i];
		Smaxbucket = i;
	      }
	    }
	    cout<<endl<<endl;
// 	    cout<<Smaxbucket<<" : "<< Smaxval<<endl;
	    
	    for(int i = 0; i < 256; i++)
	    {
	      cout<<Vbuckets[i]<<" ";
	      if(Vbuckets[i] > Vmaxval)
	      {
		Vmaxval = Vbuckets[i];
		Vmaxbucket = i;
	      }
	    }
	    cout<<endl<<endl;
// 	    cout<<Vmaxbucket<<" : "<< Vmaxval<<endl;
	    
/* Below is a sample count value for all the HSV values (using a purple object). Apparently, the values are spread over a region. We need a code to find the largest continuous sequence of detected color values. Attempting it below this sample data.
  * 39 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 3 3 2 4 3 7 10 6 7 7 4 6 13 8 11 12 9 11 11 10 10 9 9 6 12 10 7 14 10 10 6 11 11 4 2 3 3 8 3 5 3 6 3 5 4 4 1 2 0 1 0 3 1 1 3 1 3 0 1 3 3 2 0 1 0 1 0 2 1 1 0 0 0 1 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
  */
//Code to find the longest sequence of non-zero pixel values

	    int Hseqcount = 0, Hmaxseqcount = 0;
	    int Hindex = 0;
	    int Hmaxindex = 0;
	    int Hflag = 0;
	    
	    // 0 0 0 0 1 1 0 0 1 0 1 0 1 1 4 2 8 4 5 2 0 2 3 5 4 0 0 1 0 1 1 0 1 1 0 0 0 0 0 0 0 0 0 
	    for(int i = 0; i < 256; i++)
	    {
	      if(Hbuckets[i] == 0)
	      {
		if(Hflag == 0)
		  Hflag = 1;
		else if(Hflag == 1)
		{
		  if(Hseqcount > Hmaxseqcount)
		  {
		    Hmaxseqcount = Hseqcount;
		    Hmaxindex = Hindex;
		    
		  }
		  Hseqcount = 0;
		}
	      }
	      
	      else if(Hbuckets[i] > 0)
	      {
		if(Hseqcount == 0)
		  Hindex = i;
		Hflag = 0;
		Hseqcount++;
	      }
	    }
	    
	    if(Hseqcount > Hmaxseqcount)
	    {
	      Hmaxseqcount = Hseqcount;
	      Hmaxindex = Hindex;
	      
	    }
	    int Hrangestart = Hmaxindex;
	    int Hrangeend = Hmaxindex + Hmaxseqcount;
	    cout<<"HStart : "<<Hrangestart<<" ; End : "<<Hrangeend<<endl;
	    
	    int Htotalrange = Hrangeend - Hrangestart;
	    int Hn = (Htotalrange / 10) + 1;
	    int Hrangebuckets[Hn];
	    for(int i = 0; i < Hn; i++ )
	      Hrangebuckets[i] = 0;
	    int Hrangebucketcount = 0;
	    int j = 0;
	    for(int i = Hrangestart; i <= Hrangeend; i++)
	    {
	      Hrangebuckets[j] += Hbuckets[i];
	      Hrangebucketcount++;
	      if(Hrangebucketcount == 9)
	      {
		j++;
		Hrangebucketcount = 0;
	      }
	    }
	    
	    int Hmaxrangebucket = 0;
	    int Hmaxrangestart = 0;
	    for(int i = 0; i < Hn; i++)
	    {
	      if(Hrangebuckets[i] > Hmaxrangebucket)
	      {
		Hmaxrangebucket = Hrangebuckets[i];
		Hmaxrangestart = i;
	      }
	    }
	    
	    
	    
	    
	    int Sseqcount = 0, Smaxseqcount = 0;
	    int Sindex = 0;
	    int Smaxindex = 0;
	    int Sflag = 0;
	    for(int i = 0; i < 256; i++)
	    {
	      if(Sbuckets[i] == 0)
	      {
		if(Sflag == 0)
		{
		  Sflag = 1;
		}
		else if(Sflag == 1)
		{
		  if(Sseqcount > Smaxseqcount)
		  {
		    Smaxseqcount = Sseqcount;
		    Smaxindex = Sindex;
		    
		  }
		  Sseqcount = 0;
		}
	      }
	      
	      else if(Sbuckets[i] > 0)
	      {
		if(Sseqcount == 0)
		{
		  Sindex = i;
		}
		Sflag = 0;
		Sseqcount++;
	      }
	    }
	    
	    if(Sseqcount > Smaxseqcount)
	    {
	      Smaxseqcount = Sseqcount;
	      Smaxindex = Sindex;
	      
	    }
	    
	    int Srangestart = Smaxindex;
	    int Srangeend = Smaxindex + Smaxseqcount;
	    cout<<"SStart : "<<Srangestart<<" ; End : "<<Srangeend<<endl;
	    
	    int Stotalrange = Srangeend - Srangestart;
	    int Sn = (Stotalrange / 10) + 1;
	    int Srangebuckets[Sn];
	    for(int i = 0; i < Sn; i++ )
	      Srangebuckets[i] = 0;
	    int Srangebucketcount = 0;
	    j = 0;
	    for(int i = Srangestart; i <= Srangeend; i++)
	    {
	      Srangebuckets[j] += Sbuckets[i];
	      Srangebucketcount++;
	      if(Srangebucketcount == 9)
	      {
		j++;
		Srangebucketcount = 0;
	      }
	    }
	    
	    int Smaxrangebucket = 0;
	    int Smaxrangestart = 0;
	    for(int i = 0; i < Sn; i++)
	    {
	      if(Srangebuckets[i] > Smaxrangebucket)
	      {
		Smaxrangebucket = Srangebuckets[i];
		Smaxrangestart = i;
	      }
	    }
	    
	    int Vseqcount = 0, Vmaxseqcount = 0;
	    int Vindex = 0;
	    int Vmaxindex = 0;
	    int Vflag = 0;
	    for(int i = 0; i < 256; i++)
	    {
	      if(Vbuckets[i] == 0)
	      {
		if(Vflag == 0)
		  Vflag = 1;
		else if(Vflag == 1)
		{
		  if(Vseqcount > Vmaxseqcount)
		  {
		    Vmaxseqcount = Vseqcount;
		    Vmaxindex = Vindex;
		  }
		  Vseqcount = 0;
		}
	      }
	      
	      else if(Vbuckets[i] > 0)
	      {
		if(Vseqcount == 0)
		  Vindex = i;
		Vflag = 0;
		Vseqcount++;
	      }
	    }
	    
	    if(Vseqcount > Vmaxseqcount)
	    {
	      Vmaxseqcount = Vseqcount;
	      Vmaxindex = Vindex;
	    }
	    
	    int Vrangestart = Vmaxindex;
	    int Vrangeend = Vmaxindex + Vmaxseqcount;
	    cout<<"VStart : "<<Vrangestart<<" ; End : "<<Vrangeend<<endl;
	    
	    int Vtotalrange = Vrangeend - Vrangestart;
	    int Vn = (Vtotalrange / 10) + 1;
	    int Vrangebuckets[Vn];
	    for(int i = 0; i < Vn; i++ )
	      Vrangebuckets[i] = 0;
	    int Vrangebucketcount = 0;
	    j = 0;
	    for(int i = Vrangestart; i <= Vrangeend; i++)
	    {
	      Vrangebuckets[j] += Vbuckets[i];
	      Vrangebucketcount++;
	      if(Vrangebucketcount == 9)
	      {
		j++;
		Vrangebucketcount = 0;
	      }
	    }
	    
	    int Vmaxrangebucket = 0;
	    int Vmaxrangestart = 0;
	    for(int i = 0; i < Vn; i++)
	    {
	      if(Vrangebuckets[i] > Vmaxrangebucket)
	      {
		Vmaxrangebucket = Vrangebuckets[i];
		Vmaxrangestart = i;
	      }
	    }
	    
	    
	    
	    
// 	    Hrangestart = (Hmaxrangestart * 10) + Hrangestart;
// 	    Hrangeend = Hrangestart + 10;
//code to get the color range ends here.
// 	    cout<<pixelcount<<endl;
// 	    sd[0] = total[0]/pixelcount;
// 	    sd[1] = total[1]/pixelcount;
// 	    sd[2] = total[2]/pixelcount;
// 	    
// // 	    cout<<"ROI SD:  "<<sd[0]<<" "<<sd[1]<<" "<<sd[2]<<endl;
// 	    
// 	    sd[0] = fabs(sd[0]);
// 	    sd[1] = fabs(sd[1]);
// 	    sd[2] = fabs(sd[2]);
// 	    
// 	    sd[0] = sqrt(sd[0]);//fabs(sqrt(sd[0]));
// 	    sd[1] = sqrt(sd[1]);//fabs(sqrt(sd[1]));
// 	    sd[2] = sqrt(sd[2]);//fabs(sqrt(sd[2]));
// 	    
//   
    cvResetImageROI(frame);
    double avg_val[4], sdv_val[4];
    for(int i = 0; i < 4; i++)
    {
      avg_val[i] = hsv_avg.val[i];
      sdv_val[i] = hsv_sdv.val[i];
    }
        
     // Values 20,100,100 to 30,255,255 working perfect for yellow at around 6pm
     //Set min and max values for the object to be tracked
    double min[4];
    min[0] = Hrangestart; //*/avg_val[0] - hsv_sdv.val[0];
    min[1] = Srangestart;// - 50; //*/avg_val[1] - hsv_sdv.val[1] -50;
    min[2] = Vrangestart;// - 25; //*/avg_val[2] - hsv_sdv.val[2] - 50;
    min[3] = avg_val[3] - hsv_sdv.val[3];
    if(min[1] < 0)
      min[1] = 0.00;
    if(min[2] < 0)
      min[2] = 0.00;
    CvScalar hsv_min = cvScalar( min[0], min[1], min[2], min[3] );
//     CvScalar hsv_min = cvScalar( 0, 200, 200, 0 );
      
      
    double max[4];
    max[0] = Hrangeend; //*/avg_val[0] + hsv_sdv.val[0];
    max[1] = 255;//Srangeend;// + 50; //*/avg_val[1] + hsv_sdv.val[1] + 50; 
    max[2] = 255;//Vrangeend;// + 50; //*/avg_val[2] + hsv_sdv.val[2] + 50;
    max[3] = avg_val[3] + hsv_sdv.val[3];
    if(max[1] > 255)
      max[1] = 255.00;
    if(max[2] > 255)
      max[2] = 255.00;
    CvScalar hsv_max = cvScalar( max[0], max[1], max[2], max[3] );
//     CvScalar hsv_max = cvScalar( 180, 255, 255, 0);
 
    
    cout<<"HSV MIN: "<<hsv_min.val[0]<<" "<<hsv_min.val[1]<<" "<<hsv_min.val[2]<<" "<<endl;
    cout<<"HSV MAX: "<<hsv_max.val[0]<<" "<<hsv_max.val[1]<<" "<<hsv_max.val[2]<<" "<<endl;
//     cout<<"HSV AVG: "<<avg_val[0]<<" "<<avg_val[1]<<" "<<avg_val[2]<<endl;
//     cout<<"ROI AVG: "<<ROIavg[0]<<" "<<ROIavg[1]<<" "<<ROIavg[2]<<endl;
//     cout<<"ROI SD:  "<<sd[0]<<" "<<sd[1]<<" "<<sd[2]<<endl;
//     cout<<"HSV SDV: "<<hsv_sdv.val[0]<<" "<<hsv_sdv.val[1]<<" "<<hsv_sdv.val[2]<<endl;
    cvNamedWindow("ROI");
    cvShowImage("ROI", sub_img);
//   cvNamedWindow("video");
//   cvNamedWindow("thresh");
  
  int left = 0, right = 0, up = 0, down = 0;
  char ch;
  
  while(true)
  {
    frame = cvQueryFrame(capture);
    framecount++;

    if(!frame)
      break;
    

    IplImage* imgThresh = GetThresholdedImage(frame, hsv_min, hsv_max);

    // Calculate the moments to estimate the position of the object
    CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
    cvMoments(imgThresh, moments, 1);

    // The actual moment values
    double moment10 = cvGetSpatialMoment(moments, 1, 0);
    double moment01 = cvGetSpatialMoment(moments, 0, 1);
    double area = cvGetCentralMoment(moments, 0, 0);

    // Holding the last and current object positions
    static int posX = 100;
    static int posY = 100;

    int lastX = posX;
    int lastY = posY;

    posX = moment10/area;
    posY = moment01/area;
    
    if(posX <= 0)
      posX = lastX;
    if(posY <= 0)
      posY = lastY;
    
//     if( lastX > (posX + 50) || lastX < (posX - 50))
//       posX = lastX;
//     if(lastY > (posY + 50) || lastY < (posY - 50))
//       posY = lastY;
    //invert the sideways movement to get a mirror image of movement
    posX = 640 - posX;		//the camera doesn't give a mirror image. We need a mirror image to get the left-right directions correct.

    char command[50], clickcmd[50];
//     ch = cvWaitKey(1);
//     if(ch == ' ')
//       strcpy(clickcmd,"xdotool mousedown 1");
//     else
//       strcpy(clickcmd,"xdotool mouseup 1");
    sprintf(command, "xdotool mousemove %d %d", (int)posX*2, (int) (posY*1.66)); //converting 640x480 into 1280x800
//     cout<<posX<<"\t"<<posY<<endl;
    if(choice == 1)
    {
//       system(clickcmd);
      system(command);
    }
    else
    {
      int count = 0;
      
      //Convert  object positions into directions, and emulate the appropriate direction key
      left = 0, right = 0, up = 0, down = 0;
      if(posX > 250 && posX < 390 )
      {
	count ++;
	left = right = 0;
	system("xdotool keyup Left");
	system("xdotool keyup Down");
      }
      else if( posX < 250 )
      {
	count ++;
	left = 1;
	system("xdotool keyup Right");
	system("xdotool keydown Left");
      }
      else if( posX > 390 )
      {
	count ++;
	right = 1;
	system("xdotool keyup Left");
	system("xdotool keydown Right");
      }
      
      
      if( posY > 190 && posY < 270 )
      {
	count ++;
	up = down = 0;
	system("xdotool keyup Down");
	system("xdotool keyup Up");
      }
      else if( posY < 190 )
      {
	count ++;
	up = 1;
	system("xdotool keyup Down");
	system("xdotool keydown Up");
      }
      else if( posY > 270 )
      {
	count ++;
	down = 1;
	system("xdotool keyup Up");
	system("xdotool keydown Down");
      }
      
      cout<<"up = "<<up<<" down = "<<down<<" left = "<<left<<" right = "<<right<<" frame = "<<framecount<<endl;
    }
    
    /*
     * Center of the screen is somewhere around (270,210). Now the movement can be controlled by comparing current position with the approximate center value.
     */

    //Invert the positions again to get the rectangle around the cap in the output video
    posX = 640 - posX;

    cvRectangle(frame, cvPoint(posX + 10, posY - 10), cvPoint(posX - 10, posY + 10), cvScalar(0, 255, 255));
    cvShowImage("thresh", imgThresh);
    cvShowImage("video", frame);

    int c = cvWaitKey(10);

    if(c!=-1)
    {
      if( c == 27 || c == 1048603)
	break;
    }

    cvReleaseImage(&imgThresh);
    delete moments;

  }
//   system("clear");
  cvReleaseCapture(&capture);
//   cvReleaseImage(&frame);
  cvDestroyAllWindows();
  return 0;
}