/*************************************************

 Purpose:
   This dialog will show the parameters from
   the calibration or allow user to change or
   enter new parameters.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: CalibrationParameters.cpp,v 1.2 2004/08/21 17:18:56 cygnus78 Exp $

*************************************************/

#include "CalibrationParameters.h"

CalibrationParameters::CalibrationParameters( CCOCV* c,
				Peru* p,
				QWidget* parent, 
				const char* name,
				WFlags fl) 
  : CalibrationParametersBase(parent, name, fl), gui(p), calib(c) 
{
  
  connect( matrixLE,   SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( matrixLE_2, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( matrixLE_3, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( matrixLE_4, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( matrixLE_5, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( matrixLE_6, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( matrixLE_7, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( matrixLE_8, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( matrixLE_9, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );

  connect( distortionLE,   SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( distortionLE_2, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( distortionLE_3, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );
  connect( distortionLE_4, SIGNAL( textChanged(const QString&) ), 
	   this, SLOT( slot_parametersEdited() ) );

  connect( enableCB, SIGNAL( clicked() ), this, SLOT( enable() ) );

  connect( matrixLE,        SIGNAL( textChanged(const QString&) ),
	   focalLengthLE,   SLOT( setText(const QString&) ) );
  connect( matrixLE_5,      SIGNAL( textChanged(const QString&) ),
	   focalLengthLE_2, SLOT( setText(const QString&) ) );
  connect( matrixLE_3,      SIGNAL( textChanged(const QString&) ),
	   principalLE,     SLOT( setText(const QString&) ) );
  connect( matrixLE_6,      SIGNAL( textChanged(const QString&) ),
	   principalLE_2,   SLOT( setText(const QString&) ) );

}

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

  gui->setCalibrated( false, 1 );

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

  gui->setCalibrated( true, 1 );

  matrixLE_9->setText( s.setNum(cp.matrix[8]) );

}

void 
CalibrationParameters::slot_parametersEdited()
{
  parametersChanged();
  emit parametersEdited();
}

void
CalibrationParameters::enable()
{
  focalLengthLE  ->setText( "0" );
  focalLengthLE_2->setText( "0" );

  distortionLE  ->setText( "0" );
  distortionLE_2->setText( "0" );
  distortionLE_3->setText( "0" );
  distortionLE_4->setText( "0" );

  principalLE  ->setText( "0" );
  principalLE_2->setText( "0" );

  matrixLE  ->setText( "0" );
  matrixLE_2->setText( "0" );
  matrixLE_3->setText( "0" );
  matrixLE_4->setText( "0" );
  matrixLE_5->setText( "0" );
  matrixLE_6->setText( "0" );
  matrixLE_7->setText( "0" );
  matrixLE_8->setText( "0" );
  matrixLE_9->setText( "0" );

  gui->setCalibrated( true, 1 );

  parametersChanged();

  enableCB->setEnabled( false );

}
