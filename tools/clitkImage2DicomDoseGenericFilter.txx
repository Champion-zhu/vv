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
#ifndef clitkImage2DicomDoseGenericFilter_txx
#define clitkImage2DicomDoseGenericFilter_txx

/* =================================================
 * @file   clitkImage2DicomDosemGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/

#include "math.h"

#include "clitkImage2DicomDoseGenericFilter.h"
#include "clitkCommon.h"
#if GDCM_MAJOR_VERSION >= 2
#include "gdcmUIDGenerator.h"
#include <gdcmImageHelper.h>
#include <gdcmAttribute.h>
#include <gdcmReader.h>
#include <gdcmWriter.h>
#else
#include "gdcmFile.h"
#include "gdcmUtil.h"
#endif

//#include "gdcmBase.h"
//#include "gdcmDocEntry.h"



namespace clitk
{


//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
template<class args_info_type>
Image2DicomDoseGenericFilter<args_info_type>::Image2DicomDoseGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void Image2DicomDoseGenericFilter<args_info_type>::Update()
{
  // Read the Dimension and PixelType
  int Dimension;
  std::string PixelType;
  ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);


  // Call UpdateWithDim
  if(Dimension==2) UpdateWithDim<2>(PixelType);
  else if(Dimension==3) UpdateWithDim<3>(PixelType);
  // else if (Dimension==4)UpdateWithDim<4>(PixelType);
  else {
    std::cout<<"Error, Only for 2 or 3  Dimensions!!!"<<std::endl ;
    return;
  }
}

//-------------------------------------------------------------------
// Update with the number of dimensions
//-------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dimension>
void
Image2DicomDoseGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType)
{
  if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

  if(PixelType == "short") {
    if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, signed short>();
  }
  else if(PixelType == "unsigned_short"){
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, unsigned short>();
  }

  else if (PixelType == "unsigned_char") {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, unsigned char>();
  }

  //     else if (PixelType == "char"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, signed char>();
  //     }
  else if (PixelType == "double") {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and double..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, double>();
  }
  else {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, float>();
  }
}

//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype read from
// the dicom files. The MHD files may be resampled to match the
// dicom spacing (and number of slices). Rounding errors in resampling
// are handled by removing files when generating the output dicom
// series.
//-------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
Image2DicomDoseGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{

#if GDCM_MAJOR_VERSION == 2
  // ImageTypes
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image<PixelType, Dimension> OutputImageType;
  typedef itk::ImageFileReader< InputImageType >     ReaderType;
  typedef itk::ImageSeriesReader< InputImageType >     ReaderSeriesType;
  typedef itk::ImageRegionIterator< InputImageType > IteratorType;
  typedef itk::GDCMImageIO ImageIOType;

  //-----------------------------------------------------------------------------
  // opening image input file
  typename ReaderType::Pointer reader = ReaderType::New();
  const char * filename = m_ArgsInfo.input_arg;
  reader->SetFileName( filename );
  reader->Update();
  typename InputImageType::Pointer image = reader->GetOutput();
  std::ostringstream value;

  // Read Dicom model file
  typename ReaderSeriesType::Pointer readerSeries = ReaderSeriesType::New();
  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  std::string filename_out = m_ArgsInfo.output_arg;
  gdcmIO->LoadPrivateTagsOn();
  gdcmIO->KeepOriginalUIDOn();
  typename ReaderSeriesType::FileNamesContainer fileNames;
  fileNames.push_back(m_ArgsInfo.DicomInputFile_arg);
  readerSeries->SetImageIO( gdcmIO );
  readerSeries->SetFileNames( fileNames );
  try {
    readerSeries->Update();
  } catch (itk::ExceptionObject &excp) {
    std::cerr << "Error: Exception thrown while reading the DICOM model file !!" << std::endl;
    std::cerr << excp << std::endl;
  }

  // update output dicom keys/tags
  typename ReaderSeriesType::DictionaryRawPointer inputDict = (*(readerSeries->GetMetaDataDictionaryArray()))[0];
  typename ReaderSeriesType::DictionaryArrayType outputArray;
  typename ReaderSeriesType::DictionaryRawPointer outputDict = new typename ReaderSeriesType::DictionaryType;
  CopyDictionary (*inputDict, *outputDict);

  // origin
  typename InputImageType::PointType origin = image->GetOrigin();
  value.str("");
  value<<origin[0]<<'\\'<<origin[1]<<'\\'<<origin[2];
  itk::EncapsulateMetaData<std::string>(*outputDict, "0020|0032", value.str());
  DD(origin);

  // size
  typename InputImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();
  //DD(imageSize);
  int NbCols=imageSize[0];	// col
  int NbRows=imageSize[1];	// row
  int NbFrames=imageSize[2];	// frame
  itk::EncapsulateMetaData<std::string>(*outputDict, "0028|0008", NumberToString(NbFrames));
  itk::EncapsulateMetaData<std::string>(*outputDict, "0028|0010", NumberToString(NbRows));
  itk::EncapsulateMetaData<std::string>(*outputDict, "0028|0011", NumberToString(NbCols));
  DD(NbCols);
  DD(NbRows);
  DD(NbFrames);

  // spacing
  typename InputImageType::SpacingType Spacing = image->GetSpacing();
  value.str("");
  value<<Spacing[0]<<'\\'<<Spacing[1];
  itk::EncapsulateMetaData<std::string>(*outputDict, "0028|0030", value.str());
  value.str("");
  value<<Spacing[2];
  itk::EncapsulateMetaData<std::string>(*outputDict, "0018|0050", value.str());
  DD(Spacing);

  // offset
  float offset = 0.;
  value.str("");
  value << offset;
  for (int i=1; i<NbFrames ; i++){
    offset+=Spacing[2];
    value << '\\';
    value << offset;
  }
  itk::EncapsulateMetaData<std::string>(*outputDict, "3004|000c", value.str());
  DD(value.str());

  // scaling
  float highestValue=pow(10,-10);
  IteratorType out( image, image->GetRequestedRegion() );
  for (out.GoToBegin(); !out.IsAtEnd(); ++out){
    //DD(out.Get());
    if (out.Get()>highestValue) highestValue=out.Get();
  }
  double doseScaling = 0.0003080821;//highestValue/(pow(2,16)-1);
  value.str("");
  value<<doseScaling;
  itk::EncapsulateMetaData<std::string>(*outputDict, "3004|000e", value.str());
  DD(value.str());

  // image data
  /*std::vector<unsigned short int> ImageData;
  typename InputImageType::IndexType pixelIndex;
  int l=0;
  unsigned short int pixelValue;
  //DD(highestValue);
  for (int i=0; i<NbFrames; i++){
    pixelIndex[2] = i;
    for (int j=0; j<NbRows; j++){
      pixelIndex[1] = j;
      for (int k=0; k<NbCols; k++){
        pixelIndex[0] = k;
        pixelValue=image->GetPixel(pixelIndex)/doseScaling;
        if(float(image->GetPixel(pixelIndex)/doseScaling)>(pow(2,16)-1.)) {
          std::cout<<"\n!!!!! WARNING !!!!! pixel index: "<<pixelIndex<<"unsigned short int capacity ful or overfuled => Highest value may become 0"<<std::endl;
          DD(pixelIndex);
          DD(image->GetPixel(pixelIndex));
          //DD(image->GetPixel(pixelIndex)/doseScaling);
          DD(pixelValue);
          std::cout<<"Pixel Value should be equal to "<<(pow(2,16)-1)<<" but should not be 0"<<std::endl;
          std::cout<<"\n"<<std::endl;
          //assert(pixelValue<=(pow(2,16)-1));	should work, but do not...
        }
        //DD(pixelValue);
        ImageData.push_back(pixelValue);
        l++;
      }
    }
  }
  DD(ImageData.size());*/

  // Output directory and filenames
  typedef itk::ImageSeriesWriter<OutputImageType, OutputImageType>  WriterSerieType;
  typename WriterSerieType::Pointer writerSerie = WriterSerieType::New();
  outputArray.push_back(outputDict);
  writerSerie->SetInput( image );
  writerSerie->SetImageIO( gdcmIO );
  typename ReaderSeriesType::FileNamesContainer fileNamesOutput;
  fileNamesOutput.push_back(filename_out);
  writerSerie->SetFileNames( fileNamesOutput );
  writerSerie->SetMetaDataDictionaryArray(&outputArray);

  // Write
  try {
    if (m_ArgsInfo.verbose_flag)
      std::cout << writerSerie << std::endl;
    writerSerie->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error: Exception thrown while writing the series!!" << std::endl;
    std::cerr << excp << std::endl;
  }


