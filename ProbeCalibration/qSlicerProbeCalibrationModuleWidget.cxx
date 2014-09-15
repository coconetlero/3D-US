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
#include <QtGui>

// vtkIncludes
#include <vtkMatrix4x4.h>
#include <vtkNew.h>

// MRML includes 
#include "vtkMRMLMarkupsNode.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLAbstractSliceViewDisplayableManager.h"
#include "vtkMRMLVolumeNode.h"
#include "vtkMRMLAnnotationFiducialNode.h"

// other includes
#include <sstream>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

// SlicerQt includes
#include "qSlicerProbeCalibrationModuleWidget.h"
#include "ui_qSlicerProbeCalibrationModuleWidget.h"

// this includes
#include "CalibrationPointsSquaresFunction.h"
#include "vtkSlicerProbeCalibrationLogic.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate

class qSlicerProbeCalibrationModuleWidgetPrivate : public Ui_qSlicerProbeCalibrationModuleWidget
{

protected:
    qSlicerProbeCalibrationModuleWidget * const q_ptr;

public:
    qSlicerProbeCalibrationModuleWidgetPrivate(qSlicerProbeCalibrationModuleWidget & object);
    vtkSlicerProbeCalibrationLogic *logic() const;
};

//-----------------------------------------------------------------------------
// qSlicerProbeCalibrationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------

qSlicerProbeCalibrationModuleWidgetPrivate::
qSlicerProbeCalibrationModuleWidgetPrivate(qSlicerProbeCalibrationModuleWidget & object) : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
// qSlicerProbeCalibrationModuleWidget methods
//-----------------------------------------------------------------------------

qSlicerProbeCalibrationModuleWidget::qSlicerProbeCalibrationModuleWidget(QWidget* _parent)
: Superclass(_parent)
, d_ptr(new qSlicerProbeCalibrationModuleWidgetPrivate(* this))
{
}

//-----------------------------------------------------------------------------

qSlicerProbeCalibrationModuleWidget::~qSlicerProbeCalibrationModuleWidget()
{
}

//-----------------------------------------------------------------------------

vtkSlicerProbeCalibrationLogic *qSlicerProbeCalibrationModuleWidgetPrivate::logic() const
{
    return vtkSlicerProbeCalibrationLogic::SafeDownCast(this->logic());
}

//-----------------------------------------------------------------------------

void qSlicerProbeCalibrationModuleWidget::setup()
{
    Q_D(qSlicerProbeCalibrationModuleWidget);
    d->setupUi(this);
    this->Superclass::setup();

    d->volumeMRMLNodeComboBox->setNoneEnabled(true);
    connect(d->volumeMRMLNodeComboBox,
            SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onVolumeSelected()));

    d->fiducialsMRMLNodeComboBox->setNoneEnabled(true);
    connect(d->fiducialsMRMLNodeComboBox,
            SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onFiducialListSelected()));
}

// ----------------------------------------------------------------------------
// load translations implementation

void qSlicerProbeCalibrationModuleWidget::loadTranslationsFile()
{
    std::cout << "load translations" << std::endl;
    QString translationFilename =
            QFileDialog::getOpenFileName(this, tr("Load Translation File"),
            QDir::currentPath());

    if (!translationFilename.isEmpty())
    {
        QFile file(translationFilename);
        if (!file.open(QIODevice::ReadOnly))
            return;

        QTextStream stream(&file);
        QString line;

        int rows = 0;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            rows++;
        }
        file.close();

        // declare a translation matrix 
        this->translations.set_size(rows, 3);

        if (!file.open(QIODevice::ReadOnly))
            return;

        int idx = 0;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            QStringList lineList = line.split(" ");

            for (int j = 0; j < lineList.size(); j++)
            {
                translations.put(idx, j, lineList.at(j).toDouble());
            }
            idx++;
        }

        file.close(); // when your done.     

        // get UI element and modify
        Q_D(qSlicerProbeCalibrationModuleWidget);
        d->translationsLineEdit->insert(translationFilename);
    }
}

//------------------------------------------------------------------------------
// loadRotations implementation

