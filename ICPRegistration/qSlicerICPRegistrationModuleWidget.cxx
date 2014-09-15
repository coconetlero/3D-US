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
#include "qSlicerICPRegistrationModuleWidget.h"
#include "ui_qSlicerICPRegistrationModuleWidget.h"

// VTK includes
#include <vtkLandmarkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>

// this includes 
#include <vtkSlicerICPRegistrationLogic.h>

// MRMLLogic includes
#include <vtkMRMLApplicationLogic.h>

//MRML includes 
#include <vtkMRMLNode.h>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate

class qSlicerICPRegistrationModuleWidgetPrivate : public Ui_qSlicerICPRegistrationModuleWidget
{

public:
    qSlicerICPRegistrationModuleWidgetPrivate();

};

//-----------------------------------------------------------------------------
// qSlicerICPRegistrationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------

qSlicerICPRegistrationModuleWidgetPrivate::qSlicerICPRegistrationModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerICPRegistrationModuleWidget methods

//-----------------------------------------------------------------------------

qSlicerICPRegistrationModuleWidget::qSlicerICPRegistrationModuleWidget(QWidget* _parent)
: Superclass(_parent)
, d_ptr(new qSlicerICPRegistrationModuleWidgetPrivate)
{
}

//-----------------------------------------------------------------------------

qSlicerICPRegistrationModuleWidget::~qSlicerICPRegistrationModuleWidget()
{
}


//-----------------------------------------------------------------------------

void qSlicerICPRegistrationModuleWidget::setup()
{
    Q_D(qSlicerICPRegistrationModuleWidget);

    d->setupUi(this);
    this->Superclass::setup();

    //  this->setMRMLScene(d->logic()->GetMRMLScene());

    // connect the UI with the implementation    
    connect(d->inputMRMLNodeComboBox,
            SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onInputModelSelected()));
    
    connect(d->targetMRMLNodeComboBox,
            SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onTarjetModelSelected()));

    d->outputMRMLNodeComboBox->setNoneEnabled(true);
    connect(d->outputMRMLNodeComboBox,
            SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onOutputModelSelected()));
}


//-----------------------------------------------------------------------------

void qSlicerICPRegistrationModuleWidget::onModelRegistration()
{
    Q_D(qSlicerICPRegistrationModuleWidget);

    if (!this->Target || !this->Moving || !this->TransformationNode)
        return;

    // Setup ICP transform
    vtkSmartPointer<vtkIterativeClosestPointTransform> icp =
            vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
    icp->SetSource(this->Moving);
    icp->SetTarget(this->Target);
    icp->SetMaximumNumberOfIterations(d->iterationsSpinBox->value());
    icp->SetMaximumNumberOfLandmarks(d->landmarksSpinBox->value());

    if (d->rigidRadioButton->isChecked())
        icp->GetLandmarkTransform()->SetModeToRigidBody();
    if (d->similarRadioButton->isChecked())
        icp->GetLandmarkTransform()->SetModeToSimilarity();
    if (d->affineRadioButton->isChecked())
        icp->GetLandmarkTransform()->SetModeToAffine();
    if (d->rmsRadioButton->isChecked())
        icp->SetMeanDistanceModeToRMS();
    if (d->absRadioButton->isChecked())
        icp->SetMeanDistanceModeToAbsoluteValue();

    if (d->centroidsCheckBox->isChecked())
        icp->StartByMatchingCentroidsOn();
    else
        icp->StartByMatchingCentroidsOff();

    if (d->meanDistanceCheckBox->isChecked())
    {
        icp->CheckMeanDistanceOn();
        icp->SetMaximumMeanDistance(d->meanDistanceDoubleSpinBox->value());
    }

    std::cout << "Target points: " << icp->GetTarget()->GetNumberOfPoints() << std::endl;
    std::cout << "source points: " << icp->GetSource()->GetNumberOfPoints() << std::endl;

    // start registration 
    icp->Modified();
    icp->Update();

    vtkSmartPointer<vtkMatrix4x4> transformationMatrix = icp->GetMatrix();    
    std::cout << "The resulting matrix is: " << *transformationMatrix << std::endl;

    
    
    // add transformation matrix 
    vtkSlicerICPRegistrationLogic * logic =
            vtkSlicerICPRegistrationLogic::SafeDownCast(this->logic());
    if (logic)
    {               
            TransformationNode->SetAndObserveMatrixTransformToParent(transformationMatrix);
            logic->AddTransformToScene(TransformationNode, MovingNode);        
    }
    else
    {
        std::cout << "no logic" << std::endl;
    }
    
    std::cout << "registration donde" << std::endl;
}


//-----------------------------------------------------------------------------

void qSlicerICPRegistrationModuleWidget::onInputModelSelected()
{
    Q_D(qSlicerICPRegistrationModuleWidget);
    Q_ASSERT(d->inputMRMLNodeComboBox);

    vtkSmartPointer<vtkMRMLNode> node = d->inputMRMLNodeComboBox->currentNode();
    if (node)
    {
        vtkSmartPointer<vtkMRMLModelNode> modelNode = vtkMRMLModelNode::SafeDownCast(node);
        if (modelNode)
        {
            this->MovingNode = modelNode;
            this->Moving = modelNode->GetPolyData();
            std::cout << "add moving mesh" << std::endl;
        }
    }
}


//-----------------------------------------------------------------------------

void qSlicerICPRegistrationModuleWidget::onTarjetModelSelected()
{
    Q_D(qSlicerICPRegistrationModuleWidget);
    Q_ASSERT(d->targetMRMLNodeComboBox);

    vtkSmartPointer<vtkMRMLNode> node = d->targetMRMLNodeComboBox->currentNode();
    if (node)
    {
        vtkSmartPointer<vtkMRMLModelNode> modelNode = vtkMRMLModelNode::SafeDownCast(node);
        if (modelNode)
        {
            this->Target = modelNode->GetPolyData();
            std::cout << "add target mesh" << std::endl;
        }
    }
}


//-----------------------------------------------------------------------------

void qSlicerICPRegistrationModuleWidget::onOutputModelSelected()
{
    Q_D(qSlicerICPRegistrationModuleWidget);

    vtkSmartPointer<vtkMRMLNode> node = d->outputMRMLNodeComboBox->currentNode();
    if (node)
    {
        vtkSmartPointer<vtkMRMLLinearTransformNode> tNode = 
                vtkMRMLLinearTransformNode::SafeDownCast(node);
        if (tNode)
        {            
            this->TransformationNode = tNode;
        }
        else
        {
            std::cout << "no transformation node" << std::endl;
        }
    }
}











