/*************************************************

 Purpose:
   Sorting the found corners right

 Original Author:
   James Stewart

 Modifications:
   Daniel Bengtsson 
     - Corrected code to max 80 columns
     - Classified
     - Quadruple commented out unused variables   

 Version:
   $Id: SortCorners.cpp,v 1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#include "SortCorners.h"

SortCorners::SortCorners(CvPoint2D32f* corners, 
			 int corners_found, int cx, int cy)
{
  m_nTotalCorners = corners_found;
  m_nCornersFound = corners_found;
  
  m_nXCorners  = cx;
  m_nYCorners  = cy;

  m_fSequenceAngleRange = DEFAULT_SEQUENCER_TOLERANCE;
  
  m_nMaxOrderIterations = DEFAULT_ORDERING_ITERATIONS;
  
  m_pCornersPixels = corners;

  m_bApproxCornersOrdered = false;

  m_ppCornersNeighboursLists = new int *[m_nTotalCorners];
  m_ppCornersNeighboursDistances = new float *[m_nTotalCorners];
  m_ppAnglesCornerToNeighbour =  new float *[m_nTotalCorners];
  m_ppAnglesNeighbourToNeighbourZero =  new float *[m_nTotalCorners];
  m_pCategorisedUnsortedCorners = new int[m_nTotalCorners];


  // prepare the necessary arrays
  
  // blank the contents of the arrays
  for(int i=0;i<m_nTotalCorners;i++)
    {
      m_ppCornersNeighboursLists[i] = 0;
      m_ppCornersNeighboursDistances[i] = 0;
      m_pCategorisedUnsortedCorners[i] = CORNER_FINDER_CORNER_UNCATEGORISED;
      m_ppAnglesCornerToNeighbour[i]=0;
      m_ppAnglesNeighbourToNeighbourZero[i] = 0;
    }

  // allocate the lists for neighbours of each corner
  for(int i=0;i<m_nTotalCorners;i++)
    {
      m_ppCornersNeighboursLists[i] = 
	new int[CORNER_FINDER_NEIGHBOUR_LIMIT];
      m_ppCornersNeighboursDistances[i] = 
	new float[CORNER_FINDER_NEIGHBOUR_LIMIT];
      m_ppAnglesCornerToNeighbour[i] = 
	new float[CORNER_FINDER_NEIGHBOUR_LIMIT];
      m_ppAnglesNeighbourToNeighbourZero[i] = 
	new float[CORNER_FINDER_NEIGHBOUR_LIMIT];
        
      if(m_ppCornersNeighboursLists[i]==0 || 
	 m_ppCornersNeighboursDistances[i]==0 ||
	 m_ppAnglesCornerToNeighbour[i]==0 || 
	 m_ppAnglesNeighbourToNeighbourZero[i]==0)
	{
	  // Release();                      
	  // ASSERT(FALSE);
	  // return false;
	}

      // fill the slots with -1 to indicate no neighbour, FLT_MAX 
      // to indicate no distance , 0,0f to zero angle test arrays
      for(int n=0;n<CORNER_FINDER_NEIGHBOUR_LIMIT;n++)
	{
	  m_ppCornersNeighboursLists[i][n] = -1;
	  m_ppCornersNeighboursDistances[i][n] = FLT_MAX;
	  m_ppAnglesCornerToNeighbour[i][n] = 0.0f;
	  m_ppAnglesNeighbourToNeighbourZero[i][n] = 0.0f;
	}
    }

  success = OrderApproxCorners();
  if(ccv::debug) printf("Return of OrderApproxCorners() = %d\n",success);
  

}

bool SortCorners::getSuccess() { return success; }

// the progress control is used to show how the sort is progressing. 
// You can pass 0 as the argument and the progress 
// bar StepIt() calls will have no effect
bool SortCorners::OrderApproxCorners()
{
  m_bApproxCornersOrdered = false;
  
  // no point in trying to order if we do not 
  // have enough neighbours for comparisons
  if(m_nCornersFound<CORNER_FINDER_NEIGHBOUR_LIMIT)
    return true;

  // find the nearest CORNER_FINDER_NEIGHBOUR_LIMIT neighbouring corners 
  // to each corner, store their indexes in ppCornersNeighboursLists
  float fXDist = 0.0f;
  float fYDist = 0.0f;
  float fDist = 0.0f;
  int nTestCorner=0;
  int nNeighbourCorner = 0;
  int nInsertPos = CORNER_FINDER_NEIGHBOUR_LIMIT;
  int * pIndex = 0;
  float * pDistance = 0;

  for(nTestCorner=0;nTestCorner<m_nTotalCorners;nTestCorner++)
    {
      for(nNeighbourCorner = 0;
	  nNeighbourCorner<m_nTotalCorners;
	  nNeighbourCorner++)
	{
	  // don't check if a corner is closest to itself, thats just silly
	  if(nTestCorner!=nNeighbourCorner)
	    {
	      fXDist = (float)m_pCornersPixels[nNeighbourCorner].x - 
		(float)m_pCornersPixels[nTestCorner].x;
	      fYDist = (float)m_pCornersPixels[nNeighbourCorner].y - 
		(float)m_pCornersPixels[nTestCorner].y;
	      fDist = (float)sqrt(fXDist*fXDist + fYDist*fYDist);
                                
	      /* see if the distance is less than the current nearest 
		 CORNER_FINDER_NEIGHBOUR_LIMIT neighbours, if it is insert 
		 it into a distance ordered location, closest first */
	      pIndex = m_ppCornersNeighboursLists[nTestCorner];
	      pDistance = m_ppCornersNeighboursDistances[nTestCorner];

	      // reset the insert position
	      nInsertPos = CORNER_FINDER_NEIGHBOUR_LIMIT;

	      /* start with the last distance in the list and work 
		 back towards the first, comparing each to the 
		 calculated distance fDist */
	      for(int i=(CORNER_FINDER_NEIGHBOUR_LIMIT-1);i>=0;i--)
		{
		  if(fDist<pDistance[i])
		    {
		      nInsertPos = i;
		    }
		}

	      /*if nInsertPos is in the range 0 to 
		CORNER_FINDER_NEIGHBOUR_LIMIT-1 inclusive then the distance 
		fDist can be inserted at that position in the distance 
		list for nTestCorner */
	      if(nInsertPos>=0 && nInsertPos<CORNER_FINDER_NEIGHBOUR_LIMIT)
		{
		  /* move all corner indexes and distance from 
		     nInsertPos up one place, the furthest corner 
		     drops off the end of the list */
		  for(int i=(CORNER_FINDER_NEIGHBOUR_LIMIT-1);i>nInsertPos;i--)
		    {
		      pIndex[i] = pIndex[i-1];
		      pDistance[i] = pDistance[i-1];
		    }
                                        
		  // insert the neighbouring corner and its distance
		  pIndex[nInsertPos] = nNeighbourCorner;
		  pDistance[nInsertPos] = fDist;
		}
	    }
	}
    }

  int nNeighbourCount = 0;
  float fAngleTestToClosestNeighbour = 0.0f;
  float fAngleTestToOtherNeighbour = 0.0f;
  float fAngleNeighbourToClosestNeighbour = 0.0f;
  int nAngleValidCount = 0;
  bool bErrorsFound = false;  // flag to indicate that something has gone wrong

  for(nTestCorner = 0;nTestCorner<m_nTotalCorners;nTestCorner++)
    {
      // pointer to list of neighbouring corner indexes to nTestCorner
      pIndex = m_ppCornersNeighboursLists[nTestCorner];

      // get angle of test corner to nearest neighbour
      fXDist = (float)( m_pCornersPixels[ pIndex[0]].x - 
			m_pCornersPixels[nTestCorner].x );
      fYDist = (float)( m_pCornersPixels[ pIndex[0]].y - 
			m_pCornersPixels[nTestCorner].y );
      fAngleTestToClosestNeighbour = (float)atan2(fYDist,fXDist);

      /* go through other angles taking the difference of the 
	 angle-of-test-to-neighbour_n and angle-of-test-to-neighbour_0 */
      nNeighbourCount = 0;
      nAngleValidCount = 0;

      for(int i=0;i<CORNER_FINDER_NEIGHBOUR_LIMIT;i++)
	{
	  // get the angle-of-test-to-neighbour_n
	  fXDist = (float)( m_pCornersPixels[ pIndex[i]].x - 
			    m_pCornersPixels[nTestCorner].x );
	  fYDist = (float)( m_pCornersPixels[ pIndex[i]].y - 
			    m_pCornersPixels[nTestCorner].y );
	  fAngleTestToOtherNeighbour = (float)atan2(fYDist,fXDist);

	  m_ppAnglesCornerToNeighbour[nTestCorner][i] = 
	    (float)RAD_TO_DEG(fAngleTestToOtherNeighbour);

	  /* subtract angle-of-test-to-neighbour_0, take the absolute 
	     value of the result, then convert to degrees */
	  fAngleNeighbourToClosestNeighbour = 
	    (float)RAD_TO_DEG( (float)fabs( fAngleTestToOtherNeighbour - 
					    fAngleTestToClosestNeighbour ) );

	  m_ppAnglesNeighbourToNeighbourZero[nTestCorner][i] = 
	    fAngleNeighbourToClosestNeighbour;
	}

      /* look at the first 3 neighbouring corners and see the range 
	 of their angles is less than 120 degrees */

      // get the 3 neighbours and convert them into 0-360 range
      float f3Angles[3];
      f3Angles[0] = m_ppAnglesCornerToNeighbour[nTestCorner][0];
      f3Angles[1] = m_ppAnglesCornerToNeighbour[nTestCorner][1];
      f3Angles[2] = m_ppAnglesCornerToNeighbour[nTestCorner][2];

      // determine extremes of angle
      float fTestAngle = 0.0f;
      float fAngleMin = 500.0f;
      float fAngleMax = -500.0f;

      // keep counts of positive and negative angles
      int nPositiveAngles = 0;
      int nNegativeAngles = 0;

      // track the indexes of the min and max angles in the 3 element array
      int nMaxIndex = -1;
      int nMinIndex = -1;

      ////      bool bAnglesRecalculated = false;
      for(int i=0;i<3;i++)
	{
	  fTestAngle = f3Angles[i];
	  if(fTestAngle<0.0)
	    nNegativeAngles++;
	  else
	    nPositiveAngles++;
	}

      float fRange = 
	IndexExtremeAnglesFrom3AndGetRange(f3Angles,nMinIndex,nMaxIndex);
      fAngleMin = m_ppAnglesCornerToNeighbour[nTestCorner][nMinIndex];
      fAngleMax = m_ppAnglesCornerToNeighbour[nTestCorner][nMaxIndex];

      /* do all 4 nearest neighbour angles lie 
	 inside the range of the first three? */
      bool bAll8InsideRangeOf3 = true;      // start by assuming that they do

      // get those angles
      float fAngleExtremeMin = fAngleMin;
      float fAngleExtremeMax = fAngleMax;
      float tmp;
      if(fAngleExtremeMin>fAngleExtremeMax) {
	//Swap(&fAngleExtremeMin,&fAngleExtremeMax);
	tmp=fAngleExtremeMin;
	fAngleExtremeMin = fAngleExtremeMax;
	fAngleExtremeMax = tmp;
      }

      float f4Neighbours[4] = {0.0f};
      for(int i=0;i<4;i++)
	f4Neighbours[i] = m_ppAnglesCornerToNeighbour[nTestCorner][i];

      for(int i=0;i<4;i++)
	{
	  // don't test the extremes of the first 3 neighbours
	  if(i!=nMinIndex && i!=nMaxIndex)
	    {
	      fTestAngle = m_ppAnglesCornerToNeighbour[nTestCorner][i];

	      // apply a tolerance of 10 degrees to the tests
	      if(fAngleExtremeMax>90.0f && fAngleExtremeMin< -90.0f)
		{
		  // swap test against min and max
		  if(fTestAngle>(fAngleExtremeMin+10.0f) && 
		     fTestAngle<(fAngleExtremeMax-10.0f) )
		    {
		      // found an angle to a neighbour corner that is 
		      // outside the angular range of the nearest three neighbours
		      bAll8InsideRangeOf3 = false;
		    }
		}
	      else
		{
		  /* check positive range of extreme angles to see if 
		     it is more than 180 degrees, if it is this changes 
		     the test slightly */
		  if(fAngleExtremeMin< -90.0f && 
		     fAngleExtremeMax>(fAngleExtremeMin+180.0f))
		    {
		      // test for greater than minimum and less  
		      // than maximum, applying tolerance
		      if( fTestAngle>(fAngleExtremeMin+10.0f) && 
			  fTestAngle<(fAngleExtremeMax-10.0f) )
			{
			  /* found an angle to a neighbour corner that is 
			     outside the angular range of the nearest three neighbours */
			  bAll8InsideRangeOf3 = false;
			}
		    }
		  else
		    {
		      if(fTestAngle>(fAngleExtremeMax+10.0f) || 
			 fTestAngle<(fAngleExtremeMin-10.0f) )
			{
			  /* found an angle to a neighbour corner that is 
			     outside the angular range of the 
			     nearest three neighbours */
			  bAll8InsideRangeOf3 = false;
			}
		    }
		}
	    }
	}
                
      // but only if all 8 neighbour angles are inside the range of the first 3
      if(!bAll8InsideRangeOf3)
	{
	  fRange = 500.0f;
	  /*
	  // find the index of the non-extreme neighbour out of the 3 nearest
	  int nNonExtremeNeighbour = -1;
	  for(int i=0;i<3;i++)
	  {
	  if(i!=nMinIndex && i!=nMaxIndex)
	  nNonExtremeNeighbour = i;
	  }
	  float fAngleNonExtreme = f3Angles[nNonExtremeNeighbour];

	  // test to ensure that the minimum and maximum are correct
	  // for example min = -175 and max = 141, but the extremes are not correct 
	  // when the non-extreme angle = 61
	  // correct range in this example is min = -175, max = 61, range = -175 - (61-360)
	  if(fAngleMin< -90.0f && fAngleMax>(fAngleMin + 180.0f) && 
	  fAngleNonExtreme>fAngleMin && fAngleNonExtreme<fAngleMax)
	  {
	  fAngleMax = fAngleNonExtreme;
	  nMaxIndex = nNonExtremeNeighbour;
	  }

	  // first check is is all angles are positive
	  if(nPositiveAngles==3)
	  {
	  // all angles positive, range is straighforward subtraction
	  fRange = fAngleMax - fAngleMin;
	  }
	  else
	  {
	  // check to see if all angles are negative
	  if(nNegativeAngles==3)
	  {
	  // all angles negative, range calculation is straighforward again
	  fRange = fAngleMax - fAngleMin;
	  }
	  else
	  {
	  // mix of positive and negative angles
	  if(nPositiveAngles==2)
	  {
	  // two +ve and 1 -ve

	  // check for outside +- 90 degrees in min and max
	  if(fAngleMax<=90.0f && fAngleMin>=(-90.0f))
	  {
	  // inside this range of angles our range 
	  // calculation is pretty simple again
	  fRange = fAngleMax - fAngleMin;
	  }
	  else if(fAngleMax>90.0f && fAngleMin>=(-90.0f))
	  {
	  if( (fAngleMax - 180.0f)<fAngleMin )
	  {
	  fRange = fAngleMax - fAngleMin;
	  }
	  }
	  else if(fAngleMin< -90.0f && fAngleMax>(fAngleMin+180.0f))
	  {
	  fRange = fAngleMin - (fAngleMax - 360.0f);
	  }
	  }
	  else
	  {
	  // 2 -ve and 1 +ve angles

	  // check quadrant of min and max
	  if(fAngleMax>90.0f && fAngleMin<(-90.0f))
	  {
	  // here is where the range calculations start to get more complex
                                                        
	  // using the index of the maximum angle element, make the maximum the 
	  // minimum by subtracting 360 degrees from it
	  f3Angles[nMaxIndex] -= 360.0f;

	  // now recalculate the minimum and maximum angles
	  fAngleMin = 500.0f;
	  fAngleMax = -500.0f;
	  for(int i=0;i<3;i++)
	  {
	  fTestAngle = f3Angles[i];

	  if(fTestAngle<fAngleMin)
	  {
	  fAngleMin = fTestAngle;
	  }
	  if(fTestAngle>fAngleMax)
	  {
	  fAngleMax = fTestAngle;
	  }
	  }

	  // range calculation is again a simple subtraction of min from max
	  fRange = fAngleMax - fAngleMin;
	  }
	  }
	  }
	  }
	  */
	}

      // check the range
      if(fRange<=(90.0f + m_fSequenceAngleRange) )
	{
	  // range is less than 90 degrees + tolerance, 
	  // corner is an outer corner
	  m_pCategorisedUnsortedCorners[nTestCorner] = 
	    CORNER_FINDER_CORNER_OUTER;
	}
      else
	{
	  // test for edge and inner corners
	  // take differences of angles to the lowest 
	  // neighbour angle and check sectors
	  bool bSector0_90 = true;           /* the sector 0-90 degrees 
						is always occupied because
						of the minimum angle neighbour*/
	  bool bSector90_180 = false;        // other sectors start as empty
	  bool bSector180_270 = false;
	  bool bSector270_360 = false;

	  // need to find the smallest angle of the 8 neighbours
	  fAngleMin = FLT_MAX;
	  int nMinAngleIndex = 0;         /* for saving the neighbour index 
					     of the lowest angle neighbour */
	  for(int i=0;i<8;i++)
	    {
	      // get angle
	      fTestAngle = m_ppAnglesCornerToNeighbour[nTestCorner][i];

	      // compare to current minimum
	      if(fTestAngle<fAngleMin)
		{
		  /* angle is less than current minimum 
		     so set it as the new minimum */
		  fAngleMin = fTestAngle;

		  // store the neighbour index of the new angle
		  nMinAngleIndex = i;
		}
	    }
                                
	  float fDifference = 0.0f;

	  // go through the 8 nearest neighbours
	  for(int i=0;i<8;i++)
	    {
	      // but there is no need to check the neighbour 
	      // with the lowest angle
	      if(i!=nMinAngleIndex)
		{
		  // get the angle to the neighbour
		  fTestAngle = m_ppAnglesCornerToNeighbour[nTestCorner][i];

		  // find the difference
		  fDifference = fTestAngle - fAngleMin;

		  // determine sector code from the difference
		  // ASSERT(fDifference>=0.0f);    /* difference should 
                  //				   never be negative */
                                        
		  // check the first sector
		  if(fDifference<=90.0f)
		    {
		      // angle in the first sector
		      bSector0_90 = true;
		    }
		  else
		    {
		      if(fDifference>90.0f && fDifference<=180.0f)
			{
			  // angle in the second sector
			  bSector90_180 = true;
			}
		      else
			{
			  if(fDifference>180.0f && fDifference<=270.0f)
			    {
			      // angle in the third sector
			      bSector180_270 = true;
			    }
			  else
			    {
			      if(fDifference>270.0f)
				{
				  // angle in the fourth sector
				  bSector270_360 = true;
				}
			    }
			}
		    }
		}
	    }

	  // check sector codes for type value

	  if( bSector0_90 && !bSector90_180 && 
	      !bSector180_270 && bSector270_360)
	    {
	      /* these sector codes identify an outer corner 
		 that was missed by the range testing */
	      m_pCategorisedUnsortedCorners[nTestCorner] = 
		CORNER_FINDER_CORNER_OUTER;
	    }
	  else
	    {
	      /* edge corners will have neighbours occupying 
		 the first 2 sectors and sometimes the third, 
		 or three out the four sectors */
	      if( (bSector0_90 && bSector90_180 && 
		   !bSector180_270 && !bSector270_360) ||
		  (bSector0_90 && bSector90_180 && 
		   bSector180_270 && !bSector270_360) ||
		  (bSector0_90 && !bSector90_180 && 
		   bSector180_270 && bSector270_360) ||
		  (bSector0_90 && bSector90_180 && 
		   !bSector180_270 && bSector270_360) )
		{
		  m_pCategorisedUnsortedCorners[nTestCorner] = 
		    CORNER_FINDER_CORNER_EDGE;
		}
	      else
		{
		  // inner corners will have neighbours occupying all 4 sectors
		  if(bSector0_90 && bSector90_180 && 
		     bSector180_270 && bSector270_360)
		    {
		      m_pCategorisedUnsortedCorners[nTestCorner] = 
			CORNER_FINDER_CORNER_INNER;
		    }
		  else
		    {
		      // if we get here there is a problem
		      bErrorsFound = true;
		    }
		}
	    }
	}
    }

  // has something gone wrong ?
  if(bErrorsFound)
    {
      // yes...
      return false;
    }

  // having determined corner types, we can sequence them

  // allocate a temporary array to sort the corners into
  CvPoint2D32f* pSortedCorners = new CvPoint2D32f[m_nTotalCorners];

  // don't forget to sort the corner types when we sort the corner sequence
  int* pSortedTypes = new int[m_nTotalCorners];

  if(pSortedCorners==0 || pSortedTypes==0)
    {
      delete[]pSortedTypes;
      delete[]pSortedCorners;
//       Release();
//       ASSERT(FALSE);
      return false;
    }

  /* fill the sorted corner array with -1,-1 as initial values, the corner 
     viewer will ignore these values, and only show the real values */
  for(int i=0;i<m_nTotalCorners;i++)
    {
      pSortedCorners[i].x = -1;
      pSortedCorners[i].y = -1;
      pSortedTypes[i] = CORNER_FINDER_CORNER_UNCATEGORISED;
    }

  // start sequencing with the outer corners
