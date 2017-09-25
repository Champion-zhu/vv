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
#ifndef CLITKIMAGECREATE_CXX
#define CLITKIMAGECREATE_CXX
/**
   ------------------------------------------------=
   * @file   clitkImageCreate.cxx
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   14 Oct 2008 08:37:53
   ------------------------------------------------=*/

// clitk include
#include "clitkImageCreate_ggo.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkIO.h"

template<class ImageType>
void NewFilledImage(int * size, float * spacing, double * origin, double * direction,
                    double value,typename ImageType::Pointer output)
{
  static const unsigned int Dim = ImageType::GetImageDimension();

  typename ImageType::SizeType mSize;
  mSize.Fill (0);
  for(unsigned int i=0; i<Dim; i++) mSize[i] = size[i];

  typename ImageType::RegionType mRegion;
  mRegion.SetSize(mSize);

  typename ImageType::SpacingType mSpacing;
  for(unsigned int i=0; i<Dim; i++) mSpacing[i] = spacing[i];

  typename ImageType::DirectionType directionImage;
  for(unsigned int i=0; i<Dim; i++)
    for(unsigned int j=0; j<Dim; j++)
      directionImage[i][j] = direction[i*Dim+j];

  output->SetRegions(mRegion);
  output->SetSpacing(mSpacing);
  output->SetDirection(directionImage);
  output->Allocate();

  typename ImageType::PointType mOrigin;
  for(unsigned int i=0; i<Dim; i++) mOrigin[i] = origin[i];
  output->SetOrigin(mOrigin);

  typedef typename ImageType::PixelType PixelType;
  PixelType p = clitk::PixelTypeDownCast<double, PixelType>(value);
  output->FillBuffer(p);
}


//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkImageCreate, args_info);
  CLITK_INIT;

  // Check --like option
  int dim;
  if (args_info.like_given) {
    itk::ImageIOBase::Pointer header = clitk::readImageHeader(args_info.like_arg);
    dim = header->GetNumberOfDimensions();
    //mPixelTypeName = header->GetComponentTypeAsString(header->GetComponentType());
    //mNbOfComponents = header->GetNumberOfComponents();
    args_info.size_given = dim;
    args_info.size_arg = new int[dim];
    args_info.spacing_given = dim;
    args_info.spacing_arg = new float[dim];
    args_info.origin_given = dim;
    args_info.origin_arg = new double[dim];
    args_info.transformMatrix_given = dim*dim;
    args_info.transformMatrix_arg = new double[dim*dim];

    for(int i=0; i<dim; i++) {
      args_info.size_arg[i] = header->GetDimensions(i);
      args_info.spacing_arg[i] = header->GetSpacing(i);
      args_info.origin_arg[i]=   header->GetOrigin(i);
      for (int j=0; j<dim; ++j)
        args_info.transformMatrix_arg[i*dim+j] = header->GetDirection(i)[j];
    }
  }

  // Check dimension
  if ((args_info.size_given > 4) || (args_info.size_given <2)) {
    std::cerr << "ERROR : only 2D/3D/4D image ! Please give 2 or 3 or 4 number to the --size option." << std::endl;
    exit(-1);
  }
  dim = args_info.size_given;

  // origin
  std::vector<double> origin;
  origin.resize(dim);
  for(int i=0; i<dim; i++) origin[i]=0.;
  if (args_info.origin_given) {
    if (args_info.origin_given==1)
      for(int i=0; i<dim; i++) origin[i] = args_info.origin_arg[0];
    else {
      if (args_info.origin_given != args_info.size_given) {
        std::cerr << "ERROR : please give the same number of values for --origin and --size." << std::endl;
        exit(-1);
      }
      for(int i=0; i<dim; i++) origin[i] = args_info.origin_arg[i];
    }
  }

  // spacing
  std::vector<float> spacing;
  spacing.resize(dim);
  if (args_info.spacing_given == 1) {
    for(int i=0; i<dim; i++) spacing[i] = args_info.spacing_arg[0];
  } else {
    if (args_info.spacing_given != args_info.size_given) {
      std::cerr << "ERROR : please give the same number of values for --size and --spacing." << std::endl;
      exit(-1);
    }
    for(int i=0; i<dim; i++) spacing[i] = args_info.spacing_arg[i];
  }

  // direction
  std::vector<double> direction;
  direction.resize(dim*dim);
  for(int i=0; i<dim; i++)
  {
    for(int j=0; j<dim; j++)
    {
      if (i == j)
        direction[i*dim+j] = 1;
      else
        direction[i*dim+j] = 0;
    }
  }
  if (args_info.transformMatrix_given) {
    if (args_info.transformMatrix_given != dim*dim) {
      std::cerr << "ERROR : please give the same number of values for --transfomMatrix and --spacing." << std::endl;
      exit(-1);
    }
    for(int i=0; i<dim; i++)
      for(int j=0; j<dim; j++)
        direction[i*dim+j] = args_info.transformMatrix_arg[i*dim+j];
  }

  // Create new image
  typedef float PixelType;
  if (dim == 2) {
    const int Dim=2;
    typedef itk::Image<PixelType, Dim> ImageType;
    ImageType::Pointer output = ImageType::New();
    NewFilledImage<ImageType>(args_info.size_arg, &spacing[0], &origin[0], &direction[0], args_info.value_arg, output);
    clitk::writeImage<ImageType>(output, args_info.output_arg);
  }
  if (dim == 3) {
    const int Dim=3;
    typedef itk::Image<PixelType, Dim> ImageType;
    ImageType::Pointer output = ImageType::New();
    NewFilledImage<ImageType>(args_info.size_arg, &spacing[0], &origin[0], &direction[0], args_info.value_arg, output);
    clitk::writeImage<ImageType>(output, args_info.output_arg);
  }
  if (dim == 4) {
    const int Dim=4;
    typedef itk::Image<PixelType, Dim> ImageType;
    ImageType::Pointer output = ImageType::New();
    NewFilledImage<ImageType>(args_info.size_arg, &spacing[0], &origin[0], &direction[0], args_info.value_arg, output);
    clitk::writeImage<ImageType>(output, args_info.output_arg);
  }

  // this is the end my friend
  return 0;
} // end main

#endif //define CLITKIMAGEARITHM_CXX
