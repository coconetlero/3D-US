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

#ifndef __qSlicerGraphCutSegmentationModuleWidget_h
#define __qSlicerGraphCutSegmentationModuleWidget_h

// itk includes
#include <itkImage.h>

// vtk includes
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerGraphCutSegmentationModuleExport.h"

class qSlicerGraphCutSegmentationModuleWidgetPrivate;
class vtkMRMLNode;

//// ITK typedefs
//// Image dimension
const unsigned int Dimension = 3;
// Pixel Types
//typedef unsigned char PixelType;
typedef unsigned char PixelType;
// Image Types 
typedef itk::Image< PixelType, Dimension > ImageType;
//


/// \ingroup Slicer_QtModules_ExtensionTemplate

class Q_SLICER_QTMODULES_GRAPHCUTSEGMENTATION_EXPORT qSlicerGraphCutSegmentationModuleWidget :
public qSlicerAbstractModuleWidget
{

    Q_OBJECT

public:

    typedef qSlicerAbstractModuleWidget Superclass;
    qSlicerGraphCutSegmentationModuleWidget(QWidget *parent = 0);
    virtual ~qSlicerGraphCutSegmentationModuleWidget();

public slots:

    /**
     * \breif
     */
    void segment();


protected:
    QScopedPointer<qSlicerGraphCutSegmentationModuleWidgetPrivate> d_ptr;

    virtual void setup();

protected slots:

    void onInputVolumeSelected();

    void onObjectVolumeSelected();

    void onBkgVolumeSelected();

private:
    Q_DECLARE_PRIVATE(qSlicerGraphCutSegmentationModuleWidget);
    Q_DISABLE_COPY(qSlicerGraphCutSegmentationModuleWidget);




    /**
     * \brief
     */
    vtkSmartPointer<vtkImageData> toVTKImageData(ImageType::Pointer itkImage);

    /**
     * \brief
     */
    ImageType::Pointer toITKImage(vtkSmartPointer<vtkImageData> vtkImage);

};

#endif
