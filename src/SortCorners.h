/*************************************************

 Purpose:
   Sorting the found corners right

 Original Author:
   James Stewart

 Modifications:
   Daniel Bengtsson 
     - Corrected code to max 80 columns
     - Classified

 Version:
   $Id: SortCorners.h,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#ifndef SORT_CORNERS_H
#define SORT_CORNERS_H

#include "ccv.h"
#include <opencv/cv.h>    // CvPoint2D32f
#include <float.h> // MAX_FLT
#include <cmath>   // atan
#include <stdlib.h>
#include <stdio.h>

// you will need this definition in your source file
#define CORNER_FINDER_NEIGHBOUR_LIMIT   8

// and these ones
#define RAD_TO_DEG(r) (360.0f/(2.0f*3.1415927f)*r)
#define PI      3.1415927

// these definitions are for defaults of sorting tolerances
#define DEFAULT_SEQUENCER_TOLERANCE     35.0f
#define DEFAULT_ORDERING_ITERATIONS     10000

// corner categories
#define CORNER_FINDER_CORNER_UNCATEGORISED            0
#define CORNER_FINDER_CORNER_OUTER                    1
#define CORNER_FINDER_CORNER_EDGE                     2
#define CORNER_FINDER_CORNER_INNER                    3
#define CORNER_FINDER_CORNER_TOP_LEFT                 4
#define CORNER_FINDER_CORNER_TOP_RIGHT                5
#define CORNER_FINDER_CORNER_BOTTOM_LEFT              6
#define CORNER_FINDER_CORNER_BOTTOM_RIGHT             7

// corner finder extreme signs
#define CORNER_FINDER_EXTREME_NEGATIVE               -1
#define CORNER_FINDER_EXTREME_POSITIVE                1

// angle comparisons
#define ANGLE_TL_TR                             1
#define ANGLE_TL_BL                             2
#define ANGLE_TL_BR                             3
#define ANGLE_TR_TL                             4
#define ANGLE_TR_BL                             5
#define ANGLE_TR_BR                             6
#define ANGLE_BL_TL                             7
#define ANGLE_BL_TR                             8
#define ANGLE_BL_BR                             9
#define ANGLE_BR_TL                             10
#define ANGLE_BR_TR                             11
#define ANGLE_BR_BL                             12


// you will need these structures defined in your source or header file:

// declare a structure for determining the top-left, top-right, bottom-left 
// and bottom-right corner indexes
struct tagDISTANCEDIFFERENCE
{
  bool bXExtremeNeighbour1;
  bool bXExtremeNeighbour2;
  int nSignExtremeNeighbour1;
  int nSignExtremeNeighbour2;
};

// declare a structure for storing the angles of 
// each outer corner to each other outer corner
struct tagOUTERANGLES
{
  float fAngleTL_TR;
  float fAngleTL_BL;
  float fAngleTL_BR;

  float fAngleTR_TL;
  float fAngleTR_BL;
  float fAngleTR_BR;

  float fAngleBL_TL;
  float fAngleBL_TR;
  float fAngleBL_BR;

  float fAngleBR_TL;
  float fAngleBR_TR;
  float fAngleBR_BL;
};



class SortCorners 
{

 public:

  SortCorners(CvPoint2D32f* corners, int corners_found, int cx, int cy);

  //! this is the main sorting routine
  bool OrderApproxCorners();  /*!< put the approximate corner in 
				row/column order from top-left 
				to bottom-right  */

//! corner type determination
  float IndexExtremeAnglesFrom3AndGetRange(float * pfAngles,
					   int &refnMinimumIndex,
					   int &refnMaximumIndex);

  void IndexExtremeAnglesFrom4(float * pfAngles,
			       int &refnExtremeMin,
			       int &refnExtremeMax);

  //! sequencing validity testing
  void FindDiagonalNeighboursFrom4(float * pf4NeighbourAngles,
				   tagOUTERANGLES * pOuterAngles,bool * pbDiagonals);


  bool getSuccess();

 private:

  bool success; //!< Was the ordering successful 
  
  // these two variables really need to hold the same numbers
  // the code uses this variable for the number of corners in the image
  int m_nTotalCorners;
  //! and this variable holds the number of corners that have been found
  int m_nCornersFound;
  
  //!the numbers of horizontal and vertical inner corners in the pattern
  int m_nXCorners;
  int m_nYCorners;
  
  CvPoint2D32f* m_pCornersPixels;   /*!< fill this with 
				      the approximate 
				      corner locations 
				      for a single 
				      image */

 
  /*! the sort relies on the 8 nearest neighbours to each corner in the image, so 
    these arrays of pointers define the neighbours to each corner
    and the distance of each neighbour to each corner */
  int ** m_ppCornersNeighboursLists;                    /*!< lists of indexes of 
							  nearest neighbours 
							  to each corner */
  
  float ** m_ppCornersNeighboursDistances;              /*!< lists of distances 
							  to nearest neighbour 
							  corners */
  
  // angles between neighbours are also needed
  float ** m_ppAnglesCornerToNeighbour;                 /*!< holds angles of test 
							  corner to each 
							  neighbour */
  
  float ** m_ppAnglesNeighbourToNeighbourZero;          /*!< holds differences 
							  between the test-corner
							  to the nearest neighbour
							  angle and the tes-corner
							  to the other 
							  neighbour angles */
 
  //! corners fit into 3 categories - outer, edge and inner
  int * m_pCategorisedUnsortedCorners;

  /*! a flag to indicate the sorted status -  you probably don't need this, it
    was from the C++ class that this code was extracted from */
  bool m_bApproxCornersOrdered;
  
  // I use this variable to allow some tolerance control over the sorting routine
  float m_fSequenceAngleRange;
  
  // this variable prevents an infinite loop in the sequencer
  int m_nMaxOrderIterations;

  

};











#endif
