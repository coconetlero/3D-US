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

#ifndef __qSlicerProbeCalibrationModuleWidget_h
#define __qSlicerProbeCalibrationModuleWidget_h

// vtk includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkCollection.h>

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerProbeCalibrationModuleExport.h"

// Other includes 
#include <vnl/vnl_matrix.h>

class qSlicerProbeCalibrationModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate

class Q_SLICER_QTMODULES_PROBECALIBRATION_EXPORT qSlicerProbeCalibrationModuleWidget :
public qSlicerAbstractModuleWidget
{

    Q_OBJECT

public:

    typedef qSlicerAbstractModuleWidget Superclass;
    qSlicerProbeCalibrationModuleWidget(QWidget *parent = 0);
    virtual ~qSlicerProbeCalibrationModuleWidget();

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
    void calibrateUSProbe();

protected slots:

    /**
     * \brief
     */
    void onFiducialListSelected();

    /**
     * \brief
     */
    void onVolumeSelected();


protected:
    QScopedPointer<qSlicerProbeCalibrationModuleWidgetPrivate> d_ptr;

    virtual void setup();

private:
    Q_DECLARE_PRIVATE(qSlicerProbeCalibrationModuleWidget);
    Q_DISABLE_COPY(qSlicerProbeCalibrationModuleWidget);
    
    /** \brief a vnl_matrix to store the translations of each image given by the tracker */
    vnl_matrix<double> translations;

    /** \brief a vnl_matrix to store the rotations of each image given by the tracker */
    vnl_matrix<double> rotations;   
    
    /** \brief a vnl_matrix to store the selected coordinates on each image */
    vnl_matrix<int> coords;
    
    /** \brief the input images as an image data */
    vtkSmartPointer<vtkImageData> images;
    
    /** \brief the fiducial nodes added by the user */
    vtkSmartPointer <vtkCollection> fiducialsList;
    
    
    /** number of frames into the image sequence */
    int nFrames;
};

#endif
