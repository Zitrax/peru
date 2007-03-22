/*************************************************

 Purpose:
   Class CCOCV will take calibration inputimages 
   and calculate the cameras intrinsic parameters.
   Using Intels OpenCV

 Author:
   Daniel Bengtsson 2002, daniel@bengtssons.info

 Version:
   $Id: CCOCV.cpp,v 1.25 2007/03/22 23:42:39 cygnus78 Exp $

*************************************************/

#include "CCOCV.h"

CCOCV::CCOCV() : 
  sortCorners(false),
  wth(false),
  initialized(false),
  calibrated(false),
  corners(0),
  allcorners(0),
  failcorners(0),
  objectPoints(0),
  numPoints(0),
  rgb_image(0),
  gray_image(0),
  thresh(0),
  tmpwth(0)
{
  correct = new int[100];
  failed = new int[100];

  // Make the settings for tophat and apply some 
  // default values.
  ths = new sTopHatSettings;
  ths->iterations = 1;
  ths->shape      = 1;
  ths->xsize      = 9;
  ths->ysize      = 9;
  ths->threshold  = 220;
}

CCOCV::~CCOCV()
{
  zapArr(correct);
  zapArr(failed);
  zapArr(allcorners);
  zapArr(failcorners);
  zapArr(corners);
  zapArr(numPoints);
  zapArr(objectPoints);

  delete ths;

  zapImg(rgb_image);
  zapImg(thresh);
  zapImg(gray_image);
  zapImg(tmpwth);
}

void
CCOCV::setEtalonSize(int x, int y)
{
  etalon_size.width=x;
  etalon_size.height=y;
  if(ccv::debug) 
    std::cerr << "etalon_size set to x=" << x << " , y=" << y << "\n";
  zapArr(corners);
  corners = new CvPoint2D32f[x*y];
}

void
CCOCV::setDimension(double x,double y)
{
  dimx = x; dimy = y;
  if(ccv::debug) 
    std::cerr << "dimension set to x=" << x << " , y=" << y << "\n";
}

void
CCOCV::initializeCalibration()
{
  if(ccv::debug) std::cerr << "CCOCV::initializeCalibration\n";

  failed_index = 0;
  totalFailedCorners = 0;

  no_images = filenames.size();
  
  images_processed = 0;
  t_no_images = 0;

  int no_corners = etalon_size.width * etalon_size.height;
  
  zapArr(allcorners);
  zapArr(failcorners);
  allcorners    = new CvPoint2D64d[no_images*no_corners];
  failcorners   = new CvPoint2D64d[no_images*no_corners];
  
  initialized=true;
}

