/*************************************************

 Purpose:
   This is the main display, subclass of Perubase
   made in designer.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: Peru.cpp,v 1.6 2003/09/20 02:17:34 cygnus78 Exp $

*************************************************/

#include "Peru.h"

Peru::Peru( QWidget* parent, const char* name,
		  WFlags fl) : Perubase(parent,name,fl)
{

  pyrSB->hide();
  pyrL->hide();
  pyrtSB->hide();
  pyrtL->hide();

  correct_images=0;
  setCalibrated(false,1);
  setCalibrated(false,2);
  
  magick = !system("montage > /dev/null");             // Check if montage 
                                                       // exists in path
  if(magick) montageCB->setChecked(true);
  else montageCB->setEnabled(false);

  ccocv = new CCOCV();                                 // Create camera 
  ccocv2 = 0;                                          // calibration objects

  stereo = 0;

  Image_widget = new ImageWidget(scrollView->viewport());     
  Image_widget->setFixedWidth(3000);
  Image_widget->setFixedHeight(3000);
  scrollView->addChild(Image_widget);

  ccv::debug = debugCB->isChecked();

  if(ccv::debug) std::cerr << endl;
  
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

  calPar = new CalibrationParameters( ccocv, this, this );
  calPar->hide();
}


// Not strictly needed...
Peru::~Peru(){
  if(ccv::debug) std::cerr << "DESTRUCTING Peru\n"; 
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

  connect( scaleCB,                                    // Scale image checkbox
	   SIGNAL( toggled(bool) ), 
	   Image_widget, 
	   SLOT( setScaled(bool) ) 
	   );

  connect( calibCB,                                    // Undistort checkbox
	   SIGNAL( toggled(bool) ),
	   this, 
	   SLOT( undistortImage(bool) )
	   );

  connect( this,                                       // Receives string 
	   SIGNAL( stringSignal(const QString&) ),     // and prints them
	   infoText,
	   SLOT( insert(const QString&) )
	   );

  connect( calibB,
	   SIGNAL( clicked() ),
	   this,
	   SLOT( calibrate() )
	   );

  connect( montageCB,
	   SIGNAL( toggled(bool) ),
	   this,
	   SLOT( montageCheck(bool) )
	   );

  connect( saveParamsB,
	   SIGNAL( clicked() ),
	   this,
	   SLOT( saveParams() )
	   );

  connect( load_leftB,
	   SIGNAL( clicked() ),
	   this,
	   SLOT( setFileNameL() )
	   );

  connect( load_rightB,
	   SIGNAL( clicked() ),
	   this,
	   SLOT( setFileNameR() )
	   );
  
  connect( loadParamsB,
	   SIGNAL( clicked() ),
	   this,
	   SLOT( loadParams() )
	   );

  connect( left_paramsB,
	   SIGNAL( clicked() ),
	   this,
	   SLOT( loadParams() )
	   );

  connect( right_paramsB,
	   SIGNAL( clicked() ),
	   this,
	   SLOT( loadParams2() )
	   );

  connect( undistortB,
	   SIGNAL( clicked() ),
	   this,
	   SLOT( undistortSerie() )
	   );

  connect( calculateB,
	   SIGNAL( clicked() ),
	   this,
	   SLOT( calculateStereo() )
	   );

  connect( sortCornersCB,
	   SIGNAL( toggled(bool) ),
	   this,
	   SLOT( setSort(bool) )
	   );

  connect( wthCB,
	   SIGNAL( toggled(bool) ),
	   this,
	   SLOT( setWth(bool) )
	   );
  
  connect( stereo_algCOB,
	   SIGNAL( activated( const QString &) ),
	   this, 
	   SLOT( toggleParameters(const QString&) )
	   );

  connect( debugCB,
	   SIGNAL( toggled(bool) ),
	   this,
	   SLOT( debugToggle(bool) )
	   );

  connect( Image_widget,
	   SIGNAL( sendMousePressed(QMouseEvent*) ),
	   this,
	   SLOT( mousePressed(QMouseEvent*) )
	   );

  connect( Image_widget,
	   SIGNAL( sendMouseReleased(QMouseEvent*) ),
	   this,
	   SLOT( mouseReleased(QMouseEvent*) )
	   );
  
  connect( saveImageB,
	   SIGNAL( clicked() ),
	   this,
	   SLOT( saveImage() )
	   );

}

void 
Peru::imageOpen()
{
  QString filename;

  filename = QFileDialog::getOpenFileName( "",
					   supportedFormats,
					   this,
					   "open file dialog"
					   "Choose a file" );
 
  if(filename.isEmpty()) { if(ccv::debug) std::cerr << "Error empty file\n"; }
  else { 
    emit stringSignal("\nOpened image: "+filename+"\n");
    calibCB->setChecked(false);
    fileName = filename.latin1();
    if(ccv::debug) std::cerr << filename << "\n"; 
    QImage *p_image = new QImage(filename,0);
    if (*p_image==NULL) { emit stringSignal("ERROR - image\n"); }
    else {
      imageOpen(*p_image);
    }
  }
    
}

