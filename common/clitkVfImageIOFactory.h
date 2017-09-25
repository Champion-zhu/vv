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
#ifndef CLITKVFIMAGEIOFACTORY_H
#define CLITKVFIMAGEIOFACTORY_H

/**
 * @file   clitkVfImageIOFactory.h
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   Mon Sep 18 10:14:12 2006
 * 
 * @brief  
 * 
 * 
 */

// clitk include
#include "clitkVfImageIO.h"

// itk include
#include "itkImageIOBase.h"
#include "itkObjectFactoryBase.h"
#include "itkVersion.h"

namespace clitk {
  
  //====================================================================
  // Factory for reading Vf Image file format
  class VfImageIOFactory: public itk::ObjectFactoryBase
  {
  public:
	/** Standard class typedefs. */
	typedef VfImageIOFactory              Self;
	typedef itk::ObjectFactoryBase         Superclass;
	typedef itk::SmartPointer<Self>        Pointer;
	typedef itk::SmartPointer<const Self>  ConstPointer;
	
	/** Class methods used to interface with the registered factories. */
	const char* GetITKSourceVersion(void) const ITK_OVERRIDE {
	  return ITK_SOURCE_VERSION;
	}
	
	const char* GetDescription(void) const ITK_OVERRIDE {
	  return "Vf ImageIO Factory, allows the loading of Vf images into insight";
	}
	
	/** Method for class instantiation. */
	itkFactorylessNewMacro(Self);
	
	/** Run-time type information (and related methods). */
	itkTypeMacro(VfImageIOFactory, ObjectFactoryBase);
	
	/** Register one factory of this type  */
	static void RegisterOneFactory(void) {
	  ObjectFactoryBase::RegisterFactory( Self::New() );
	}	

  protected:
	VfImageIOFactory();
	~VfImageIOFactory() {};
	typedef VfImageIOFactory myProductType;
	const myProductType* m_MyProduct;
	
  private:
	VfImageIOFactory(const Self&); //purposely not implemented
	void operator=(const Self&); //purposely not implemented
  };

} // end namespace

#endif /* end #define CLITKVFIMAGEIOFACTORY_H */

