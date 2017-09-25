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

// clitk include
#include "clitkCommon.h"
#include "clitkMemoryUsage.h"
#ifndef _WIN32
#  include <unistd.h>
#endif

void clitk::PrintMemory(bool verbose, std::string s)
{
#if CLITK_MEMORY_INFO == 1
  if (verbose) {
    static double previous=0;
    double mem = GetMemoryUsageInMb();
    if (s != "") std::cout << "==> " << s << ": ";
    std::cout << mem << "MB (" << mem-previous << "MB)" << std::endl;
    previous = mem;
    //DD(stat[i].proc_resident/1000/1000);
    //DD(stat[i].pid);
  }
#endif
  }

double clitk::GetMemoryUsageInMb()
{
#if CLITK_MEMORY_INFO == 1
  //sleep(1); // wait to refresh memory ? need to let the system refresh the mem
  size_t entries;
  sg_process_stats *stat;
  int i=0;
  sg_init(1);

  stat = sg_get_process_stats(&entries);
  if(stat == NULL)
    return 0.0;

  // Search the current pid in the list of processes
  while (stat->pid != getpid() && i < entries)
  {
    ++i;
    ++stat;
  }

  // Display total memory size
  double mem = stat->proc_resident/1000/1000; // in MB
  return mem;
#else
  return 0.0;
#endif
  }
