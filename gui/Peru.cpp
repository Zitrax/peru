/*************************************************

 Purpose:
   This is the main display, subclass of Perubase
   made in designer.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: Peru.cpp,v 1.28 2005/05/25 21:29:30 cygnus78 Exp $

*************************************************/

#include "Peru.h"

#include <qsplitter.h>

const QString Peru::tmpImage = QString("tmp_montage.bmp");

Peru::Peru( QWidget* parent, const char* name,
	    WFlags fl) : Perubase(parent,name,fl), 
			 calc_stop_flag(false),
			 correct_images(0),
			 ccocv2(0),
			 stereo(0)
{
  prefs = new Preferences(this);
  prefs->readSettings();
  toggleParameters( stereo_algCOB->currentText() );

  ccv::debug = debugCB->isChecked();

  setCalibrated(false,1);
  setCalibrated(false,2);
  
  ccocv = new CCOCV();                                 // Create camera 

  Image_widget = new ImageWidget(scrollView->viewport(),"Image view", scaleCB->isChecked());     
  scrollView->addChild(Image_widget);

  if(ccv::debug) std::cerr << endl;

  calPar = new CalibrationParameters( ccocv, this, this );
  calPar->hide();
  
  magick = !system("montage > /dev/null");             // Check if montage 
                                                       // exists in path
  if(!magick) {
    montageCB->setChecked(false);
    montageCB->setEnabled(false);
  }

  connectSignalsToSlots();

  KStartupLogo *start_logo = new KStartupLogo();
  start_logo->show();

  start_logo->raise();
  start_logo->setHideEnabled( true );
  QApplication::flushX();
  sleep( 1 );
  zap(start_logo);

  supportedFormats = "Images (*.bmp *.jpg *.ppm *.png)";

  // Create Forms and Initially hide them
  ths = new TopHatSettings( ccocv, this, this );
  ths->hide();

  // Fix splitter layout
  QValueList<int> sizes;
  sizes.append( 300 );
  sizes.append( 100 );
  tab_text_splitter->setSizes(sizes);
}

Peru::~Peru(){
  if(ccv::debug) std::cerr << "DESTRUCTING Peru\n"; 

  prefs->writeSettings();

  delete prefs;

  zap(ccocv);
  zap(ccocv2);
  zap(stereo);

  zap(ths);
  zap(calPar);
  zap(Image_widget);
}

void
Peru::connectSignalsToSlots()
{
  /**************************
   * Connect Signals to Slots
   **************************/

  // Scale image checkbox
  connect( scaleCB,      SIGNAL(        toggled(bool) ), 
	   Image_widget,   SLOT(      setScaled(bool) ));

  // Undistort checkbox
  connect( calibCB,      SIGNAL(        toggled(bool) ),
	   this,           SLOT( undistortImage(bool) ));

  // Receives string and prints them
  connect( this,         SIGNAL( stringSignal(const QString&) ),     
	   infoText,       SLOT(       insert(const QString&) ));

  connect( calibB,       SIGNAL(   clicked() ),
	   this,	   SLOT( calibrate() ));

  connect( montageCB,    SIGNAL(      toggled(bool) ),
	   this,           SLOT( montageCheck(bool) ));

  connect( saveParamsB,  SIGNAL(    clicked() ),
	   this,           SLOT( saveParams() ));

  connect( load_leftB,   SIGNAL(      clicked() ),
	   this,           SLOT( setFileNameL() ));

  connect( load_rightB,  SIGNAL(      clicked() ),
	   this,	   SLOT( setFileNameR() ));
  
  connect( loadParamsB,  SIGNAL(    clicked() ),
	   this,	   SLOT( loadParams() ));

  connect( left_paramsB, SIGNAL(    clicked() ),
	   this,           SLOT( loadParams() ));

  connect( right_paramsB,SIGNAL(     clicked() ),
	   this,           SLOT( loadParams2() ));

  connect( undistortB,   SIGNAL(        clicked() ),
	   this,           SLOT( undistortSerie() ));

  connect( calculateB,   SIGNAL(         clicked() ),
	   this,           SLOT( calculateStereo() ));

  connect( sortCornersCB,SIGNAL( toggled(bool) ),
	   this,	   SLOT( setSort(bool) ));

  connect( wthCB,        SIGNAL( toggled(bool) ),
	   this,	   SLOT(  setWth(bool) ));
  
  connect( stereo_algCOB,SIGNAL(        activated(const QString&) ),
	   this, 	   SLOT( toggleParameters(const QString&) ));

  connect( debugCB,      SIGNAL(     toggled(bool) ),
	   this,           SLOT( debugToggle(bool) ));

  connect( saveImageB,   SIGNAL(   clicked() ),
	   this,           SLOT( saveImage() ));

  connect( load_groundB, SIGNAL( clicked() ),
	   this,           SLOT( setFileNameG() ));

  connect( calPar,       SIGNAL( parametersEdited() ),
	   this,           SLOT( undistortImage() ));

  connect( stopB,         SIGNAL( clicked() ),
	   this,            SLOT( stopCalculation() ));

  connect( montageView,   SIGNAL( openImage( QString ) ),
 	   this,            SLOT( imageOpen( QString ) ));

  connect( montageView,   SIGNAL(                     removedItem(QString) ),
	   this,            SLOT( removeImageFromCalibrationQueue(QString)));

  connect( montageView,   SIGNAL(       removedAllItems() ),
	   this,            SLOT( clearCalibrationQueue() ));

}

