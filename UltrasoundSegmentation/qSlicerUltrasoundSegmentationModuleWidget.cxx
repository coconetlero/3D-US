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

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerUltrasoundSegmentationModuleWidget.h"
#include "ui_qSlicerUltrasoundSegmentationModuleWidget.h"

// VTK includes
#include <vtkNew.h>
#include <vtkImageShiftScale.h>
#include <vtkImageIterator.h>

// ITK includes
#include <itkRescaleIntensityImageFilter.h>

//MRML includes 
#include <vtkMRMLNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLScalarVolumeNode.h>

// VNL includes
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

// STD includes
#include <ios>

// glue inlcudes
#include "../VtkGlue/include/itkImageToVTKImageFilter.h"
#include "../VtkGlue/include/itkVTKImageToImageFilter.h"

// local imports
#include "HessianFeatures.h"
#include "FeatureRegionGrowing.h"
#include "vtkSlicerUltrasoundSegmentationLogic.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate


class qSlicerUltrasoundSegmentationModuleWidgetPrivate : public Ui_qSlicerUltrasoundSegmentationModuleWidget
{

public:
  qSlicerUltrasoundSegmentationModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerUltrasoundSegmentationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------


qSlicerUltrasoundSegmentationModuleWidgetPrivate::qSlicerUltrasoundSegmentationModuleWidgetPrivate() { }

//-----------------------------------------------------------------------------
// qSlicerUltrasoundSegmentationModuleWidget methods

//-----------------------------------------------------------------------------


qSlicerUltrasoundSegmentationModuleWidget::qSlicerUltrasoundSegmentationModuleWidget(QWidget* _parent)
: Superclass(_parent)
, d_ptr(new qSlicerUltrasoundSegmentationModuleWidgetPrivate) { }

//-----------------------------------------------------------------------------


qSlicerUltrasoundSegmentationModuleWidget::~qSlicerUltrasoundSegmentationModuleWidget() { }

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::setup()
{
  Q_D(qSlicerUltrasoundSegmentationModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // trainning setup
  d->trainingGroupBox->setEnabled(false);

  connect(d->sourceTrainMRMLNodeComboBox,
          SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onSourceTrainSelected()));
  connect(d->boneMRMLNodeComboBox,
          SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onBoneLabelSelected()));
  connect(d->tissueMRMLNodeComboBox,
          SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onTissueLabelSelected()));
  connect(d->shadowMRMLNodeComboBox,
          SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onShadowLabelSelected()));

  // segmentation setup
  connect(d->sourceSegmentMRMLNodeComboBox,
          SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onSourceSegmentSelected()));


  //this->m_stream.open ("us_seg_log.txt", std::ios_base::app | std::ios_base::out);
  //this->m_stream << "\n --- Start Ultrasound segmentation debug --- " << std::endl;

}

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::enableTraining()
{
  Q_D(qSlicerUltrasoundSegmentationModuleWidget);
  bool useFile = d->useFileCheckBox->isChecked();
  std::cout << "use file " << useFile << std::endl;
  if (useFile)
    {
      d->trainingGroupBox->setEnabled(false);
    }
  else
    {
      d->trainingGroupBox->setEnabled(true);
      d->sourceTrainMRMLNodeComboBox->setEnabled(true);
      d->boneMRMLNodeComboBox->setEnabled(true);
      d->tissueMRMLNodeComboBox->setEnabled(true);
      d->shadowMRMLNodeComboBox->setEnabled(true);
      d->loadFileButton->setEnabled(false);
      d->fileLineEdit->setEnabled(false);
    }
}

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::loadTrainingFile() { }

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::saveTraining() { }

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::train()
{
  Q_D(qSlicerUltrasoundSegmentationModuleWidget);

  std::cout << "loading inputs into scene" << std::endl;

  vtkNew<vtkImageData> sourceVolume;
  vtkSmartPointer<vtkImageData> boneVolume;
  vtkSmartPointer<vtkImageData> tissueVolume;
  vtkSmartPointer<vtkImageData> shadowVolume;


  // get inputs
  vtkMRMLNode *sourceNode = d->sourceTrainMRMLNodeComboBox->currentNode();
  vtkMRMLNode *boneNode = d->boneMRMLNodeComboBox->currentNode();
  vtkMRMLNode *tissueNode = d->tissueMRMLNodeComboBox->currentNode();
  vtkMRMLNode *shadowNode = d->shadowMRMLNodeComboBox->currentNode();

  if (sourceNode)
    {
      vtkMRMLVolumeNode *inputVolumeNode = vtkMRMLVolumeNode::SafeDownCast(sourceNode);
      if (inputVolumeNode)
        {
          sourceVolume->DeepCopy(inputVolumeNode->GetImageData());
        }
    }
  if (boneNode)
    {
      vtkMRMLVolumeNode *inputVolumeNode = vtkMRMLVolumeNode::SafeDownCast(boneNode);
      if (inputVolumeNode)
        {
          boneVolume = inputVolumeNode->GetImageData();
        }
    }
  if (tissueNode)
    {
      vtkMRMLVolumeNode *inputVolumeNode = vtkMRMLVolumeNode::SafeDownCast(tissueNode);
      if (inputVolumeNode)
        {
          tissueVolume = inputVolumeNode->GetImageData();
        }
    }
  if (shadowNode)
    {
      vtkMRMLVolumeNode *inputVolumeNode = vtkMRMLVolumeNode::SafeDownCast(shadowNode);
      if (inputVolumeNode)
        {
          shadowVolume = inputVolumeNode->GetImageData();
        }
    }


  vtkNew<vtkImageShiftScale> shiftScaleFilter;
  shiftScaleFilter->SetOutputScalarTypeToFloat();
#if VTK_MAJOR_VERSION <= 5
  shiftScaleFilter->SetInput(sourceVolume.GetPointer());
#else
  shiftScaleFilter->SetInputData(sourceVolume.GetPointer());
#endif
  shiftScaleFilter->Update();

  FloatImageType::Pointer sourceImage = this->toITKImage(shiftScaleFilter->GetOutput());

  // input data
  float sigma_min = d->minSpinBox->value();
  float sigma_max = d->maxSpinBox->value();
  int scaleSteps = d->scalesSpinBox->value();
  float alpha = d->alphaSpinBox->value();
  float beta = d->betaSpinBox->value();
  float gamma = d->gammaSpinBox->value();
//  bool isWhite = d->isBrightCheckBox->isChecked();

  // enhance image     
  HessianFeatures hessianFeatures;
  hessianFeatures.SetSigmaMin(sigma_min);
  hessianFeatures.SetSigmaMax(sigma_max);
  hessianFeatures.SetSigmaStepMethodToEquispaced();
  hessianFeatures.SetAlpha(alpha);
  hessianFeatures.SetBeta(beta);
  hessianFeatures.SetGamma(gamma);
  hessianFeatures.SetImage(sourceImage);


  std::cout << " <- start enhancement -> " << std::endl;

  FloatImageType::Pointer objectness = FloatImageType::New();
  if (d->sheetRadioButton->isChecked())
    objectness = hessianFeatures.MultiscaleSurfaceness(scaleSteps);
  else if (d->tubeRadioButton->isChecked())
    objectness = hessianFeatures.MultiscaleTubeness(scaleSteps);
  else if (d->blobRadioButton->isChecked())
    objectness = hessianFeatures.MultiscaleSurfaceness(scaleSteps);


  // convert to vtkImage
  vtkSmartPointer<vtkImageData> enhancedVolume = toVTKImageData(objectness);
  std::cout << "conversion to vtk DONE " << std::endl;






  std::cout << "find trainning values" << std::endl;

  int boneSize = 0;
  int tissueSize = 0;
  int shadowSize = 0;



  int *dims = sourceVolume->GetDimensions();

  for (int z = 0; z < dims[2]; z++)
    {
      for (int y = 0; y < dims[1]; y++)
        {
          for (int x = 0; x < dims[0]; x++)
            {
              unsigned char * bonePixel = static_cast<unsigned char *>
                      (boneVolume->GetScalarPointer(x, y, z));
              unsigned char * tissuePixel = static_cast<unsigned char *>
                      (tissueVolume->GetScalarPointer(x, y, z));
              unsigned char * shadowPixel = static_cast<unsigned char *>
                      (shadowVolume->GetScalarPointer(x, y, z));

              if (bonePixel[0] != 0)
                ++boneSize;
              if (tissuePixel[0] != 0)
                ++tissueSize;
              if (shadowPixel[0] != 0)
                ++shadowSize;
            }
        }
    }


  this->P_bone = (double) boneSize / (double) (dims[0] * dims[1] * dims[2]);
  this->P_tissue = (double) tissueSize / (double) (dims[0] * dims[1] * dims[2]);
  this->P_shadow = (double) shadowSize / (double) (dims[0] * dims[1] * dims[2]);



  std::cout << "label sizes: b = " << boneSize << ", t = " << tissueSize << ", s = " << shadowSize << std::endl;
  std::cout << "A priori probabilities: Pb = " << P_bone << ", Pt = " << P_tissue << ", Ps = " << P_shadow << std::endl;


  std::cout << "build observations matrix" << std::endl;

  int bIdx = 0;
  int tIdx = 0;
  int sIdx = 0;
  //        vnl_matrix<float> boneObs(boneSize, 4);
  //        vnl_matrix<float> tissueObs(tissueSize, 4);
  //        vnl_matrix<float> shadowObs(shadowSize, 4);

  vnl_matrix<float> boneObs(boneSize, 2);
  vnl_matrix<float> tissueObs(tissueSize, 2);
  vnl_matrix<float> shadowObs(shadowSize, 2);



  for (int z = 0; z < dims[2]; z++)
    {
      for (int y = 0; y < dims[1]; y++)
        {
          for (int x = 0; x < dims[0]; x++)
            {
              unsigned char * sourcePixel = static_cast<unsigned char *> (sourceVolume->GetScalarPointer(x, y, z));
              float * enhancedPixel = static_cast<float *> (enhancedVolume->GetScalarPointer(x, y, z));

              unsigned char * bonePixel = static_cast<unsigned char *> (boneVolume->GetScalarPointer(x, y, z));
              unsigned char * tissuePixel = static_cast<unsigned char *> (tissueVolume->GetScalarPointer(x, y, z));
              unsigned char * shadowPixel = static_cast<unsigned char *> (shadowVolume->GetScalarPointer(x, y, z));

              if (bonePixel[0] != 0)
                {
                  //                                        float v[] = {x, y, z, sourcePixel[0]};
                  //                                        vnl_vector<float> row(4, 4, v);

                  float v[] = {sourcePixel[0], enhancedPixel[0]};
                  vnl_vector<float> row(2, 2, v);

                  boneObs.set_row(bIdx, row);
                  ++bIdx;
                }
              if (tissuePixel[0] != 0)
                {
                  //                                        float v[] = {x, y, z, sourcePixel[0]};
                  //                                        vnl_vector<float> row(4, 4, v);

                  float v[] = {sourcePixel[0], enhancedPixel[0]};
                  vnl_vector<float> row(2, 2, v);

                  tissueObs.set_row(tIdx, row);
                  ++tIdx;
                }
              if (shadowPixel[0] != 0)
                {
                  //                                        float v[] = {x, y, z, sourcePixel[0]};
                  //                                        vnl_vector<float> row(4, 4, v);

                  float v[] = {sourcePixel[0], enhancedPixel[0]};
                  vnl_vector<float> row(2, 2, v);

                  shadowObs.set_row(sIdx, row);
                  ++sIdx;
                }
            }
        }
    }

  std::cout << "finish to fill" << std::endl;

  this->BoneMean.set_size(boneObs.cols());
  this->TissueMean.set_size(boneObs.cols());
  this->ShadowMean.set_size(boneObs.cols());

  for (unsigned int i = 0; i < boneObs.cols(); i++)
    {
      this->BoneMean(i) = boneObs.get_column(i).mean();
      this->TissueMean(i) = tissueObs.get_column(i).mean();
      this->ShadowMean(i) = shadowObs.get_column(i).mean();
    }

  std::cout << "bone mean \n" << BoneMean << std::endl;
  std::cout << "Tissue mean \n" << TissueMean << std::endl;
  std::cout << "shadow mean \n" << ShadowMean << std::endl;

  this->BoneCov = this->cov(boneObs);
  this->TissueCov = this->cov(tissueObs);
  this->ShadowCov = this->cov(shadowObs);

  std::cout << "Tranning Done" << std::endl;

}

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::onSourceSegmentSelected() { }

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::onSourceTrainSelected() { }

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::onBoneLabelSelected() { }

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::onTissueLabelSelected() { }

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::onShadowLabelSelected() { }