int 
CCOCV::findCorners(int& corners_found){

  int result = 0;         // Indicates state of findCorners2
  
  int no_corners = etalon_size.width * etalon_size.height;

  if(!(filenames.size()==0 && !initialized) ) {

  if(!initialized){       // Special treatment before the first image
    
    initializeCalibration();

  }

  result = findCorners2(corners_found);
  
  switch (result) {

  case 0:                          // We have traversed all images
    
    if(no_images==0) {             // If we never detected all corners
      if(ccv::debug) std::cerr << "Sorry no correct images\n" << endl; 
      calibrated=false;
      initialized=false;
      break;
    }
    else {
      if(ccv::debug) 
	std::cerr << "numPoints are going to be initialized..." << endl;
      if(ccv::debug) 
	std::cerr << "no_images=" << no_images << endl;
      
      zapArr(numPoints);
      numPoints = new int[no_images];
      for(int i=0;i<no_images;i++) numPoints[i]=no_corners;
      
      if(ccv::debug) 
	std::cerr << "numPoints initialized..." << endl;
      
      zapArr(objectPoints);
      objectPoints = new CvPoint3D64d[no_images*no_corners];

      for(int i=0;i<no_images;i++)               //number of images
	for(int j=0;j<etalon_size.height;j++)    //vertical component (Y)
	  for(int k=0;k<etalon_size.width;k++)   //horizontal component (X)
	    {
	      objectPoints[i*no_corners+(etalon_size.width-k-1)+
			   etalon_size.width*j].x = k*dimx;
	      objectPoints[i*no_corners+(etalon_size.width-k-1)+
			   etalon_size.width*j].y = j*dimy;
	      objectPoints[i*no_corners+(etalon_size.width-k-1)+
			   etalon_size.width*j].z = 0;
	    }
      //printObjectPoints(no_images);
    
      zapArr(cp.rotMatr);
      zapArr(cp.transVect);
      cp.rotMatrSize = no_images*9;
      cp.transVectSize = no_images*3;
      cp.rotMatr = new double[cp.rotMatrSize];
      cp.transVect = new double[cp.transVectSize];

      if(ccv::debug) std::cerr << "Calibrating...\n" ;
      if(ccv::debug) std::cerr 
          << "Size x=" 
          << imageSize.width << " y=" 
          << imageSize.height << endl;
      
      cvCalibrateCamera_64d( no_images, numPoints, imageSize, allcorners, 
			     objectPoints, &cp.distortion[0], cp.matrix, 
			     cp.transVect, cp.rotMatr, 0);
    
      calibrated = true;             // Should also check if successfull!
    
      cp.focalLength[0]    = cp.matrix[0];  // So this is actually stored twice
      cp.focalLength[1]    = cp.matrix[4];  // just for the confusion =)
      cp.principalPoint[0] = cp.matrix[2];  // (Or actually portability with
      cp.principalPoint[1] = cp.matrix[5];  //  older .ccv files )

      if(ccv::debug) std::cerr << "Calibrated!" << endl;

      cp.print();
      initialized=false;
    
      break;
    }
  case CORRECT_IMAGE:                // If corners were detected properly
    correct[images_processed] = t_no_images; 
    t_no_images++;
    images_processed++;
    // Fill allcorners with corners found in last image
    for(int i=0;i<no_corners;i++) {  
      allcorners[(images_processed-1)*no_corners+i].x = corners[i].x;
      allcorners[(images_processed-1)*no_corners+i].y = corners[i].y;
    }
    break;
  case FAILED_IMAGE:                 // Corners were not detected properly
    if(ccv::debug) std::cerr << "Failed image\n";
    if( abs(corners_found) > 0 ) { 
      failed[failed_index++] = t_no_images; 
      // Fill failcorners with corners found in last image
      for(int i=0;i<corners_found;i++) {  
	failcorners[totalFailedCorners].x = abs(corners_found);
	failcorners[totalFailedCorners+i+1].x = corners[i].x;
	failcorners[totalFailedCorners+i+1].y = corners[i].y;
      }
      totalFailedCorners += abs(corners_found)+1;
    }
    no_images--;
    t_no_images++;
    if(ccv::debug) std::cerr << "no_images = " << no_images << "\n";
    return 1;                      // Return normally
    break;
    
  } // End switch

  } 

  return result;

}

