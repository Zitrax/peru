/*************************************************

 Purpose:
   This dialog will show the parameters from
   the calibration or allow user to change or
   enter new parameters.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: CalibrationParameters.cpp,v 1.1 2003/09/17 11:59:37 cygnus78 Exp $

*************************************************/

#include "CalibrationParameters.h"

CalibrationParameters::CalibrationParameters( CCOCV* c,
				Peru* p,
				QWidget* parent, 
				const char* name,
				WFlags fl) 
  : CalibrationParametersBase(parent, name, fl), gui(p), calib(c) {}

CalibrationParameters::~CalibrationParameters(){}
 
void
CalibrationParameters::parametersChanged()
{
  if(ccv::debug) std::cerr << "CalibrationParameters::parametersChanged\n";

  struct CameraParams cp;

  cp.focalLength[0] = focalLengthLE  ->text().toDouble();
  cp.focalLength[1] = focalLengthLE_2->text().toDouble();

  cp.distortion[0] = distortionLE  ->text().toDouble();
  cp.distortion[1] = distortionLE_2->text().toDouble();
  cp.distortion[2] = distortionLE_3->text().toDouble();
  cp.distortion[3] = distortionLE_4->text().toDouble();

  cp.principalPoint[0] = principalLE  ->text().toDouble();
  cp.principalPoint[1] = principalLE_2->text().toDouble();

  cp.matrix[0] = matrixLE  ->text().toDouble();
  cp.matrix[1] = matrixLE_2->text().toDouble();
  cp.matrix[2] = matrixLE_3->text().toDouble();
  cp.matrix[3] = matrixLE_4->text().toDouble();
  cp.matrix[4] = matrixLE_5->text().toDouble();
  cp.matrix[5] = matrixLE_6->text().toDouble();
  cp.matrix[6] = matrixLE_7->text().toDouble();
  cp.matrix[7] = matrixLE_8->text().toDouble();
  cp.matrix[8] = matrixLE_9->text().toDouble();

  calib->setCameraParams(cp);
}

void 
CalibrationParameters::updateParameters(struct CameraParams cp)
{
  if(ccv::debug) std::cerr << "CalibrationParameters::updateParameters\n";
  QString s;

  focalLengthLE  ->setText( s.setNum(cp.focalLength[0]) );
  focalLengthLE_2->setText( s.setNum(cp.focalLength[1]) );

  distortionLE  ->setText( s.setNum(cp.distortion[0]) );
  distortionLE_2->setText( s.setNum(cp.distortion[1]) );
  distortionLE_3->setText( s.setNum(cp.distortion[2]) );
  distortionLE_4->setText( s.setNum(cp.distortion[3]) );

  principalLE  ->setText( s.setNum(cp.principalPoint[0]) );
  principalLE_2->setText( s.setNum(cp.principalPoint[1]) );

  matrixLE  ->setText( s.setNum(cp.matrix[0]) );
  matrixLE_2->setText( s.setNum(cp.matrix[1]) );
  matrixLE_3->setText( s.setNum(cp.matrix[2]) );
  matrixLE_4->setText( s.setNum(cp.matrix[3]) );
  matrixLE_5->setText( s.setNum(cp.matrix[4]) );
  matrixLE_6->setText( s.setNum(cp.matrix[5]) );
  matrixLE_7->setText( s.setNum(cp.matrix[6]) );
  matrixLE_8->setText( s.setNum(cp.matrix[7]) );
  matrixLE_9->setText( s.setNum(cp.matrix[8]) );

}