//-----------------------------------------------------------------------------


void qSlicerUltrasoundSegmentationModuleWidget::segment()
{
  Q_D(qSlicerUltrasoundSegmentationModuleWidget);

  vtkNew<vtkImageData> inputVolume; 

  // get input volume 
  vtkMRMLNode *node = d->sourceSegmentMRMLNodeComboBox->currentNode();
  vtkMRMLVolumeNode *inputVolumeNode;
  if (node)
    {
      inputVolumeNode = vtkMRMLVolumeNode::SafeDownCast(node);
      if (inputVolumeNode)
        {
          std::cout << "loading input volume in scene" << std::endl;
          this->InputSegmentationNode = inputVolumeNode;
          inputVolume->DeepCopy(inputVolumeNode->GetImageData());
        }
    }

  vtkNew<vtkImageShiftScale> shiftScaleFilter;
  shiftScaleFilter->SetOutputScalarTypeToFloat();

#if VTK_MAJOR_VERSION <= 5
  shiftScaleFilter->SetInput(inputVolume.GetPointer());
#else
  shiftScaleFilter->SetInputData(inputVolume.GetPointer());
#endif
  shiftScaleFilter->Update();

  this->InputImage = this->toITKImage(shiftScaleFilter->GetOutput());

  // input data
  float sigma_min = d->minSpinBox->value();
  float sigma_max = d->maxSpinBox->value();
  int scaleSteps = d->scalesSpinBox->value();
  float alpha = d->alphaSpinBox->value();
  float beta = d->betaSpinBox->value();
  float gamma = d->gammaSpinBox->value();
//  bool isWhite = d->isBrightCheckBox->isChecked();

  // enhance image     
  HessianFeatures hessianFeatures;
  hessianFeatures.SetSigmaMin(sigma_min);
  hessianFeatures.SetSigmaMax(sigma_max);
  hessianFeatures.SetSigmaStepMethodToEquispaced();
  hessianFeatures.SetAlpha(alpha);
  hessianFeatures.SetBeta(beta);
  hessianFeatures.SetGamma(gamma);
  hessianFeatures.SetImage(this->InputImage);


  // -------------------------- start mesuring time -------------------------
  std::cout << "start counting time " << std::endl;
  clock_t begin = clock();
  // ------------------------------------------------------------------------

  std::cout << " <- start enhancement -> " << std::endl;

  FloatImageType::Pointer objectness = FloatImageType::New();
  if (d->sheetRadioButton->isChecked())
    objectness = hessianFeatures.MultiscaleSurfaceness(scaleSteps);
  else if (d->tubeRadioButton->isChecked())
    objectness = hessianFeatures.MultiscaleTubeness(scaleSteps);
  else if (d->blobRadioButton->isChecked())
    objectness = hessianFeatures.MultiscaleSurfaceness(scaleSteps);


  // convert to vtkImage
  vtkSmartPointer<vtkImageData> outputVolume = toVTKImageData(objectness);
  std::cout << "conversion to vtk DONE " << std::endl;

  // -----------------------   count partial time   --------------------------
  clock_t partial = clock();
  double diffticks = partial - begin;
  double diffms = diffticks / CLOCKS_PER_SEC;
  std::cout << "Time elapsed in surface enhancement: " << double(diffms) << " seconds" << std::endl;
  // -----------------------   count partial time   --------------------------

  // create an output volume node
  vtkNew<vtkMRMLScalarVolumeNode> enhancedVolumeNode;
  enhancedVolumeNode->SetOrigin(this->InputSegmentationNode->GetOrigin());
  enhancedVolumeNode->SetSpacing(this->InputSegmentationNode->GetSpacing());
  enhancedVolumeNode->SetAndObserveImageData(outputVolume);

  vtkSlicerUltrasoundSegmentationLogic * logic =
          vtkSlicerUltrasoundSegmentationLogic::SafeDownCast(this->logic());
  if (logic)
    {
      logic->AddAndDisplayVolume(inputVolumeNode, enhancedVolumeNode.GetPointer());
    }

  std::cout << "Enhancement Done " << std::endl;


  std::cout << "start region growing" << std::endl;
  FeatureRegionGrowing regionGrow;
  //    regionGrow.SetImage(objectness);
  //    regionGrow.SetImage(this->InputImage);
  regionGrow.SetImage(this->InputImage);
  regionGrow.SetEnhancedImage(objectness);
  regionGrow.SetEigenvectors(hessianFeatures.GetEigenvectors());
  regionGrow.SetEigenvalues(hessianFeatures.GetEigenvalues());

  std::vector<int> seeds = regionGrow.FindSeedPoints();
  MaskImageType::Pointer seeds_Image = regionGrow.SeedPointsToImage(seeds);

  MaskImageType::Pointer grow_Image = regionGrow.Grow(seeds_Image,
                                                      this->P_bone, this->P_tissue, this->P_shadow,
                                                      this->BoneMean, this->TissueMean, this->ShadowMean,
                                                      this->BoneCov, this->TissueCov, this->ShadowCov);

  // convert to vtkImage
  vtkSmartPointer<vtkImageData> segmentVolume = toVTKMaskImageData(grow_Image);
  std::cout << "conversion to mask vtk DONE " << std::endl;

  // create an output volume node
  vtkNew<vtkMRMLScalarVolumeNode> segmentVolumeNode;
  segmentVolumeNode->SetOrigin(this->InputSegmentationNode->GetOrigin());
  segmentVolumeNode->SetSpacing(this->InputSegmentationNode->GetSpacing());
  segmentVolumeNode->SetAndObserveImageData(segmentVolume);


  if (logic)
    {
      logic->AddAndDisplayLabelVolume(inputVolumeNode, segmentVolumeNode.GetPointer());
    }

  std::cout << "Segmentation Done " << std::endl;

}




