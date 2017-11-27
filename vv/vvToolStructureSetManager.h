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

#ifndef VVTOOLSTRUCTURESETMANAGER_H
#define VVTOOLSTRUCTURESETMANAGER_H

#if QT_VERSION >= 0x050000
#include <QtUiPlugin/QDesignerExportWidget>
#else
#include <QtDesigner/QDesignerExportWidget>
#endif
#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "clitkDicomRT_StructureSet.h"
#include "ui_vvToolStructureSetManager.h"
#include "vvROIActor.h"

#include <QSharedPointer>

class vvStructureSetActor;

//------------------------------------------------------------------------------
class vvToolStructureSetManager:
  public vvToolWidgetBase,
  public vvToolBase<vvToolStructureSetManager>, 
  private Ui::vvToolStructureSetManager
{
  Q_OBJECT
  public:
  vvToolStructureSetManager(vvMainWindowBase* parent=0, Qt::WindowFlags f=0, vvSlicerManager * c = NULL);
  virtual ~vvToolStructureSetManager();

  static void Initialize();
  virtual void InputIsSelected(vvSlicerManager *m);

  int AddStructureSet(clitk::DicomRT_StructureSet * mStructureSet);
  void UpdateStructureSetInTreeWidget(int index, clitk::DicomRT_StructureSet * s);
  void AddRoiInTreeWidget(clitk::DicomRT_ROI * roi, QTreeWidget * w);

  static vvToolStructureSetManager * AddImage(vvSlicerManager * m, std::string name, vvImage::Pointer image, double BG, bool modeBG=true);    
  void AddImage(vvImage * image, std::string filename, double BG, bool modeBG=true);
  void UpdateImage();

public slots:
  virtual void apply();
  void Open(int type);
  void OpenBinaryImage();
  void SelectedItemChangedInTree();
  void VisibleROIToggled(bool b);
  void VisibleContourROIToggled(bool b);
  void OpacityChanged(int v);
  void ChangeColor();
  void ChangeContourColor();
  void ChangeContourWidth(int n);
  void ChangeDepth(int n);
  void AllVisibleROIToggled(int b);
  void AllVisibleContourROIToggled(bool b);
  void ReloadCurrentROI();

protected:
  Ui::vvToolStructureSetManager ui;
  clitk::DicomRT_StructureSet* mCurrentStructureSet;
  vvStructureSetActor * mCurrentStructureSetActor;
  int mCurrentStructureSetIndex;
  clitk::DicomRT_ROI::Pointer mCurrentROI;
  vvROIActor * mCurrentROIActor;
  vtkSmartPointer<vtkLookupTable> mDefaultLUTColor;
  bool mIsAllVisibleEnabled;
  int mNumberOfVisibleROI;
  int mNumberOfVisibleContourROI;
  std::vector<clitk::DicomRT_StructureSet::Pointer > mStructureSetsList;
  std::vector< QSharedPointer<vvStructureSetActor> > mStructureSetActorsList;
  std::vector< QSharedPointer<QTreeWidgetItem> > mTreeWidgetList;
  std::map<clitk::DicomRT_ROI::Pointer, QTreeWidgetItem *> mMapROIToTreeWidget;
  std::map<QTreeWidgetItem *, clitk::DicomRT_ROI::Pointer> mMapTreeWidgetToROI;
  std::vector<int> mLoadedROIIndex;
  std::vector<vvImage::Pointer> mOpenedBinaryImage;
 
  void setCurrentSelectedROI(clitk::DicomRT_ROI * roi);
  void UpdateAllROIStatus();
  virtual bool close();
  virtual void closeEvent(QCloseEvent *event);

  // Management of all instances
  virtual void CheckInputList(std::vector<vvSlicerManager*> & l, int & index);
  static std::vector<vvSlicerManager*> mListOfInputs;
  static std::map<vvSlicerManager*, vvToolStructureSetManager*> mListOfOpenTool;
  static int m_NumberOfTool;
  bool MustOpenDialogWhenCreated;
 
}; // end class vvToolStructureSetManager
//------------------------------------------------------------------------------

#endif

