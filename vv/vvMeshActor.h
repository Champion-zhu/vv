/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/
#ifndef vvMeshActor_h
#define vvMeshActor_h
#include "vvMesh.h"
#include "vvImage.h"

class vtkActor;
class vtkPolyDataMapper;
class vtkMarchingSquares;
class vtkImageData;

/** An actor that takes a vvMesh as input, and displays it
 ** sliced along the given slice plane */
class vvMeshActor
{
public:
    vvMeshActor();
    ///Sets slicing orientation
    void SetSlicingOrientation(unsigned int d);
    ///Changes the cut plane value in real world coordinates
    void SetCutSlice(double slice);
    /**Initialize the actor and set the inputs. If vf is not null, will use
     **it to propagate the contour on all time frames */ 
    void Init(vvMesh::Pointer mesh,int time_slice, vvImage::Pointer vf=ITK_NULLPTR);
    ///Set the time slice (only useful when using a 4D contour)
    void SetTimeSlice(int time);
    ///Toggles between normal 4D mode and superposition mode
    void ToggleSuperposition();
    vtkActor* GetActor() {return mActor;}
    ~vvMeshActor();

protected:
    ///0 for x, 1 for y, 2 for z
    unsigned int mCutDimension;
    vvMesh::Pointer mMesh;
    vtkMarchingSquares * mMarching;
    vtkPolyDataMapper* mMapper;
    vtkActor* mActor;
    double mCutPlaneValue;
    bool mSuperpostionMode;
    int mTimeSlice;
    double mCutSlice;
};

#endif
