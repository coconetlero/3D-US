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

#ifndef __qSlicerVolumeReconstructionModuleWidget_h
#define __qSlicerVolumeReconstructionModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerVolumeReconstructionModuleExport.h"

// vtk includes 
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>

// Other includes 
#include <vnl/vnl_matrix.h>

class qSlicerVolumeReconstructionModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate

class Q_SLICER_QTMODULES_VOLUMERECONSTRUCTION_EXPORT qSlicerVolumeReconstructionModuleWidget :
public qSlicerAbstractModuleWidget
{

    Q_OBJECT

public:

    typedef qSlicerAbstractModuleWidget Superclass;
    qSlicerVolumeReconstructionModuleWidget(QWidget *parent = 0);
    virtual ~qSlicerVolumeReconstructionModuleWidget();

public slots:

    /**
     * \brief Render file chooser to select the rotations file, loads and create 
     * a local variable rotations
     */
    void loadRotationsFile();

    /**
     * \brief Render file chooser to select the translations file, loads and create 
     * a local variable translations
     */
    void loadTranslationsFile();

    /**
     * Action for calibrate the ultrasound probe 
     */
    void generateReconstruction();


protected slots:

    /**
     * \brief
     */
    void onImagesSelected();

    /**
     * \brief
     */
    void onTransformationSelected();


protected:
    QScopedPointer<qSlicerVolumeReconstructionModuleWidgetPrivate> d_ptr;

    virtual void setup();

private:
    Q_DECLARE_PRIVATE(qSlicerVolumeReconstructionModuleWidget);
    Q_DISABLE_COPY(qSlicerVolumeReconstructionModuleWidget);

    /** \brief a vnl_matrix to store the translations of each image given by the tracker */
    vnl_matrix<float> translations;

    /** \brief a vnl_matrix to store the rotations of each image given by the tracker */
    vnl_matrix<float> rotations;

    /** \brief the input images as an image data */
    vtkSmartPointer<vtkImageData> images;

    /** \brief the calibration transform required to generate the volume*/
    vtkSmartPointer<vtkMatrix4x4> rTp;

    /** \brief image dimensions */
    int * imageDimensions;

    /**
     * \brief given the 
     * @param width
     * @param height
     * @return 
     */
    int * findVolumeBounds(int imageWidth, int imageHeight, int imageSize,
            float xScale, float yScale, float zScale, float volumeScale);
    /**
     * 
     * @param volume
     * @param imageWidth
     * @param imageHeight
     * @param imageSize
     * @param xScale
     * @param yScale
     * @param zScale
     * @param volumeScale
     */
    void BinFilling(vtkImageData * volume, vtkImageData * filledVolume,
            int imageWidth, int imageHeight, int imageSize,
            float xScale, float yScale, float zScale, float volumeScale);


    /**
     * 
     * @param volume
     * @param windowSize
     */
    void HoleFilling(vtkImageData * volume, vtkImageData * filledVolume, int windowSize);

    /**
     * 
     * @param xScale
     * @param yScale
     * @param zScale
     * @param volumeScale
     * @return 
     */
    vtkMatrix4x4 * getVolumeTransformation(float xScale, float yScale, float zScale,
            float volumeScale);

};

#endif
