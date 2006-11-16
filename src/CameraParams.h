// -*- mode: C++; -*-
/*************************************************

 Purpose:
   Class for saving the calculated camera values.

 Author:
   Daniel Bengtsson 2002-2006, daniel@bengtssons.info

 Version:
   $Id: CameraParams.h,v 1.1 2006/11/16 23:48:31 cygnus78 Exp $

*************************************************/

//! Holds all camera parameters
class CameraParams {
public:

    CameraParams();
    ~CameraParams();

    CameraParams(const CameraParams& rhs);
    CameraParams& operator=(const CameraParams& rhs);

    double focalLength[2];
    double distortion[4];
    float distortionf[4]; 
    double principalPoint[2];
    double matrix[9];
    float matrixf[9];
    double* rotMatr;
    double* transVect;

    unsigned int rotMatrSize;
    unsigned int transVectSize;

    bool save(const char* file) const; 
    bool load(const char* file);

    void print() const;
};
