// -*- mode: C++; -*-
/*************************************************

 Purpose:
   Class for saving the calculated camera values.

 Author:
   Daniel Bengtsson 2002-2006, daniel@bengtssons.info

 Version:
   $Id: CameraParams.cpp,v 1.1 2006/11/16 23:48:31 cygnus78 Exp $

*************************************************/

#include "CameraParams.h"

#include "ccv.h"
#include <fstream>

using namespace std;

CameraParams::CameraParams() :
    rotMatr(0),
    transVect(0),
    rotMatrSize(0),
    transVectSize(0)
{
    // Isnt there a nicer way to initialize these ?
    for(int i=0;i<2;++i)
    {
        focalLength[i] = 0;
        principalPoint[i]=0;
    }

    for(int i=0;i<4;++i)
    {
        distortion[i]=0;
        distortionf[i]=0;
    }

    for(int i=0;i<9;++i)
    {
        matrix[i] = 0;
        matrixf[i] = 0;
    }
}

CameraParams::~CameraParams()
{
    zapArr(rotMatr);
    zapArr(transVect);
}

CameraParams::CameraParams(const CameraParams& rhs) :
    rotMatr(0),
    transVect(0),
    rotMatrSize(rhs.rotMatrSize),
    transVectSize(rhs.transVectSize)
{
    // Just initialize everything using the assignment operator
    *this = rhs;
}

CameraParams& CameraParams::operator=(const CameraParams& rhs)
{
    if( this == &rhs )
        return *this;

    for(int i=0;i<2;i++)
    {
        focalLength[i] = rhs.focalLength[i];
        principalPoint[i] = rhs.principalPoint[i];
    }

    for(int i=0;i<4;i++)
    {    
        distortion[i] = rhs.distortion[i];
        distortionf[i] = rhs.distortionf[i];
    }

    for(int i=0;i<9;i++)
    {    
        matrix[i] = rhs.matrix[i];
        matrixf[i] = rhs.matrixf[i];
    }

    // Be sure they are in sync
    assert( XNOR(rhs.rotMatr  ,rhs.rotMatrSize  ) );
    assert( XNOR(rhs.transVect,rhs.transVectSize) );

    rotMatrSize    = rhs.rotMatrSize;
    transVectSize  = rhs.transVectSize;

    delete [] rotMatr;
    delete [] transVect;
    
    if(rhs.rotMatr)
    {
        rotMatr = new double[rotMatrSize];
        for(uint i=0;i<rotMatrSize;++i)
            rotMatr[i]=rhs.rotMatr[i];
    }
    else rotMatr = 0;

    if(rhs.transVect)
    {
        transVect = new double[transVectSize];
        for(uint i=0;i<transVectSize;++i)
            transVect[i]=rhs.transVect[i];
    }
    else transVect = 0;

    return *this;
}

bool CameraParams::save(const char* file) const
{
    if(ccv::debug) std::cerr << "Saving parameters\n";
  
    ofstream of(file);
  
    of << "Focallength = " 
       << focalLength[0] << " "
       << focalLength[1] << " " << endl;
  
    of << "Distortion = "
       << distortion[0] << " "
       << distortion[1] << " "
       << distortion[2] << " "
       << distortion[3] << " " << endl;
  
    of << "PrincipalPoint = " 
       << principalPoint[0] << " "
       << principalPoint[1] << " " << endl;
  
    of << "Matrix = "
       << matrix[0] << " "
       << matrix[1] << " "
       << matrix[2] << " " << endl
       << matrix[3] << " " 
       << matrix[4] << " "
       << matrix[5] << " " << endl
       << matrix[6] << " "
       << matrix[7] << " "
       << matrix[8] << " " << endl;
  
    of.close();
  
    if(ccv::debug) std::cerr << "Parameters saved\n";
  
    return true;
}

bool CameraParams::load(const char* file)
{
    int MAX_LENGTH=40;
    int MAX_WORDS=40;
    char words[MAX_WORDS][MAX_LENGTH];
    
    int i=0;
    
    ifstream in(file);

    while(in.getline(words[i++],MAX_LENGTH,' ') && i<MAX_WORDS)
        if(ccv::debug) std::cerr << "i=%d" << i-1 << " : " << words[i-1] << endl;
  
    if(i<13) return false;

    if(ccv::debug) std::cerr << "Read from file...\n";
    for(int k=0;k<i;k++) if(ccv::debug) std::cerr << words[k] << endl;
  
    focalLength[0]    = atof(words[2]);
    focalLength[1]    = atof(words[3]);
  
    distortion[0]     = atof(words[6]);
    distortion[1]     = atof(words[7]);
    distortion[2]     = atof(words[8]);
    distortion[3]     = atof(words[9]);
  
    principalPoint[0] = atof(words[12]);
    principalPoint[1] = atof(words[13]);
  
    matrix[0]         = atof(words[16]);
    matrix[1]         = atof(words[17]);
    matrix[2]         = atof(words[18]);
    matrix[3]         = atof(words[19]);
    matrix[4]         = atof(words[20]);
    matrix[5]         = atof(words[21]);
    matrix[6]         = atof(words[22]);
    matrix[7]         = atof(words[23]);
    matrix[8]         = atof(words[24]);
  
    print();

    return true;

}

void CameraParams::print() const
{
    if(ccv::debug) std::cerr << "Focallength = " 
                             << focalLength[0] << " "
                             << focalLength[1] << endl;
    
    if(ccv::debug) std::cerr << "Distortion = "
                             << distortion[0] << " "
                             << distortion[1] << " "
                             << distortion[2] << " "
                             << distortion[3] << endl;
    
    if(ccv::debug) std::cerr << "PrincipalPoint = " 
                             << principalPoint[0] << " "
                             << principalPoint[1] << endl;
    
    if(ccv::debug) std::cerr << "Matrix = ";
    for(int i=0;i<9;i++)
        if(ccv::debug) std::cerr << matrix[i] << " ";
    if(ccv::debug) std::cerr << endl;
}
