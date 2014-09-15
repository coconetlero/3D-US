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

// StructureEnhancer Logic includes
#include "vtkSlicerStructureEnhancerLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSelectionNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>
#include <vtkImageThreshold.h>

// STD includes
#include <cassert>

// Slicer includes
#include "qSlicerCoreApplication.h"
#include "vtkSlicerApplicationLogic.h"

// MRML includes
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerStructureEnhancerLogic);

//----------------------------------------------------------------------------

vtkSlicerStructureEnhancerLogic::vtkSlicerStructureEnhancerLogic()
{
}

//----------------------------------------------------------------------------

vtkSlicerStructureEnhancerLogic::~vtkSlicerStructureEnhancerLogic()
{
}

//----------------------------------------------------------------------------

void vtkSlicerStructureEnhancerLogic::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------

void vtkSlicerStructureEnhancerLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
    events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
    events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
    this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------

void vtkSlicerStructureEnhancerLogic::RegisterNodes()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------

void vtkSlicerStructureEnhancerLogic::UpdateFromMRMLScene()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------

void vtkSlicerStructureEnhancerLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------

void vtkSlicerStructureEnhancerLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}


// --------------------------------------------------------------------------

void vtkSlicerStructureEnhancerLogic::AddAndDisplayVolume(vtkMRMLVolumeNode *outputVolumeNode)
{
    vtkMRMLScene * scene = this->GetMRMLScene();

    if (scene == NULL || outputVolumeNode == NULL)
    {
        std::cout << "scene or volume node are null" << std::endl;
        return;
    }

    // create a display node
    vtkNew<vtkMRMLScalarVolumeDisplayNode> labelDisplayNode;

    labelDisplayNode->SetLowerThreshold(0);
    labelDisplayNode->SetUpperThreshold(255);
    labelDisplayNode->SetDefaultColorMap();
    labelDisplayNode->SetInterpolate(false);
    scene->AddNode(labelDisplayNode.GetPointer());

    std::string uname = scene->GetUniqueNameByString("Enhanced Volume");
    outputVolumeNode->SetName(uname.c_str());
    outputVolumeNode->SetAndObserveDisplayNodeID(labelDisplayNode->GetID());

    // add the label volume to the scene
    scene->AddNode(outputVolumeNode);


    // -- Update orthogonal viewers --
    // get Slicer logic
    vtkSlicerApplicationLogic * appLogic =
            qSlicerCoreApplication::application()->applicationLogic();

    // get for each view the selection node
    vtkMRMLSelectionNode * selectionNode = appLogic->GetSelectionNode();
    selectionNode->SetReferenceActiveVolumeID(outputVolumeNode->GetID());
    // put the volume in all views 
    appLogic->PropagateVolumeSelection();
    appLogic->FitSliceToAll();
}

