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

// ProbeCalibration Logic includes
#include <vtkSlicerProbeCalibrationLogic.h>

// ProbeCalibration includes
#include "qSlicerProbeCalibrationModule.h"
#include "qSlicerProbeCalibrationModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerProbeCalibrationModule, qSlicerProbeCalibrationModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerProbeCalibrationModulePrivate
{
public:
  qSlicerProbeCalibrationModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerProbeCalibrationModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerProbeCalibrationModulePrivate
::qSlicerProbeCalibrationModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerProbeCalibrationModule methods

//-----------------------------------------------------------------------------
qSlicerProbeCalibrationModule
::qSlicerProbeCalibrationModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerProbeCalibrationModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerProbeCalibrationModule::~qSlicerProbeCalibrationModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerProbeCalibrationModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerProbeCalibrationModule::acknowledgementText()const
{
  return "Founded by CONACyT";
}

//-----------------------------------------------------------------------------
QStringList qSlicerProbeCalibrationModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Zian Fanti (UNAM)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerProbeCalibrationModule::icon()const
{
  return QIcon(":/Icons/ProbeCalibration.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerProbeCalibrationModule::categories() const
{
  return QStringList() << "3DFreehandUS";
}

//-----------------------------------------------------------------------------
QStringList qSlicerProbeCalibrationModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerProbeCalibrationModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerProbeCalibrationModule
::createWidgetRepresentation()
{
  return new qSlicerProbeCalibrationModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerProbeCalibrationModule::createLogic()
{
  return vtkSlicerProbeCalibrationLogic::New();
}