void Peru::imageOpen( QString filename ) { imageOpen_(filename); }
void Peru::imageOpen( ) { imageOpen_( QString::null ); }

void 
Peru::imageOpen_( QString filename )
{

  if( filename.isEmpty() ) {

    filename = QFileDialog::getOpenFileName( "",
					     supportedFormats,
					     this,
					     "open file dialog"
					     "Choose a file" );
  }
 
  if(filename.isEmpty()) { if(ccv::debug) std::cerr << "Error empty file\n"; }
  else { 
    fileName = filename.latin1();
    if(ccv::debug) std::cerr << filename << "\n"; 
    QImage *p_image = new QImage(filename,0);
    if (*p_image==NULL) { err("ERROR - Could not open image\n"); }
    else {
      imageOpen(*p_image);
      write("\nOpened image: "+filename+"\n");
      zap(p_image);
    }
  }
    
}

void 
Peru::calibImageOpen()
{
  QStringList filelist = openFiles();
  QString filename;
  
  if(filelist.isEmpty()) { err("Error - No files\n"); }
  else { 
    calibPB->reset();
    montage(filelist);
    // Add images to calibration class
    while(!filelist.isEmpty()){
      filename = filelist.back();
      write("Added calib image: "+filename+"\n");
      if(ccv::debug) std::cerr << filename << "\n"; 
      calibImageOpen(filename);
      filelist.pop_back();
    }
  }
}

QStringList
Peru::openFiles()
{
  return QFileDialog::getOpenFileNames( supportedFormats,
					"",
					this,
					"open file dialog"
					"Choose a file" );
}

void 
Peru::imageOpen(QImage& image)
{
  if(ccv::debug) std::cerr << "Running imageOpen2\n";
  Image_widget->displayImage(image);
  imageTabWidget->setCurrentPage( findTabPage( imageTabWidget, QString("ImageView")) );
}

// Adds calibration pattern images to calibration class
void
Peru::calibImageOpen(const QString& name)
{
  if(ccv::debug) std::cerr << "Inside calibImageOpen with string " << name << "\n";
  ccocv->addFileName(name.latin1());
  updateImagesInQueueL();
}


