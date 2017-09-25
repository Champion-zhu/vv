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
#ifndef CLITKVOXIMAGEIO_H
#define CLITKVOXIMAGEIO_H
/**
 ===================================================================
 * @file   clitkVoxImageIO.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   17 May 2006 08:01:35

 * @brief  

 ===================================================================*/

// clitk include
#include "clitkCommon.h"

// itk include
#include "itkImageIOBase.h"

// std include
#include <fstream>

namespace clitk {
  
  //====================================================================
  // Class for reading Vox Image file format
  class VoxImageIO: public itk::ImageIOBase
  {
  public: 
	/** Standard class typedefs. */
	typedef VoxImageIO              Self;
	typedef itk::ImageIOBase        Superclass;
	typedef itk::SmartPointer<Self> Pointer;	
	typedef signed short int        PixelType;
	
	VoxImageIO():Superclass() { mustWriteHeader = false; }

	/** Method for creation through the object factory. */
	itkNewMacro(Self);
	
	/** Run-time type information (and related methods). */
	itkTypeMacro(VoxImageIO, ImageIOBase);
	
	/*-------- This part of the interface deals with reading data. ------ */
	virtual void ReadImageInformation() ITK_OVERRIDE;
	virtual bool CanReadFile( const char* FileNameToRead ) ITK_OVERRIDE;
	virtual void Read(void * buffer) ITK_OVERRIDE;

	/*-------- This part of the interfaces deals with writing data. ----- */
	virtual void WriteImageInformation(bool keepOfStream);
	virtual void WriteImageInformation() ITK_OVERRIDE { WriteImageInformation(false); }
	virtual bool CanWriteFile(const char* filename) ITK_OVERRIDE;
	virtual void Write(const void* buffer) ITK_OVERRIDE;
	
  protected:
	bool mustWriteHeader;
	int m_HeaderSize;
	std::ofstream file;

  }; // end class VoxImageIO  

} // end namespace

  // explicit template instantiation
//template class itk::CreateObjectFunction<clitk::VoxImageIO>;

#endif /* end #define CLITKVOXIMAGEIO_H */