int
CCOCV::findCorners2(int& corners_found, bool singleTrial)
{
  if(ccv::debug) 
    std::cerr << "running findCorners()... filenames.size()= " 
	      << filenames.size() << "\n";
  if(filenames.size()>0){
    int etalon_points = etalon_size.width * etalon_size.height;

    /* Obviously it's important to declare
       how many points we are expected to
       find. (Update: OpenCV searches for
       max this number of corners. ) */
    corner_count = etalon_points; 
					
    if(ccv::debug) 
      std::cerr << "Loading image: '" << filenames.back() << "'\n";

    // Read last file from list
    zapImg( rgb_image );
    rgb_image = cvvLoadImage(filenames.back().latin1()); 

    // Remove from list when done
    if(!singleTrial) 
      filenames.pop_back();

    // Get size of image
    cvGetImageRawData(rgb_image, 0, 0, &imageSize);         

    // Create temporary image
    zapImg(thresh);
    thresh     = cvCreateImage(imageSize,IPL_DEPTH_8U, 1); 
    
    // Prepare gray image
    zapImg(gray_image);
    gray_image = cvCreateImage(imageSize,IPL_DEPTH_8U, 1); 

    if(ccv::debug) std::cerr << "Going to convert to gray...\n";
    //    iplColorToGray( rgb_image, gray_image );
    cvCvtColor(rgb_image,gray_image,CV_BGR2GRAY);

    // Make thresh contain copy of image
    zapImg(thresh);
    thresh = cvCloneImage( gray_image );

    if(ccv::debug) std::cerr << "Image depth = " << gray_image->depth << "\n";

    //    cvvSaveImage("gray_image.bmp",gray_image);

    CvMemStorage *storage;
    storage = cvCreateMemStorage(0); 

    if(ccv::debug) std::cerr << "Trying to find corners...\n";
    int status = 
      cvFindChessBoardCornerGuesses( gray_image, thresh, storage,
				     etalon_size, corners, &corner_count );
    if(ccv::debug) 
      std::cerr << "Done trying to find corners. status=" << status << endl;
    if(ccv::debug) 
      std::cerr << "Found " << corner_count << " corners\n";    

    if(wth && corner_count!=etalon_points){
      corner_count = etalon_points; 
      
      if(ccv::debug) 
	std::cerr << "Trying to find more corners using wth\n" << endl;
      zapImg(tmpwth);
      tmpwth = cvCloneImage( gray_image );
      whiteTopHat( tmpwth, ths );
      status = 
	cvFindChessBoardCornerGuesses( tmpwth, thresh, storage,
				       etalon_size, corners, &corner_count );
      if(ccv::debug) std::cerr << "Now found " << corner_count << " corners\n";
      
      // Try some other parameters if the first try was unsuccessful 
//       if( corner_count != etalon_points ) {
// 	corner_count = etalon_points;
// 	zapImg(tmpwth);
// 	tmpwth = cvCloneImage( gray_image );
// 	ths->threshold-=10;
// 	whiteTopHat( tmpwth, ths);
// 	status = 
// 	  cvFindChessBoardCornerGuesses( tmpwth, thresh, storage,
// 					 etalon_size, corners, &corner_count );
// 	if(ccv::debug) 
// 	  std::cerr << "Now found " << corner_count << " corners\n";
//       }

      if(!singleTrial) zapImg(tmpwth);
    }          

    corners_found=corner_count;

    if(corners_found==etalon_points){              // Only if we find them all

      printCorners(corners_found);
      

      if(ccv::debug) std::cerr << "Refining found corners..." << endl;
      
      cvFindCornerSubPix(gray_image, corners, 
			 corners_found, 
			 cvSize(5,5), cvSize(-1, -1),
			 cvTermCriteria (CV_TERMCRIT_ITER|
					 CV_TERMCRIT_EPS, 10, 0.1));

//       if(sortCorners) {
// 	if(ccv::debug) std::cerr << "Sorting corners\n" << endl;
// 	if(!(new SortCorners(corners,corners_found,
// 			     etalon_size.width, 
// 			     etalon_size.height))->getSuccess())
// 	  {
// 	    corners_found *= -1;
// 	  }
//       }
      
    }
    
    // Later to make this faster, 
    // we can use the same image all the time

    cvReleaseMemStorage( &storage );
    if(ccv::debug) std::cerr << "storage is released...\n";

    cvReleaseImage(&gray_image);            // Should maybe be moved later
    if(ccv::debug) std::cerr << "gray_mage is released...\n";
    cvReleaseImage(&rgb_image);             // Should maybe be moved later
    if(ccv::debug) std::cerr << "rgb_image is released...\n";
    cvReleaseImage(&thresh);                // Should maybe be moved later
    if(ccv::debug) std::cerr << "thresh is released...\n";

    printCorners(corners_found);

    if(corners_found==etalon_points)
      return CORRECT_IMAGE;
    else return FAILED_IMAGE;               // Corners not detected properly
  }
  else { if(ccv::debug) std::cerr << "No more files to read\n"; return 0;}
  
}

void
CCOCV::addFileName(string name)
{
  if(ccv::debug) 
    std::cerr << "Inside addFileName with string '" << name << "'\n";
  filenames.push_front(name);
  if(ccv::debug) 
    std::cerr << "Filenames now has string '" << filenames.back() 
	      << "' as last element\n";
}

void
CCOCV::removeFileName(string name)
{
  filenames.remove(name);
}

int 
CCOCV::getNumberOfFilesInList() { return filenames.size(); }

