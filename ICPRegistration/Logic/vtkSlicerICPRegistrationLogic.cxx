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

// ICPRegistration Logic includes
#include "vtkSlicerICPRegistrationLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLLinearTransformNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerICPRegistrationLogic);

//----------------------------------------------------------------------------

vtkSlicerICPRegistrationLogic::vtkSlicerICPRegistrationLogic()
{
}

//----------------------------------------------------------------------------

vtkSlicerICPRegistrationLogic::~vtkSlicerICPRegistrationLogic()
{
}

//----------------------------------------------------------------------------

void vtkSlicerICPRegistrationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------

void vtkSlicerICPRegistrationLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
    events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
    events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
    this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------

void vtkSlicerICPRegistrationLogic::RegisterNodes()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------

void vtkSlicerICPRegistrationLogic::UpdateFromMRMLScene()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------

void vtkSlicerICPRegistrationLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------

void vtkSlicerICPRegistrationLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//------------------------------------------------------------------------------

void vtkSlicerICPRegistrationLogic::AddTransformToScene(
        vtkSmartPointer<vtkMRMLLinearTransformNode> transformationNode,
        vtkSmartPointer<vtkMRMLModelNode> modelNode)
{
    vtkMRMLScene * scene = this->GetMRMLScene();

    scene->AddNode(transformationNode);
    modelNode->SetAndObserveTransformNodeID(transformationNode->GetID());
    
    std::cout << "add transformation node: " << transformationNode->GetName() << std::endl;

}