////   int nTopLeft = -1;
////   int nTopRight = -1;
////   int nBottomLeft = -1;
////   int nBottomRight = -1;

  int nOuterCorners[4] = {-1};
  int nOutersFound = 0;
        
  // get the indexes of the outer corners
  for(int i=0;i<m_nTotalCorners;i++)
    {
      if(m_pCategorisedUnsortedCorners[i]==CORNER_FINDER_CORNER_OUTER)
	{
	  if(nOutersFound<4)
	    {
	      nOuterCorners[nOutersFound] = i;
	    }
	  nOutersFound++;
	}
    }

  // check that the correct number of outer corners is found
  if(nOutersFound<4)
    {
      // not found enough outer corners
      delete[]pSortedTypes;
      delete[]pSortedCorners;

      printf("Sequencing Error\n\nZeus has only found %d corners"
	     " as outer corners.\n\nThis is obviously incorrect. "
	     "Please manually sequence the corners for this frame."
	     ,nOutersFound);

      return false;
    }

  if(nOutersFound>4)
    {
      // there should only be 4 outers
      delete[]pSortedTypes;
      delete[]pSortedCorners;

      printf("Sequencing Error\n\nZeus has identified %d corners "
	     "as outer corners.\n\nThis is obviously incorrect. "
	     "Please manually sequence the corners for this frame."
	     ,nOutersFound);
      return false;
    }

  /* build database of differences between nearest 
     2 corners of each outer corner in terms of x and y */
  tagDISTANCEDIFFERENCE DistDiff[4];

  // distance differences
  int nXDiff1 = 0,
    nXDiff2 = 0,
    nYDiff1 = 0,
    nYDiff2 = 0;

  for(int i=0;i<4;i++)
    {
      nXDiff1 = static_cast<int> (
	m_pCornersPixels[ m_ppCornersNeighboursLists[ nOuterCorners[i] ][0] ].x -  
	m_pCornersPixels[ nOuterCorners[i] ].x ) ; 
      nYDiff1 = static_cast<int> (
	m_pCornersPixels[ m_ppCornersNeighboursLists[ nOuterCorners[i] ][0] ].y - 
	m_pCornersPixels[ nOuterCorners[i] ].y ) ;
      nXDiff2 = static_cast<int> (
	m_pCornersPixels[ m_ppCornersNeighboursLists[ nOuterCorners[i] ][1] ].x - 
	m_pCornersPixels[ nOuterCorners[i] ].x ) ;
      nYDiff2 = static_cast<int> (
	m_pCornersPixels[ m_ppCornersNeighboursLists[ nOuterCorners[i] ][1] ].y - 
	m_pCornersPixels[ nOuterCorners[i] ].y ) ;

      if( abs(nXDiff1) > abs(nYDiff1) )
	{
	  DistDiff[i].bXExtremeNeighbour1 = true;
	  DistDiff[i].nSignExtremeNeighbour1 = 
	    (nXDiff1<0 ? CORNER_FINDER_EXTREME_NEGATIVE : 
	     CORNER_FINDER_EXTREME_POSITIVE);
	}
      else
	{
	  DistDiff[i].bXExtremeNeighbour1 = false;
	  DistDiff[i].nSignExtremeNeighbour1 = 
	    (nYDiff1<0 ? CORNER_FINDER_EXTREME_NEGATIVE : 
	     CORNER_FINDER_EXTREME_POSITIVE);
	}

      if( abs(nXDiff2) > abs(nYDiff2) )
	{
	  DistDiff[i].bXExtremeNeighbour2 = true;
	  DistDiff[i].nSignExtremeNeighbour2 = 
	    (nXDiff2<0 ? CORNER_FINDER_EXTREME_NEGATIVE : 
	     CORNER_FINDER_EXTREME_POSITIVE);
	}
      else
	{
	  DistDiff[i].bXExtremeNeighbour2 = false;
	  DistDiff[i].nSignExtremeNeighbour2 = 
	    (nYDiff2<0 ? CORNER_FINDER_EXTREME_NEGATIVE : 
	     CORNER_FINDER_EXTREME_POSITIVE);
	}
    }

  /* analyse database to determine which outer corner goes where
     need to know how original corners map to sorted corners, 
     so build a mapping array */
  int * pCornerMapping = new int[m_nTotalCorners];
  if(pCornerMapping==0)
    {
      delete[]pSortedTypes;
      delete[]pSortedCorners;
//       Release();
//       ASSERT(FALSE);
      return false;
    }
  for(int i=0;i<m_nTotalCorners;i++)
    pCornerMapping[i] = -1;            // initialise as -1 to mean unmapped

  bool bCornerLocatedTL = false;       /* outer corners should always fall 
					  into one of the 4 categories below, 
					  if not then the algorithm has failed 
					  and I need to know about it!!!! */
  bool bCornerLocatedTR = false;
  bool bCornerLocatedBL = false;
  bool bCornerLocatedBR = false;

  for(int i=0;i<4;i++)
    {
      if(DistDiff[i].nSignExtremeNeighbour1==CORNER_FINDER_EXTREME_POSITIVE && 
	 DistDiff[i].nSignExtremeNeighbour2==CORNER_FINDER_EXTREME_POSITIVE &&
	 ( (!DistDiff[i].bXExtremeNeighbour1 && 
	    DistDiff[i].bXExtremeNeighbour2) || 
	   (DistDiff[i].bXExtremeNeighbour1 && 
	    !DistDiff[i].bXExtremeNeighbour2)) )
	{
	  // found the top-left corner
	  pSortedCorners[0].x = m_pCornersPixels[ nOuterCorners[i] ].x;
	  pSortedCorners[0].y = m_pCornersPixels[ nOuterCorners[i] ].y;
	  pSortedTypes[0] = CORNER_FINDER_CORNER_TOP_LEFT;
	  pCornerMapping[0] = nOuterCorners[i];
	  bCornerLocatedTL = true;
	}
      else if(DistDiff[i].nSignExtremeNeighbour1==
	      CORNER_FINDER_EXTREME_NEGATIVE && 
	      DistDiff[i].nSignExtremeNeighbour2==
	      CORNER_FINDER_EXTREME_NEGATIVE)
	{
	  // found the bottom-right corner
	  pSortedCorners[m_nTotalCorners-1].x = 
	    m_pCornersPixels[ nOuterCorners[i] ].x;
	  pSortedCorners[m_nTotalCorners-1].y = 
	    m_pCornersPixels[ nOuterCorners[i] ].y;
	  pSortedTypes[m_nTotalCorners-1] = 
	    CORNER_FINDER_CORNER_BOTTOM_RIGHT;
	  pCornerMapping[m_nTotalCorners-1] = nOuterCorners[i];
	  bCornerLocatedBR = true;
	}
      else if( (DistDiff[i].bXExtremeNeighbour1 && 
		DistDiff[i].nSignExtremeNeighbour1==
		CORNER_FINDER_EXTREME_NEGATIVE && 
		!DistDiff[i].bXExtremeNeighbour2 && 
		DistDiff[i].nSignExtremeNeighbour2==
		CORNER_FINDER_EXTREME_POSITIVE) ||
	       (!DistDiff[i].bXExtremeNeighbour1 && 
		DistDiff[i].nSignExtremeNeighbour1==
		CORNER_FINDER_EXTREME_POSITIVE && 
		DistDiff[i].bXExtremeNeighbour2 && 
		DistDiff[i].nSignExtremeNeighbour2==
		CORNER_FINDER_EXTREME_NEGATIVE) ||
	       (!DistDiff[i].bXExtremeNeighbour1 && 
		DistDiff[i].nSignExtremeNeighbour1==
		CORNER_FINDER_EXTREME_POSITIVE && 
		!DistDiff[i].bXExtremeNeighbour2 && 
		DistDiff[i].nSignExtremeNeighbour2==
		CORNER_FINDER_EXTREME_POSITIVE) )
	{
	  // found top-right corner
	  pSortedCorners[m_nXCorners-1].x = 
	    m_pCornersPixels[ nOuterCorners[i] ].x;
	  pSortedCorners[m_nXCorners-1].y = 
	    m_pCornersPixels[ nOuterCorners[i] ].y;
	  pSortedTypes[m_nXCorners-1] = 
	    CORNER_FINDER_CORNER_TOP_RIGHT;
	  pCornerMapping[m_nXCorners-1] = 
	    nOuterCorners[i];
	  bCornerLocatedTR = true;
	}
      else if( (DistDiff[i].bXExtremeNeighbour1 && 
		DistDiff[i].nSignExtremeNeighbour1==
		CORNER_FINDER_EXTREME_POSITIVE && 
		!DistDiff[i].bXExtremeNeighbour2 && 
		DistDiff[i].nSignExtremeNeighbour2==
		CORNER_FINDER_EXTREME_NEGATIVE) ||
	       (!DistDiff[i].bXExtremeNeighbour1 && 
		DistDiff[i].nSignExtremeNeighbour1==
		CORNER_FINDER_EXTREME_NEGATIVE && 
		DistDiff[i].bXExtremeNeighbour2 && 
		DistDiff[i].nSignExtremeNeighbour2==
		CORNER_FINDER_EXTREME_POSITIVE) )
	{
	  // found bottom-left corner
	  pSortedCorners[(m_nYCorners-1)*m_nXCorners].x = 
	    m_pCornersPixels[ nOuterCorners[i] ].x;
	  pSortedCorners[(m_nYCorners-1)*m_nXCorners].y = 
	    m_pCornersPixels[ nOuterCorners[i] ].y;
	  pSortedTypes[(m_nYCorners-1)*m_nXCorners] = 
	    CORNER_FINDER_CORNER_BOTTOM_LEFT;
	  pCornerMapping[(m_nYCorners-1)*m_nXCorners] = 
	    nOuterCorners[i];
	  bCornerLocatedBL = true;
	}
    }

  if(!bCornerLocatedTL || !bCornerLocatedTR || 
     !bCornerLocatedBL || !bCornerLocatedBR)
    {                       
      //      ASSERT(FALSE);

      delete[]pSortedTypes;
      delete[]pSortedCorners;
      // Release();
                
      return false;
    }

  // now locate the other corners based on their proximity to the outer corners
  // need to know quickly which corners are correctly placed and which are not
  bool * pCornersPlaced = new bool[m_nTotalCorners];

  if(pCornersPlaced==0)
    {
      delete[]pCornerMapping;
      delete[]pSortedTypes;
      delete[]pSortedCorners;
                
//       ASSERT(FALSE);
//       Release();
      return false;
    }

  for(int i=0;i<m_nTotalCorners;i++)
    pCornersPlaced[i] = false;

  // mark the 4 outer corners as being placed
  pCornersPlaced[0] = true;                                    // top-left
  pCornersPlaced[m_nTotalCorners-1] = true;                    // bottom-right
  pCornersPlaced[m_nXCorners-1] = true;                        // top-right
  pCornersPlaced[(m_nYCorners-1)*m_nXCorners] = true;          // bottom-left

  // determine angles from each outer to each other outer
  tagOUTERANGLES OuterAngles;
  float fXDiff = 0.0f;
  float fYDiff = 0.0f;

  // top-left to top-right
  fXDiff = (float)( pSortedCorners[m_nXCorners-1].x - pSortedCorners[0].x );
  fYDiff = (float)( pSortedCorners[m_nXCorners-1].y - pSortedCorners[0].y );
  OuterAngles.fAngleTL_TR = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // top-left to bottom-left
  fXDiff = (float)( pSortedCorners[(m_nYCorners-1)*m_nXCorners].x - 
		    pSortedCorners[0].x );
  fYDiff = (float)( pSortedCorners[(m_nYCorners-1)*m_nXCorners].y - 
		    pSortedCorners[0].y );
  OuterAngles.fAngleTL_BL = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // top-left to bottom-right
  fXDiff = (float)( pSortedCorners[m_nTotalCorners-1].x - 
		    pSortedCorners[0].x );
  fYDiff = (float)( pSortedCorners[m_nTotalCorners-1].y - 
		    pSortedCorners[0].y );
  OuterAngles.fAngleTL_BR = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // top-right to top-left
  fXDiff = (float)( pSortedCorners[0].x - pSortedCorners[m_nXCorners-1].x );
  fYDiff = (float)( pSortedCorners[0].y - pSortedCorners[m_nXCorners-1].y );
  OuterAngles.fAngleTR_TL = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // top-right to bottom-left
  fXDiff = (float)( pSortedCorners[(m_nYCorners-1)*m_nXCorners].x - 
		    pSortedCorners[m_nXCorners-1].x );
  fYDiff = (float)( pSortedCorners[(m_nYCorners-1)*m_nXCorners].y - 
		    pSortedCorners[m_nXCorners-1].y );
  OuterAngles.fAngleTR_BL = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // top-right to bottom-right
  fXDiff = (float)( pSortedCorners[m_nTotalCorners-1].x - 
		    pSortedCorners[m_nXCorners-1].x );
  fYDiff = (float)( pSortedCorners[m_nTotalCorners-1].y - 
		    pSortedCorners[m_nXCorners-1].y );
  OuterAngles.fAngleTR_BR = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // bottom-left to top-left
  fXDiff = (float)( pSortedCorners[0].x - 
		    pSortedCorners[(m_nYCorners-1)*m_nXCorners].x );
  fYDiff = (float)( pSortedCorners[0].y - 
		    pSortedCorners[(m_nYCorners-1)*m_nXCorners].y );
  OuterAngles.fAngleBL_TL = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // bottom-left to top-right
  fXDiff = (float)( pSortedCorners[m_nXCorners-1].x - 
		    pSortedCorners[(m_nYCorners-1)*m_nXCorners].x );
  fYDiff = (float)( pSortedCorners[m_nXCorners-1].y - 
		    pSortedCorners[(m_nYCorners-1)*m_nXCorners].y );
  OuterAngles.fAngleBL_TR = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // bottom-left to bottom-right
  fXDiff = (float)( pSortedCorners[m_nTotalCorners-1].x - 
		    pSortedCorners[(m_nYCorners-1)*m_nXCorners].x );
  fYDiff = (float)( pSortedCorners[m_nTotalCorners-1].y - 
		    pSortedCorners[(m_nYCorners-1)*m_nXCorners].y );
  OuterAngles.fAngleBL_BR = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // bottom-right to top-left
  fXDiff = (float)( pSortedCorners[0].x - pSortedCorners[m_nTotalCorners-1].x );
  fYDiff = (float)( pSortedCorners[0].y - pSortedCorners[m_nTotalCorners-1].y );
  OuterAngles.fAngleBR_TL = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // bottom-right to top-right
  fXDiff = (float)( pSortedCorners[m_nXCorners-1].x - 
		    pSortedCorners[m_nTotalCorners-1].x );
  fYDiff = (float)( pSortedCorners[m_nXCorners-1].y - 
		    pSortedCorners[m_nTotalCorners-1].y );
  OuterAngles.fAngleBR_TR = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // bottom-right to bottom-left
  fXDiff = (float)( pSortedCorners[(m_nYCorners-1)*m_nXCorners].x - 
		    pSortedCorners[m_nTotalCorners-1].x );
  fYDiff = (float)( pSortedCorners[(m_nYCorners-1)*m_nXCorners].y - 
		    pSortedCorners[m_nTotalCorners-1].y );
  OuterAngles.fAngleBR_TR = (float)RAD_TO_DEG( atan2(fYDiff,fXDiff) );

  // keep a count of the number of corners sequenced, it starts at 4 
  // because we have placed the outer corners already
  int nCornersSorted = 4;
  int * pNeighbours = 0;
  ////  bool * pAnglesValidList = 0;
  ////  float * pNeighboursAngles = 0;
  ////  float fAnglePlacedNeighbourToCorner = 0.0f;
  int nNeighbour = 0;
  bool bAllowTest = false;
  bool bPlaced = false;
  int nNewListPosition = 0;
  int nIterations = 0;
  ////  int * pPlacedCornerNeighbours = 0;
  int nIndexMin = -1;
  int nIndexMax = -1;
  bool bMapped = false;
  float fFirst4NeighbourAngles[4] = {0.0f};
  ////  bool bFoundFirstInternal = false;       /* flag for deciding which 
  //// 					     internal variable to 
  //// 					     assign the index to */
  //// int nInternal1 = -1;    // holds 1 internal index
  //// int nInternal2 = -1;    // holds the other internal index
  //// float fDifference1 = 0.0f;
  //// float fDifference2 = 0.0f;
  //// float fDiffOfDiffsInternal1 = 0.0f;
  //// float fDiffOfDiffsInternal2 = 0.0f;
  //// float fAngleExtremeMin = 0.0f;
  //// float fAngleExtremeMax = 0.0f;
  //// float fTestAngle = 0.0f;
  //// float fAngleNeighbour1 = 0.0f;
  //// float fAngleOtherNeighbour = 0.0f;
  //// float fDifference = 0.0f;
  //// int nValidAngleIndex = 0;
 int nLoop = 0;          // loop counter

