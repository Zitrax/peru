// -*- mode: C++; -*-
/*************************************************

 Purpose:
   Subclasses of Stereo will contain stereo-
   algorithms that generates disparity maps.

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: stereo.h,v 1.2 2003/09/05 12:15:12 cygnus78 Exp $

*************************************************/

#ifndef STEREO_H
#define STEREO_H

#include "../ccv.h"
#include <ipl/ipl.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream> 
#include <math.h>

#include "../CmdLine.h"
#include "../Filter.h"

class Stereo 
{

 protected:
 
  CCmdLine cmdLine;   
  string left_file, right_file, out_file;
  CvSize imageSize;                      //! Size of images

  int iterations;

  int times;                             //!< Number of times to run

  int width, height;                     //!< Resolution of image

  bool memory;                           /*!< Indicates wheter we have 
					   the images in memory or on disk*/
  bool writeToDisk;                      //!< Write disparity maps to disc

  IplImage* leftI;                       //!< Left image frame
  IplImage* rightI;                      //!< Right image frame
  IplImage* dispI;                       //!< The resulting disparity image
  IplImage* ground;                      //!< Ground Truth Image

  bool status;                           //!< Set to true if we have loaded images

  vector<Filter*>* preFilters;           //!< Filters applied before algorithm
  vector<Filter*>* postFilters;          //!< Filters applied after algorithm

  void saveDisparityImage();             //!< Call this when a disparity image has been
                                         //!< calculated

  void postProcess();                    //!< Median filtering
  void preProcess();                     //!< Blur and histogram equalization  
  void histogramEqualize();              //!< Histogram equalization (1 - 3 channels)
  void blurImages();                     //!< Gaussian smoothing
  void medianFilter(IplImage* img, 
		    int size);           //!< Noise removal by median filtering
  double findMean(IplImage* img);        //!< Finds meanvale of pixels (fist channel if not 3)
  void addC(IplImage* img, int value);   //!< Adds a constant to an image

 public:
  Stereo(string left,                    //!< Constructor
	 string right,
	 string out,
	 bool m=false); 

  virtual ~Stereo();                     //!< Destructor (virtual)

  bool start();                          //!< Start the algorithm

  void setImages(IplImage* l,            //!< Sets the two images to use
		 IplImage* r); 
  void loadImages(char* l, char* r);     //!< Load images from filenames
  void unSetImages();                    //!< No images in memory

  IplImage* getDisparityImage();         //!< returns the calculated disp.img.

  void setDisparityToDisk(bool e);       //!< Turn on disk writing of frames
  virtual bool calculateDisparity        //!< Should implement an algorithm
    () = 0;                              // pure virtual -> ABSTRACT CLASS

  void setOutFileName(string out);  
  void setFileNames(string left,
		    string right);
  bool getStatus();                      //!< Status indicates startable

  void addPreFilter(Filter* f);          //!< Adds a filter to prefilter vector
  void addPostFilter(Filter* f);         //!< Adds a filter to postfilter vector

  IplImage** leftImagePointer();         //!< Get pointer to the left image
  IplImage** rightImagePointer();        //!< Get pointer to the right image

  void loadGround(const char* filename);
  double findError();                    //!< Finds mse between contrast stretched dispI and ground
};

#endif
