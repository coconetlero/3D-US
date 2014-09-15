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

// STD Includes
#include <vector>
#include <math.h>

// boost includes
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/mean.hpp>

// Qt includes
#include <QDebug>

//itk includes
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodIterator.h"

//vtk includes
#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkImageShiftScale.h>
#include <vtkImageIterator.h>

// MRML Node
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLNode.h>

// glue inlcudes
#include "../VtkGlue/include/itkImageToVTKImageFilter.h"
#include "../VtkGlue/include/itkVTKImageToImageFilter.h"

// SlicerQt includes
#include "qSlicerGraphCutSegmentationModuleWidget.h"
#include "ui_qSlicerGraphCutSegmentationModuleWidget.h"
#include "qSlicerGraphCutSegmentationModule.h"
#include "vtkSlicerGraphCutSegmentationLogic.h"


#include "GraphCutSegmentation.h"

using namespace boost::accumulators;



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate

class qSlicerGraphCutSegmentationModuleWidgetPrivate : public Ui_qSlicerGraphCutSegmentationModuleWidget
{

public:
    qSlicerGraphCutSegmentationModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerGraphCutSegmentationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------

qSlicerGraphCutSegmentationModuleWidgetPrivate::qSlicerGraphCutSegmentationModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerGraphCutSegmentationModuleWidget methods

//-----------------------------------------------------------------------------

qSlicerGraphCutSegmentationModuleWidget::qSlicerGraphCutSegmentationModuleWidget(QWidget* _parent)
: Superclass(_parent)
, d_ptr(new qSlicerGraphCutSegmentationModuleWidgetPrivate)
{
}

//-----------------------------------------------------------------------------

qSlicerGraphCutSegmentationModuleWidget::~qSlicerGraphCutSegmentationModuleWidget()
{
}

//-----------------------------------------------------------------------------

void qSlicerGraphCutSegmentationModuleWidget::setup()
{
    Q_D(qSlicerGraphCutSegmentationModuleWidget);
    d->setupUi(this);
    this->Superclass::setup();

    d->inputMRMLNodeComboBox->setNoneEnabled(true);
    connect(d->inputMRMLNodeComboBox,
            SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onInputVolumeSelected()));

    d->objectMRMLNodeComboBox->setNoneEnabled(true);
    connect(d->objectMRMLNodeComboBox,
            SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onObjectVolumeSelected()));

    d->backgroundMRMLNodeComboBox->setNoneEnabled(true);
    connect(d->backgroundMRMLNodeComboBox,
            SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onBkgVolumeSelected()));


}


//-----------------------------------------------------------------------------

void qSlicerGraphCutSegmentationModuleWidget::onInputVolumeSelected()
{
    Q_D(qSlicerGraphCutSegmentationModuleWidget);
}


//-----------------------------------------------------------------------------

void qSlicerGraphCutSegmentationModuleWidget::onObjectVolumeSelected()
{
    Q_D(qSlicerGraphCutSegmentationModuleWidget);
}


//-----------------------------------------------------------------------------

void qSlicerGraphCutSegmentationModuleWidget::onBkgVolumeSelected()
{
    Q_D(qSlicerGraphCutSegmentationModuleWidget);
}



//-----------------------------------------------------------------------------

