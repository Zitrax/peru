/*************************************************

 Purpose:
   This class will handle settings in between
   sessions.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: preferences.cpp,v 1.2 2005/05/25 21:08:10 cygnus78 Exp $

*************************************************/

#include "preferences.h"
#include "Peru.h"

#include <qsettings.h>

Preferences::Preferences( Peru* peru ) : m_peru( peru ) {}

void Preferences::readSettings() const
{
  QSettings settings( QSettings::Ini );

  settings.setPath( "bengtssons.info", "Peru" );

  // Global
  m_peru->scaleCB->setChecked( settings.readBoolEntry( "/peru/scale_image", false ) );
  m_peru->calibCB->setChecked( settings.readBoolEntry( "/peru/undistort_image", false ) );

  // Calibration Tab
  m_peru->wthCB->setChecked( settings.readBoolEntry( "/peru/white_top_hat", false ) );
  m_peru->montageCB->setChecked( settings.readBoolEntry( "/peru/montage", true ) );
  m_peru->debugCB->setChecked( settings.readBoolEntry( "/peru/debug", false ) );

  m_peru->etxSB->setValue( settings.readNumEntry( "/peru/eta_x_size", 4 ) );
  m_peru->etySB->setValue( settings.readNumEntry( "/peru/eta_y_size", 6 ) );

  m_peru->dimxLE->setText( QString::number( settings.readDoubleEntry( "/peru/dim_x_size", 1.0 ) ) );
  m_peru->dimyLE->setText( QString::number( settings.readDoubleEntry( "/peru/dim_y_size", 1.0 ) ) );

  // Stereo Tab
  m_peru->leftframeLE->setText( settings.readEntry( "/peru/left_frame" ) );
  m_peru->rightframeLE->setText( settings.readEntry( "/peru/right_frame" ) );
  m_peru->groundframeLE->setText( settings.readEntry( "/peru/ground_frame" ) );

  m_peru->stereo_algCOB->setCurrentItem( settings.readNumEntry( "/peru/algorithm", 0 ) );
  m_peru->undist_stCB->setChecked( settings.readBoolEntry( "/peru/undistort_frames", false ) );
  m_peru->display_disparityCB->setChecked( settings.readBoolEntry( "/peru/display_disparity_images", true ) );
  m_peru->write_disparityCB->setChecked( settings.readBoolEntry( "/peru/write_disparity_images", false ) );
  m_peru->colorCB->setChecked( settings.readBoolEntry( "/peru/use_color", true ) );
  m_peru->errorCB->setChecked( settings.readBoolEntry( "/peru/find_error", false ) );

  m_peru->framestartLE->setText( settings.readEntry( "/peru/frame_start", "0" ) );
  m_peru->frameendLE->setText( settings.readEntry( "/peru/frame_end", "0" ) );
  m_peru->mdSB->setValue( settings.readNumEntry( "/peru/max_disparity", 30 ) );
  m_peru->bsSB->setValue( settings.readNumEntry( "/peru/block_size", 3 ) );
  m_peru->pyrSB->setValue( settings.readNumEntry( "/peru/pyramid_levels", 3 ) );
  m_peru->pyrtSB->setValue( settings.readNumEntry( "/peru/pyramid_tolerance", 5 ) );

  m_peru->blurCB->setChecked( settings.readBoolEntry( "/peru/pre_gaussian", false ) );
  m_peru->hieqCB->setChecked( settings.readBoolEntry( "/peru/pre_histeq", false ) );
  m_peru->preMedianCB->setChecked( settings.readBoolEntry( "/peru/pre_median", false ) );
  m_peru->preMeanCB->setChecked( settings.readBoolEntry( "/peru/pre_meancorr", false ) );
  m_peru->postMedianCB->setChecked( settings.readBoolEntry( "/peru/post_median", false ) );
  m_peru->gradientrCB->setChecked( settings.readBoolEntry( "/peru/post_gradient_rem", false ) );
  m_peru->perspectivecCB->setChecked( settings.readBoolEntry( "/peru/post_perspective_corr", false ) );
  m_peru->contrastCB->setChecked( settings.readBoolEntry( "/peru/post_contrast_stretch", true ) );
  
}

void Preferences::writeSettings()
{
  QSettings settings( QSettings::Ini );

  settings.setPath( "bengtssons.info", "Peru" );

  // Global
  settings.writeEntry( "/peru/scale_image", m_peru->scaleCB->isChecked() );
  settings.writeEntry( "/peru/undistort_image", m_peru->calibCB->isChecked() );

  // Calibration Tab
  settings.writeEntry( "/peru/white_top_hat", m_peru->wthCB->isChecked() );
  settings.writeEntry( "/peru/montage", m_peru->montageCB->isChecked() );
  settings.writeEntry( "/peru/debug", m_peru->debugCB->isChecked() );
  
  settings.writeEntry( "/peru/eta_x_size", m_peru->etxSB->value() );
  settings.writeEntry( "/peru/eta_y_size", m_peru->etySB->value() );

  settings.writeEntry( "/peru/dim_x_size", m_peru->dimxLE->text().toDouble() );
  settings.writeEntry( "/peru/dim_y_size", m_peru->dimyLE->text().toDouble() );

  // Stereo Tab
  settings.writeEntry( "/peru/left_frame", m_peru->leftframeLE->text() );
  settings.writeEntry( "/peru/right_frame", m_peru->rightframeLE->text() );
  settings.writeEntry( "/peru/ground_frame", m_peru->groundframeLE->text() );

  settings.writeEntry( "/peru/algorithm", m_peru->stereo_algCOB->currentItem() );
  settings.writeEntry( "/peru/undistort_frames", m_peru->undist_stCB->isChecked() );
  settings.writeEntry( "/peru/display_disparity_images", m_peru->display_disparityCB->isChecked() );
  settings.writeEntry( "/peru/write_disparity_images", m_peru->write_disparityCB->isChecked() );

  settings.writeEntry( "/peru/use_color", m_peru->colorCB->isChecked() );
  settings.writeEntry( "/peru/find_error", m_peru->errorCB->isChecked() );

  settings.writeEntry( "/peru/frame_start", m_peru->framestartLE->text().toInt() );
  settings.writeEntry( "/peru/frame_end", m_peru->frameendLE->text().toInt() );
  settings.writeEntry( "/peru/max_disparity", m_peru->mdSB->value() );
  settings.writeEntry( "/peru/block_size", m_peru->bsSB->value() );
  settings.writeEntry( "/peru/pyramid_levels", m_peru->pyrSB->value() );
  settings.writeEntry( "/peru/pyramid_tolerance", m_peru->pyrtSB->value() );

  settings.writeEntry( "/peru/pre_gaussian", m_peru->blurCB->isChecked() );
  settings.writeEntry( "/peru/pre_histeq", m_peru->hieqCB->isChecked() );
  settings.writeEntry( "/peru/pre_median", m_peru->preMedianCB->isChecked() );
  settings.writeEntry( "/peru/pre_meancorr", m_peru->preMeanCB->isChecked() );
  settings.writeEntry( "/peru/post_median", m_peru->postMedianCB->isChecked() );
  settings.writeEntry( "/peru/post_gradient_rem", m_peru->gradientrCB->isChecked() );
  settings.writeEntry( "/peru/post_perspective_corr", m_peru->perspectivecCB->isChecked() );
  settings.writeEntry( "/peru/post_contrast_stretch", m_peru->contrastCB->isChecked() );

}
