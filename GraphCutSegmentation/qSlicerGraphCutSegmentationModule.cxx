/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QtPlugin>

// GraphCutSegmentation Logic includes
#include <vtkSlicerGraphCutSegmentationLogic.h>

// GraphCutSegmentation includes
#include "qSlicerGraphCutSegmentationModule.h"
#include "qSlicerGraphCutSegmentationModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerGraphCutSegmentationModule, qSlicerGraphCutSegmentationModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerGraphCutSegmentationModulePrivate
{
public:
  qSlicerGraphCutSegmentationModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerGraphCutSegmentationModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerGraphCutSegmentationModulePrivate
::qSlicerGraphCutSegmentationModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerGraphCutSegmentationModule methods

//-----------------------------------------------------------------------------
qSlicerGraphCutSegmentationModule
::qSlicerGraphCutSegmentationModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerGraphCutSegmentationModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerGraphCutSegmentationModule::~qSlicerGraphCutSegmentationModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerGraphCutSegmentationModule::helpText()const
{
  return "This is an implementation for a graph cut segmentation method";
}

//-----------------------------------------------------------------------------
QString qSlicerGraphCutSegmentationModule::acknowledgementText()const
{
  return "This work was was partially funded by CONACyT grant.";
}

//-----------------------------------------------------------------------------
QStringList qSlicerGraphCutSegmentationModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Zian Fanti Gutiérrez (IIMAS - UNAM)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerGraphCutSegmentationModule::icon()const
{
  return QIcon(":/Icons/GraphCutSegmentation.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerGraphCutSegmentationModule::categories() const
{
  return QStringList() << "3DFreehandUS";
}

//-----------------------------------------------------------------------------
QStringList qSlicerGraphCutSegmentationModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerGraphCutSegmentationModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerGraphCutSegmentationModule
::createWidgetRepresentation()
{
  return new qSlicerGraphCutSegmentationModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerGraphCutSegmentationModule::createLogic()
{
  return vtkSlicerGraphCutSegmentationLogic::New();
}