void qSlicerGraphCutSegmentationModuleWidget::segment()
{
    Q_D(qSlicerGraphCutSegmentationModuleWidget);

    vtkMRMLNode *inputNode = d->inputMRMLNodeComboBox->currentNode();
    vtkMRMLNode *objectNode = d->objectMRMLNodeComboBox->currentNode();
    vtkMRMLNode *bkgNode = d->backgroundMRMLNodeComboBox->currentNode();

    if (!inputNode && !objectNode && bkgNode)
    {
        return;
    }
    vtkMRMLVolumeNode *inputVolumeNode = vtkMRMLVolumeNode::SafeDownCast(inputNode);
    vtkMRMLVolumeNode *inputObjectVolumeNode = vtkMRMLVolumeNode::SafeDownCast(objectNode);
    vtkMRMLVolumeNode *inputBkgVolumeNode = vtkMRMLVolumeNode::SafeDownCast(bkgNode);

    // get input volume
    vtkSmartPointer<vtkImageData> inputVolume = inputVolumeNode->GetImageData();


    vtkNew<vtkImageShiftScale> shiftScaleFilter1;
    shiftScaleFilter1->SetOutputScalarTypeToUnsignedChar();
    shiftScaleFilter1->SetInput(inputObjectVolumeNode->GetImageData());
    shiftScaleFilter1->Update();

    // get object volume
    vtkSmartPointer<vtkImageData> objectVolume = shiftScaleFilter1->GetOutput();


    vtkNew<vtkImageShiftScale> shiftScaleFilter2;
    shiftScaleFilter2->SetOutputScalarTypeToUnsignedChar();
    shiftScaleFilter2->SetInput(inputBkgVolumeNode->GetImageData());
    shiftScaleFilter2->Update();

    // get background volume
    vtkSmartPointer<vtkImageData> bkgVolume = shiftScaleFilter2->GetOutput();



    // transform to the itk volumes 
    // convert to itkImage from vtkImageData
    ImageType::Pointer inputImage = toITKImage(inputVolume);
    ImageType::Pointer objectImage = toITKImage(objectVolume);
    ImageType::Pointer bkgImage = toITKImage(bkgVolume);

    accumulator_set<double, stats<tag::mean, tag::variance> > objectAcc;
    accumulator_set<double, stats<tag::mean, tag::variance> > backgroundAcc;
    accumulator_set<double, stats<tag::mean, tag::variance> > imageAcc;

    // fill output image
    typedef itk::ImageRegionIterator<ImageType> IteratorImageType;
    IteratorImageType imageIterator(inputImage, inputImage->GetLargestPossibleRegion());
    IteratorImageType objectIterator(objectImage, objectImage->GetLargestPossibleRegion());
    IteratorImageType backgroundIterator(bkgImage, bkgImage->GetLargestPossibleRegion());


    while (!objectIterator.IsAtEnd())
    {
        int objValue = (int) objectIterator.Get();
        if (objValue > 0)
            objectAcc((double) imageIterator.Get());


        int bkgValue = (int) backgroundIterator.Get();
        if (bkgValue > 0)
            backgroundAcc((double) imageIterator.Get());

        imageAcc((double) imageIterator.Get());

        ++objectIterator;
        ++backgroundIterator;
        ++imageIterator;
    }


    // statistical values for the images
    double obj_mean = mean(objectAcc);
    double obj_stdv = sqrt(variance(objectAcc));
    double bkg_mean = mean(backgroundAcc);
    double bkg_stdv = sqrt(variance(backgroundAcc));
    double im_stdv = sqrt(variance(imageAcc));

    std::cout << "image stdv: " << im_stdv << std::endl;
    std::cout << "obj mean: " << obj_mean << std::endl;
    std::cout << "obj stdv: " << obj_stdv << std::endl;
    std::cout << "bkg mean: " << bkg_mean << std::endl;
    std::cout << "bkg stdv: " << bkg_stdv << std::endl;


    GraphCutSegmentation segmenter;
    ImageType::Pointer segmentedImage =
            segmenter.segment(inputImage, obj_mean, obj_stdv, bkg_mean, bkg_stdv, im_stdv);




    // convert to vtkImage
    vtkSmartPointer<vtkImageData> segmentedVolume = toVTKImageData(segmentedImage);
    std::cout << "conversion to vtk DONE " << std::endl;


    // create an output volume node
    vtkNew<vtkMRMLScalarVolumeNode> outputVolumeNode;
    outputVolumeNode->SetOrigin(inputVolumeNode->GetOrigin());
    outputVolumeNode->SetSpacing(inputVolumeNode->GetSpacing());
    outputVolumeNode->SetAndObserveImageData(segmentedVolume);

    vtkSlicerGraphCutSegmentationLogic * logic =
            vtkSlicerGraphCutSegmentationLogic::SafeDownCast(this->logic());

    if (logic)
    {
        logic->AddAndDisplayVolume(outputVolumeNode.GetPointer());
    }



    std::cout << "Segmentation volume DONE" << std::endl;



}


//-----------------------------------------------------------------------------

vtkSmartPointer<vtkImageData> qSlicerGraphCutSegmentationModuleWidget::
toVTKImageData(ImageType::Pointer itkImage)
{
    typedef itk::ImageToVTKImageFilter <ImageType> vtkConnectorType;
    vtkConnectorType::Pointer vtkConnector = vtkConnectorType::New();
    vtkConnector->GetExporter()->SetInput(itkImage);
    vtkConnector->GetImporter()->Update();

    vtkNew<vtkImageData> outputImage;
    outputImage->Initialize();
    outputImage->DeepCopy(vtkConnector->GetImporter()->GetOutput());
    outputImage->Update();

    return outputImage.GetPointer();
}


//-----------------------------------------------------------------------------

ImageType::Pointer qSlicerGraphCutSegmentationModuleWidget::
toITKImage(vtkSmartPointer<vtkImageData> vtkImage)
{
    typedef itk::VTKImageToImageFilter <ImageType> itkConnectorType;

    ImageType::Pointer itkImage = ImageType::New();
    itkConnectorType::Pointer itkConnector = itkConnectorType::New();
    itkConnector->SetInput(vtkImage);
    itkConnector->Update();

    itkImage->Graft(itkConnector->GetOutput());
    itkConnector = NULL;

    return itkImage;
}