QStringList
CCOCV::getFiles() { return filenames; }

int
CCOCV::getImageSizeX() { return imageSize.width; }

int
CCOCV::getImageSizeY() { return imageSize.height; }

void
CCOCV::printCorners(int nr)
{
  for(int i=0;i<nr;i++) 
    if(ccv::debug) 
      std::cerr << "x=" << corners[i].x << " y=" << corners[i].y << endl;
}

void
CCOCV::printObjectPoints(int no_images)
{
  for(int i=0;i<etalon_size.width*etalon_size.height*no_images;i++){
    if(ccv::debug) std::cerr << "x=" << objectPoints[i].x
	 << " y=" << objectPoints[i].y
	 << " z=" << objectPoints[i].z << endl;
  }
}

IplImage*
CCOCV::rectifyImage(string srcImageName, bool /*write*/)
{
  if(ccv::debug) std::cerr << "CCOCV::rectify\n";
  if(&srcImageName && (srcImageName.size() > 0) ) {

    IplImage* src = cvLoadImage(srcImageName.c_str());      
    CvSize warpSize = cvSize( src->width, src->height );
    IplImage* dst = cvCreateImage( warpSize, src->depth, src->nChannels);
    
    double coeffs[2][3][3];
    
    if( calibrated ) {
      
      cvZero( dst );
      
      CvMat* tmpMap;
      tmpMap = cvCreateMat(warpSize.height,warpSize.width,CV_32FC2);
      
      CvMat* rectMap = cvCreateMat(warpSize.height,warpSize.width,CV_32SC3);
      
      cvComputePerspectiveMap(*coeffs, tmpMap);
      
      cvConvertMap(src,tmpMap,rectMap,1);
      
      cvReleaseMat(&tmpMap);
      
      cvRemap( src, dst, rectMap, 1 );
    }
    
    zapImg(src);

    return dst;
  }
  
  else return 0;
}

IplImage*
CCOCV::undistortImage(string srcImageName, bool write)
{

  IplImage* srcImage = cvLoadImage(srcImageName.c_str());      
  CvSize size;
  cvGetImageRawData(srcImage, 0, 0, &size);         // Get size of image
  IplImage* dstImage;
  dstImage = cvCreateImage(size,IPL_DEPTH_8U, 1); 
  dstImage = cvCloneImage(srcImage);

  if(ccv::debug) std::cerr << "Opened srcImage\n";

  convertParametersToFloat();

  if(calibrated){
    cvUnDistortOnce( srcImage, dstImage, cp.matrixf, cp.distortionf, 1 );
    if( write ) {
      string dstImageName = srcImageName+"_undistorted";
      if(ccv::debug) std::cerr << dstImageName;
      cvvSaveImage(dstImageName.c_str(),dstImage);
    }
  }
  else{
    if(ccv::debug) 
      std::cerr << "Error - Must perform a calibration first\n";
  }
  cvReleaseImage(&srcImage); 
  // What about releasing dstImage ?

  return dstImage;
}

bool
CCOCV::saveParams(const char* file)
{
    if(calibrated)
        return cp.save(file);

    return false;
}

bool
CCOCV::loadParams(const char* file)
{
    calibrated = cp.load(file);
    return calibrated;
}

void 
CCOCV::setSort(bool b) {
  sortCorners = b; 
  if(ccv::debug) std::cerr << "CCOCV has recieved a sort toggle (" << sortCorners << ")\n" ;
}

