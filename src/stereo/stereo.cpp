/*************************************************

 Purpose:
   These classes will contain stereoalgorithms
   that generates disparity maps.

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: stereo.cpp,v 1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#include "stereo.h"

///////////////////////////////////////////////
// This constructor should really be rewritten !!
// to deal better with alg that do not read OpenCV
///////////////////////////////////////////////

///////////////////////////////////////////////
//REALLY!!!!
///////////////////////////////////////////////

Stereo::Stereo(int argc, char** argv, bool m)
{
  dispI=0; left=0; right=0; 

  if(ccv::debug) std::cerr << "Running Stereo constructor\n";
  iterations=0; 
  
  memory=m;
  status=false;

  if (cmdLine.SplitLine(argc, argv) < 1)
    {
      // no switches were given on the command line, abort
      if(ccv::debug) std::cerr << "\nOptions -left and -right are required\n\n";
      show_help();
      return;
    }   
  
  if (cmdLine.HasSwitch("-h"))
    {
      show_help();
      exit(0);
    }   
  
  if(!memory) {
    try
      {
	left_file  = cmdLine.GetArgument("-left",  0);      
	right_file = cmdLine.GetArgument("-right", 0);      
      }
    catch (...)
      {
	// one of the required arguments was missing, abort
	if(ccv::debug) std::cerr << "\nOptions -left and -right are required\nCatched\n";
	return;
      }  
  }
  
  // get the optional parameters   
  BSIZE = str2int( cmdLine.GetSafeArgument( "-bs", 0, "3" ) );   
  MAXD  = str2int( cmdLine.GetSafeArgument( "-md", 0, "30" ) );
  outfile = cmdLine.GetSafeArgument("-outf", 0, "disparity.bmp" );    
  method = cmdLine.GetSafeArgument( "-method", 0, "rojas" );   
  
  if(!memory) {
    if(ccv::debug) std::cerr << "Loading images...\n";
    if(ccv::debug) std::cerr << "left = " << left_file .c_str() << "\n";
    left  = cvvLoadImage(left_file .c_str());  
    right = cvvLoadImage(right_file.c_str());  
    if( left  == 0 ) if(ccv::debug) std::cerr << "ERROR - left image\n";
    if( right == 0 ) if(ccv::debug) std::cerr << "ERROR - right image\n";
    if( left != 0 && right != 0 )  status = true; 
    else return;
    if(ccv::debug) std::cerr << "Loaded!\n";

  
    cvGetImageRawData(left, 0, 0, &imageSize);         // Get size of image
    width = imageSize.width;
    height = imageSize.height;
  }
  
  preFilters = new vector<Filter*>;
  postFilters = new vector<Filter*>;

  status=true;
} 

Stereo::Stereo(int argc, char** argv){ 
  if(ccv::debug) std::cerr << "Running Stereo constructor\n";
  iterations=0; 
  
  dispI=0; left=0; right=0;

  memory=false;
  status=false;

  if (cmdLine.SplitLine(argc, argv) < 1)
    {
      // no switches were given on the command line, abort
      if(ccv::debug) std::cerr << "\nOptions -left and -right are required\n\n";
      show_help();
      return;
    }   
  
  if (cmdLine.HasSwitch("-h"))
    {
      show_help();
      exit(0);
    }   
  
  if(!memory) {
    try
      {
	left_file  = cmdLine.GetArgument("-left",  0);      
	right_file = cmdLine.GetArgument("-right", 0);      
      }
    catch (...)
      {
	// one of the required arguments was missing, abort
	if(ccv::debug) std::cerr << "\nOptions -left and -right are required\nCatched\n";
	return;
      }  
  }
  
  // get the optional parameters   
  BSIZE = str2int( cmdLine.GetSafeArgument( "-bs", 0, "3" ) );   
  MAXD  = str2int( cmdLine.GetSafeArgument( "-md", 0, "30" ) );
  outfile = cmdLine.GetSafeArgument("-outf", 0, "disparity.bmp" );    
  method = cmdLine.GetSafeArgument( "-method", 0, "rojas" );   
  
  if(!memory) {
    if(ccv::debug) std::cerr << "Loading images...\n";
    left  = cvvLoadImage(left_file .c_str());  
    right = cvvLoadImage(right_file.c_str());  
    if( left  == 0 ) if(ccv::debug) std::cerr << "ERROR - left image\n";
    if( right == 0 ) if(ccv::debug) std::cerr << "ERROR - right image\n";
    if( left != 0 && right != 0 )  status = true; 
    else return;
    if(ccv::debug) std::cerr << "Loaded!\n";
  }
  
  cvGetImageRawData(left, 0, 0, &imageSize);         // Get size of image
  width = imageSize.width;
  height = imageSize.height;

  preFilters = new vector<Filter*>;
  postFilters = new vector<Filter*>;
  
}

Stereo::~Stereo()
{ 
  if(ccv::debug) std::cerr << "Stereo main destructor\n";
  zapImg(dispI);
  zapImg(left);
  zapImg(right);

  zap(postFilters);
  zap(preFilters);

//   delete &left_file;
//   delete &right_file;

  if(ccv::debug) std::cerr << "Destructing Stereo\n";
}
  

void Stereo::show_help() 
{
  cout << "\nUsage:\n" 
       << "-md n     : Max disparityn"
       << "-bs n     : Blocksize\n"
       << "-blur     : Blurs the images before computation\n"
       << "-edge n   : Edgelimit value (rojas)\n"
       << "-outf s   : Genereates a disparity image with file name s\n"
       << "-method s : s = 'cvblock' or 'magickblock' or 'rojas'\n";
}

bool Stereo::start() 
{ 

  if(status) {

    times = str2int( cmdLine.GetSafeArgument( "-t", 0, "1"));
   
    preProcess();
    
    for(int i=0;i<times;i++) {
      if(ccv::debug) std::cerr << "Running stereo algorithm...\n";
      if(!calculateDisparity())
	return false;
    }

    postProcess();

    if(writeToDisk) saveDisparityImage();
    
  }
  
  zapImg(left);     // CAREFUL HERE ! 
  zapImg(right);    // What if we need these later
  
  if(ccv::debug) std::cerr << "Number of iterations made: " << iterations << "\n";
  return true; 
}

void
Stereo::setImages(IplImage* l, IplImage* r) 
{
  zapImg(left);
  zapImg(right);
  left = l; right = r;
  memory = true;
  if(ccv::debug) std::cerr << "Stereo class has received image pointers\n";
}

void
Stereo::unSetImages() { memory=false; }

void
Stereo::loadImages(char* l, char* r)
{
  if(ccv::debug) std::cerr << "Loading images\n";
  zapImg(left);
  zapImg(right);
  left_file = l; right_file = r;
  left = cvvLoadImage(left_file.c_str());  
  right = cvvLoadImage(right_file.c_str());  
}

void
Stereo::saveDisparityImage()
{
  cvvSaveImage( outfile.c_str(), dispI );
}

void
Stereo::setOutFileName(string out) { outfile = out; }

bool
Stereo::getStatus() { return status; }

void
Stereo::blurImages()
{
  // Create temporary images
  IplImage* tmpL;
  IplImage* tmpR;
  tmpL = cvCloneImage( left );        
  tmpR = cvCloneImage( right );  

  cvSmooth(tmpL, left, CV_GAUSSIAN, 3, 3); 
  cvSmooth(tmpR, right, CV_GAUSSIAN, 3, 3); 

//   iplBlur(tmpL ,left,3,3,1,1);         
//   iplBlur(tmpR,right,3,3,1,1);
  zapImg(tmpL);
  zapImg(tmpR);
}

void
Stereo::histogramEqualize()
{
  if(ccv::debug) std::cerr << "WARNING - Stereo::histogramEqualize does"
			   << " nothing at the moment... (needs remake)\n";
  
//   IplImage* tmpL;
//   IplImage* tmpR;
//   tmpL = cvCloneImage( left );        
//   tmpR = cvCloneImage( right );  
	
//   // To equalize the histogram we first need a lookup table
//   int range = 256;
//   IplLUT** plutL = new IplLUT*[3];
//   IplLUT** plutR = new IplLUT*[3];

//   // Yiiiikes you scream, why not a loop ?
//   // Well I thought about that too but...  fix it if you are pedantic ;)
//   // If you want to use only one channel that is probably a good idea
//   IplLUT lutL = { range+1, NULL,NULL,NULL, IPL_LUT_INTER };
//   IplLUT lutR = { range+1, NULL,NULL,NULL, IPL_LUT_INTER };
//   lutL.key    = new int[range+1];
//   lutL.value  = new int[range];
//   lutL.factor = new int[range];
//   lutR.key    = new int[range+1];
//   lutR.value  = new int[range];
//   lutR.factor = new int[range];
      
//   for( int i=0; i<=range; i++) {
//     lutL.key[i] = i;
//     lutR.key[i] = i;
//   }
	
//   IplLUT lutL2 = { range+1, NULL,NULL,NULL, IPL_LUT_INTER };
//   IplLUT lutR2 = { range+1, NULL,NULL,NULL, IPL_LUT_INTER };
//   lutL2.key    = new int[range+1];
//   lutL2.value  = new int[range];
//   lutL2.factor = new int[range];
//   lutR2.key    = new int[range+1];
//   lutR2.value  = new int[range];
//   lutR2.factor = new int[range];
      
//   for( int i=0; i<=range; i++) {
//     lutL2.key[i] = i;
//     lutR2.key[i] = i;
//   }

//   IplLUT lutL3 = { range+1, NULL,NULL,NULL, IPL_LUT_INTER };
//   IplLUT lutR3 = { range+1, NULL,NULL,NULL, IPL_LUT_INTER };
//   lutL3.key    = new int[range+1];
//   lutL3.value  = new int[range];
//   lutL3.factor = new int[range];
//   lutR3.key    = new int[range+1];
//   lutR3.value  = new int[range];
//   lutR3.factor = new int[range];
      
//   for( int i=0; i<=range; i++) {
//     lutL3.key[i] = i;
//     lutR3.key[i] = i;
//   }

//   plutL[0] = &lutL; plutL[1] = &lutL2; plutL[2] = &lutL3; 
//   plutR[0] = &lutR; plutR[1] = &lutR2; plutR[2] = &lutR3; 

//   iplComputeHisto( tmpL, plutL );
//   iplComputeHisto( tmpR, plutR );
//   iplHistoEqualize( tmpL, left , plutL); 
//   iplHistoEqualize( tmpR, right, plutR); 
//   cvvSaveImage("barkmannen.bmp",left);
//   cvvSaveImage("snorkelfröken.bmp",right);
	
//   delete [] lutL.key;
//   delete [] lutL.value;
//   delete [] lutL.factor;
//   delete [] lutR.key;
//   delete [] lutR.value;
//   delete [] lutR.factor;
//   delete [] lutL2.key;
//   delete [] lutL2.value;
//   delete [] lutL2.factor;
//   delete [] lutR2.key;
//   delete [] lutR2.value;
//   delete [] lutR2.factor;
//   delete [] lutL3.key;
//   delete [] lutL3.value;
//   delete [] lutL3.factor;
//   delete [] lutR3.key;
//   delete [] lutR3.value;
//   delete [] lutR3.factor;

//   delete [] plutL;
//   delete [] plutR;
//   zapImg(tmpL);
//   zapImg(tmpR);
}

void
Stereo::preProcess()
{
  if(ccv::debug) std::cerr << "I(stereo): " << right << "\n";

  if(preFilters->size()>0) {
    vector<Filter*>::iterator p = preFilters->begin();
    for(p; p!=preFilters->end(); p++) {
      static_cast<Filter*>(*p)->apply(left);
      static_cast<Filter*>(*p)->apply(right);
    }
  }
//   if(cmdLine.HasSwitch("-premean")) {
//     if(ccv::debug) std::cerr << "Mean: " << findMean(left) << "," << findMean(right) << "\n";
//     int add = cvRound(findMean(left)-findMean(right));
//     if(ccv::debug) std::cout << "Add: " << add << "\n";
//     addC(right,add);
//   }
//   if(cmdLine.HasSwitch("-premedian")){
//     medianFilter(left,3);
//     medianFilter(right,3);
//   }
//   if(cmdLine.HasSwitch("-blur")) 
//     blurImages();
//   if(cmdLine.HasSwitch("-hieq"))
//     histogramEqualize();
 
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
  
  //  iplMedianFilter(tmpD, img, size, size, 1, 1);
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
Stereo::leftImagePointer() { return &left; }

IplImage**
Stereo::rightImagePointer() { 
  if(ccv::debug) std::cerr << "Returning: " << &right << "\n";
  return &right; 
}
