// -*- mode: C++; -*-
/*************************************************

 Purpose:
   This dialog will show the parameters from
   the calibration or allow user to change or
   enter new parameters.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: CalibrationParameters.h,v 1.3 2004/08/21 17:18:56 cygnus78 Exp $

*************************************************/

#ifndef CALIBRATIONPARAMETERS_H
#define CALIBRATIONPARAMETERS_H

#include "ccv.h"
#include "CalibrationParametersBase.h"
#include "CCOCV.h"
#include "Peru.h"

#include <qwidget.h>


class CCOCV;
class Peru;

class CalibrationParameters : public CalibrationParametersBase
{
  Q_OBJECT
public:
  CalibrationParameters( CCOCV* c,
			 Peru* p,
			 QWidget* parent = 0, 
			 const char* name = 0,
			 WFlags fl = 0 );
  ~CalibrationParameters();
 
  void updateParameters(struct CameraParams cp);

public slots:
  void parametersChanged();

private:
  Peru* gui;
  CCOCV* calib;

private slots:

  void slot_parametersEdited();
  void enable(); //!< Enable the dialog for manual entering

signals:

  void parametersEdited();

};


#endif