//   CFileCSV fileData;
//   bool bDataFile = fileData.CreateNewFile(CString("c:\\sortdata.csv"));

  do
    {
      /* go through all the corners, seeing which ones are sequence, 
	 from there we can sequence their neighbours */
      for(int i=0;i<m_nTotalCorners;i++)
	{
	  if(pCornersPlaced[i])
	    {
	      /* get the original list of neighbours for the corner, 
		 from its mapped location */
	      int nOriginalCorner = pCornerMapping[i];
	      if(nOriginalCorner>=0)
		{
		  pNeighbours = m_ppCornersNeighboursLists[ nOriginalCorner ];

		  for(nNeighbour=0;nNeighbour<4;nNeighbour++)
		    {
		      /* look at each neighbour, see if it is 
			 placed in the mapping array */
		      bMapped = false;
		      for(int nMap=0;nMap<m_nTotalCorners;nMap++)
			{
			  if(pCornerMapping[nMap]==pNeighbours[nNeighbour])
			    bMapped = true;
			}
                                                
		      /* if a neighbour is not already mapped we can use the 
			 angles from the corner to the neighbour to determine
			 where the neighbour should be */

		      if(!bMapped)
			{
			  /* can only calculate position for valid-angle 
			     neighbours that are immediate to the placed 
			     corner */
			  bAllowTest = false;

			  /* test first 4 neighbours for relative orthogonal 
			     angle to angle for neighbour 1 */
			  // neighbour 1 is always valid so can be tested
			  if(nNeighbour==0)
			    bAllowTest = true;
			  else
			    {
			      if( m_pCategorisedUnsortedCorners[ nOriginalCorner ] 
				  == CORNER_FINDER_CORNER_OUTER)
				{
				  /* the angle to the corner that is in between 
				     the extremes of the three angles to the three 
				     nearest neighbours is the invalid one, 
				     the corners of the angles at the extremes are 
				     the corners to be tested for position   
				     identify which neighbours for the test corner 
				     are the at the anglular extremes 
				     to the test corner */
				  nIndexMin = -1;
				  nIndexMax = -1;
				  IndexExtremeAnglesFrom3AndGetRange
				    (m_ppAnglesCornerToNeighbour
				     [ nOriginalCorner ],
				     nIndexMin,nIndexMax);

				  /* by rights only the angular extremes will 
				     be valid corners horizontally or vertically 
				     neighbouring the test corner 
				     the angle between the 2 extremes 
				     will be the angle to the diagonally 
				     neighbouring corner which is not the one we 
				     are looking for here */
				  if(nNeighbour==nIndexMin || 
				     nNeighbour==nIndexMax)
				    bAllowTest = true;
				  else
				    bAllowTest = false;
				}
			      else
				{

				  /* make a copy of the list of angles and pass 
				     that to the indexing function, since in 
				     some cases depending on the angles the 
				     list can be changed */
				  for(nLoop=0;nLoop<4;nLoop++)
				    fFirst4NeighbourAngles[nLoop] = 
				      m_ppAnglesCornerToNeighbour
				      [ nOriginalCorner ][ nLoop ];
 
				  // call the diagonal tester
				  bool bDiagonalNeighbours[4];
				  FindDiagonalNeighboursFrom4
				    (fFirst4NeighbourAngles,
				     &OuterAngles,bDiagonalNeighbours);

				  /* see if the current neighbour being 
				     looked at is the one that is most likely 
				     the diagonal one */
				  bAllowTest = !bDiagonalNeighbours[nNeighbour];
				}
			    }

			  if(bAllowTest)
			    {
			      bPlaced = false; 

			      /* get the x and y distance differences between 
				 placed corner and valid neighbours */
			      nXDiff1 = static_cast<int> 
				( m_pCornersPixels
				  [ pNeighbours[nNeighbour]].x - 
				  pSortedCorners[i].x ) ;
			      nYDiff1 = static_cast<int> 
				( m_pCornersPixels
				[ pNeighbours[nNeighbour]].y - 
				pSortedCorners[i].y ) ;

			      /* use the more significant difference i.e. 
				 abs(nXDiff1) > abs(nYDiff1) ? to determine 
				 horizontal or vertical (relative) neighbour */
			      /* use the sign of the more significant distance 
				 to determine left or right/up or down */
			      bool bXSignificant = false;
			      if(abs(nXDiff1)>abs(nYDiff1))
				bXSignificant = true;


			      if(bXSignificant && nXDiff1<0)
				{
				  // X is negative significant - 
				  // neighbour is to the left
				  bPlaced = true;
				  nNewListPosition = i - 1;
				}
			      else
				{
				  if(bXSignificant && nXDiff1>0)
				    {
				      // X is positve significant - 
				      // neighbour is to the right
				      bPlaced = true;
				      nNewListPosition = i + 1;
				    }
				  else
				    {
				      if(!bXSignificant && nYDiff1<0)
					{
					  // Y is negative significant - 
					  // neighbour is above
					  bPlaced = true;
					  nNewListPosition = i - m_nXCorners;
					}
				      else
					{
					  if(!bXSignificant && nYDiff1>0)
					    {
					      // Y is positive significant, 
					      // neighbour is below
					      bPlaced = true;
					      nNewListPosition = 
						i + m_nXCorners;
					    }
					  else
					    {
					      // ASSERT(FALSE);
					    }
					}
				    }
				}

			      if(bPlaced)
				{
				  if(nNewListPosition>=0 && 
				     nNewListPosition<m_nTotalCorners)
				    {
				      if(!pCornersPlaced[nNewListPosition])
					{
					  pCornersPlaced
					    [nNewListPosition] = true;
					  pSortedCorners[nNewListPosition] = 
					    m_pCornersPixels[pNeighbours
							     [nNeighbour]];
					  pSortedTypes[nNewListPosition] = 
					    m_pCategorisedUnsortedCorners
					    [pNeighbours[nNeighbour]];
					  nCornersSorted++;
					  pCornerMapping[nNewListPosition] = 
					    pNeighbours[nNeighbour];

// 					  if(bDataFile)
// 					    {
// 					      CString cstrText;
// 					      cstrText.Format("Moving Corner"
// 							      " %d to List"
// 							      " Position %d"
// 							      ,pNeighbours
// 							      [nNeighbour],
// 							      nNewListPosition);
// 					      fileData.WriteString
// 						(cstrText,false,true);
// 					    }
					}
				    }
				  else
				    {
				      // ASSERT(FALSE);
				    }
				}
			    }
			}
		    }
		}
	    }
	}
      nIterations++;
    }
  while(nCornersSorted<m_nTotalCorners && nIterations<m_nMaxOrderIterations);

