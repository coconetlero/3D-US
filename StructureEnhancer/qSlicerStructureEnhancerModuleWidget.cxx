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

// STD includes
#include <vector>
#include <cmath>
#include <float.h>
#include <time.h>

// Qt includes
#include <QDebug>
#include <QtGui>

// Slicer includes
#include <vtkMRMLNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLScalarVolumeNode.h>

// ITK Includes
#include <itkImage.h> 
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageRegionConstIterator.h"

#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkHessianToObjectnessMeasureImageFilter.h"

// glue inlcudes
#include "../VtkGlue/include/itkImageToVTKImageFilter.h"
#include "../VtkGlue/include/itkVTKImageToImageFilter.h"

// vtk includes
#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>


// SlicerQt includes
#include "qSlicerStructureEnhancerModuleWidget.h"
#include "ui_qSlicerStructureEnhancerModuleWidget.h"
#include "vtkSlicerStructureEnhancerLogic.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate


class qSlicerStructureEnhancerModuleWidgetPrivate : public Ui_qSlicerStructureEnhancerModuleWidget
{

public:
  qSlicerStructureEnhancerModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerStructureEnhancerModuleWidgetPrivate methods

//-----------------------------------------------------------------------------


qSlicerStructureEnhancerModuleWidgetPrivate::qSlicerStructureEnhancerModuleWidgetPrivate() { }

//-----------------------------------------------------------------------------
// qSlicerStructureEnhancerModuleWidget methods

//-----------------------------------------------------------------------------


qSlicerStructureEnhancerModuleWidget::qSlicerStructureEnhancerModuleWidget(QWidget* _parent)
: Superclass(_parent)
, d_ptr(new qSlicerStructureEnhancerModuleWidgetPrivate) { }

//-----------------------------------------------------------------------------


qSlicerStructureEnhancerModuleWidget::~qSlicerStructureEnhancerModuleWidget() { }

//-----------------------------------------------------------------------------


void qSlicerStructureEnhancerModuleWidget::setup()
{
  Q_D(qSlicerStructureEnhancerModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  d->inputVolMRMLNodeComboBox->setNoneEnabled(true);
  connect(d->inputVolMRMLNodeComboBox,
          SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onVolumeSelected()));
}


//-----------------------------------------------------------------------------


void qSlicerStructureEnhancerModuleWidget::onVolumeSelected()
{
  Q_D(qSlicerStructureEnhancerModuleWidget);

  vtkMRMLNode *node = d->inputVolMRMLNodeComboBox->currentNode();
  if (!node)
    {
      return;
    }

  vtkMRMLVolumeNode *inputVolumeNode = vtkMRMLVolumeNode::SafeDownCast(node);

  if (inputVolumeNode)
    {
      //        std::cout << "loading input volume in scene" << std::endl;
      //
      //        this->inputVolume = inputVolumeNode->GetImageData();
      //        int *dimensions = inputVolume->GetDimensions();
      //
      //        std::cout << "dimensions: (" <<
      //                dimensions[0] << ", " <<
      //                dimensions[1] << ",  " <<
      //                dimensions[2] << ")" << std::endl;
    }
}


//-----------------------------------------------------------------------------


void qSlicerStructureEnhancerModuleWidget::startEnhancement()
{

  Q_D(qSlicerStructureEnhancerModuleWidget);
  // input data
  float minScale = d->minDoubleSpinBox->value();
  float maxScale = d->maxDoubleSpinBox->value();
  int scaleSteps = d->stepSpinBox->value();
  float alpha = d->alphaDoubleSpinBox->value();
  float beta = d->betaDoubleSpinBox->value();
  float gamma = d->gammaDoubleSpinBox->value();
  bool isWhite = d->brightCheckBox->isChecked();



  int structure = 0;
  if (d->sheetRadioButton->isChecked())
    structure = 1;
  else if (d->tubeRadioButton->isChecked())
    structure = 2;
  else if (d->blobRadioButton->isChecked())
    structure = 3;

  std::cout << " get volume from selected node " << std::endl;
  vtkMRMLNode *inputNode = d->inputVolMRMLNodeComboBox->currentNode();
  if (!inputNode)
    {
      return;
    }
  vtkMRMLVolumeNode *inputVolumeNode = vtkMRMLVolumeNode::SafeDownCast(inputNode);

  double * spacing = inputVolumeNode->GetSpacing();
  std::cout << "volume spacing = " << spacing[0] << "," << spacing[1] << "," << spacing[2] << std::endl;

  double * origin = inputVolumeNode->GetOrigin();
  std::cout << "volume origin = " << origin[0] << "," << origin[1] << "," << origin[2] << std::endl;

  vtkNew<vtkImageShiftScale> shiftScaleFilter;
  shiftScaleFilter->SetOutputScalarTypeToFloat();
  shiftScaleFilter->SetInputData(inputVolumeNode->GetImageData());

  

  shiftScaleFilter->Update();

  this->inputVolume = shiftScaleFilter->GetOutput();

  int *dimensions = inputVolume->GetDimensions();

  std::cout << "dimensions: (" <<
          dimensions[0] << ", " <<
          dimensions[1] << ", " <<
          dimensions[2] << ")" << std::endl;

  std::cout << " <- start enhancement -> " << std::endl;

  // ------------------------------------------------------------------------

  // start mesuring time 
  std::cout << "start counting time " << std::endl;
  clock_t begin = clock();

  // ------------------------------------------------------------------------

  // Data Types
  typedef itk::SymmetricSecondRankTensor< PixelType, Dimension > TensorPixelType;

  // Image Types 
  typedef itk::Image< TensorPixelType, Dimension > TensorImageType;

  // convert to itkImage from vtkImageData
  InputImageType::Pointer InputImage = toITKImage(inputVolume);






  typedef itk::HessianToObjectnessMeasureImageFilter< TensorImageType, InputImageType > ObjectnessFilterType;
  ObjectnessFilterType::Pointer objectnessFilter = ObjectnessFilterType::New();
  objectnessFilter->SetScaleObjectnessMeasure(true);
  objectnessFilter->SetBrightObject(isWhite);
  objectnessFilter->SetAlpha(alpha);
  objectnessFilter->SetBeta(beta);
  objectnessFilter->SetGamma(gamma);
  switch (structure)
    {
    case 1: // surface
      objectnessFilter->SetObjectDimension(2);
      break;
    case 2: // line              
      objectnessFilter->SetObjectDimension(1);
      break;
    case 3: // blob
      objectnessFilter->SetObjectDimension(0);
      break;
    }

  typedef itk::MultiScaleHessianBasedMeasureImageFilter< InputImageType, TensorImageType, InputImageType >
          MultiScaleHessianFilterType;
  MultiScaleHessianFilterType::Pointer multiScaleHessianFilterFilter =
          MultiScaleHessianFilterType::New();

  multiScaleHessianFilterFilter->SetInput(InputImage);
  multiScaleHessianFilterFilter->SetHessianToMeasureFilter(objectnessFilter);
  multiScaleHessianFilterFilter->SetSigmaStepMethodToEquispaced();
  multiScaleHessianFilterFilter->SetSigmaMinimum(minScale);
  multiScaleHessianFilterFilter->SetSigmaMaximum(maxScale);
  multiScaleHessianFilterFilter->SetNumberOfSigmaSteps(scaleSteps);






  // -------------------------   final process   ----------------------------

  // convert to vtkImage
  typedef itk::RescaleIntensityImageFilter<InputImageType, OutputImageType> RescaleFilterType;
  RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
  //    rescaleFilter->SetInput(maxImage);
  rescaleFilter->SetInput(multiScaleHessianFilterFilter->GetOutput());
  rescaleFilter->SetOutputMinimum(0);
  rescaleFilter->SetOutputMaximum(255);
  rescaleFilter->Update();

  vtkSmartPointer<vtkImageData> outputVolume = toVTKImageData(rescaleFilter->GetOutput());
  std::cout << "conversion to vtk DONE " << std::endl;



  // -----------------------   count partial time   --------------------------
  clock_t partial = clock();
  double diffticks = partial - begin;
  double diffms = diffticks / CLOCKS_PER_SEC;
  std::cout << "Time elapsed in surface enhancement: " << double(diffms) << " seconds" << std::endl;
  // -----------------------   count partial time   --------------------------


  // create an output volume node
  vtkNew<vtkMRMLScalarVolumeNode> outputVolumeNode;
  outputVolumeNode->SetOrigin(origin);
  outputVolumeNode->SetSpacing(spacing);
  outputVolumeNode->SetAndObserveImageData(outputVolume);

  vtkSlicerStructureEnhancerLogic * logic =
          vtkSlicerStructureEnhancerLogic::SafeDownCast(this->logic());

  if (logic)
    {
      logic->AddAndDisplayVolume(outputVolumeNode.GetPointer());
    }

  std::cout << "Enhancement Done " << std::endl;
}


// ----------------------------------------------------------------------------


InputImageType::Pointer qSlicerStructureEnhancerModuleWidget::toITKImage(vtkImageData * vtkImage)
{
  typedef itk::VTKImageToImageFilter <InputImageType> itkConnectorType;

  InputImageType::Pointer itkImage = InputImageType::New();
  itkConnectorType::Pointer itkConnector = itkConnectorType::New();
  itkConnector->SetInput(vtkImage);
  itkConnector->Update();

  itkImage->Graft(itkConnector->GetOutput());
  itkConnector = NULL;

  return itkImage;
}


//-----------------------------------------------------------------------------


vtkSmartPointer<vtkImageData> qSlicerStructureEnhancerModuleWidget::
toVTKImageData(OutputImageType::Pointer itkImage)
{
  typedef itk::ImageToVTKImageFilter <OutputImageType> vtkConnectorType;
  vtkConnectorType::Pointer vtkConnector = vtkConnectorType::New();
  vtkConnector->GetExporter()->SetInput(itkImage);
  vtkConnector->GetImporter()->Update();

  vtkSmartPointer<vtkImageData> outputImage = vtkSmartPointer<vtkImageData>::New();
  outputImage->Initialize();
  outputImage->DeepCopy(vtkConnector->GetImporter()->GetOutput());
//  outputImage->Update();

  return outputImage;
}


//-----------------------------------------------------------------------------


InputImageType::Pointer qSlicerStructureEnhancerModuleWidget::
toITKImage(vtkSmartPointer<vtkImageData> vtkImage)
{
  typedef itk::VTKImageToImageFilter <InputImageType> itkConnectorType;

  InputImageType::Pointer itkImage = InputImageType::New();
  itkConnectorType::Pointer itkConnector = itkConnectorType::New();
  itkConnector->SetInput(vtkImage);
  itkConnector->Update();

  itkImage->Graft(itkConnector->GetOutput());
  itkConnector = NULL;

  return itkImage;
}