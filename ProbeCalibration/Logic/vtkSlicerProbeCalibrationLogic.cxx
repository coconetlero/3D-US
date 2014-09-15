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

// ProbeCalibration Logic includes
#include "vtkSlicerProbeCalibrationLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include "vtkMRMLAnnotationFiducialNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLVolumeNode.h"

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <string>
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerProbeCalibrationLogic);

//----------------------------------------------------------------------------
vtkSlicerProbeCalibrationLogic::vtkSlicerProbeCalibrationLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerProbeCalibrationLogic::~vtkSlicerProbeCalibrationLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerProbeCalibrationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerProbeCalibrationLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerProbeCalibrationLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerProbeCalibrationLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerProbeCalibrationLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerProbeCalibrationLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//------------------------------------------------------------------------------
void vtkSlicerProbeCalibrationLogic::AddTransformToScene(vtkMatrix4x4* matrix, std::string name)
{
    vtkMRMLScene * scene = this->GetMRMLScene();
    
    vtkNew<vtkMRMLLinearTransformNode> transformNode;
    scene->AddNode(transformNode.GetPointer());
    
    std::string uname = scene->GetUniqueNameByString(name.c_str());
    transformNode->SetAndObserveMatrixTransformToParent(matrix);
    
    std::cout << "add transformation node: " << uname << std::endl;

}