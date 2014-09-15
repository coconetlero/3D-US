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

// .NAME vtkSlicerICPRegistrationLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerICPRegistrationLogic_h
#define __vtkSlicerICPRegistrationLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerICPRegistrationModuleLogicExport.h"

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkMRMLModelNode.h>

// MRML includes
#include <vtkMRMLLinearTransformNode.h>

/// \ingroup Slicer_QtModules_ExtensionTemplate

class VTK_SLICER_ICPREGISTRATION_MODULE_LOGIC_EXPORT vtkSlicerICPRegistrationLogic :
public vtkSlicerModuleLogic
{

public:

    static vtkSlicerICPRegistrationLogic *New();
    vtkTypeMacro(vtkSlicerICPRegistrationLogic, vtkSlicerModuleLogic);
    void PrintSelf(ostream& os, vtkIndent indent);

    /** 
     * \brief add a new transform to the scene resulting of the registration process 
     * and insert the input model to this new transformation
     */
    void AddTransformToScene(vtkSmartPointer<vtkMRMLLinearTransformNode> transformationNode,
            vtkSmartPointer<vtkMRMLModelNode> modelNode);

protected:

    vtkSlicerICPRegistrationLogic();
    virtual ~vtkSlicerICPRegistrationLogic();

    virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
    /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
    virtual void RegisterNodes();
    virtual void UpdateFromMRMLScene();
    virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
    virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

private:

    vtkSlicerICPRegistrationLogic(const vtkSlicerICPRegistrationLogic&); // Not implemented
    void operator=(const vtkSlicerICPRegistrationLogic&); // Not implemented
};

#endif