//-----------------------------------------------------------------------------


vtkSmartPointer<vtkImageData> qSlicerUltrasoundSegmentationModuleWidget
::toVTKImageData(FloatImageType::Pointer itkImage)
{
  typedef itk::ImageToVTKImageFilter <FloatImageType> vtkConnectorType;
  vtkConnectorType::Pointer vtkConnector = vtkConnectorType::New();
  vtkConnector->GetExporter()->SetInput(itkImage);
  vtkConnector->GetImporter()->Update();

  vtkSmartPointer<vtkImageData> outputImage = vtkSmartPointer<vtkImageData>::New();
  outputImage->Initialize();
  outputImage->DeepCopy(vtkConnector->GetImporter()->GetOutput());
  //    outputImage->Update();

  return outputImage;
}

//-----------------------------------------------------------------------------


vtkSmartPointer<vtkImageData> qSlicerUltrasoundSegmentationModuleWidget
::toVTKMaskImageData(MaskImageType::Pointer itkImage)
{
  typedef itk::ImageToVTKImageFilter <MaskImageType> vtkConnectorType;
  vtkConnectorType::Pointer vtkConnector = vtkConnectorType::New();
  vtkConnector->GetExporter()->SetInput(itkImage);
  vtkConnector->GetImporter()->Update();

  vtkSmartPointer<vtkImageData> outputImage = vtkSmartPointer<vtkImageData>::New();
  outputImage->Initialize();
  outputImage->DeepCopy(vtkConnector->GetImporter()->GetOutput());
  //    outputImage->Update();

  return outputImage;
}


//-----------------------------------------------------------------------------


FloatImageType::Pointer qSlicerUltrasoundSegmentationModuleWidget
::toITKImage(vtkSmartPointer<vtkImageData> vtkImage)
{
  typedef itk::VTKImageToImageFilter <FloatImageType> ITKConnectorType;
  ITKConnectorType::Pointer itkConnector = ITKConnectorType::New();
  itkConnector->SetInput(vtkImage);
  itkConnector->Update();

  FloatImageType::Pointer itkImage = FloatImageType::New();
  itkImage->Graft(itkConnector->GetOutput());
  itkConnector = NULL;

  return itkImage;
}


//-----------------------------------------------------------------------------


vnl_matrix<float> qSlicerUltrasoundSegmentationModuleWidget::cov(vnl_matrix<float> observations)
{
  int obsSize = observations.rows();
  std::cout << "size: " << obsSize << std::endl;
  vnl_matrix<float> ones(obsSize, 1);
  ones.fill(1.0);
  vnl_matrix<float> dev = observations - ((ones * ones.transpose()) * (observations / obsSize));
  vnl_matrix<float> cov = (dev.transpose() * dev) / (observations.rows() - 1);


  std::cout << "bone cov \n" << cov << std::endl;

  return cov;
}



