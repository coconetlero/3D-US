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

#ifndef __qSlicerStructureEnhancerModuleWidget_h
#define __qSlicerStructureEnhancerModuleWidget_h

// itk includes
#include <itkImage.h>

//vtk includes 
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerStructureEnhancerModuleExport.h"

class qSlicerStructureEnhancerModuleWidgetPrivate;
class vtkMRMLNode;

// Image dimension
const unsigned int Dimension = 3;
// Pixel Types
typedef float PixelType;
typedef unsigned char OutputPixelType;
// Image Types 
typedef itk::Image< PixelType, Dimension > InputImageType;
typedef itk::Image< OutputPixelType, Dimension > OutputImageType;


/// \ingroup Slicer_QtModules_ExtensionTemplate

class Q_SLICER_QTMODULES_STRUCTUREENHANCER_EXPORT qSlicerStructureEnhancerModuleWidget :
public qSlicerAbstractModuleWidget
{

    Q_OBJECT

public:

    typedef qSlicerAbstractModuleWidget Superclass;
    qSlicerStructureEnhancerModuleWidget(QWidget *parent = 0);
    virtual ~qSlicerStructureEnhancerModuleWidget();

public slots:

    /**
     * \brief
     */
    void startEnhancement();

protected slots:

    /**
     * \brief
     */
    void onVolumeSelected();


protected:
    QScopedPointer<qSlicerStructureEnhancerModuleWidgetPrivate> d_ptr;

    virtual void setup();

private:
    Q_DECLARE_PRIVATE(qSlicerStructureEnhancerModuleWidget);
    Q_DISABLE_COPY(qSlicerStructureEnhancerModuleWidget);


    /** \brief the input images as an image data */
    vtkSmartPointer<vtkImageData> inputVolume;


    /**
     * \brief
     */
    InputImageType::Pointer toITKImage(vtkImageData * vtkImage);

    /**
     * \brief
     */
    vtkSmartPointer<vtkImageData> toVTKImageData(OutputImageType::Pointer itkImage);

    /**
     * \brief
     */
    InputImageType::Pointer toITKImage(vtkSmartPointer<vtkImageData> vtkImage);


};

#endif