void qSlicerProbeCalibrationModuleWidget::loadRotationsFile()
{
    std::cout << "load rotations " << std::endl;
    QString rotationFilename =
            QFileDialog::getOpenFileName(this,
            tr("Load Rotations File"), QDir::currentPath());

    if (!rotationFilename.isEmpty())
    {

        QFile file(rotationFilename);
        if (!file.open(QIODevice::ReadOnly))
            return;

        QTextStream stream(&file);
        QString line;

        int rows = 0;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            rows++;
        }
        file.close();

        // declare a rotation matrix 
        this->rotations.set_size(rows, 4);


        if (!file.open(QIODevice::ReadOnly))
            return;

        int idx = 0;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            QStringList lineList = line.split(" ");

            for (int j = 0; j < lineList.size(); j++)
            {
                rotations.put(idx, j, lineList.at(j).toDouble());
            }

            idx++;
        }
        file.close();

        // get UI element and modify
        Q_D(qSlicerProbeCalibrationModuleWidget);
        d->rotationsLineEdit->insert(rotationFilename);
    }
}

//------------------------------------------------------------------------------

void qSlicerProbeCalibrationModuleWidget::onFiducialListSelected()
{
    Q_D(qSlicerProbeCalibrationModuleWidget);

    vtkMRMLNode * mrmlNode = d->fiducialsMRMLNodeComboBox->currentNode();
    vtkMRMLMarkupsFiducialNode * fiducialsListNode = NULL;

    if (mrmlNode)
    {
        fiducialsListNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(mrmlNode);
    }

    if (fiducialsListNode)
    {
        std::cout << "load fiducial in scene " << std::endl;
        this->fiducialsList = vtkSmartPointer<vtkCollection>::New();

        if (nFrames > 0)
        {
            if (nFrames == fiducialsListNode->GetNumberOfFiducials())
            {
                this->coords.set_size(nFrames, 2);

                for (int i = 0; i < fiducialsListNode->GetNumberOfFiducials(); i++)
                {
                    double * point;
                    fiducialsListNode->GetNthFiducialPosition(i, point);

                    int z = floor(point[2] + 0.5);
                    coords[z][0] = -point[0];
                    coords[z][1] = -point[1];

                    //                    std::cout << "fiducial " << z << " = (" << coords[z][0] << ", " << coords[z][1] << ")" << std::endl;
                }
            }
            else
            {
                std::cout << "number of frames and fiducials are different" << std::endl;
                std::cout << "frames = " << nFrames << " fiducials = " << fiducialsListNode->GetNumberOfFiducials() << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Markup node is null" << std::endl;
    }
}


// ----------------------------------------------------------------------------

void qSlicerProbeCalibrationModuleWidget::onVolumeSelected()
{
    Q_D(qSlicerProbeCalibrationModuleWidget);

    vtkMRMLNode * node = d->volumeMRMLNodeComboBox->currentNode();
    vtkMRMLVolumeNode * inputVolume = vtkMRMLVolumeNode::SafeDownCast(node);

    if (inputVolume)
    {
        std::cout << "load volume in scene " << std::endl;

        this->images = inputVolume->GetImageData();
        int * dims = images->GetDimensions();
        this->nFrames = dims[2];

        std::cout << "number of frames " << nFrames << std::endl;
    }
}


// ----------------------------------------------------------------------------

void qSlicerProbeCalibrationModuleWidget::calibrateUSProbe()
{
    if (!translations.empty() || !rotations.empty())
    {
        std::cout << "calibrate probe " << std::endl;

        // create coordinates list of the each crosspoint if the list it's not created
        if (coords.empty())
        {
            if (fiducialsList->GetNumberOfItems() > 0)
            {
                this->coords.set_size(this->nFrames, 2);

                for (int i = 0; i < fiducialsList->GetNumberOfItems(); i++)
                {
                    vtkMRMLMarkupsFiducialNode * fiducialsListNode =
                            vtkMRMLMarkupsFiducialNode::SafeDownCast(fiducialsList->GetItemAsObject(i));

                    double * point;
                    fiducialsListNode->GetNthFiducialPosition(i, point);

                    //                    int z = floor(point[2] + 0.5);
                    //                    coords[z][0] = point[0];
                    //                    coords[z][1] = point[1];


                    double RASPoint[3];
                    RASPoint[0] = point[0];
                    RASPoint[1] = point[1];
                    RASPoint[2] = point[2];

                    double XYZPoint[3];

                    vtkNew<vtkMRMLAbstractSliceViewDisplayableManager> dm;
                    dm->ConvertRASToXYZ(RASPoint, XYZPoint);


                    int z = floor(XYZPoint[2] + 0.5);
                    coords[z][0] = XYZPoint[0];
                    coords[z][1] = XYZPoint[1];

                    std::cout << "fiducial: " << i << std::endl;
                    std::cout << "ras -> " << "x = " << RASPoint[0] << " y = " << RASPoint[1] <<
                            " z = " << floor(RASPoint[2] + 0.5) << std::endl;
                    std::cout << "xyz -> " << "x = " << XYZPoint[0] << " y = " << XYZPoint[1] <<
                            " z = " << floor(XYZPoint[2] + 0.5) << std::endl;

                }
            }
        }

        // once with all parameters Calibrate!!!

        // generate the transformation (rotation and translation) matrixes for each image
        //        std::cout << "Transformation matrices" << std::endl;
        //        std::cout << std::endl;

        // vnl_matrix<double> transformationArray [nFrames];
        std::vector<vnl_matrix <double> > transformationArray(nFrames);
        std::vector<vnl_matrix <double>* > transformationSet(nFrames);

        for (int i = 0; i < nFrames; i++)
        {

            // the given parameters is [0]=scale factor, [1]=x, [2]=y, [3]=z                       
            vnl_quaternion<double> quaternion(
                    rotations[i][1],
                    rotations[i][2],
                    rotations[i][3],
                    rotations[i][0]);
            vnl_matrix<double> transformation = quaternion.rotation_matrix_transpose_4();
            transformation = transformation.transpose();

            transformation.put(0, 3, translations[i][0]);
            transformation.put(1, 3, translations[i][1]);
            transformation.put(2, 3, translations[i][2]);

            transformationArray[i] = transformation;
            transformationSet[i] = &transformationArray[i];

            //            std::cerr << transformation;
            //            std::cout << std::endl;

        }

        //        coords.print(std::cout);
        //        std::cout << std::endl;

        CalibrationPointsSquaresFunction optimizationFunction(&transformationSet, &coords);


        vnl_vector<double> x0(11);
        x0.fill(1.0);
        vnl_vector<double> x1 = x0.as_ref();

        x1.fill(1.0);

        vnl_levenberg_marquardt LM(optimizationFunction);
        LM.set_verbose(TRUE);

        LM.set_f_tolerance(10e-10);
        LM.set_x_tolerance(10e-10);

        // max iterations 5000
        LM.set_max_function_evals(5000);

        bool okOptimization = false;

        try
        {
            okOptimization = LM.minimize(x1);
        }
        catch (std::exception& e)
        {
            qCritical() << "Exception thrown:" << e.what();
        }

        LM.diagnose_outcome(std::cout);
        std::cout << "x1 = " << x1 << std::endl;

        Q_D(qSlicerProbeCalibrationModuleWidget);

        if (okOptimization)
        {
            QString t11, t12, t13, t21, t22, t23, w1, w2, w3, sx, sy;
            t11.setNum(x1[0]);
            t12.setNum(x1[1]);
            t13.setNum(x1[2]);
            d->t11Label->setText(t11);
            d->t12Label->setText(t12);
            d->t13Label->setText(t13);

            t21.setNum(x1[3]);
            t22.setNum(x1[4]);
            t23.setNum(x1[5]);
            d->t21Label->setText(t21);
            d->t22Label->setText(t22);
            d->t23Label->setText(t23);

            w1.setNum(x1[6]);
            w2.setNum(x1[7]);
            w3.setNum(x1[8]);
            d->w1Label->setText(w1);
            d->w2Label->setText(w2);
            d->w3Label->setText(w3);

            sx.setNum(x1[9]);
            sy.setNum(x1[10]);
            d->sxLabel->setText(sx);
            d->syLabel->setText(sy);
        }


        // create transform 
        // this transform is the rTp transformation in the calibration equation
        vnl_quaternion<double> rTp_Quat(x1[8], x1[7], x1[6]);
        vnl_matrix<double> transformation = rTp_Quat.rotation_matrix_transpose_4();
        transformation = transformation.transpose();
        transformation.put(0, 3, x1[3]);
        transformation.put(1, 3, x1[4]);
        transformation.put(2, 3, x1[5]);

        vtkNew<vtkMatrix4x4> vtkMatrix;

        // Copy vnl matrix to  vtkmatrix 
        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                vtkMatrix->SetElement(r, c, transformation[r][c]);
            }
        }

        // create scale transformation matrix
        vtkNew<vtkMatrix4x4> scaleMatrix;
        scaleMatrix->Identity();
        scaleMatrix->SetElement(0, 0, x1[9]);
        scaleMatrix->SetElement(1, 1, x1[10]);



        vtkSlicerProbeCalibrationLogic * logic =
                vtkSlicerProbeCalibrationLogic::SafeDownCast(this->logic());
        if (logic)
        {
            logic->AddTransformToScene(vtkMatrix.GetPointer(), (char *) "rTp_");
            logic->AddTransformToScene(scaleMatrix.GetPointer(), (char *) "scale_");
        }
        else
        {
            std::cout << "no logic" << std::endl;
        }
    }
}