QPointArray
CCOCV::getCorners(const QSize& size)
{
  if(ccv::debug) std::cerr << "CCOCV::getCorners\n";

  int no_corners = etalon_size.width*etalon_size.height;
  int total_corners = no_corners*no_images + totalFailedCorners;

  QPointArray pa( total_corners > 0 ? total_corners : 0 );

  if( total_corners ) {
  
      if(ccv::debug) std::cerr << "Corners = " << total_corners << "(" << no_corners*no_images << "+" << totalFailedCorners << ")\n";
      
      // Scales
      double xs = static_cast<double>(size.width())/getImageSizeX();
      double ys = static_cast<double>(size.height())/getImageSizeY();
  
      // Create order of failed/correct images
      bool* images = new bool[t_no_images];
      for( int i=0; i<no_images; ++i )
	images[correct[i]] = true;
      for( int i=0; i<failed_index; ++i )
	images[failed[i]] = false;

      // Run through all images and fill the pointarray 
      // ----------------------------------------------
      int total_failed = 0; // Current index among the failed corners
      int ac = 0;           // Current index among the correct corners
      int tp = 0;           // Current total index

      for( int i=0; i<t_no_images; i++ ) {

	// Add correct points
	if( images[i] ) {
	  for( int j = 0; j<no_corners; ++j ) {
	    if(ccv::debug) std::cerr << "Adding C(" << tp << "," << allcorners[ac+j].x*xs << "," << allcorners[ac+j].y*ys << ")\n";
	    pa.setPoint( tp, ccv::round(allcorners[ac+j].x*xs), ccv::round(allcorners[ac+j].y*ys) );
	    tp++;
	  }
	  ac += no_corners;
	}

	// Add failed points
	else {
	  for( int j = 1; j<=failcorners[total_failed].x; ++j ) {
	    if(ccv::debug) std::cerr << "Adding F(" << j << "," << failcorners[total_failed+j].x*xs << "," << failcorners[total_failed+j].y*ys << ")\n";
	    pa.setPoint( tp, ccv::round(failcorners[total_failed+j].x*xs), ccv::round(failcorners[total_failed+j].y*ys) );
	    tp++;
	  }
	  total_failed += static_cast<int>(failcorners[total_failed].x + 1);
	}
      }
  
      delete [] images;
      
  }
  else
    if(ccv::debug) std::cerr << "CCOCV::getCorners() - No corners to draw\n";

  return pa;
}


#if 0  // THIS IS OBSOLETE BUT KEPT FOR NOSTALGIC REASONS :)
void
CCOCV::drawCorners(string filename, int xgap, int pointsize, 
		   int nx, int bw, int bh,
		   int ow, int oh,
		   bool drawFailed)
  
