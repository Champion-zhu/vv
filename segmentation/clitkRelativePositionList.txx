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


//--------------------------------------------------------------------
template <class TImageType>
clitk::RelativePositionList<TImageType>::
RelativePositionList() {
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void
clitk::RelativePositionList<TImageType>::
Read(std::string filename) {
  /*
    The goal here is to read a text file that contains options for the
    RelativePosition filter. The text file contains options in the
    same form of the config file of the clitkRelativePosition tool. In
    this text file, each time a "object" option is found, a new set of
    options is considered.
   */

  // Open the file
  std::ifstream is;
  openFileForReading(is, filename);

  // Read input -> the structure name that will be used as input
  // (initial support)
  skipComment(is);
  std::string s;
  is >> s;
  if (s != "input") {
    FATAL("while reading RelativePositionList file. This file must start with 'input = '");
    exit(0);
  }
  is >> s; 
  if (s != "=") {
    FATAL("while reading RelativePositionList file. This file must start with 'input = '");
    exit(0);
  }
  std::string name;
  is >> name;
  skipComment(is);

  // Create a temporary filename 
  char n[] = "tmp_clitkRelativePosition_XXXXXX";
  mkstemp(n); // tmpnam(n); 
  std::string tmpfilename(n);
  
  // Loop on the file text ; Every time we see a "object" token, we
  // split the file.
  while (is) {
    bool stop=false;
    std::ostringstream ss;
    // first part of ss is the last 'object = ' found, stored in s
    ss << s << std::endl; 
    while (!stop) {
      skipComment(is);
      if (!is) stop = true;
      else {
        std::getline(is, s);
        // DD(s);
        if (s.find("object") != std::string::npos) stop=true;
        else ss << s << std::endl;
        if (!is) stop = true;
      }
    }
    std::string text = ss.str();
    if (text.size() > 10) { // if too small, it is not a list of option
      std::ofstream os;
      openFileForWriting(os, tmpfilename);
      os << text;
      os.close();

      // Create a struct to store options
      ArgsInfoType args_info;
      args_info.input_given = 1;
      args_info.input_arg = new char[1];
      args_info.output_given = 1;
      args_info.output_arg = new char[1];
      std::vector<char> writable(tmpfilename.size() + 1);
      std::copy(tmpfilename.begin(), tmpfilename.end(), writable.begin());
      cmdline_parser_clitkRelativePosition_configfile(&writable[0], &args_info, 1, 1, 0);
      
      // Store the args
      mArgsInfoList.push_back(args_info);
      
      // Delete the temporary file
      std::remove(tmpfilename.c_str());
    }
  }

  // Set the main input name
  SetInputName(name);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::RelativePositionList<TImageType>::
GenerateInputRequestedRegion() 
{
  // Call default
  itk::ImageToImageFilter<ImageType, ImageType>::GenerateInputRequestedRegion();
  // Get input pointers and set requested region to common region
  ImagePointer input1 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  input1->SetRequestedRegion(input1->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------



//--------------------------------------------------------------------
template <class TImageType>
void
clitk::RelativePositionList<TImageType>::
GenerateOutputInformation() {

  // Get input
  m_working_input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));

  // Loop on RelativePositionList of operations
  std::string s = GetInputName();
  for(uint i=0; i<mArgsInfoList.size(); i++) {
    std::string text = "["+s+"] limits "+
      mArgsInfoList[i].orientation_arg[0]+" "+
      mArgsInfoList[i].object_arg+" "+
      toString(mArgsInfoList[i].threshold_arg);
    if (mArgsInfoList[i].sliceBySlice_flag) {
      text += " slice by slice";
    }
    else text += " 3D";

    StartNewStep(text);  
    typename RelPosFilterType::Pointer relPosFilter;

    // DD(mArgsInfoList[i].sliceBySlice_flag);
    if (mArgsInfoList[i].sliceBySlice_flag) {
      relPosFilter = SliceRelPosFilterType::New();
      dynamic_cast<SliceRelPosFilterType*>(&*relPosFilter)->SetDirection(2);
    }
    else {
      relPosFilter = clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::New();
    }
    relPosFilter->VerboseStepFlagOff();
    relPosFilter->WriteStepFlagOff();
    relPosFilter->SetVerboseImageSizeFlag(GetVerboseImageSizeFlag());
    relPosFilter->SetInput(m_working_input);
    SetFilterOptions(relPosFilter, mArgsInfoList[i]);    
    //relPosFilter->PrintOptions();
    relPosFilter->Update();
    m_working_input = relPosFilter->GetOutput();  
    StopCurrentStep<ImageType>(m_working_input);
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void
clitk::RelativePositionList<TImageType>::
GenerateData() 
{
  // Final Step -> set output
  this->GraftOutput(m_working_input);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void
clitk::RelativePositionList<TImageType>::
SetFilterOptions(typename RelPosFilterType::Pointer filter, 
                 ArgsInfoType & options) {

  if (options.orientation_given != 1) {
    DD("ERRROR DEBUG TODO no more than 1 orientation yet");
    exit(0);
  }

  ImagePointer object = GetAFDB()->template GetImage<ImageType>(options.object_arg);
  filter->SetInputObject(object);
  filter->SetFuzzyThreshold(options.threshold_arg);
  filter->SetInverseOrientationFlag(options.inverse_flag); // MUST BE BEFORE AddOrientationTypeString
  for(uint i=0; i<options.orientation_given; i++) 
    filter->AddOrientationTypeString(options.orientation_arg[i]);
  filter->SetIntermediateSpacing(options.spacing_arg);
  if (options.spacing_arg == -1) filter->IntermediateSpacingFlagOff();
  filter->IntermediateSpacingFlagOn();
  
  if (options.sliceBySlice_flag) {
    SliceRelPosFilterType * f = dynamic_cast<SliceRelPosFilterType*>(&*filter);
    f->SetUniqueConnectedComponentBySliceFlag(options.uniqueCCL_flag);
    f->SetObjectCCLSelectionFlag(options.uniqueObjectCCL_flag);
    f->IgnoreEmptySliceObjectFlagOn();
    //filter->SetObjectCCLSelectionDimension(0);
    //filter->SetObjectCCLSelectionDirection(-1);
    //filter->SetAutoCropFlag(false);
  }
  filter->SetAutoCropFlag(!options.noAutoCrop_flag); 
}
//--------------------------------------------------------------------