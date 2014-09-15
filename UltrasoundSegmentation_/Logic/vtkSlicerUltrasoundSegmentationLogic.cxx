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

// UltrasoundSegmentation Logic includes
#include "vtkSlicerUltrasoundSegmentationLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkNew.h>
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>

// STD includes
#include <cassert>

// Slicer includes
#include "qSlicerCoreApplication.h"
#include "vtkSlicerApplicationLogic.h"

// MRML includes
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLLabelMapVolumeDisplayNode.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerUltrasoundSegmentationLogic);

//----------------------------------------------------------------------------

vtkSlicerUltrasoundSegmentationLogic::vtkSlicerUltrasoundSegmentationLogic()
{
}

//----------------------------------------------------------------------------

vtkSlicerUltrasoundSegmentationLogic::~vtkSlicerUltrasoundSegmentationLogic()
{
}

//----------------------------------------------------------------------------

void vtkSlicerUltrasoundSegmentationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------

void vtkSlicerUltrasoundSegmentationLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
    events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
    events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
    this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------

void vtkSlicerUltrasoundSegmentationLogic::RegisterNodes()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------

void vtkSlicerUltrasoundSegmentationLogic::UpdateFromMRMLScene()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------

void vtkSlicerUltrasoundSegmentationLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------

void vtkSlicerUltrasoundSegmentationLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------

void vtkSlicerUltrasoundSegmentationLogic
::AddAndDisplayVolume(vtkMRMLVolumeNode* inputVolumeNode, vtkMRMLVolumeNode* outputVolumeNode)
{
    vtkMRMLScene * scene = this->GetMRMLScene();

    if (scene == NULL || outputVolumeNode == NULL || inputVolumeNode == NULL)
    {
        std::cout << "scene or volume node are null" << std::endl;
        return;
    }

    std::cout << "IJKToRAS" << std::endl;
    vtkNew <vtkMatrix4x4> IJKToRAS;
    inputVolumeNode->GetIJKToRASMatrix(IJKToRAS.GetPointer());


    // create a display node
    vtkNew<vtkMRMLScalarVolumeDisplayNode> displayNode;

//    labelDisplayNode->SetLowerThreshold(0);
//    labelDisplayNode->SetUpperThreshold(255);
    displayNode->AutoScalarRangeOn();
    displayNode->SetDefaultColorMap();
    displayNode->SetInterpolate(false);
    scene->AddNode(displayNode.GetPointer());

    std::string uname = scene->GetUniqueNameByString("Enhanced Volume");
    outputVolumeNode->SetName(uname.c_str());
    outputVolumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
    outputVolumeNode->SetIJKToRASMatrix(IJKToRAS.GetPointer());

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

//---------------------------------------------------------------------------

void vtkSlicerUltrasoundSegmentationLogic
::AddAndDisplayLabelVolume(vtkMRMLVolumeNode* inputVolumeNode, 
        vtkMRMLScalarVolumeNode* labelVolumeNode)
{
    vtkMRMLScene * scene = this->GetMRMLScene();

    if (scene == NULL || labelVolumeNode == NULL || inputVolumeNode == NULL)
    {
        std::cout << "scene or volume node are null" << std::endl;
        return;
    }

    std::cout << "IJKToRAS" << std::endl;
    vtkNew <vtkMatrix4x4> IJKToRAS;
    inputVolumeNode->GetIJKToRASMatrix(IJKToRAS.GetPointer());


    // create a display node
    vtkNew<vtkMRMLLabelMapVolumeDisplayNode> labelDisplayNode;
    
    labelDisplayNode->AutoScalarRangeOn();
    labelDisplayNode->SetDefaultColorMap();
    scene->AddNode(labelDisplayNode.GetPointer());

    std::string uname = scene->GetUniqueNameByString("Segmented Volume");
    labelVolumeNode->SetName(uname.c_str());
    labelVolumeNode->LabelMapOn();
    labelVolumeNode->SetAndObserveDisplayNodeID(labelDisplayNode->GetID());
    labelVolumeNode->SetIJKToRASMatrix(IJKToRAS.GetPointer());

    // add the label volume to the scene
    scene->AddNode(labelVolumeNode);


    // -- Update orthogonal viewers --
    // get Slicer logic
    vtkSlicerApplicationLogic * appLogic =
            qSlicerCoreApplication::application()->applicationLogic();

    // get for each view the selection node
    vtkMRMLSelectionNode * selectionNode = appLogic->GetSelectionNode();
    selectionNode->SetReferenceActiveVolumeID(labelVolumeNode->GetID());
    // put the volume in all views 
    appLogic->PropagateVolumeSelection();
    appLogic->FitSliceToAll();
}
