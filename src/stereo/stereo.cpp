/*************************************************

 Purpose:
   These classes will contain stereoalgorithms
   that generates disparity maps.

 Author:
   Daniel Bengtsson 2002, daniel@bengtssons.info

 Version:
   $Id: stereo.cpp,v 1.11 2006/11/14 20:55:15 cygnus78 Exp $

*************************************************/

#include "stereo.h"

#include "../gui/Peru.h"

Stereo::Stereo(string left,
	       string right,
	       string out,
	       bool m) :
  preFilters(0),
  postFilters(0)
{ 

  if(ccv::debug) std::cerr << "Running Stereo constructor\n";

  iterations=0; 

  memory=m;

  dispI=0; leftI=0; rightI=0; ground=0;

  status=false;

  find_error = false;

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

  // We must check such that the images has the same size
  CvSize imageSizeR;
  cvGetImageRawData(rightI, 0, 0, &imageSizeR);
  int widthR = imageSizeR.width;
  int heightR = imageSizeR.height;

  if( (width != widthR) || (height != heightR) ) {
    if(ccv::debug) std::cerr << "ERROR - Image sizes not equal!\n";
    status = false;
    return;
  }

  preFilters = new vector<Filter*>;
  postFilters = new vector<Filter*>;

  Peru* gui = Peru::getInstance();
  connect(this,SIGNAL(percentageDone(double)),
	  gui, SLOT(setProgress(double)));
}

Stereo::~Stereo()
{ 
  if(ccv::debug) std::cerr << "Stereo main destructor\n";
  zapImg(dispI);
  zapImg(leftI);
  zapImg(rightI);
  zapImg(ground);

  for_each( postFilters->begin(), postFilters->end(), ccv::delete_object());
  zap(postFilters);
  for_each( preFilters->begin(), preFilters->end(), ccv::delete_object());
  zap(preFilters);

  if(ccv::debug) std::cerr << "Destructing Stereo\n";
}

// Returns -1 if errormeasuring is not used or the
// error otherwise.  
double Stereo::start() 
{ 
  double error = ccv::NOTUSED;

  if(status) {

    // STEP 1
    preProcess();
    
    if(ccv::debug) std::cerr << "Running stereo algorithm...\n";
    if(ccv::debug) std::cerr << "Left = " << left_file << endl
			     << "Right = " << right_file << endl
			     << "Out = " << out_file << endl;

    // STEP 2
    if(!calculateDisparity()) {
      ccv::ERRFLAG = true;
      return ccv::ERROR;
    }
    
    if(find_error)
      error = findError();
    
    // STEP 3
    postProcess();

    if(writeToDisk) saveDisparityImage();
    
  }
  
  zapImg(leftI);
  zapImg(rightI);
  
  if(ccv::debug) std::cerr << "Number of iterations made: " << iterations << "\n";
  return error; 
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

double
Stereo::findError()
{
  if(ccv::debug) std::cerr << "Stereo::findError\n";
  if(ccv::debug) std::cerr << "ground = " << ground << "\n";

  if(!ground || !dispI) {
    if(ccv::debug) std::cerr << "ERROR - Ground or Disp not available\n";
    return -1;
  }
  
  if(ground->width != dispI->width ||
     ground->height != dispI->height ) {
    if(ccv::debug) std::cerr << "ERROR - Ground truth has wrong dimensions\n";
    return -1;
  }
  
  double errsum=0;
  for(int x=0; x<dispI->width; x++)
    for(int y=0; y<dispI->height; y++) 
      if(*ccv::pixel_u(ground,x,y)!=255) // Values without known disparity
	errsum += pow( static_cast<double>
		       (*ccv::pixel_u(ground,x,y) - *ccv::pixel_u(dispI,x,y)), 2);
  
  errsum /= (dispI->width*dispI->height); 
  
  return errsum;
}

void
Stereo::loadGround(const char* filename)
{
  if(ccv::debug) std::cerr << "Stereo::loadGround\n";

  zapImg(ground);
  if(ccv::debug) std::cerr << "ground = " << ground << "\n";

  if(strcmp(filename, "")!=0) {
    ground = cvLoadImage(filename, 0);
  }
  else
    if(ccv::debug) std::cerr << "ERROR - No filename\n";
}