{

  if(ccv::debug) std::cerr << "CURRENTLY DISABLED DUE TO IMAGEMAGIC REMOVAL\n";
  return;

  // xgap is the gap set in imagemagick
  // ygap must be manually checked on montageimage and hardcoded

  IplImage* image = cvvLoadImage(filename.c_str());

  int no_corners = etalon_size.width*etalon_size.height;

  int tx, ty, y, x;
  int img_nr, img_nr_x, img_nr_y;

  double bhdoh = static_cast<double>(bh)/oh;
  double bwdow = static_cast<double>(bw)/ow;
  double ohdow = static_cast<double>(oh)/ow;

  int ytop = 0; // static_cast<int>(100*(1-ohdow)/2)+1;
  int ygap = 20; // 2*ytop+pointsize+6;

  uchar* iptr;
 
  if(ccv::debug) std::cerr << "xgap = " << xgap << endl 
       << "ygap = " << ygap << endl
       << "nx   = " << nx   << endl
       << "bw   = " << bw   << endl
       << "bh   = " << bh   << endl
       << "ow   = " << ow   << endl
       << "oh   = " << oh   << endl;

  // Draw correct corners
  for( int i=0; i < no_corners*no_images; i++ ) {
    tx = static_cast<int> (allcorners[i].x);
    ty = static_cast<int> (allcorners[i].y);
    // Using nr's in range [ 0 , t_no_images-1 ]
    img_nr = t_no_images -1 - correct[ i/no_corners ];
    img_nr_y = img_nr/nx;
    img_nr_x = img_nr - nx*img_nr_y;
    
    y = static_cast<int> (ty*bhdoh+(ygap+bh)*img_nr_y+ytop);
    x = static_cast<int> (xgap+tx*(bwdow)+(2*xgap+bw)*img_nr_x);
    if(ccv::debug) std::cerr << "(" << x << "," << y << ")\n";

    iptr = (uchar*)(image->imageData + 
		    y*image->widthStep + 
		    x*3);
    
    //     1
    //    234
    //     5

    int value = i%no_corners;  // [ 0 - no_corners-1 ]
    
    value *= static_cast<int>( 255.0/no_corners );

    // Green to red transition 
    // 1
    (iptr-image->widthStep)[0] = 0;
    (iptr-image->widthStep)[1] = 255-value;
    (iptr-image->widthStep)[2] = value;
    // 5
    (iptr+image->widthStep)[0] = 0;
    (iptr+image->widthStep)[1] = 255-value;
    (iptr+image->widthStep)[2] = value;
    // 2
    (iptr-3)[0] = 0; 
    (iptr-3)[1] = 255-value;
    (iptr-3)[2] = value;
    // 3
    (iptr+0)[0] = 0;
    (iptr+0)[1] = 255-value;
    (iptr+0)[2] = value;
    // 4
    (iptr+3)[0] = 0; 
    (iptr+3)[1] = 255-value;
    (iptr+3)[2] = value;
  }

  // Draw failed corners
  if(drawFailed) {
    int total = 0;
    for(int i=0; i<failed_index; i++) {
      // Using nr's in range [ 0 , t_no_images-1 ]
      img_nr = t_no_images -1 - failed[ i ];
      img_nr_y = img_nr/nx;
      img_nr_x = img_nr - nx*img_nr_y;
      
      for(int j=1; j<=failcorners[total].x; j++) {
	tx = static_cast<int> (failcorners[total+j].x);
	ty = static_cast<int> (failcorners[total+j].y);

	y = static_cast<int> (ty*bhdoh+(ygap+bh)*img_nr_y+ytop);
	x = static_cast<int> (xgap+tx*(bwdow)+(2*xgap+bw)*img_nr_x);
	if(ccv::debug) std::cerr << "(" << x << "," << y << ")\n";
      
	iptr = (uchar*)(image->imageData + 
			y*image->widthStep + 
			x*3);
      
	//     1
	//    234
	//     5
      
	int value = static_cast<int> ( (j-1)*(255.0/(failcorners[total].x-1)));

	// Blue to yellow transition 
	// 1
	(iptr-image->widthStep)[0] = 255-value;
	(iptr-image->widthStep)[1] = value;
	(iptr-image->widthStep)[2] = value;
	// 5
	(iptr+image->widthStep)[0] = 255-value;
	(iptr+image->widthStep)[1] = value;
	(iptr+image->widthStep)[2] = value;
	// 2
	(iptr-3)[0] = 255-value; 
	(iptr-3)[1] = value;
	(iptr-3)[2] = value;
	// 3
	(iptr+0)[0] = 255-value;
	(iptr+0)[1] = value;
	(iptr+0)[2] = value;
	// 4
	(iptr+3)[0] = 255-value; 
	(iptr+3)[1] = value;
	(iptr+3)[2] = value;
      }
      total += failcorners[total].x + 1 ;
    }
  }
  
  cvvSaveImage(filename.c_str(),image);
  
  cvReleaseImage(&image);

}
#endif // 0

void
CCOCV::whiteTopHat(IplImage* source, struct sTopHatSettings* ts)
{
  if(ccv::debug) std::cerr << "Running white top hat\n";

  IplConvKernel* strel = 
    cvCreateStructuringElementEx(ts->xsize,
				 ts->ysize,
				 ts->xsize/2,
				 ts->xsize/2,
				 ts->shape); 

  IplImage* tmp1  = cvCreateImage( cvGetSize(source), IPL_DEPTH_8U, 1);
  IplImage* tmp2  = cvCreateImage( cvGetSize(source), IPL_DEPTH_8U, 1);
  IplImage* tmp3  = cvCloneImage(source);
  
  complement(tmp3);
//   cvvSaveImage("wth_complement.bmp",tmp3);
//   cvvSaveImage("wth_original.bmp",source);

  if(ccv::debug) std::cerr << "Opening...";
  //  iplOpen(tmp3, tmp2, iterations);
  cvMorphologyEx( tmp3, tmp2, NULL, strel, 
		  CV_MOP_OPEN, ts->iterations );
  //  cvvSaveImage("wth_opened.bmp",tmp2);

  if(ccv::debug) std::cerr << " Subtracting...";
  //  iplSubtract(tmp3, tmp2, tmp1);
  cvSub( tmp3, tmp2, tmp1 );
  complement(tmp1);
  //  cvvSaveImage("wth_sub.bmp",tmp1);

  if(ccv::debug) std::cerr << " Thresholding...";
  //  iplThreshold(tmp1, source, limit);
  cvThreshold(tmp1, source, ts->threshold, 255, CV_THRESH_BINARY ); 

  //  cvvSaveImage("wth_thresh.bmp",source);
  
  if(ccv::debug) std::cerr << " Releasing..." << endl;
  cvReleaseImage(&tmp1);
  cvReleaseImage(&tmp2);
  cvReleaseImage(&tmp3);
}