void 
Peru::calibImageOpen()
{
  QStringList filelist = openFiles();
  QString filename;
  
  if(filelist.isEmpty()) { emit stringSignal("Error no files\n"); }
  else { 
    montage(filelist);
    calibPB->reset();
    calibPB->setTotalSteps(filelist.size()-1);
    while(!filelist.isEmpty()){
      filename = filelist.back();
      emit stringSignal("Added calib image: "+filename+"\n");
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
Peru::imageOpen(QImage image)
{
  if(ccv::debug) std::cerr << "Running imageOpen2\n";
  Image_widget->displayImage(image);
}

void
Peru::calibImageOpen(const QString& name)
{
  if(ccv::debug) std::cerr << "Inside calibImageOpen with string " << name << "\n";
  ccocv->addFileName(name.latin1());
  updateImagesInQueueL();
}

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
  correct_images = 0;

  initializeCCOCV();

  QString str;
  QTextStream ts( &str, IO_WriteOnly );
  
  calibPB->reset();
  
  while(ccocv->findCorners(corners)==1){
    if(corners==(etxSB->value()-1)*(etySB->value()-1)) correct_images++;
    calibPB->setProgress(calibPB->progress()+1);
    str="";
    if( corners < 0 )
      ts << "\nFound " << -1*corners << " chess corners (Rejected by sort)";
    else {
      ts << "\nFound " << corners << " chess corners";
      if(corners==(etxSB->value()-1)*(etySB->value()-1))
	ts << " *";
      updateImagesInQueueL();
    }
  
    emit stringSignal(str);
    repaint();
  }

  str="\n";
  ts << correct_images << " images correctly calibrated\n";
  if(correct_images>0) {

    int tiles = static_cast<int>(sqrt(static_cast<float>(orig_nr_images)));
    if(tiles*tiles<orig_nr_images) tiles++;

    if( montageCB->isChecked() ){
      ccocv->drawCorners("tmp_montage.bmp",1,12,
			 tiles,
			 100,
			 static_cast<int> (100*(static_cast<float> 
						(ccocv->getImageSizeY())/
						ccocv->getImageSizeX())),
			 ccocv->getImageSizeX(),
			 ccocv->getImageSizeY(), true);
      
      QImage* p_image = new QImage("tmp_montage.bmp",0);
      if (*p_image==NULL) { emit stringSignal("ERROR - image (drawCorners)\n"); }
      else {
	imageOpen(*p_image);
	zap(p_image);
      }
    }

    saveParamsB->setEnabled(true);
    setCalibrated(true,1);
  }
  else {
    saveParamsB->setEnabled(false);
    setCalibrated(false,1);
  }
  emit stringSignal(str);
}

void
Peru::undistortImage(bool undistort)
{
  char* s = new char[200];         // Allocate memory for string
  s[0]='\0';                       // Empty string
  strcat(s,fileName.c_str());      // Append filename
  QImage *p_image;

  if(calibrated) {
    if(undistort && fileName.size()>0){
      ccocv->undistortImage(fileName,true);
      strcat(s,"_undistorted");
      p_image = new QImage(QString(s),0);  
      if(ccv::debug) std::cerr << "p_image is " << s << endl;
    }
    else {
      if(ccv::debug) std::cerr << "Taking back - " << s << endl;
      p_image = new QImage(QString(s),0);    
    }
    imageOpen(*p_image);
    zap(p_image);
    delete[] s;

  }
  else emit stringSignal("WARNING - You have not calibrated "
			 "or calibration has failed\n");
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
      emit stringSignal("\nUndistorted image "+filename);
      filelist.pop_back();
    }
  }
  else emit stringSignal("WARNING - You have not calibrated "
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
  int images=0;
  if(montageCB->isChecked() && !flist.isEmpty()){
    
    QString image_list;
    QString tmp = "tmp_montage.bmp";

    while( ! flist.isEmpty() )
      {
	image_list.append(flist.back()+" ");
	images++;
	flist.pop_back();
      }

    QString command;
    QTextStream ts( &command, IO_WriteOnly );
    int tiles = static_cast<int>(sqrt(static_cast<float>(images)));
    if(tiles*tiles<images) tiles++;
    ts << "montage -geometry 100x+1+0 -borderwidth 0x0 -label \"%f\""
       << " -filter triangle -font ps:courier -pointsize 12"
       << " -tile " 
       << tiles << "x" << tiles << " ";
    if(ccv::debug) std::cerr << command;;
    
    command.append(image_list);
    command.append(tmp);
    
    emit stringSignal("Making thumbnails...\n");

    system(command.latin1());

    emit stringSignal("Done\n");

    QImage* p_image = new QImage(tmp,0);
    if (*p_image==NULL) { emit stringSignal("ERROR - image\n"); }
    else {
      imageOpen(*p_image);
      zap(p_image);
      // system("rm "+tmp);
    }

  }
  else return;
}

void 
Peru::montageCheck(bool checked)
{
  if(!magick && checked) {
    montageCB->setChecked(false);
    emit stringSignal("Sorry - can't fint montage in your path\n");
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
      std::cerr << "Creating ccocv2\n"; ccocv2 = new CCOCV; 
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
      if(ccocv->loadParams( filename.latin1() )) 
	setCalibrated(true,cam);
      saveParamsB->setEnabled(true);
    }
    else if( cam==2 ) {
      if(ccocv2->loadParams( filename.latin1() ))
	setCalibrated(true,cam);
    }
    else if(ccv::debug) std::cerr << "ERROR of parameter to loadParams\n";
  }
}

