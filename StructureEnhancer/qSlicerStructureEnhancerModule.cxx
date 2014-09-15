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

// StructureEnhancer Logic includes
#include <vtkSlicerStructureEnhancerLogic.h>

// StructureEnhancer includes
#include "qSlicerStructureEnhancerModule.h"
#include "qSlicerStructureEnhancerModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerStructureEnhancerModule, qSlicerStructureEnhancerModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerStructureEnhancerModulePrivate
{
public:
  qSlicerStructureEnhancerModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerStructureEnhancerModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerStructureEnhancerModulePrivate
::qSlicerStructureEnhancerModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerStructureEnhancerModule methods

//-----------------------------------------------------------------------------
qSlicerStructureEnhancerModule
::qSlicerStructureEnhancerModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerStructureEnhancerModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerStructureEnhancerModule::~qSlicerStructureEnhancerModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerStructureEnhancerModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerStructureEnhancerModule::acknowledgementText()const
{
  return "This work was was partially funded by CONACyT";
}

//-----------------------------------------------------------------------------
QStringList qSlicerStructureEnhancerModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Zian Fanti (UNAM)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerStructureEnhancerModule::icon()const
{
  return QIcon(":/Icons/StructureEnhancer.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerStructureEnhancerModule::categories() const
{
  return QStringList() << "3DFreehandUS";
}

//-----------------------------------------------------------------------------
QStringList qSlicerStructureEnhancerModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerStructureEnhancerModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerStructureEnhancerModule
::createWidgetRepresentation()
{
  return new qSlicerStructureEnhancerModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerStructureEnhancerModule::createLogic()
{
  return vtkSlicerStructureEnhancerLogic::New();
}
