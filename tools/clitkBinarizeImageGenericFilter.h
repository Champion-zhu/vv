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

#ifndef clitkBinarizeImageGenericFilter_h
#define clitkBinarizeImageGenericFilter_h

#include "clitkIO.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkBinarizeImage_ggo.h"

//--------------------------------------------------------------------
namespace clitk 
{
  
  class ITK_EXPORT BinarizeImageGenericFilter: 
    public ImageToImageGenericFilter<BinarizeImageGenericFilter>
  {
    
  public:
    //--------------------------------------------------------------------
    typedef BinarizeImageGenericFilter         Self;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
    typedef args_info_clitkBinarizeImage       args_info_type;
   
    //--------------------------------------------------------------------
    // Method for creation through the object factory
    // and Run-time type information (and related methods)
    itkNewMacro(Self);  
    itkTypeMacro(BinarizeImageGenericFilter, LightObject);

    //--------------------------------------------------------------------
    void SetArgsInfo(const args_info_type & a);
    void SetPercentage(double p) { mPercentage = p; }
    double GetPercentage() const { return mPercentage; }

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

    template<class InputImageType>
    void MaskOfIntegratedIntensity();

  protected:
    BinarizeImageGenericFilter();
    template<unsigned int Dim> void InitializeImageType();
    args_info_type mArgsInfo;
    double mPercentage;
    
  }; // end class
  //--------------------------------------------------------------------
    
//Implementation of the pair comparative function
template <typename T> bool comparator ( const std::pair<T, size_t>& l, const std::pair<T, size_t>& r);
} // end namespace clitk
//--------------------------------------------------------------------

#endif // #define clitkBinarizeImageGenericFilter_h