//   if(bDataFile)
//     fileData.Close();

  if(nIterations>=m_nMaxOrderIterations && nCornersSorted!=m_nTotalCorners)
    {
      printf("Failed to sequence corners after %d iterations.",
	     m_nMaxOrderIterations);
      return false;
    }

  // copy the sorted corners and types
  for(int i=0;i<m_nTotalCorners;i++)
    {
      m_pCornersPixels[i] = pSortedCorners[i];
      m_pCategorisedUnsortedCorners[i] = pSortedTypes[i];
    }

  delete[]pCornersPlaced;
  delete[]pCornerMapping;
  delete[]pSortedTypes;
  delete[]pSortedCorners;

  m_bApproxCornersOrdered = true;

  return true;
}

float SortCorners::IndexExtremeAnglesFrom3AndGetRange
(float * pfAngles,int &refnMinimumIndex,int &refnMaximumIndex)
{
  /* takes three angles, determines the extremes and the range
     the range is the difference between the max and min angles 
     of the two extremes, the range will be minimised to be <= 180 */
  // assumes that angles are in degrees

  // declare a value to use for comparison with other angles
  float fTestAngle = 0.0f;

  // count number of positive and negative angles
  int nPositiveAngles = 0;
  int nNegativeAngles = 0;
  int nAngles = 3;

  for(int i=0;i<nAngles;i++)
    {
      fTestAngle = pfAngles[i];
      if(fTestAngle<0.0)
	nNegativeAngles++;
      else
	nPositiveAngles++;
    }

  // make a copy of the 3 angles
  float fAnglesCopy[3];
  for(int i=0;i<3;i++)
    fAnglesCopy[i] = pfAngles[i];

  // initially get range just by finding min and max
  float fInitialMin = 500.0f;
  float fInitialMax = -500.0f;
  int nInitialMinIndex = 0;
  int nInitialMaxIndex = 0;
  for(int i=0;i<3;i++)
    {
      fTestAngle = fAnglesCopy[i];
      if(fTestAngle<fInitialMin)
	{
	  fInitialMin = fTestAngle;
	  nInitialMinIndex = i;
	}
      if(fTestAngle>fInitialMax)
	{
	  fInitialMax = fTestAngle;
	  nInitialMaxIndex = i;
	}
    }

  // run some tests to see if we need to do a more advanced analysis
  if( (fInitialMin + 180.0f) >= fInitialMax)
    {
      // simplest scenario, no more work needed
      refnMinimumIndex = nInitialMinIndex;
      refnMaximumIndex = nInitialMaxIndex;

      return (fInitialMax - fInitialMin);
    }

  // now run more advanced tests

  /* convert all angles in the list of 3 to the same sign, the sign chosen 
     will depend upon the number of positive and negative angles in the list of 3
     if there are more positive than negative, the sequence is made all positive
     if there are more negative than positive the sequence is made negative  */
  if(nPositiveAngles>nNegativeAngles)
    {
      // convert all negative to positive
      for(int i=0;i<3;i++)
	{
	  if(fAnglesCopy[i]<0.0f)
	    {
	      fAnglesCopy[i] += 360.0f;
	    }
	}
    }
  else
    {
      // convert all positive angles to negative
      for(int i=0;i<3;i++)
	{
	  if(fAnglesCopy[i]>0.0f)
	    {
	      fAnglesCopy[i] -= 360.0f;
	    }
	}
    }

  // now determine extremes of angle
  float fAngleMin = 500.0f;
  float fAngleMax = -500.0f;

  // track the indexes of the min and max angles in the angles array
  int nMaxIndex = -1;
  int nMinIndex = -1;

  for(int i=0;i<3;i++)
    {
      fTestAngle = fAnglesCopy[i];
      if(fTestAngle<fAngleMin)
	{
	  fAngleMin = fTestAngle;
	  nMinIndex = i;
	}
      if(fTestAngle>fAngleMax)
	{
	  fAngleMax = fTestAngle;
	  nMaxIndex = i;
	}
    }

  // calculate range
  float fRange = fAngleMax - fAngleMin;

  // make sure that we get the smallest range of the 
  // angles i.e. max range = 180 degrees
  if(fRange>180.0f)
    fRange = 360.0f - fRange;

  /*
  // find the index of the non-extreme neighbour out of the 3 nearest
  int nNonExtremeNeighbour = -1;
  for(int i=0;i<3;i++)
  {
  if(i!=nMinIndex && i!=nMaxIndex)
  nNonExtremeNeighbour = i;
  }
  float fAngleNonExtreme = pfAngles[nNonExtremeNeighbour];

  // test to ensure that the minimum and maximum are correct
  // for example min = -175 and max = 141, but the extremes are 
  // not correct when the non-extreme angle = 61
  // correct range in this example is 
  // min = -175, max = 61, range = -175 - (61-360)
  if(fAngleMin< -90.0f && fAngleMax>(fAngleMin + 180.0f) && 
  fAngleNonExtreme>fAngleMin && fAngleNonExtreme<fAngleMax)
  {
  fAngleMax = fAngleNonExtreme;
  nMaxIndex = nNonExtremeNeighbour;
  }

  if(nPositiveAngles==1)
  {
  // 2 -ve and 1 +ve angles

  // check quadrant of min and max
  if(fAngleMax>90.0f && fAngleMin<(-90.0f))
  {
  // here is where the range calculations start to get more complex
                        
  // using the index of the maximum angle element, make the maximum 
  // the minimum by subtracting 360 degrees from it
  pfAngles[nMaxIndex] -= 360.0f;

  // now recalculate the minimum and maximum angles
  fAngleMin = 500.0f;
  fAngleMax = -500.0f;
  for(int i=0;i<nAngles;i++)
  {
  fTestAngle = pfAngles[i];

  if(fTestAngle<fAngleMin)
  {
  fAngleMin = fTestAngle;
  nMinIndex = i;
  }
  if(fTestAngle>fAngleMax)
  {
  fAngleMax = fTestAngle;
  nMaxIndex = i;
  }
  }
  }
  }
  */
  ////  bool bOutOfRange = false;
  if(nPositiveAngles>nNegativeAngles)
    {
      if(fAngleMax>180.0f)
	{
	  // swap min and max indexes
	  int nTemp = nMinIndex;
	  nMinIndex = nMaxIndex;
	  nMaxIndex = nTemp;
	}
    }

  refnMinimumIndex = nMinIndex;
  refnMaximumIndex = nMaxIndex;

  return fRange;
}