// Run to initialize ccocv with current settings
// applied in the gui.
void
Peru::initializeCCOCV()
{
  ccocv->setEtalonSize(etxSB->value()-1,etySB->value()-1);
  ccocv->setDimension(dimxLE->text().toDouble(),
		      dimyLE->text().toDouble() );

  // Apply settings from dialog
  int shape=0;
  if(ths->shapeCB->currentText()=="Ellipse")
    shape = CV_SHAPE_ELLIPSE;
  if(ths->shapeCB->currentText()=="Cross")
    shape = CV_SHAPE_CROSS;
  if(ths->shapeCB->currentText()=="Rectangle")
    shape = CV_SHAPE_RECT;

  ccocv->setTopHatSettings(ths->iterationsSB->value(),
			   ths->shapeCB->currentItem(),
			   ths->xsizeSB->value(),
			   ths->ysizeSB->value(),
			   ths->thresholdSB->value());
}

void
Peru::calibrate()
{
  int corners;
  int orig_nr_images = ccocv->getNumberOfFilesInList();

  if( !orig_nr_images ) {
    err("ERROR - Must add some images to calibrate first\n");
    return;
  }

  QStringList all_files = ccocv->getFiles();

  correct_images = 0;

  initializeCCOCV();

  stopB->setEnabled( true );

  QString str;
  QTextStream ts( &str, IO_WriteOnly );
  
  // Reset progress bar
  calibPB->setTotalSteps(ccocv->getNumberOfFilesInList()-1);
  calibPB->reset();
  
  // Run through all images in CCOCV
  // findCorners returns 1 as long as there are more images to read
  while(ccocv->findCorners(corners)==1){
    if(corners==(etxSB->value()-1)*(etySB->value()-1)) correct_images++;
    calibPB->setProgress(calibPB->progress()+1);
    ts.device()->reset();
    if( corners < 0 )
      ts << "\nFound " << -1*corners << " chess corners (Rejected by sort)";
    else {
      ts << "\nFound " << corners << " chess corners";
      if(corners==(etxSB->value()-1)*(etySB->value()-1))
	ts << " *";
      updateImagesInQueueL();
    }
  
    write(str);
    repaint();
    qApp->processEvents();
    if( calc_stop_flag ) {
      calc_stop_flag = false;
      correct_images = 0;
      err("\nABORTED - Throwing away data\n");
      clearCalibrationQueue();
      break;
    }
      
  }

  updateImagesInQueueL();

  ts.device()->reset();
  ts << "\n" << correct_images << " images correctly calibrated\n";

  // If we have camera parameters (successful calibration)
  // we draw the corners onto the montage image (if used)
  // and enable saving of parameters.
  if(correct_images>0) {

    int tiles = static_cast<int>(sqrt(static_cast<float>(orig_nr_images)));
    if(tiles*tiles<orig_nr_images) tiles++;

    if( montageCB->isChecked() ){
      ccocv->drawCorners(tmpImage,1,12,
			 tiles,
			 100,
			 static_cast<int> (100*(static_cast<float> 
						(ccocv->getImageSizeY())/
						ccocv->getImageSizeX())),
			 ccocv->getImageSizeX(),
			 ccocv->getImageSizeY(), true);
      
      QImage* p_image = new QImage(tmpImage,0);
      if (*p_image==NULL) { err("ERROR - Image (drawCorners)\n"); }
      else {
	fileName = tmpImage.operator std::string();
	imageOpen(*p_image);
	zap(p_image);
      }
    }

    updateParamsDialog();
    setCalibrated(true,1);
    write(str);
  }
  else {
    setCalibrated(false,1);
    err(str);
  }

  stopB->setEnabled( false );

  // Reset the que to the original state
  ccocv->setFileList(all_files);
  updateImagesInQueueL();
}

void
Peru::undistortImage()
{
  if(ccv::debug) std::cerr << "Peru::undistortImage()\n";
  undistortImage( calibCB->isChecked() );
}

