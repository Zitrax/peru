/*************************************************

 Purpose:
   These classes will contain stereoalgorithms
   that generates disparity maps.

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: stereo.cpp,v 1.4 2003/09/14 18:24:00 cygnus78 Exp $

*************************************************/

#include "stereo.h"

Stereo::Stereo(string left,
	       string right,
	       string out,
	       bool m){ 
  if(ccv::debug) std::cerr << "Running Stereo constructor\n";
  iterations=0; 

  memory=m;

  dispI=0; leftI=0; rightI=0;

  status=false;

  left_file  = left;
  right_file = right;
  out_file   = out;

  if(!memory) {
    if(ccv::debug) std::cerr << "Loading images...\n";
    leftI  = cvvLoadImage(left_file .c_str());  
    rightI = cvvLoadImage(right_file.c_str());  
    if( leftI  == 0 ) if(ccv::debug) std::cerr << "ERROR - left image\n";
    if( rightI == 0 ) if(ccv::debug) std::cerr << "ERROR - right image\n";
    if( leftI != 0 && rightI != 0 )  status = true; 
    else return;
    if(ccv::debug) std::cerr << "Loaded!\n";
  }

  cvGetImageRawData(leftI, 0, 0, &imageSize);         // Get size of image
  width = imageSize.width;
  height = imageSize.height;

  preFilters = new vector<Filter*>;
  postFilters = new vector<Filter*>;
  
}

Stereo::~Stereo()
{ 
  if(ccv::debug) std::cerr << "Stereo main destructor\n";
  zapImg(dispI);
  zapImg(leftI);
  zapImg(rightI);

  zap(postFilters);
  zap(preFilters);

  if(ccv::debug) std::cerr << "Destructing Stereo\n";
}
  
bool Stereo::start() 
{ 
  if(status) {

    // STEP 1
    preProcess();
    
    if(ccv::debug) std::cerr << "Running stereo algorithm...\n";
    if(ccv::debug) std::cerr << "Left = " << left_file << endl
			     << "Right = " << right_file << endl
			     << "Out = " << out_file << endl;

    // STEP 2
    if(!calculateDisparity())
      return false;
    

    // STEP 3
    postProcess();

    if(writeToDisk) saveDisparityImage();
    
  }
  
  zapImg(leftI);
  zapImg(rightI);
  
  if(ccv::debug) std::cerr << "Number of iterations made: " << iterations << "\n";
  return true; 
}

void
Stereo::setImages(IplImage* l, IplImage* r) 
{
  zapImg(leftI);
  zapImg(rightI);
  leftI = l; rightI = r;
  memory = true;
  if(ccv::debug) std::cerr << "Stereo class has received image pointers\n";
}

void
Stereo::unSetImages() { memory=false; }

void
Stereo::loadImages(char* l, char* r)
{
  if(ccv::debug) std::cerr << "Loading images\n";
  zapImg(leftI);
  zapImg(rightI);
  left_file = l; right_file = r;
  leftI = cvvLoadImage(left_file.c_str());  
  rightI = cvvLoadImage(right_file.c_str());  
}

void
Stereo::saveDisparityImage()
{
  cvvSaveImage( out_file.c_str(), dispI );
}

void
Stereo::setOutFileName(string out) { out_file = out; }

bool
Stereo::getStatus() { return status; }

void
Stereo::blurImages()
{
  // Create temporary images
  IplImage* tmpL;
  IplImage* tmpR;
  tmpL = cvCloneImage( leftI );        
  tmpR = cvCloneImage( rightI );  

  cvSmooth(tmpL, leftI, CV_GAUSSIAN, 3, 3); 
  cvSmooth(tmpR, rightI, CV_GAUSSIAN, 3, 3); 

  zapImg(tmpL);
  zapImg(tmpR);
}

void
Stereo::preProcess()
{
  if(ccv::debug) std::cerr << "I(stereo): " << right << "\n";

  if(preFilters->size()>0) {
    vector<Filter*>::iterator p = preFilters->begin();
    for(p; p!=preFilters->end(); p++) {
      static_cast<Filter*>(*p)->apply(leftI);
      static_cast<Filter*>(*p)->apply(rightI);
    }
  }
}

void
Stereo::postProcess()
{
  if(postFilters->size()>0) {
    vector<Filter*>::iterator p = postFilters->begin();
    for(p; p!=postFilters->end(); p++) 
      static_cast<Filter*>(*p)->apply(dispI);
  }
}

void 
Stereo::medianFilter(IplImage* img, int size)
{
  IplImage* tmpD;
  tmpD = cvCloneImage( img );  
  
  cvSmooth( tmpD, img, CV_MEDIAN, size, size);

  zapImg(tmpD);
}

void
Stereo::setDisparityToDisk(bool e) { writeToDisk = e; }

IplImage* 
Stereo::getDisparityImage() { return dispI; }

void
Stereo::addPreFilter(Filter* f)
{
  preFilters->push_back(f);
  if(ccv::debug) std::cerr << "Prefilter added\n";
}

void
Stereo::addPostFilter(Filter* f)
{
  postFilters->push_back(f);
  if(ccv::debug) std::cerr << "Postfilter added\n";
}

IplImage**
Stereo::leftImagePointer() { return &leftI; }

IplImage**
Stereo::rightImagePointer() { return &rightI; }

void
Stereo::setFileNames(string left, string right)
{
  if(ccv::debug) std::cerr << "Stereo::setFileNames\n";
  left_file = left; 
  right_file = right;
}