void SortCorners::IndexExtremeAnglesFrom4
(float * pfAngles,int &refnExtremeMin,int &refnExtremeMax)
{
  // declare a value to use for comparison with other angles
  float fTestAngle = 0.0f;

  // count number of positive and negative angles
  int nPositiveAngles = 0;
  int nNegativeAngles = 0;
  int nAngles = 4;

  for(int i=0;i<nAngles;i++)
    {
      fTestAngle = pfAngles[i];
      if(fTestAngle<0.0)
	nNegativeAngles++;
      else
	nPositiveAngles++;
    }

  // make a copy of the 4 angles
  float fAnglesCopy[4];
  for(int i=0;i<4;i++)
    fAnglesCopy[i] = pfAngles[i];

  // initially get range just by finding min and max
  float fInitialMin = 500.0f;
  float fInitialMax = -500.0f;
  int nInitialMinIndex = 0;
  int nInitialMaxIndex = 0;
  for(int i=0;i<4;i++)
    {
      fTestAngle = fAnglesCopy[i];
      if(fTestAngle<fInitialMin)
	{
	  fInitialMin = fTestAngle;
	  nInitialMinIndex = i;
	}
      if(fTestAngle>fInitialMax)
	{
	  fInitialMax = fTestAngle;
	  nInitialMaxIndex = i;
	}
    }

  // run some tests to see if we need to do a more advanced analysis
  if( (fInitialMin + 180.0f) >= fInitialMax)
    {
      // simplest scenario, no more work needed
      refnExtremeMin = nInitialMinIndex;
      refnExtremeMax = nInitialMaxIndex;

      return;
    }

  // now run more advanced tests

  // convert all angles in the list of 4 to the same sign, the sign chosen 
  // will depend upon the number of positive and 
  // negative angles in the list of 4
  // if there are more positive than negative, the sequence is made all positive
  // if there are more negative than positive the sequence is made negative
  if(nPositiveAngles>nNegativeAngles)
    {
      // assess the range of the initial minimum and maximum angles
      if(nNegativeAngles==1)
	{
	  // special case for when there is only one negative angle
	  if(fInitialMin>= -90.0f)
	    {
	      // negative angle is in the -90 to 0 quadrant
	      // set the minimum as the negative
	      refnExtremeMin = nInitialMinIndex;

	      // the other extreme is the initial maximum
	      refnExtremeMax = nInitialMaxIndex;

	      // no need to do any more
	      return;
	    }
	  else
	    {
	      // negative angle is in the -180 to -90 quadrant

	      // so the maximum angle is now the initial minimum, 
	      // and the minimum is the minimum of the positve angles
	      refnExtremeMax = nInitialMinIndex;

	      // find the minimum of the positive angles
	      int nPosMin = 0;
	      float fPosMinAngle = 500.0f;
	      for(int n=0;n<4;n++)
		{
		  fTestAngle = fAnglesCopy[n];
		  if(fTestAngle>=0.0f)
		    {
		      if(fTestAngle<fPosMinAngle)
			{
			  fPosMinAngle = fTestAngle;
			  nPosMin = n;
			}
		    }
		}

	      // set the new minimum angle
	      refnExtremeMin = nPosMin;

	      // finished here
	      return;
	    }
	}

      // convert all negative to positive
      for(int i=0;i<4;i++)
	{
	  if(fAnglesCopy[i]<0.0f)
	    {
	      fAnglesCopy[i] += 360.0f;
	    }
	}
    }
  else if(nPositiveAngles<nNegativeAngles)
    {
      // convert all positive angles to negative
      for(int i=0;i<4;i++)
	{
	  if(fAnglesCopy[i]>0.0f)
	    {
	      fAnglesCopy[i] -= 360.0f;
	    }
	}
    }
  else
    {
      // same number of positive angles as negative angles
      //AfxMessageBox("Same Positive Count AS Negative Count",
      //               MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
      refnExtremeMin = nInitialMinIndex;
      refnExtremeMax = nInitialMaxIndex;
      return;
    }

  // now determine extremes of angle
  float fAngleMin = 500.0f;
  float fAngleMax = -500.0f;

  // track the indexes of the min and max angles in the angles array
  int nMaxIndex = -1;
  int nMinIndex = -1;

  for(int i=0;i<4;i++)
    {
      fTestAngle = fAnglesCopy[i];
      if(fTestAngle<fAngleMin)
	{
	  fAngleMin = fTestAngle;
	  nMinIndex = i;
	}
      if(fTestAngle>fAngleMax)
	{
	  fAngleMax = fTestAngle;
	  nMaxIndex = i;
	}
    }

  refnExtremeMin = nMinIndex;
  refnExtremeMax = nMaxIndex;
}