void
CCOCV::setWth(bool b) { wth = b; }

void
CCOCV::complement(IplImage* source)
{
  int* lut = new int[256];

  for(int i=0; i<256; i++) lut[i]=255-i;

  int w = source->width;
  int h = source->height;

  uchar* pixel;

  for(int x=0; x<w; x++)
    for(int y=0; y<h; y++) {
       pixel = (uchar*)(source->imageData   + 
			y*source->widthStep + 
			x*source->nChannels);
       *pixel=lut[*pixel];
    }
  
  delete [] lut;
}

void
CCOCV::setTopHatSettings(int iterations, 
			 int shape,
			 int xsize,
			 int ysize,
			 int threshold)
{
  if(ccv::debug) std::cerr << "CCOCV::setTopHatSettings\n";
  ths->iterations = iterations;
  ths->shape      = shape;
  ths->xsize      = xsize;
  ths->ysize      = ysize;
  ths->threshold  = threshold;
}

IplImage*
CCOCV::trialCalib(int& corners_found)
{
  if(ccv::debug) std::cerr << "CCOCV::trialCalib\n";

  if(filenames.size() > 0) {

    initializeCalibration();

    findCorners2(corners_found, true);

    // Important such that real calib starts from scratch
    zapArr(allcorners);
    zapArr(failcorners);
    initialized = false;


    CvSize imageSize;
    CvPoint p;
    IplImage* tmp;

    // If we did find every corner without TopHat
    if(!tmpwth) {
      imageSize.width  = 400;
      imageSize.height = 100;
      p.x = 10;
      p.y = 50;
      CvFont font;
      cvInitFont( &font, CV_FONT_VECTOR0, 0.5f, 0.5f, 0, 1 );
      tmp = cvCreateImage(imageSize, IPL_DEPTH_8U, 1);
      cvZero( tmp );
      cvPutText( tmp, "TopHat not needed on this image", p,
		 &font, CV_RGB(255,255,255));
    }

    else {
      // Create new color image to paint corners on
      cvGetImageRawData(tmpwth, 0, 0, &imageSize);         
      tmp = cvCreateImage(imageSize,IPL_DEPTH_8U, 3);
      cvCvtColor(tmpwth,tmp,CV_GRAY2BGR);    
      
      CvSize axes1;
      CvSize axes2;
      axes1.width = 3; axes1.height = 3;
      axes2.width = 5; axes2.height = 5;

      for(int i=0; i<corners_found;i++) {
	p.x = ccv::round(corners[i].x);
	p.y = ccv::round(corners[i].y);
	cvEllipse(tmp, p, axes1, 0, 0, 360, CV_RGB(255,0,0) );
	cvEllipse(tmp, p, axes2, 0, 0, 360, CV_RGB(255,255,0) );
      }

    }

    return tmp;

  } 

  return 0;
}

void
CCOCV::setCameraParams(struct CameraParams cp)
{
  if(ccv::debug) std::cerr << "CCOCV::setCameraParameters\n";

  this->cp = cp;

  calibrated = true;
  if(ccv::debug) 
    cp.print();
}

void
CCOCV::convertParametersToFloat()
{
  if(ccv::debug) std::cerr << "CCOCV::convertParametersToFloat\n";
  for(int i=0; i<9; i++)
    cp.matrixf[i] = static_cast<float>(cp.matrix[i]);
  for(int i=0; i<4; i++)
    cp.distortionf[i] = static_cast<float>(cp.distortion[i]);
}