void
Peru::undistortImage(bool undistort)
{
  if(ccv::debug) std::cerr << "Peru::undistortImage(bool undistort)\n";
  
  QImage* p_image = 0;

  if(calibrated) {
    if(ccv::debug) std::cerr << "Undistorting\n";
    if(undistort && fileName.size()>0){
      p_image = ccv::iplImageToQImage(ccocv->undistortImage(fileName,false) );
    }
    else {
      if(ccv::debug) std::cerr << "Reverting\n";
      p_image = new QImage(fileName,0);    
    }
    imageOpen(*p_image);
    zap(p_image);
  }
  else
    if(ccv::debug) std::cerr << "Not undistorting - no params\n";
}

void 
Peru::undistortSerie()
{
  QStringList filelist = openFiles();
  QString filename;

  if(calibrated) {
    while( !filelist.isEmpty() ) {
      filename = filelist.back();
      ccocv->undistortImage(filename.latin1(),true);
      write("\nUndistorted image "+filename);
      filelist.pop_back();
    }
  }
  else err("WARNING - You have not calibrated "
	   "or calibration has failed\n");

}

bool
Peru::scaled()
{
  return scaleCB->isChecked();
}

void
Peru::montage(QStringList flist)
{
  if(montageCB->isChecked() && !flist.isEmpty()){
    
    montageView->show();

    QStringList::ConstIterator it  = flist.begin();
    QStringList::ConstIterator end = flist.end();
    
    QSize icon_size(100,100);
    for( it; it!=end; ++it )
      {
	QImage icon( *it );
	QPixmap icon_p = icon.smoothScale( icon_size );
	
	if(ccv::debug) std::cerr << "Adding " << *it << " to montage view\n";
	QIconViewItem* item = new QIconViewItem(montageView, *it, icon_p );
      }

    imageTabWidget->setCurrentPage( findTabPage( imageTabWidget, QString("Thumbnails")) );

  }
  else return;
}

void 
Peru::montageCheck(bool checked)
{
  if(!magick && checked) {
    montageCB->setChecked(false);
    err("ERROR - Can't fint montage in your path\n");
  }
  
}

void
Peru::saveParams()
{

  QString filename; 

  filename = QFileDialog::getSaveFileName( "",
					   "Calibration parameters (*.ccv)",
					   this,
					   "open file dialog"
					   "Choose a file" );
  
  if(ccv::debug) std::cerr << "filename = " << filename << "\n";

  if(filename.isEmpty()) {
    if(ccv::debug) std::cerr << "Error empty filename\n"; 
  }
  else {
    if(!filename.contains(".ccv")) filename.append(".ccv");
    if(ccv::debug) std::cerr << "filename = " << filename << "\n";
    ccocv->saveParams( filename.latin1() );
  }
}

void Peru::loadParams()  { loadParams(1); }
void Peru::loadParams2() 
{
  if( !ccocv2 ) { 
    if(ccv::debug) 
      std::cerr << "Creating ccocv2\n"; 
    ccocv2 = new CCOCV; 
  }
  loadParams(2); 
}

void
Peru::loadParams(int cam)
{
  QString filename; 

  filename = QFileDialog::getOpenFileName( "",
					   "Calibration parameters (*.ccv)",
					   this,
					   "open file dialog"
					   "Choose a file" );
  
  if(filename.isEmpty()) {  
    if(ccv::debug) std::cerr << "Error empty filename\n";
    setCalibrated(false,cam);
  }
  else {
    if( cam==1 ) {
      if(ccocv->loadParams( filename.latin1() )) {
	setCalibrated(true,cam);
	updateParamsDialog();
	undistortImage();
      }
    }
    else if( cam==2 ) {
      if(ccocv2->loadParams( filename.latin1() ))
	setCalibrated(true,cam);
    }
    else if(ccv::debug) std::cerr << "ERROR of parameter to loadParams\n";
  }
}

// Set the led's to green or red
// depending on the state of calibration parameters.
void
Peru::setCalibrated(bool c, int cam)
{
  if( cam==1 ) {
    calibrated = c;
    if(!c) {
      calibLed1->setLed(false);
      saveParamsB->setEnabled(false);
    }
    else {
      calibLed1->setLed(true);
      saveParamsB->setEnabled(true);
      if( calibrated2 )
	calibLed2->setLed(true);
    }
  }
  if( cam==2 ) {
    calibrated2 = c;
    if(!c)
      calibLed2->setLed(false);
    else if ( calibrated )
      calibLed2->setLed(true);
  }
}