void SortCorners::FindDiagonalNeighboursFrom4
(float * pf4NeighbourAngles,tagOUTERANGLES * pOuterAngles,bool * pbDiagonals)
{
  /* try an alternative method

  compare the angle of the test corner to a neighbour with each angle of 
  outer corner to other outer corners
  
  the angle from one outer corner to another that is closest to the angle 
  from the test corner to a neighbour indicates the general horizontal, 
  vertical or diagonal position of the neighbour corner to the test corner
  
  neighbours that are at a diagonal angle are invalid for placement

  valid angle comparisons for placement are the corners that are closest 
  to the horizontal and vertical outer corner angles e.g.
  
  from top-left to bottom-left and vice versa
  from top-left to top-right and vice versa
  from bottom-right to bottom-left and vice versa
  from bottom-right to top-top and vice versa  */

  float fMinDifference = 500.0f;
  float fCurrentDifference = 0.0f;
  float fNeighbourAngle = 0.0f;
  int nBestOuterToOuterTypeMatch = 0;

  // look at each angle...
  for(int i=0;i<4;i++)
    {
      // ..and subtract it from each outer-to-outer angle, 
      // taking the absolute of the answer
      fNeighbourAngle = pf4NeighbourAngles[i];
      fMinDifference = 500.0f;
      nBestOuterToOuterTypeMatch = 0;

      // compare top-left to top-right angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleTL_TR - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_TL_TR;
	}

      // compare top-left to bottom-left angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleTL_BL - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_TL_BL;
	}

      // compare top-left to bottom-right angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleTL_BR - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_TL_BR;
	}

      // compare top-right to top-left angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleTR_TL - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_TR_TL;
	}

      // compare top-right to bottom-left angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleTR_BL - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_TR_BL;
	}

      // compare top-right to bottom-right angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleTR_BR - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_TR_BR;
	}

      // compare bottom-left to top-left angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleBL_TL - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_BL_TL;
	}

      // compare bottom-left to top-right angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleBL_TR - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_BL_TR;
	}

      // compare bottom-left to bottom-right angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleBL_BR - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_BL_BR;
	}

      // compare bottom-right to top-left angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleBR_TL - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_BR_TL;
	}

      // compare bottom-right to top-right angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleBR_TR - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_BR_TR;
	}

      // compare bottom-right to bottom-left angle
      fCurrentDifference = 
	(float)fabs( pOuterAngles->fAngleBR_BL - fNeighbourAngle );
      if(fCurrentDifference<fMinDifference)
	{
	  fMinDifference = fCurrentDifference;
	  nBestOuterToOuterTypeMatch = ANGLE_BR_BL;
	}

      /* now examine the best outer-to-outer type match value to see 
	 if the angle of the corner to its neighbour best matches a 
	 diagonal or orthogonal outer-to-outer angle */
      switch(nBestOuterToOuterTypeMatch)
	{
	case ANGLE_TL_BR:
	case ANGLE_TR_BL:
	case ANGLE_BL_TR:
	case ANGLE_BR_TL:
	  /* the angles top-left to bottom-right, top-right to 
	     bottom-left, bottom-left to top-right and bottom-right 
	     to top-left are all diagonal angles */
	  pbDiagonals[i] = true;
	  break;

	default:
	  // others are orthogonal angles
	  pbDiagonals[i] = false;
	  break;
	}

      // continue with the next neighbour angle
    }

  return;
}
