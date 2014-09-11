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

// UltrasoundSegmentation Logic includes
#include <vtkSlicerUltrasoundSegmentationLogic.h>

// UltrasoundSegmentation includes
#include "qSlicerUltrasoundSegmentationModule.h"
#include "qSlicerUltrasoundSegmentationModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerUltrasoundSegmentationModule, qSlicerUltrasoundSegmentationModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerUltrasoundSegmentationModulePrivate
{
public:
  qSlicerUltrasoundSegmentationModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerUltrasoundSegmentationModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerUltrasoundSegmentationModulePrivate
::qSlicerUltrasoundSegmentationModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerUltrasoundSegmentationModule methods

//-----------------------------------------------------------------------------
qSlicerUltrasoundSegmentationModule
::qSlicerUltrasoundSegmentationModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerUltrasoundSegmentationModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerUltrasoundSegmentationModule::~qSlicerUltrasoundSegmentationModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerUltrasoundSegmentationModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerUltrasoundSegmentationModule::acknowledgementText()const
{
  return "This work was was partially funded by CONACyT";
}

//-----------------------------------------------------------------------------
QStringList qSlicerUltrasoundSegmentationModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Zian Fanti (UNAM)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerUltrasoundSegmentationModule::icon()const
{
  return QIcon(":/Icons/UltrasoundSegmentation.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerUltrasoundSegmentationModule::categories() const
{
  return QStringList() << "3DFreehandUS";
}

//-----------------------------------------------------------------------------
QStringList qSlicerUltrasoundSegmentationModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerUltrasoundSegmentationModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerUltrasoundSegmentationModule
::createWidgetRepresentation()
{
  return new qSlicerUltrasoundSegmentationModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerUltrasoundSegmentationModule::createLogic()
{
  return vtkSlicerUltrasoundSegmentationLogic::New();
}
