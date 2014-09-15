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

// .NAME vtkSlicerProbeCalibrationLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerProbeCalibrationLogic_h
#define __vtkSlicerProbeCalibrationLogic_h

// vtk includes
#include <vtkNew.h>
#include <vtkMatrix4x4.h>

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>
#include <string>

#include "vtkSlicerProbeCalibrationModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate

class VTK_SLICER_PROBECALIBRATION_MODULE_LOGIC_EXPORT vtkSlicerProbeCalibrationLogic :
public vtkSlicerModuleLogic
{

public:

    static vtkSlicerProbeCalibrationLogic *New();
    vtkTypeMacro(vtkSlicerProbeCalibrationLogic, vtkSlicerModuleLogic);
    void PrintSelf(ostream& os, vtkIndent indent);

    /** 
     * \brief add a new transform to the scene resulting of the calibration process 
     */
    void AddTransformToScene(vtkMatrix4x4 * matrix, std::string name);


protected:
    vtkSlicerProbeCalibrationLogic();
    virtual ~vtkSlicerProbeCalibrationLogic();

    virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
    /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
    virtual void RegisterNodes();
    virtual void UpdateFromMRMLScene();
    virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
    virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

    vtkSlicerProbeCalibrationLogic(const vtkSlicerProbeCalibrationLogic&); // Not implemented
    void operator=(const vtkSlicerProbeCalibrationLogic&); // Not implemented
};

#endif
