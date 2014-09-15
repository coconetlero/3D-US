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

// ICPRegistration Logic includes
#include <vtkSlicerICPRegistrationLogic.h>

// ICPRegistration includes
#include "qSlicerICPRegistrationModule.h"
#include "qSlicerICPRegistrationModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerICPRegistrationModule, qSlicerICPRegistrationModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerICPRegistrationModulePrivate
{
public:
  qSlicerICPRegistrationModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerICPRegistrationModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerICPRegistrationModulePrivate
::qSlicerICPRegistrationModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerICPRegistrationModule methods

//-----------------------------------------------------------------------------
qSlicerICPRegistrationModule
::qSlicerICPRegistrationModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerICPRegistrationModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerICPRegistrationModule::~qSlicerICPRegistrationModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerICPRegistrationModule::helpText()const
{
  return "This is an 3D Slicer implementation of the VTK function vtkIterativeClosestPointTransform";
}

//-----------------------------------------------------------------------------
QString qSlicerICPRegistrationModule::acknowledgementText()const
{
  return "";
}

//-----------------------------------------------------------------------------
QStringList qSlicerICPRegistrationModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Zian Fanti Gutierrez (IIMAS-UNAM)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerICPRegistrationModule::icon()const
{
  return QIcon(":/Icons/ICPRegistration.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerICPRegistrationModule::categories() const
{
  return QStringList() << "3DFreehandUS";
}

//-----------------------------------------------------------------------------
QStringList qSlicerICPRegistrationModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerICPRegistrationModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerICPRegistrationModule
::createWidgetRepresentation()
{
  return new qSlicerICPRegistrationModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerICPRegistrationModule::createLogic()
{
  return vtkSlicerICPRegistrationLogic::New();
}
