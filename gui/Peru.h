// -*- mode: C++; -*-
/*************************************************

 Purpose:
   This is the main display, subclass of perubase
   made in designer.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: Peru.h,v 1.14 2005/05/25 21:29:30 cygnus78 Exp $

*************************************************/

#ifndef PERU_H
#define PERU_H

#include "ccv.h"
#include ".ui/Perubase.h"
#include "CCOCV.h"
#include "ImageWidget.h"
#include "kstartuplogo.h"
#include "stereo.h"
#include "BlockMatch.h"
#include "CvBirchfield.h"
#include "PyramidBlock.h"
#include "Filter.h"
#include "MedianFilter.h"
#include "Gaussian.h"
#include "MeanCorrection.h"
#include "HistoEq.h"
#include "GradientRemoval.h"
#include "PerspectiveCorrect.h"
#include "ContrastStretch.h"
#include "TopHatSettings.h"
#include "CalibrationParameters.h"
#include "QLed.h"
#include "preferences.h"
#include "MontageView.h"

#include <string>
#include <iostream>
#include <cmath>
#include <unistd.h>
#include <time.h>

#include <qcheckbox.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qimage.h>
#include <qstring.h>
#include <qprogressbar.h>
#include <qcolor.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qslider.h>
#include <qtabwidget.h>
#include <qdial.h>

#include <opencv/cv.h>

using namespace std;

class CCOCV;
class Matcher;
class TopHatSettings;
class CalibrationParameters;
class Stereo;

class Peru : public Perubase
{
   Q_OBJECT
 public:
   Peru( QWidget* parent = 0, 
	    const char* name = 0,
	    WFlags fl = 0 );
   ~Peru();
   void imageOpen(QImage& image);       //!< Displays image on widget
   void calibImageOpen(const QString&); //!< Adds an image to CCOCV
   bool scaled();                       //!< Returns true if image
                                        //!< should be scaled

   void montage(QStringList flist);     //!< Makes a montage image

   void write(const QString& str);      //!< Emits string signals from outside
   void write(const char* str);         //!< Emits string signals from outside
   void write(const string& str);       //!< Emits string signals from outside
   void initProgressBar(int start,      //!< Init and reset progressbar
			int stop);      //!< Progress progressbar
   void increaseProgressBar();
   void initializeCCOCV();              //!< Transfer settings to CCOCV

 public slots:
   void imageOpen();
   void imageOpen(QString);
   void calibImageOpen();               //!< Opens file dialog for
                                        //!< calibrationimages
   void calibrate();                    //!< Starts calibration
   void setCalibrated(bool c,int cam);  //!< Toggles calibration leds
   void undistortImage(bool undistort); //!< Undistorts image according to 
                                        //!< calculated parameters
   void removeImageFromCalibrationQueue(QString image);
   void clearCalibrationQueue();
   void undistortImage();               //!< Undistorts according to checkbox
   void loadParams();                   //!< Load cam1/left parameters
   void loadParams2();                  //!< Load cam2/right parameters 
   void undistortSerie();               //!< Undistorts a serie of images
   void montageCheck(bool checked);     //!< Checks if montage is possible
   void saveParams();                   //!< Saves found parameters to file

   void calculateStereo();              /*!< Calculate disparity maps for
					  serie of images-pairs */
   void setSort(bool b);                //!< Toggles corner sort in CCOCV
   void setWth(bool b);                 //!< Use white top hat
   void toggleParameters
     (const QString& string);
   void setFileNameL();      
   void setFileNameR();      
   void setFileNameG();      

   void debugToggle(bool deb);          //!< Toggle debug output

   void saveImage();

   void viewTopHatSettings();           //!< View top hat settings dialog
   void viewParams();                   //!< View parameters in a dialog

   void stopCalculation()               //!< Will stop the current calculation
   { calc_stop_flag = true; } 

 signals:

   void stringSignal(const QString&);   //!< Emits a signal with a string ;)

 private:
   CCOCV* ccocv;                        /*!< Pointer to CCOCV class which does
					  the camera calibration */
   CCOCV* ccocv2;                       /*!< Pointer to additional calibration
					  object for stereo cameras */
   Stereo* stereo;                      /*!< Pointer to stereo class which
					  calculates the disparity maps */
   Matcher* matcher;                    //<! Matcher for ct/disp data

   QImage* currentDisplayedImage;       //<! Image displayed in ImageWidget

   ImageWidget* Image_widget;           //!< Displays images
   
   TopHatSettings* ths;                 //!< Dialog for TopHatSettings

   CalibrationParameters* calPar;       //!< Constatnly updates parameters dialog

   Preferences* prefs;                  //!< This file handles the stored settings

   QString supportedFormats;            //!< Image extensions

   QStringList openFiles();             //!< Returns a list of files from a dialog
   string fileName;                     //!< Name of currently displayed image
   bool magick;                         //!< True if imagemagick is available
   int correct_images;                  //!< Number of correctly calibrated images
   int colormodel;                      //!< Color model for raycaster
   int slicestart;                      //!< Used in view distMap
   int slicestop;                       //!< Used in view distMap
   bool calibrated;                     //!< Do we have parameter values ?
   bool calibrated2;                    //!< Is camera2 calibrated
   bool matcherAllocated;               //!< Marks if matcher class object exists
   bool calc_stop_flag;                 //!< True if a calculation should stop

   const static QString tmpImage;       //!< Temporary filename of montage image

   void loadParams(int cam);            //!< Loads saved parameter file
   void connectSignalsToSlots();        //!< Take a wild guess
   void setFileName(QLineEdit* le);     //!< Fill in filename in line-edit
   void updateImagesInQueueL();         //!< Updates label imgqL with correct data
   void updateParamsDialog();
   void err( const QString& err );      //!< Give error message to the textEdit
   void imageOpen_(QString);

   int findTabPage( QTabWidget* tab, const QString page ) const;
};

#endif
