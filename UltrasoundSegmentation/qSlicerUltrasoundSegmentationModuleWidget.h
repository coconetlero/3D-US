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

#ifndef __qSlicerUltrasoundSegmentationModuleWidget_h
#define __qSlicerUltrasoundSegmentationModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerUltrasoundSegmentationModuleExport.h"

#include <UltrasoundSegmentationDefs.h>

// ITK includes
#include <itkImage.h>

// VTK includes 
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

// MRML includes 
#include <vtkMRMLVolumeNode.h>

// VNL includes
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

// STD includes
#include <iostream>
#include <fstream>


class qSlicerUltrasoundSegmentationModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate

class Q_SLICER_QTMODULES_ULTRASOUNDSEGMENTATION_EXPORT qSlicerUltrasoundSegmentationModuleWidget :
public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerUltrasoundSegmentationModuleWidget(QWidget *parent = 0);
  virtual ~qSlicerUltrasoundSegmentationModuleWidget();

public slots:

  /** \brief */
  void enableTraining();
  /** \brief */
  void loadTrainingFile();
  /** \brief */
  void train();
  /** \brief */
  void saveTraining();
  /** \brief */
  void segment();


protected:
  QScopedPointer<qSlicerUltrasoundSegmentationModuleWidgetPrivate> d_ptr;

  virtual void setup();

protected slots:

  /** \brief */
  void onSourceSegmentSelected();

  void onSourceTrainSelected();

  void onBoneLabelSelected();

  void onTissueLabelSelected();

  void onShadowLabelSelected();


private:
  Q_DECLARE_PRIVATE(qSlicerUltrasoundSegmentationModuleWidget);
  Q_DISABLE_COPY(qSlicerUltrasoundSegmentationModuleWidget);

  /** \brief input segmentation volume node */
  vtkSmartPointer<vtkMRMLVolumeNode> InputSegmentationNode;

  /** \brief the input images as an image data */
  vtkSmartPointer<vtkImageData> InputVolume;

  /** \brief the input images as ITK image */
  FloatImageType::Pointer InputImage;


  // -------------------------------------------------

  float P_bone;
  float P_tissue;
  float P_shadow;

  vnl_vector<float> BoneMean;
  vnl_vector<float> TissueMean;
  vnl_vector<float> ShadowMean;

  vnl_matrix<float> BoneCov;
  vnl_matrix<float> TissueCov;
  vnl_matrix<float> ShadowCov;


  // -------------------------------------------------


  /**
   * \brief   
   */
  vtkSmartPointer<vtkImageData> toVTKImageData(FloatImageType::Pointer itkImage);

  /**
   * \brief   
   */
  vtkSmartPointer<vtkImageData> toVTKMaskImageData(MaskImageType::Pointer itkImage);

  /**
   * \brief
   */
  FloatImageType::Pointer toITKImage(vtkSmartPointer<vtkImageData> vtkImage);

  /**
   * \brief finds the covariance matrix
   */
  vnl_matrix<float> cov(vnl_matrix<float> observations);

  /**
   * \brief
   * @param itkImage
   * @param sigma_min
   * @param sigma_max
   * @param alpha
   * @param beta
   * @param gamma
   * @param scaleSteps
   * @param isWhite
   * @return 
   */
  FloatImageType::Pointer enhance(FloatImageType::Pointer itkImage,
                                  int type, float sigma_min, float sigma_max, float alpha, float beta, float gamma,
                                  int scaleSteps, bool isWhite);
  
  // stream for write logs messages
  std::ofstream m_stream;

};

#endif
