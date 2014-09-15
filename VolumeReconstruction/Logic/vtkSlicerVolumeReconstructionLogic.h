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

// .NAME vtkSlicerVolumeReconstructionLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerVolumeReconstructionLogic_h
#define __vtkSlicerVolumeReconstructionLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// vtk includes
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>

// MRML includes
#include <vtkMRMLVolumeNode.h>

// STD includes
#include <cstdlib>

#include "vtkSlicerVolumeReconstructionModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate

class VTK_SLICER_VOLUMERECONSTRUCTION_MODULE_LOGIC_EXPORT vtkSlicerVolumeReconstructionLogic :
public vtkSlicerModuleLogic
{

public:

    static vtkSlicerVolumeReconstructionLogic *New();
    vtkTypeMacro(vtkSlicerVolumeReconstructionLogic, vtkSlicerModuleLogic);
    void PrintSelf(ostream& os, vtkIndent indent);

    /**
     * 
     * @param matrix
     */
    void AddTransformToScene(vtkMatrix4x4* matrix);
    
    /**
     * 
     * @param newScene
     */
    void AddAndDisplayVolume(vtkImageData * volume, int ext[6], double spacing[3], 
            double volumeScale);
    
    
protected:
    vtkSlicerVolumeReconstructionLogic();
    virtual ~vtkSlicerVolumeReconstructionLogic();

    virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
    /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
    virtual void RegisterNodes();
    virtual void UpdateFromMRMLScene();
    virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
    virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
    
    
private:

    vtkSlicerVolumeReconstructionLogic(const vtkSlicerVolumeReconstructionLogic&); // Not implemented
    void operator=(const vtkSlicerVolumeReconstructionLogic&); // Not implemented
    
    /**
     * \brief find the resulted volume size from the input images, rotations and
     * transforms given by the tracker. The number of the images must be equal to 
     * the number of rotations and translations.
     */
    int * findVolumeBounds(vtkImageData * images);
};

#endif