//---------------------------------------------------------------------------------------
//WRITE DICOM BIS
// The previous way of writting DICOM-RT-DOSE works only for ITK
// and resulting RT-DOSE files are not readable by commercial systems.
// The nex step is to copy again the output file with another syntax, allowing to make RT-DOSE files readable by commercial systems.
// see Jean-Pierre Roux comment below.

/*gdcm::FileHelper *fh = new gdcm::FileHelper(args_info.OutputFile_arg);
   void *imageData;
   int dataSize;
  
   dataSize  = fh->GetImageDataRawSize();
   imageData = fh->GetImageDataRaw();// somewhat important : Loads the Pixels in memory !
  
   fh->SetWriteModeToRaw();
   fh->SetWriteTypeToDcmExplVR();

   bool res = fh->Write(args_info.OutputFile_arg);

   if(!res)
      std::cout <<"Fail to write [" << args_info.OutputFile_arg << "]" <<std::endl;   
   else std::cout<<"\n DICOM File re-written, using the FileHelper syntax, in order to be processed by commercial systems !"<<std::endl;

delete fh; */
/*  gdcm::Writer w;
  w.SetFile(mDCMFile);
  w.SetFileName(m_ArgsInfo.output_arg);
  w.Write();*/
//   fh->Delete();

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Jean-Pierre Roux help for DICOM-RT-DOSE writting
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

