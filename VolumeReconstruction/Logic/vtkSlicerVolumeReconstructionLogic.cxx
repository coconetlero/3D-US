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

// VolumeReconstruction Logic includes
#include "vtkSlicerVolumeReconstructionLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSelectionNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>

// STD includes
#include <cassert>

// Slicer includes
#include "qSlicerCoreApplication.h"
#include "vtkSlicerApplicationLogic.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerVolumeReconstructionLogic);

//----------------------------------------------------------------------------

vtkSlicerVolumeReconstructionLogic::vtkSlicerVolumeReconstructionLogic()
{
}

//----------------------------------------------------------------------------

vtkSlicerVolumeReconstructionLogic::~vtkSlicerVolumeReconstructionLogic()
{
}

//----------------------------------------------------------------------------

void vtkSlicerVolumeReconstructionLogic::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------

void vtkSlicerVolumeReconstructionLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
    events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
    events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
    this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------

void vtkSlicerVolumeReconstructionLogic::RegisterNodes()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------

void vtkSlicerVolumeReconstructionLogic::UpdateFromMRMLScene()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------

void vtkSlicerVolumeReconstructionLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------

void vtkSlicerVolumeReconstructionLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}


//---------------------------------------------------------------------------

void vtkSlicerVolumeReconstructionLogic::AddTransformToScene(vtkMatrix4x4* matrix)
{
    vtkNew<vtkMRMLLinearTransformNode> transformNode;
    this->GetMRMLScene()->AddNode(transformNode.GetPointer());

    transformNode->SetAndObserveMatrixTransformFromParent(matrix);
    std::cout << "Add transformation matrix node" << std::endl;
}


// --------------------------------------------------------------------------

void vtkSlicerVolumeReconstructionLogic::
AddAndDisplayVolume(vtkImageData* volume, int ext[], double spacing[], double volumeScale)
{
    if (volume == NULL)
    {
        return;
    }

    vtkMRMLScene * scene = this->GetMRMLScene();

    // create a display node
    vtkNew<vtkMRMLScalarVolumeDisplayNode> labelDisplayNode;
    scene->AddNode(labelDisplayNode.GetPointer());
    labelDisplayNode->SetLowerThreshold(0);
    labelDisplayNode->SetUpperThreshold(255);
    labelDisplayNode->SetDefaultColorMap();
    labelDisplayNode->SetInterpolate(false);

    // create a volume with the content and unique name 
    vtkNew<vtkMRMLScalarVolumeNode> labelNode;
    labelNode->SetAndObserveStorageNodeID(NULL);
    labelNode->SetLabelMap(1);

    std::string uname = scene->GetUniqueNameByString("US Volume");
    labelNode->SetName(uname.c_str());
    labelNode->SetAndObserveDisplayNodeID(labelDisplayNode->GetID());
    labelNode->SetOrigin(ext[0] * volumeScale, ext[2] * volumeScale, ext[4] * volumeScale);
    labelNode->SetSpacing(spacing);

    labelNode->SetAndObserveImageData(volume);

    // add the label volume to the scene
    scene->AddNode(labelNode.GetPointer());

    // get Slicer logic
    vtkSlicerApplicationLogic * appLogic =
            qSlicerCoreApplication::application()->applicationLogic();

    // get for each view the selection node
    vtkMRMLSelectionNode * selectionNode = appLogic->GetSelectionNode();
    selectionNode->SetReferenceActiveVolumeID(labelNode->GetID());
    // put the volume in all views 
    appLogic->PropagateVolumeSelection();
    appLogic->FitSliceToAll();
}

// --------------------------------------------------------------------------
