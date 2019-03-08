/*=========================================================================
  Program:         vv http://www.creatis.insa-lyon.fr/rio/vv
  Main authors :   XX XX XX

  Authors belongs to:
  - University of LYON           http://www.universite-lyon.fr/
  - Léon Bérard cancer center    http://www.centreleonberard.fr
  - CREATIS CNRS laboratory      http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence
  - BSD       http://www.opensource.org/licenses/bsd-license.php
  - CeCILL-B  http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html

  =========================================================================*/

// clitk
#include "clitkXml2DicomRTStructFilter.h"
#include "clitkXml2DicomRTStruct_ggo.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkXml2DicomRTStruct, args_info);

  // Create a filter to convert Xml struct into dicomRTStruct and write to disk
  typedef float PixelType;
  clitk::Xml2DicomRTStructFilter<PixelType> filter;
  filter.SetVerboseFlag(args_info.verbose_flag);
  filter.SetUsePixel(args_info.pixel_flag);
  if (args_info.pixel_flag) {
    if (args_info.image_given)
      filter.SetImageMHD(args_info.image_arg);
    else {
      std::cout << "Set MHD image (option -j) when pixel is set" << std::endl;
      return -1;
    }
  }
  filter.SetInputFilename(args_info.input_arg);
  filter.SetDicomFolder(args_info.dicom_arg);
  filter.SetStructureSetFilename(args_info.rtstruct_arg);
  filter.SetOutputFilename(args_info.output_arg);
  filter.Update();

  // This is the end my friend
  return 0;
}
//--------------------------------------------------------------------