// 	de	Jean-Pierre Roux <jpr@creatis.insa-lyon.fr>
// répondre à	jpr@creatis.insa-lyon.fr
// à	Loic Grevillot <loic.grevillot@gmail.com>
// cc	jpr@creatis.insa-lyon.fr,
// Joël Schaerer <joel.schaerer@gmail.com>,
// David Sarrut <David.Sarrut@creatis.insa-lyon.fr>,
// Joel Schaerer <joel.schaerer@creatis.insa-lyon.fr>
// date	12 juillet 2010 12:23
// objet	Re: DICOM RT DOSE
// 	
// masquer les détails 12:23 (Il y a 21 heures)
// 	
// Bonjour,
// 
// J'aurais écrit à peut prèt la même chose.
// (Ci après un extrait de mon code -Example/ReWrite.cxx-)
// 
// L'utilisation d'un FileHelper (qui ne changera rien dans ce cas précis) est une mesure de précaution, car,  l'élément 7FE0|0010 peut être compressé (ce qui n'est pas le cas pour tes images), que la manière de stocker les pixels ainsi compressés était parfois un peu ... curieuse.
// Je décompresse, et réécrit non compressé.
// 
// ============================
//    gdcm::File *f = new gdcm::File();
//    f->SetMaxSizeLoadEntry(0x7fffffff);
//    f->SetFileName( fileName );
//    bool res = f->Load(); 
//    if ( !res )
//    {
//       delete f;
//       return 0;
//    }
// 
//    if (!f->IsReadable())
//    {
//        std::cerr << "Sorry, not a Readable DICOM / ACR File"  <<std::endl;
//        delete f;
//        return 0;
//    }
// 
//    gdcm::FileHelper *fh = new gdcm::FileHelper(f);
//    void *imageData;
//    int dataSize;
//   
//    dataSize  = fh->GetImageDataRawSize();
//    imageData = fh->GetImageDataRaw();// somewhat important : Loads the Pixels in memory !
//   
//    fh->SetWriteModeToRaw();
//    fh->SetWriteTypeToDcmExplVR();
//    res = fh->Write(outputFileName);
//   
//    if(!res)
//       std::cout <<"Fail to write [" << outputFileName << "]" <<std::endl;   
// 
//    f->Delete();
//    fh->Delete();

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// pour supprimer des tags:
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//	SOLUCE 1 QUI MARCHE POUR UN SQItem SIMPLE
/*
gdcm::DocEntry *a;
//a= ((gdcm::SQItem*)mDCMFile)->GetDocEntry(0xfffe,0xe00d);
a= ((gdcm::SQItem*)mDCMFile)->GetDocEntry(0x3004,0x000e);
((gdcm::SQItem*)mDCMFile)->RemoveEntry(a);
mDCMFile->Write (args_info.OutputFile_arg, type);
*/

//	SOLUCE 2 QUI MARCHE POUR UNE SeqEntry->SQItem 
/*
std::cout<<"\ntest correction fichier apres ecriture\n"<<std::endl;
gdcm::SeqEntry *seqEntry = mDCMFile->GetSeqEntry(0x300c,0x0002);
gdcm::SQItem* currentItem = seqEntry->GetFirstSQItem();
gdcm::DocEntry *a;
//a= currentItem->GetDocEntry(0x0008,0x1155);
a= currentItem->GetDocEntry(0xfffe,0xe00d);
currentItem->RemoveEntry(a);
mDCMFile->Write (args_info.OutputFile_arg, type);
*/

//gdcm::DocEntry *a;
//a=GetDocEntry(0x7fe0,0x0000);
//((gdcm::SQItem*)mDCMFile)->RemoveEntry(a);

//-----------------------------------------------------------------------------------
  std::cout <<"\n## DICOM Image to RT DOSE application is ended..."<<std::endl;
  std::cout <<"#########################################################\n" << std::endl;

#else
  std::cout << "Use GDCM2" << std::endl;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict)
{
  typedef itk::MetaDataDictionary DictionaryType;

  DictionaryType::ConstIterator itr = fromDict.Begin();
  DictionaryType::ConstIterator end = fromDict.End();
  typedef itk::MetaDataObject< std::string > MetaDataStringType;

  while( itr != end )
    {
    itk::MetaDataObjectBase::Pointer  entry = itr->second;

    MetaDataStringType::Pointer entryvalue =
      dynamic_cast<MetaDataStringType *>( entry.GetPointer() ) ;
    if( entryvalue )
      {
      std::string tagkey   = itr->first;
      std::string tagvalue = entryvalue->GetMetaDataObjectValue();
      itk::EncapsulateMetaData<std::string>(toDict, tagkey, tagvalue);
      }
    ++itr;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
template <typename T> std::string NumberToString ( T Number )
{
   std::ostringstream ss;
   ss << Number;
   return ss.str();
}
//---------------------------------------------------------------------------


}//end clitk

#endif //#define clitkImage2DicomDoseGenericFilter_txx