void
Peru::setCalibrated(bool c, int cam)
{
  if( cam==1 ) {
    calibrated = c;
    if(!c)
      calibLed->setPaletteBackgroundColor(QColor::QColor(255,0,0));
    else {
      calibLed->setPaletteBackgroundColor(QColor::QColor(0,255,0));
      updateParamsDialog();
      if( calibrated2 )
	calibLed_2->setPaletteBackgroundColor(QColor::QColor(0,255,0));
    }
  }
  if( cam==2 ) {
    calibrated2 = c;
    if(!c)
      calibLed_2->setPaletteBackgroundColor(QColor::QColor(255,0,0));
    else if ( calibrated )
      calibLed_2->setPaletteBackgroundColor(QColor::QColor(0,255,0));
  }
}

void
Peru::calculateStereo()
{
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

  try {

    if( undistort_frames && (!calibrated2 || !calibrated)) {
      throw ccv::error("ERROR - You have not calibrated all cameras\n");
    }
    
    if(ccv::debug) std::cerr << "Method = " 
			     << stereo_algCOB->currentText() << endl;
    
    //  Fill filename strings
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
      stereo = new CvBirchfield(left, right, out, md);

    else if(stereo_algCOB->currentText()=="pyramidblock")
      stereo = new PyramidBlock(left, right, out, md, 
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

    // Add Filters

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

    // END - FILTERS
    
    if(!undistort_frames && 
       stereo_algCOB->currentText()!="magickblock") stereo->unSetImages();

    stereo->setDisparityToDisk(write_disparityCB->isChecked());

    calibPB->reset();
    calibPB->setTotalSteps(stop-start+1);
    calibPB->setProgress(calibPB->progress()+1);

    emit stringSignal("Calculating stereo...\n");

    clock_t start_time = clock(); 

    for( int i = start; i <=stop; i++ ) {

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

      if(stereo->start()) {
	if(ccv::debug) std::cerr << "after start\n";
	calibPB->setProgress(calibPB->progress()+1);
	if(ccv::debug) std::cerr << "after setProgress\n";
	if( display_disparityCB->isChecked() ){
	  IplImage* idisp = stereo->getDisparityImage(); 
	  QImage* disp;
	  disp = ccv::iplImageToQImage(idisp);
	  imageOpen(*disp);
	  zap(disp);
	}
      }
      else emit stringSignal("ERROR - Stereoparameters\n");
    }

    clock_t stop_time = clock();
    float running_time = static_cast<float>(stop_time-start_time)/CLOCKS_PER_SEC;

    QString str;
    QTextStream ts( &str, IO_WriteOnly );
    ts << "Total time used: " 
       << running_time
       << " seconds, fps = " << ((stop-start)+1)/running_time << endl;
    emit stringSignal(str);

    if(ccv::debug) std::cerr << "freeing commandstring and stereo\n";

       emit stringSignal("Done\n");

    zap(stereo);
    if(ccv::debug) std::cerr << "deleted stereo...\n";

    free(c_left);
    free(c_right);
    free(c_out);
    
  }
  catch (ccv::error e) {
    
    free(c_left);
    free(c_right);
    free(c_out);
    emit stringSignal(e.msg);
    
  }

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
Peru::setWth(bool b) { ccocv->setWth(b); }

void
Peru::write(const QString& str)
{
  emit stringSignal(str);
}

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
  emit stringSignal(s);
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
Peru::mousePressed(QMouseEvent* e)
{
  if(ccv::debug) std::cerr << "Peru::mousePressed()\n";
  //   int x = e->x();
  //   int y = e->y();
  //   int z = viewSliceSB->value();

}

void
Peru::mouseReleased(QMouseEvent* e)
{
  if(ccv::debug) std::cerr << "Peru::mouseReleased()\n";
  //   int x = e->x();
  //   int y = e->y();
  //   int z = viewSliceSB->value();

}

void
Peru::saveImage()
{
  Image_widget->saveImage();
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
  if(ccv::debug) std::cerr << "Peru::updateParams\n";
  CameraParams cp = ccocv->getParams();
  
  calPar->updateParameters(cp);
}