void
Peru::calculateStereo()
{
  // Many variables are initialized here to make 
  // code in the rest of this method block easier to read.
  int start             = framestartLE->text().toInt();
  int stop              = frameendLE->text().toInt();

  int md                = mdSB->value();
  int bs                = bsSB->value();

  bool blur             = blurCB->isChecked();
  bool undistort_frames = undist_stCB->isChecked();
  bool hieq             = hieqCB->isChecked();
  bool postMedian       = postMedianCB->isChecked();
  bool preMedian        = preMedianCB->isChecked();
  bool preMean          = preMeanCB->isChecked();
  bool postGradRem      = gradientrCB->isChecked();
  bool perspectiveCorrect = perspectivecCB->isChecked();
  bool contrast         = contrastCB->isChecked();

  // Set up filename strings
  char* c_left  = static_cast<char*>(malloc(300));
  char* c_right = static_cast<char*>(malloc(300));
  char* c_out   = static_cast<char*>(malloc(300));

  stopB->setEnabled( true );

  try {

    if( undistort_frames && (!calibrated2 || !calibrated)) {
      throw ccv::error("ERROR - You have not calibrated all cameras\n");
    }
    
    if(ccv::debug) std::cerr << "Method = " 
			     << stereo_algCOB->currentText() << endl;
    
    //  Fill filename strings with correct numbers
    sprintf( c_left,  leftframeLE->text().latin1(), start);
    sprintf( c_right, rightframeLE->text().latin1(), start);
    strcpy( c_out, "out.bmp" );
    
    string left  = string(c_left);
    string right = string(c_right);
    string out   = string(c_out);

    if(left.length()==0 || right.length()==0) 
      throw ccv::error("ERROR - No filename provided\n");

    // Create the chosen stereo algorithm
    // ----------------------------------
    zap(stereo);
    if(stereo_algCOB->currentText()=="cvblock(abs)") 
      stereo = new BlockMatch(left, right, out, 
			      colorCB->isChecked(), md, bs);

    else if(stereo_algCOB->currentText()=="cvblock(fast)") 
      stereo = new BlockMatch(left, right, out, 
			      colorCB->isChecked(), md, bs, true);

    else if(stereo_algCOB->currentText()=="cvblock(mse)") 
      stereo = new BlockMatch(left, right, out, 
			      colorCB->isChecked(), md, bs, false, true);

    else if(stereo_algCOB->currentText()=="cvbirchfield")
      stereo = new CvBirchfield(left, right, out,  md);

    else if(stereo_algCOB->currentText()=="pyramidblock")
      stereo = new PyramidBlock(left, right, out,  md, 
				pyrSB->value(), 
				pyrtSB->value(),
				colorCB->isChecked() );
    else {
      if(ccv::debug) std::cerr << "ERROR - stereo algorithm not recognized\n";
      throw ccv::error("ERROR - stereo algorithm not recognized\n");
    }

    if( !stereo->getStatus() ) 
      throw ccv::error("ERROR - loading images\n");
    // ----------------------------------

    // START FILTERS *****

    // PREFILTERS
    if(preMedian) 
      stereo->addPreFilter(new MedianFilter()); 
    if(blur)
      stereo->addPreFilter(new Gaussian());
    if(preMean)
      stereo->addPreFilter(new MeanCorrection
			   (stereo->leftImagePointer(),
			    stereo->rightImagePointer()));
    if(hieq)
      stereo->addPreFilter(new HistoEq());

    // POSTFILTERS
    if(postMedian) 
      stereo->addPostFilter(new MedianFilter());
    if(postGradRem)
      stereo->addPostFilter(new GradientRemoval());
    if(perspectiveCorrect)
      stereo->addPostFilter(new PerspectiveCorrect());
    if(contrast) 
      stereo->addPostFilter(new ContrastStretch());

    // END - FILTERS *****
    
    if(!undistort_frames && 
       stereo_algCOB->currentText()!="magickblock") stereo->unSetImages();

    stereo->setDisparityToDisk(write_disparityCB->isChecked());

    calibPB->reset();
    calibPB->setTotalSteps(stop-start+1);
    calibPB->setProgress(calibPB->progress()+1);

    write("Calculating stereo...\n");

    clock_t start_time = clock(); 

    // Reset global errorflag before use
    ccv::resetError();

    for( int i = start; i <=stop; i++ ) {

      // Fill the filenames with current framenumbers
      sprintf( c_left,  leftframeLE->text().latin1(), i);
      sprintf( c_right, rightframeLE->text().latin1(), i);
      stereo->setFileNames(left, right);

      if( undistort_frames ) {
	if(ccv::debug) std::cerr << "Undistorting frames\n";
	stereo->setImages
	  ( ccocv ->undistortImage( c_left , false ),
	    ccocv2->undistortImage( c_right, false ) );
      }
      else stereo->loadImages( c_left, c_right );
    
      sprintf( c_out , "disparity_frame%05d.ppm", i );
      stereo->setOutFileName(string(c_out));

      if(errorCB->isChecked() && groundframeLE->text().length() > 0) {
	stereo->loadGround( groundframeLE->text().latin1() );
	stereo->setFindError(true);
      }
      else {
	if( errorCB->isChecked() ) {
	  err("ERROR - Could not calculate error (turned off)\n");
	  errorCB->setChecked(false);
	}
	stereo->setFindError(false);
      }
      
      double error = stereo->start();

      // ERRFLAG is raised if there are problems
      // with the parameters in the stereo class.
      if( !ccv::ERRFLAG ) {
	if(ccv::debug) std::cerr << "after start\n";

	calibPB->setProgress(calibPB->progress()+1);
	if(ccv::debug) std::cerr << "after setProgress\n";

	if(errorCB->isChecked() && stereo->getFindError() ) {
	  QString str;
	  QTextStream ts( &str, IO_WriteOnly );
	  ts << "Error: " 
	     << error << "\n";
	  write(str);
	}	  

	if( display_disparityCB->isChecked() ){
	  IplImage* idisp = stereo->getDisparityImage(); 
	  QImage* disp;
	  disp = ccv::iplImageToQImage(idisp);
	  imageOpen(*disp);
	  zap(disp);
	}
      }
      else {
	throw ccv::error( ccv::ERROR_MESSAGE.isEmpty() ? "ERROR - Stereoparameters\n" : 
			  ccv::ERROR_MESSAGE );
	ccv::resetError();
      }
      
      // Make time for the ui to update
      qApp->processEvents();
      if( calc_stop_flag ) {
	calc_stop_flag = false;
	throw ccv::error("ABORTED\n");
      }
    }

    clock_t stop_time = clock();
    float running_time = static_cast<float>(stop_time-start_time)/CLOCKS_PER_SEC;

    QString str;
    QTextStream ts( &str, IO_WriteOnly );
    ts << "Total time used: " 
       << running_time
       << " seconds, fps = " << ((stop-start)+1)/running_time << endl;
    write(str);

    if(ccv::debug) std::cerr << "freeing commandstring and stereo\n";

       write("Done\n");

    zap(stereo);
    if(ccv::debug) std::cerr << "deleted stereo...\n";

    free(c_left);
    free(c_right);
    free(c_out);
    
  }
  catch (ccv::error e) {
    // Must free everything allocated until error is throws
    // Should probably change this to the zap-macros later.
    free(c_left);
    free(c_right);
    free(c_out);
    err(e.msg);
  }

  stopB->setEnabled( false );

}

