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
#ifndef VVBINARYIMAGEOVERLAYACTOR_H
#define VVBINARYIMAGEOVERLAYACTOR_H

#include "clitkCommon.h"
#include "vvSlicer.h"

class vtkImageClip;
class vtkMarchingSquares;
class vtkActor;
class vvImage;
class vtkImageMapToRGBA;
class vtkImageActor;
class vtkImageReslice;
class vtkTransform;

//------------------------------------------------------------------------------
class vvBinaryImageOverlayActor : public itk::LightObject
{
  //  Q_OBJECT
 public:
  typedef vvBinaryImageOverlayActor Self;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

  void SetSlicer(vvSlicer * slicer);
  void SetColor(double r, double g, double b);
  void SetOpacity(double d);
  void SetImage(vvImage::Pointer image, double bg, bool modeBG=true);
  void SetDepth(double d) { mDepth = d; }
  void Initialize(bool IsVisible=true);
  void UpdateColor();
  void UpdateSlice(int slicer, int slice, bool force=false);
  void HideActors();
  void ShowActors();
  void RemoveActors();

 protected:
  vvSlicer * mSlicer;
  int mSlice;
  int mTSlice;
  int mPreviousTSlice;
  int mPreviousSlice;
  vvImage::Pointer mImage;
  std::vector<double> mColor;
  double mAlpha;
  double mBackgroundValue;
  double mForegroundValue;
  bool m_modeBG;
  vtkSmartPointer<vtkLookupTable> mColorLUT;
  double mDepth;

  std::vector<vtkSmartPointer<vtkImageMapToRGBA> > mMapperList;
  std::vector<vtkSmartPointer<vtkImageActor> > mImageActorList;
  vtkSmartPointer<vtkImageReslice> mFusionReslice;
  vtkSmartPointer<vtkTransform> mConcatenatedFusionTransform;

  void ComputeExtent(int orientation, 
		     int slice, 
		     int * inExtent,
		     int * outExtent);
  void ComputeExtent(int * inExtent, 
		     int * outExtent, 
		     vtkImageData * image, 
		     vtkImageData * overlay);
  void SetDisplayExtentAndCameraPosition(int orientation, 
					 int slice, 
					 int * extent, 
					 vtkImageActor * actor, 
					 double position);

private:
  vvBinaryImageOverlayActor();
  ~vvBinaryImageOverlayActor();
    
}; // end class vvBinaryImageOverlayActor
//------------------------------------------------------------------------------

#endif

