// -*- mode: C++; -*-
/*************************************************

 Purpose:
   Subclasses of Stereo will contain stereo-
   algorithms that generates disparity maps.
   Stero is an abstract class.

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: stereo.h,v 1.6 2004/09/03 21:53:19 cygnus78 Exp $

*************************************************/

#ifndef STEREO_H
#define STEREO_H

#include "../ccv.h"
#include <opencv/cv.h>
#include <iostream> 
#include <math.h>
#include <string>
#include <vector>

#include "../Filter.h"

using namespace std;

class Stereo 
{

 protected:
 
  string left_file, right_file, out_file;
  CvSize imageSize;                      //!< Size of images

  int iterations;

  int times;                             //!< Number of times to run

  int width, height;                     //!< Resolution of image

  bool memory;                           /*!< Indicates wheter we have 
					   the images in memory or on disk*/
  bool writeToDisk;                      //!< Write disparity maps to disc

  bool find_error;                       //!< If we have ground truth to compare

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

  double start();                        //!< Start the algorithm

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
  double findError();                    /*!< Finds mse between contrast 
					   stretched dispI and ground */

  void setFindError(bool fe) 
  { find_error = fe; }
  bool getFindError() const              //!< Returns if we are calculating error agains groundtruth
  { return find_error; }
};

#endif
