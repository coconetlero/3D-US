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

#ifndef __qSlicerICPRegistrationModuleWidget_h
#define __qSlicerICPRegistrationModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerICPRegistrationModuleExport.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

//MRML includes 
#include <vtkMRMLModelNode.h>
#include <vtkMRMLLinearTransformNode.h>


class qSlicerICPRegistrationModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate

class Q_SLICER_QTMODULES_ICPREGISTRATION_EXPORT qSlicerICPRegistrationModuleWidget :
public qSlicerAbstractModuleWidget
{

    Q_OBJECT

public:

    typedef qSlicerAbstractModuleWidget Superclass;
    qSlicerICPRegistrationModuleWidget(QWidget *parent = 0);
    virtual ~qSlicerICPRegistrationModuleWidget();   

public slots:
    
    void onModelRegistration();


protected:
    QScopedPointer<qSlicerICPRegistrationModuleWidgetPrivate> d_ptr;

    virtual void setup();
    
protected slots:
    
    void onInputModelSelected();

    void onTarjetModelSelected();

    void onOutputModelSelected();

private:
    Q_DECLARE_PRIVATE(qSlicerICPRegistrationModuleWidget);
    Q_DISABLE_COPY(qSlicerICPRegistrationModuleWidget);
    
    
    // registration models 
    vtkSmartPointer<vtkPolyData> Target;
    vtkSmartPointer<vtkPolyData> Moving;
    
    /** Moving model node */
    vtkSmartPointer<vtkMRMLModelNode> MovingNode;
    
    /** Generated transformation after registration */
    vtkSmartPointer<vtkMRMLLinearTransformNode> TransformationNode;
    
    
};

#endif
