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
#ifndef clitkWarpImageGenericFilter_txx
#define clitkWarpImageGenericFilter_txx

/* =================================================
 * @file   clitkWarpImageGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/

#if ( ITK_VERSION_MAJOR < 5 )
#include "itkVectorResampleImageFilter.h"
#else
#include "itkResampleImageFilter.h"
#endif
#include "clitkConvertBLUTCoeffsToVFFilter.h"

namespace clitk
{

//-------------------------------------------------------------------
// Update with the number of dimensions
//-------------------------------------------------------------------
template<unsigned int Dimension>
void
WarpImageGenericFilter::UpdateWithDim(std::string PixelType)
{
  if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

  if(PixelType == "short") {
    if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, signed short>();
  }
  //    else if(PixelType == "unsigned_short"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, unsigned short>();
  //     }

  else if (PixelType == "unsigned_char") {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, unsigned char>();
  }

  //     else if (PixelType == "char"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, signed char>();
  //     }
  else {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, float>();
  }
}


//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//-------------------------------------------------------------------
template <unsigned int Dimension, class  PixelType>
void
WarpImageGenericFilter::UpdateWithDimAndPixelType()
{

  // ImageTypes
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image<PixelType, Dimension> OutputImageType;
  typedef itk::Vector<float, Dimension> DisplacementType;
  typedef itk::Image<DisplacementType, Dimension> DeformationFieldType;
  
  // Read the input
  typedef itk::ImageFileReader<InputImageType> InputReaderType;
  typename InputReaderType::Pointer reader = InputReaderType::New();
  reader->SetFileName( m_InputFileName);
  reader->Update();
  typename InputImageType::Pointer input= reader->GetOutput();
  
  typename DeformationFieldType::Pointer deformationField;
  if (m_ArgsInfo.coeff_given) {
    typedef ConvertBLUTCoeffsToVFFilter<DeformationFieldType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInputFileName(m_ArgsInfo.coeff_arg);
    filter->SetLikeFileName(m_InputFileName);
    filter->SetVerbose(m_Verbose);
    filter->Update();
    deformationField = filter->GetOutput();
  }
  else {
    //Read the deformation field
    typedef itk::ImageFileReader<DeformationFieldType> DeformationFieldReaderType;
    typename  DeformationFieldReaderType::Pointer deformationFieldReader= DeformationFieldReaderType::New();
    deformationFieldReader->SetFileName(m_ArgsInfo.vf_arg);
    deformationFieldReader->Update();
    deformationField =deformationFieldReader->GetOutput();
  }

  // Intensity interpolator
  typedef clitk::GenericVectorInterpolator<args_info_clitkWarpImage, DeformationFieldType, double> GenericInterpolatorType;
  typename GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
  genericInterpolator->SetArgsInfo(m_ArgsInfo);


  // -------------------------------------------
  // Spacing like DVF
  // -------------------------------------------
  if (m_ArgsInfo.spacing_arg == 0) {
    // Calculate the region
    typename DeformationFieldType::SizeType newSize;
    for (unsigned int i=0 ; i <Dimension; i++)
      newSize[i]=(unsigned int) (input->GetLargestPossibleRegion().GetSize()[i]*input->GetSpacing()[i]/deformationField->GetSpacing()[i]);

    // Get the interpolator
    typedef clitk::GenericVectorInterpolator<args_info_clitkWarpImage, DeformationFieldType, double> GenericInterpolatorType;
    typename GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
    genericInterpolator->SetArgsInfo(m_ArgsInfo);

    // Resample to match the extent of the input
#if ( ITK_VERSION_MAJOR < 5 )
    typename itk::VectorResampleImageFilter<DeformationFieldType, DeformationFieldType >::Pointer
    resampler =itk::VectorResampleImageFilter<DeformationFieldType, DeformationFieldType >::New();
#else
    typename itk::ResampleImageFilter<DeformationFieldType, DeformationFieldType >::Pointer
    resampler =itk::ResampleImageFilter<DeformationFieldType, DeformationFieldType >::New();
#endif
    resampler->SetInput(deformationField);
    resampler->SetOutputSpacing(deformationField->GetSpacing());
    resampler->SetOutputDirection(deformationField->GetDirection());
    resampler->SetSize(newSize);
    resampler->SetOutputOrigin(input->GetOrigin());
    resampler->SetInterpolator(genericInterpolator->GetInterpolatorPointer());

    // Update
    if (m_Verbose) std::cout<< "Resampling the VF..." <<std::endl;
    try {
      resampler->Update();
    } catch( itk::ExceptionObject & excp ) {
      std::cerr << "Problem resampling the input vector field file" << std::endl;
      std::cerr << excp << std::endl;
      return;
    }
    deformationField= resampler->GetOutput();

  }

  // -------------------------------------------
  // Spacing like input
  // -------------------------------------------
  else if (!m_ArgsInfo.coeff_given) {
    // Get size
    typename DeformationFieldType::SizeType newSize;
    for (unsigned int i=0 ; i <Dimension; i++)
      newSize[i]=input->GetLargestPossibleRegion().GetSize()[i];

    // Resample to match the extent of the input
#if ( ITK_VERSION_MAJOR < 5 )
    typename itk::VectorResampleImageFilter<DeformationFieldType, DeformationFieldType >::Pointer
    resampler =itk::VectorResampleImageFilter<DeformationFieldType, DeformationFieldType >::New();
#else
    typename itk::ResampleImageFilter<DeformationFieldType, DeformationFieldType >::Pointer
    resampler =itk::ResampleImageFilter<DeformationFieldType, DeformationFieldType >::New();
#endif
    resampler->SetInput(deformationField);
    resampler->SetOutputSpacing(input->GetSpacing());
    resampler->SetOutputDirection(deformationField->GetDirection());
    resampler->SetSize(newSize);
    resampler->SetOutputOrigin(input->GetOrigin());
    resampler->SetInterpolator(genericInterpolator->GetInterpolatorPointer());

    // Update
    if (m_Verbose) std::cout<< "Resampling the VF..." <<std::endl;
    try {
      resampler->Update();
    } catch( itk::ExceptionObject & excp ) {
      std::cerr << "Problem resampling the input vector field file" << std::endl;
      std::cerr << excp << std::endl;
      return;
    }
    deformationField= resampler->GetOutput();
  }


  // -------------------------------------------
  // Forward Warp
  // -------------------------------------------
  typename    itk::ImageToImageFilter<InputImageType, InputImageType>::Pointer warpFilter;
  if (m_ArgsInfo.forward_flag) {
    //Forward warping: always linear
    typedef clitk::ForwardWarpImageFilter<InputImageType, InputImageType, DeformationFieldType> ForwardWarpFilterType;
    typename ForwardWarpFilterType::Pointer forwardWarpFilter= ForwardWarpFilterType::New();
    forwardWarpFilter->SetDeformationField( deformationField );
    forwardWarpFilter->SetEdgePaddingValue( static_cast<PixelType>(m_ArgsInfo.pad_arg) );
    warpFilter=forwardWarpFilter;
  }

  // -------------------------------------------
  // Backward Warp
  // -------------------------------------------
  else {
    // Get the interpolator
    typedef clitk::GenericInterpolator<args_info_clitkWarpImage, InputImageType, double> GenericInterpolatorType;
    typename GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
    genericInterpolator->SetArgsInfo(m_ArgsInfo);

    //Backward mapping
    typedef itk::WarpImageFilter<InputImageType, InputImageType, DeformationFieldType> BackwardWarpFilterType;
    typename BackwardWarpFilterType::Pointer backwardWarpFilter= BackwardWarpFilterType::New();
    backwardWarpFilter->SetDisplacementField( deformationField );
    backwardWarpFilter->SetEdgePaddingValue( static_cast<PixelType>(m_ArgsInfo.pad_arg) );
    backwardWarpFilter->SetOutputSpacing( deformationField->GetSpacing() );
    backwardWarpFilter->SetOutputOrigin( input->GetOrigin() );
    backwardWarpFilter->SetOutputSize( deformationField->GetLargestPossibleRegion().GetSize() );
    backwardWarpFilter->SetOutputDirection( input->GetDirection() );
#if ( ITK_VERSION_MAJOR < 5 )
    typename itk::VectorResampleImageFilter<DeformationFieldType, DeformationFieldType >::Pointer
    resampler =itk::VectorResampleImageFilter<DeformationFieldType, DeformationFieldType >::New();
#else
    typename itk::ResampleImageFilter<DeformationFieldType, DeformationFieldType >::Pointer
    resampler =itk::ResampleImageFilter<DeformationFieldType, DeformationFieldType >::New();
#endif
    backwardWarpFilter->SetInterpolator(genericInterpolator->GetInterpolatorPointer());
    warpFilter=backwardWarpFilter;
  }


  // -------------------------------------------
  // Update
  // -------------------------------------------
  warpFilter->SetInput(input);
  if (m_Verbose) std::cout<< "Warping the input..." <<std::endl;
  try {
    warpFilter->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Problem warping the input image" << std::endl;
    std::cerr << excp << std::endl;
    return;
  }


  // Output
  typedef itk::ImageFileWriter<OutputImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(m_ArgsInfo.output_arg);
  writer->SetInput(warpFilter->GetOutput());
  writer->Update();

}


}//end clitk

#endif //#define clitkWarpImageGenericFilter_txx