void
Peru::setSort(bool b)
{
  ccocv->setSort(b);
}

void
Peru::toggleParameters(const QString& string)
{
  if( string.compare("pyramidblock")==0 ){
    pyrSB->show();
    pyrL->show();
    pyrtSB->show();
    pyrtL->show();
  }
  else {
    pyrSB->hide();
    pyrL->hide();
    pyrtSB->hide();
    pyrtL->hide();
  }
}

void 
Peru::setFileName(QLineEdit* le)
{
  le->setText
    ( QFileDialog::getOpenFileName( "",
				    supportedFormats,
				    this,
				    "open file dialog"
				    "Choose a file" ) );
}

void 
Peru::setFileNameL() { setFileName( leftframeLE ); }

void 
Peru::setFileNameR() {  setFileName( rightframeLE ); }

void 
Peru::setFileNameG() { setFileName( groundframeLE ); }

void
Peru::setWth(bool b) { ccocv->setWth(b); }

void
Peru::write(const QString& str)
{
  emit stringSignal(str);
}

// The write functions writes to 
// gui output window.
void
Peru::write(const char* str)
{
  QString s(str);
  write(s);
}

void
Peru::write(const string& str)
{
  QString s(str.c_str());
  write(s);
}


void
Peru::initProgressBar(int start, int stop)
{
  calibPB->reset();
  calibPB->setTotalSteps(stop-start);
}

