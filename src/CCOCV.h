// -*- mode: C++; -*-
/*************************************************

 Purpose:
   Class CCOCV will take calibration inputimages 
   and calculate the cameras intrinsic parameters.
   Using Intels OpenCV

 Author:
   Daniel Bengtsson 2002, daniel@bengtssons.info

 Version:
   $Id: CCOCV.h,v 1.11.2.1 2006/11/12 18:28:38 cygnus78 Exp $

*************************************************/

#ifndef CCOCV_H
#define CCOCV_H

#include <opencv/cv.h>

#include "Peru.h"
//#include "SortCorners.h"
#include "ccv.h"

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

const int MAXFILES = 20;
const int CORRECT_IMAGE = 1;
const int FAILED_IMAGE = -1;

//! Holds all camera parameters
struct CameraParams {
  double focalLength[2];
  double distortion[4];
  float distortionf[4]; 
  double principalPoint[2];
  double matrix[9];
  float matrixf[9];
  double* rotMatr;
  double* transVect;

};

//! All settings to the TopHat function
struct sTopHatSettings {
  int iterations;
  int shape;
  int xsize;
  int ysize;
  int threshold;
};

class CCOCV                 //!< abbr: Camera Calibration OpenCV
{
 public:
  CCOCV();                  //!< Constructor is silent for now
  ~CCOCV();                 //!< Hmm destruction, danger, horror, terror...
  
  void setEtalonSize        //!< Set number of inner corners on chessboard 
    (int x, int y);  

  void printCorners(int nr);

  void printObjectPoints(int no_images);

  void setDimension         //!< Set the x an y dimension of the chesstiles
    (double x, double y);   

  int findCorners           //!< Find chessboard corners in image, and initializes
    (int& corners_found);

  void addFileName          //!< Add filenames of calibration images
    (string name);

  void removeFileName       //!< Remove filenames of calibration images
    (string name);

  int getNumberOfFilesInList(); //!< Selfexplained
  void clearFileList()      //!< Empty the list of files to be calibrated on
  { filenames.clear(); }

  void setFileList( QStringList list ) 
  { filenames = list; }

  QStringList getFiles();   //!< Get the list of files of calibration images


  CameraParams getParams()  //!< Returns the camera-parameters struct
  { return cp; }

  void printParams          //!< Prints all found parameters to stdout
    (int no_images);    

  void saveParams           //!< Saves found parameters to file with name file
    (const char* file);     

  bool loadParams           //!< Load parameters from file with name file
    (const char* file);     

  IplImage* undistortImage  //!< Takes an inputimage and undistort 
    (string srcImageName,   //!< using found parameters
     bool write);  

  IplImage* rectifyImage    //!< Takes an inputimage and undistort 
    (string srcImageName,   //!< using found parameters
     bool write);  

  void setSort(bool b);     //!< Sets value of sortCorners
  void setWth(bool b);      //!< Shall we try with white top hat

  int getImageSizeX();

  int getImageSizeY();

  QPointArray getCorners
  (const QSize& size);      //!< Get the found corners
  
  void drawCorners          //!< Draw found corners on montageimage
    ( string filename,
      int xgap, int pointsize,
      int nx, 
      int bw, int bh,
      int ow, int oh,
      bool drawFailed=false);

  void setTopHatSettings
    (int iterations, 
     int shape,
     int xsize,
     int ysize,
     int threshold);

  void setCameraParams
  (struct CameraParams cp);

  IplImage* trialCalib      //!< Finds corner in one image to test the preproc.
  (int& corners_found);

 private:
  bool sortCorners;         //!< Indicates if we should run improved sort corners
  bool wth;                 //!< Perform white top hat on difficult images
  CameraParams cp;          //!< Holds the calculated cameraparameters
  bool initialized;         //!< Is the calibration initialized for all images
  bool calibrated;          //!< Have a calibration been successfully performed
  CameraParams camP;        //!< Camera parameters will be stored here
  CvSize etalon_size;       /*! Number of inner corners per chessboard 
			      row and column. The width (the number of columns) 
			      must be less or equal to the height (the number 
			      of rows) */
  CvPoint2D32f* corners;    //!< Pointer to the corner array found
  CvPoint2D64d* allcorners; //!< Corners found in correct calibration images
  CvPoint2D64d* failcorners;//!< Corners found in failed calibration images
  CvMat* objectPoints;      //!< Coordinates of the chessboard
    
  int*   numPoints;         //!< Number of points in each image
  int* corner_count;        //!< Numbers of corners found... 
  int totalFailedCorners;   //!< Total number of failed corners found
  CvSize imageSize;         //!< Size of images
  IplImage* rgb_image;      //!< Source chessboard view; must have IPL_DEPTH_8U
  IplImage* gray_image;     //!< Source chessboard view; must have IPL_DEPTH_8U 
  IplImage* tmpwth;         //!< TopHat transformed image

  QStringList filenames;    //!< Filenames for calibration images         

  double dimx,dimy;         //!< The dimension of the chesstiles in x any y 

  int t_no_images;          //!< Total number of images tried

  int images_processed;     //!< Number of images that has curr been searched 

  int failed_index;         //!< Keeps track of the failed images

  int no_images;            //!< Images correctly processed

  int* correct;             //!< Indexes of correct images
  int* failed;              //!< Indexes of failed images

  struct sTopHatSettings* ths; 

  int findCorners2          //!< Find chessboard corners in image 
    (int& corners_found,
     bool singleTrial=false);

  void whiteTopHat          //!< Perform white top hat on source
    (IplImage* source,
     struct sTopHatSettings*);   

  void complement
    (IplImage* source);     //!< Calculates negative/positive of source

  void initializeCalibration(); //!< Initializes some variables before calibration
  
  void convertParametersToFloat();
};

#endif
