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

// VolumeReconstruction Logic includes
#include <vtkSlicerVolumeReconstructionLogic.h>

// VolumeReconstruction includes
#include "qSlicerVolumeReconstructionModule.h"
#include "qSlicerVolumeReconstructionModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerVolumeReconstructionModule, qSlicerVolumeReconstructionModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVolumeReconstructionModulePrivate
{
public:
  qSlicerVolumeReconstructionModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVolumeReconstructionModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerVolumeReconstructionModulePrivate
::qSlicerVolumeReconstructionModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVolumeReconstructionModule methods

//-----------------------------------------------------------------------------
qSlicerVolumeReconstructionModule
::qSlicerVolumeReconstructionModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVolumeReconstructionModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerVolumeReconstructionModule::~qSlicerVolumeReconstructionModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerVolumeReconstructionModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerVolumeReconstructionModule::acknowledgementText()const
{
  return "This work was was partially funded by CONACyT";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVolumeReconstructionModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Zian Fanti Gutierrez (UNAM)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerVolumeReconstructionModule::icon()const
{
  return QIcon(":/Icons/VolumeReconstruction.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerVolumeReconstructionModule::categories() const
{
  return QStringList() << "3DFreehandUS";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVolumeReconstructionModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerVolumeReconstructionModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerVolumeReconstructionModule
::createWidgetRepresentation()
{
  return new qSlicerVolumeReconstructionModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerVolumeReconstructionModule::createLogic()
{
  return vtkSlicerVolumeReconstructionLogic::New();
}
