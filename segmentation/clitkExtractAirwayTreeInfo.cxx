/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/

// clitk
#include "clitkExtractAirwayTreeInfo_ggo.h"
#include "clitkExtractAirwayTreeInfoGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkExtractAirwayTreeInfo, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::ExtractAirwayTreeInfoGenericFilter<args_info_clitkExtractAirwayTreeInfo> FilterType;
  FilterType::Pointer filter = FilterType::New();

  filter->SetArgsInfo(args_info);
  filter->Update();

  if (filter->HasError()) {
    std::cout << filter->GetLastError() << std::endl;
  }

  return EXIT_SUCCESS;
} // This is the end, my friend
//--------------------------------------------------------------------