void
Peru::increaseProgressBar()
{
  calibPB->setProgress(calibPB->progress()+1); 
}

void
Peru::debugToggle( bool deb )
{
  ccv::debug = deb;
  std::cerr << "Debug is " << ccv::debug << "\n";
}

void
Peru::saveImage()
{

  QStrList formats = QImage::outputFormats();

  QString saveFormats;

  QStrListIterator it( formats );

  QString format;
  while( (format = it.current()) != 0 ) {
    ++it;
    saveFormats += "*.";
    saveFormats += format.lower();
    saveFormats += " ";
  }

  QString filename = 
    QFileDialog::getSaveFileName( "",
				  saveFormats,
				  this,
				  "Save Image Dialog"
				  "Choose a file" );

  QStringList file = QStringList::split( QChar('.'), filename );

  QString suffix;
  if( file.size() == 2 ) {
    suffix = file[1];

    if( suffix.upper() == QString("JPG") )
      suffix = QString("JPEG");

    if( formats.find( suffix.upper() ) != -1 ) {
      if( Image_widget->saveImage( filename, suffix.upper() ) )
	write("\nSaved image as " + filename + "\n");
      else
	err("\nERROR - Could not save image\n");
      return;
    }
    else {
      err("\nERROR - Not a valid filetype\n");
      return;
    }
  }
  
  if( !filename.isEmpty() )
    err("\nERROR - Not a valid filename (must have valid suffix)\n");
  
}  


void
Peru::viewTopHatSettings()
{
  ths->show();
}

void
Peru::updateImagesInQueueL()
{
  int nr = ccocv->getNumberOfFilesInList();

  QString str;
  QTextStream ts( &str, IO_WriteOnly );

  ts << "Images in queue: " << nr;

  imgqL->setText(str);
}

void
Peru::viewParams()
{
  if(ccv::debug) std::cerr << "Peru::viewParams\n";
  calPar->show();
}

void
Peru::updateParamsDialog()
{
  if(ccv::debug) std::cerr << "Peru::updateParamsDialog\n";
  CameraParams cp = ccocv->getParams();
  
  calPar->updateParameters(cp,true);
}

void Peru::err( const QString& err )
{
  QColor c = infoText->color();
  infoText->setColor( "red" );
  emit stringSignal( err );
  infoText->setColor( c );  
} 

int Peru::findTabPage( QTabWidget* tab, const QString page ) const
{
  for( int i=0; i<tab->count(); ++i )
    if( tab->label(i) == page )
      return i;

  return -1;
}

void Peru::removeImageFromCalibrationQueue(QString image)
{
  ccocv->removeFileName(image.latin1());
  updateImagesInQueueL();    
}

void 
Peru::clearCalibrationQueue() 
{ 
  ccocv->clearFileList();
  updateImagesInQueueL();    
}
