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

#ifndef CLITKAFFINEREGISTRATIONGENERICFILTER_H
#define CLITKAFFINEREGISTRATIONGENERICFILTER_H

// clitk include
#include "clitkAffineRegistration_ggo.h"
#include "clitkImageToImageGenericFilter.h"




namespace clitk
{

//====================================================================
class ITK_EXPORT AffineRegistrationGenericFilter:
    public ImageToImageGenericFilter<AffineRegistrationGenericFilter>
{
public:

  AffineRegistrationGenericFilter();

  //================================================
  typedef AffineRegistrationGenericFilter        Self;
  
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef ImageToImageGenericFilterBase   Superclass;
  //================================================
  itkNewMacro(Self);
  //itkTypeMacro(AffineRegistrationGenericFilter, LightObject);


  //====================================================================
  // Set methods
       void SetArgsInfo(const args_info_clitkAffineRegistration & a);

  //====================================================================
  template<unsigned int Dim>
  void InitializeImageType();

  bool m_Verbose;
  args_info_clitkAffineRegistration m_ArgsInfo;

  template<class InputImageType>
  void UpdateWithInputImageType();
};
}

#endif //#define CLITKAFFINEREGISTRATIONGENERICFILTER__H
