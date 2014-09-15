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

// GraphCutSegmentation Logic includes
#include "vtkSlicerGraphCutSegmentationLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

// Slicer includes
#include "qSlicerCoreApplication.h"
#include "vtkSlicerApplicationLogic.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerGraphCutSegmentationLogic);

//----------------------------------------------------------------------------
vtkSlicerGraphCutSegmentationLogic::vtkSlicerGraphCutSegmentationLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerGraphCutSegmentationLogic::~vtkSlicerGraphCutSegmentationLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerGraphCutSegmentationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerGraphCutSegmentationLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerGraphCutSegmentationLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerGraphCutSegmentationLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerGraphCutSegmentationLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerGraphCutSegmentationLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

// --------------------------------------------------------------------------

void vtkSlicerGraphCutSegmentationLogic::AddAndDisplayVolume(vtkMRMLVolumeNode *outputVolumeNode)
{
    vtkMRMLScene * scene = this->GetMRMLScene();

    if (scene == NULL || outputVolumeNode == NULL)
    {
        std::cout << "scene or volume node are null" << std::endl;
        return;
    }

    // create a display node
    vtkNew<vtkMRMLScalarVolumeDisplayNode> scalarDisplayNode;

    scalarDisplayNode->SetLowerThreshold(0);
    scalarDisplayNode->SetUpperThreshold(255);
    scalarDisplayNode->SetDefaultColorMap();
    scalarDisplayNode->SetInterpolate(false);
    scene->AddNode(scalarDisplayNode.GetPointer());

    std::string uname = scene->GetUniqueNameByString("Segmented Volume");
    outputVolumeNode->SetName(uname.c_str());
    outputVolumeNode->SetAndObserveDisplayNodeID(scalarDisplayNode->GetID());

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

