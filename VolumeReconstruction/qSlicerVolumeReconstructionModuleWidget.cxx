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

// vtk includes
#include <vtkNew.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageReslice.h>

// MRML includes
#include "vtkMRMLNode.h"
#include "vtkMRMLVolumeNode.h"
#include <vtkMRMLScalarVolumeNode.h>
#include "vtkMRMLLinearTransformNode.h"




// other includes
#include "time.h"
#include "float.h"
#include "math.h"
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_matrix.h>


// SlicerQt includes
#include "qSlicerVolumeReconstructionModuleWidget.h"
#include "ui_qSlicerVolumeReconstructionModuleWidget.h"
#include "vtkSlicerVolumeReconstructionLogic.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate

class qSlicerVolumeReconstructionModuleWidgetPrivate : public Ui_qSlicerVolumeReconstructionModuleWidget
{

public:
    qSlicerVolumeReconstructionModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVolumeReconstructionModuleWidgetPrivate methods

//-----------------------------------------------------------------------------

qSlicerVolumeReconstructionModuleWidgetPrivate::qSlicerVolumeReconstructionModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVolumeReconstructionModuleWidget methods

//-----------------------------------------------------------------------------

qSlicerVolumeReconstructionModuleWidget::qSlicerVolumeReconstructionModuleWidget(QWidget* _parent)
: Superclass(_parent)
, d_ptr(new qSlicerVolumeReconstructionModuleWidgetPrivate)
{
}

//-----------------------------------------------------------------------------

qSlicerVolumeReconstructionModuleWidget::~qSlicerVolumeReconstructionModuleWidget()
{
}

//-----------------------------------------------------------------------------

void qSlicerVolumeReconstructionModuleWidget::setup()
{
    Q_D(qSlicerVolumeReconstructionModuleWidget);
    d->setupUi(this);
    this->Superclass::setup();

    d->imagesMRMLNodeComboBox->setNoneEnabled(true);
    connect(d->imagesMRMLNodeComboBox,
            SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onImagesSelected()));

    d->transformationMRMLNodeComboBox->setNoneEnabled(true);
    connect(d->transformationMRMLNodeComboBox,
            SIGNAL(currentNodeChanged(vtkMRMLNode*)),
            this, SLOT(onTransformationSelected()));
}


//-----------------------------------------------------------------------------

void qSlicerVolumeReconstructionModuleWidget::loadRotationsFile()
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
                rotations.put(idx, j, lineList.at(j).toFloat());
            }

            idx++;
        }
        file.close();

        // get UI element and modify
        Q_D(qSlicerVolumeReconstructionModuleWidget);
        d->rotationsLineEdit->insert(rotationFilename);
    }
}

//-----------------------------------------------------------------------------

void qSlicerVolumeReconstructionModuleWidget::loadTranslationsFile()
{
    std::cout << "load translations " << std::endl;
    QString translationFilename =
            QFileDialog::getOpenFileName(this, tr("Load Translations File"),
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
                translations.put(idx, j, lineList.at(j).toFloat());
            }
            idx++;
        }

        file.close(); // when your done.     

        // get UI element and modify
        Q_D(qSlicerVolumeReconstructionModuleWidget);
        d->translationsLineEdit->insert(translationFilename);
    }
}

//-----------------------------------------------------------------------------

void qSlicerVolumeReconstructionModuleWidget::onImagesSelected()
{
    Q_D(qSlicerVolumeReconstructionModuleWidget);

    vtkMRMLNode * node = d->imagesMRMLNodeComboBox->currentNode();
    vtkMRMLVolumeNode * inputVolume = vtkMRMLVolumeNode::SafeDownCast(node);

    if (inputVolume)
    {
        std::cout << "load input images in scene" << std::endl;

        this->images = inputVolume->GetImageData();
        this->imageDimensions = images->GetDimensions();

        int nFrames = imageDimensions[2];

        std::cout << "dims: " <<
                imageDimensions[0] << " " <<
                imageDimensions[1] << " " <<
                imageDimensions[2] << std::endl;
    }
}


//-----------------------------------------------------------------------------

void qSlicerVolumeReconstructionModuleWidget::onTransformationSelected()
{
    Q_D(qSlicerVolumeReconstructionModuleWidget);
    vtkMRMLNode * node = d->transformationMRMLNodeComboBox->currentNode();
    vtkMRMLLinearTransformNode * transformNode =
            vtkMRMLLinearTransformNode::SafeDownCast(node);

    if (transformNode)
    {
        this->rTp = transformNode->GetMatrixTransformToParent();
    }
    std::cout << "set transformation" << std::endl;
}


//-----------------------------------------------------------------------------

void qSlicerVolumeReconstructionModuleWidget::generateReconstruction()
{
    // get scale 
    Q_D(qSlicerVolumeReconstructionModuleWidget);

    vtkMRMLNode * node = d->scaleMRMLNodeComboBox->currentNode();
    vtkMRMLLinearTransformNode * scaleNode =
            vtkMRMLLinearTransformNode::SafeDownCast(node);

    vtkSmartPointer<vtkMatrix4x4> scaleM = vtkSmartPointer<vtkMatrix4x4>::New();
    if (scaleNode)
    {
        scaleM = scaleNode->GetMatrixTransformToParent();
    }

    float volumeScale = d->volumeScaleSpinBox->value();
    int windowSise = d->windowSizeSpinBox->value();

    float s[3];
    s[0] = scaleM->GetElement(0, 0);
    s[1] = scaleM->GetElement(1, 1);
    s[2] = 0.1;

    double spacing[3];
    spacing[0] = s[0] * volumeScale;
    spacing[1] = s[1] * volumeScale;
    spacing[2] = s[2] * volumeScale;

    //    //+------------------------------------------------------------
    //    double t2[3] = {-8.63806, 78.6335, -90.1075};
    //    double w[3] = {-2.88562, -1.43996, -3.05963};
    //
    //    s[0] = 0.08970;
    //    s[1] = 0.10324;
    //    s[2] = 0.1;
    //
    //    vnl_quaternion<double> rTp_Quat(w[2], w[1], w[0]);
    //    vnl_matrix<double> vnl_rTp = rTp_Quat.rotation_matrix_transpose_4();
    //    vnl_rTp = vnl_rTp.transpose();
    //    vnl_rTp.put(0, 3, t2[0]);
    //    vnl_rTp.put(1, 3, t2[1]);
    //    vnl_rTp.put(2, 3, t2[2]);
    //    
    //    for (int i = 0; i < 4; i++)
    //    {
    //        for (int j = 0; j < 4; j++)
    //        {
    //            this->rTp->SetElement(i, j, vnl_rTp[i][j]);            
    //        }
    //    }
    //    
    //    //+------------------------------------------------------------



    // find volumen bounds    
    int * bounds = findVolumeBounds(imageDimensions[0], imageDimensions[1], imageDimensions[2],
            s[0], s[1], s[2], volumeScale);

    // create the volume 
    vtkNew<vtkImageData> reconstructionVolume;
    //    newVolume->SetScalarTypeToInt();
    reconstructionVolume->SetScalarTypeToUnsignedChar();
    reconstructionVolume->SetNumberOfScalarComponents(1);
    reconstructionVolume->SetSpacing(spacing);
    reconstructionVolume->SetExtent(bounds);
    reconstructionVolume->AllocateScalars();

    int * dims = reconstructionVolume->GetDimensions();
    std::cout << "Extent = " << bounds[0] << ", " << bounds[1] << ", " << bounds[2]
            << ", " << bounds[3] << ", " << bounds[4] << ", " << bounds[5] << std::endl;
    std::cout << "Dimensions = " << dims[0] << ", " << dims[1] << ", " << dims[2] << std::endl;


    // ----------  start mesuring time  ------------- 
    std::cout << "start counting time " << std::endl;
    clock_t begin = clock();
    // ----------------------------------------------


    /** Pointer to voxels who have been filled in the bin filling stage */
    vtkNew<vtkImageData> filledVolume;
    filledVolume->SetScalarTypeToUnsignedChar();
    filledVolume->SetNumberOfScalarComponents(1);
    filledVolume->SetExtent(bounds);
    filledVolume->AllocateScalars();



    std::cout << "Bin Filling" << std::endl;
    this->BinFilling(reconstructionVolume.GetPointer(), filledVolume.GetPointer(),
            imageDimensions[0], imageDimensions[1], imageDimensions[2],
            s[0], s[1], s[2], volumeScale);



    // count partial time   -----------------------------
    clock_t partial = clock();
    double diffticks = partial - begin;
    double diffms = diffticks / CLOCKS_PER_SEC;
    cout << "Time elapsed in bin filling: " << double(diffms) << " seconds" << endl;
    // ---------------------------------------------------



    if (d->holeFillingCheckBox->isChecked())
    {
        std::cout << "Hole Filling" << std::endl;
        this->HoleFilling(reconstructionVolume.GetPointer(), filledVolume.GetPointer(), windowSise);

        // count partial time
        clock_t partial2 = clock();
        diffticks = partial2 - partial;
        diffms = diffticks / CLOCKS_PER_SEC;
        cout << "Time elapsed in hole filling: " << double(diffms) << " seconds" << endl;
    }

    /// show the reconstructionVolume into slicer
    int originExtent[6];
    originExtent[0] = 0;
    originExtent[1] = dims[0] - 1;
    originExtent[2] = 0;
    originExtent[3] = dims[1] - 1;
    originExtent[4] = 0;
    originExtent[5] = dims[2] - 1;

    reconstructionVolume->SetExtent(originExtent);
    reconstructionVolume->SetSpacing(1.0, 1.0, 1.0);

    vtkSlicerVolumeReconstructionLogic * logic =
            vtkSlicerVolumeReconstructionLogic::SafeDownCast(this->logic());

    if (logic)
    {
        logic->AddAndDisplayVolume(reconstructionVolume.GetPointer(), bounds, spacing, volumeScale * 0.1);
    }

     std::cout << "show new volume" << std::endl;
}



// ---------------------------------------------------------------------------

int * qSlicerVolumeReconstructionModuleWidget::
findVolumeBounds(int imageWidth, int imageHeight, int imageSize,
        float xScale, float yScale, float zScale, float volumeScale)
{
    vnl_matrix<float> rTp(4, 4);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            rTp[i][j] = this->rTp->GetElement(i, j);
        }
    }


    // set the boudanry images points
    float _p1[4] = {0.0, 0.0, 0.0, 1.0};
    vnl_matrix<float> p1(4, 1);
    p1.set_column(0, _p1);

    float _p2[4] = {(float) imageWidth * xScale, 0.0, 0.0, 1.0};
    vnl_matrix<float> p2(4, 1);
    p2.set_column(0, _p2);

    float _p3[4] = {0.0, (float) imageHeight * yScale, 0.0, 1.0};
    vnl_matrix<float> p3(4, 1);
    p3.set_column(0, _p3);

    float _p4[4] = {(float) imageWidth * xScale, (float) imageHeight * yScale, 0.0, 1.0};
    vnl_matrix<float> p4(4, 1);
    p4.set_column(0, _p4);

    // set initial values
    int xMin = static_cast<int> (DBL_MAX);
    int yMin = static_cast<int> (DBL_MAX);
    int zMin = static_cast<int> (DBL_MAX);
    int xMax = static_cast<int> (-DBL_MAX);
    int yMax = static_cast<int> (-DBL_MAX);
    int zMax = static_cast<int> (-DBL_MAX);

    for (int z = 0; z < imageSize; z++)
    {
        // Find Image transforamtion matrix
        vnl_quaternion<float> tTr_Quat(rotations[z][1], rotations[z][2],
                rotations[z][3], rotations[z][0]);
        vnl_matrix<float> tTr = tTr_Quat.rotation_matrix_transpose_4();
        tTr = tTr.transpose();
        tTr.put(0, 3, translations[z][0]);
        tTr.put(1, 3, translations[z][1]);
        tTr.put(2, 3, translations[z][2]);

        vnl_matrix<float> pT1 = tTr * rTp * p1;
        int xT = floor((pT1[0][0] / (xScale * volumeScale)) + 0.5);
        int yT = floor((pT1[1][0] / (yScale * volumeScale)) + 0.5);
        int zT = floor((pT1[2][0] / (zScale * volumeScale)) + 0.5);

        xMin = (xT < xMin) ? xT : xMin;
        yMin = (yT < yMin) ? yT : yMin;
        zMin = (zT < zMin) ? zT : zMin;
        xMax = (xT > xMax) ? xT : xMax;
        yMax = (yT > yMax) ? yT : yMax;
        zMax = (zT > zMax) ? zT : zMax;

        vnl_matrix<float> pT2 = tTr * rTp * p2;
        xT = floor((pT2[0][0] / (xScale * volumeScale)) + 0.5);
        yT = floor((pT2[1][0] / (yScale * volumeScale)) + 0.5);
        zT = floor((pT2[2][0] / (zScale * volumeScale)) + 0.5);



        xMin = (xT < xMin) ? xT : xMin;
        yMin = (yT < yMin) ? yT : yMin;
        zMin = (zT < zMin) ? zT : zMin;
        xMax = (xT > xMax) ? xT : xMax;
        yMax = (yT > yMax) ? yT : yMax;
        zMax = (zT > zMax) ? zT : zMax;

        vnl_matrix<float> pT3 = tTr * rTp * p3;
        xT = floor((pT3[0][0] / (xScale * volumeScale)) + 0.5);
        yT = floor((pT3[1][0] / (yScale * volumeScale)) + 0.5);
        zT = floor((pT3[2][0] / (zScale * volumeScale)) + 0.5);


        xMin = (xT < xMin) ? xT : xMin;
        yMin = (yT < yMin) ? yT : yMin;
        zMin = (zT < zMin) ? zT : zMin;
        xMax = (xT > xMax) ? xT : xMax;
        yMax = (yT > yMax) ? yT : yMax;
        zMax = (zT > zMax) ? zT : zMax;

        vnl_matrix<float> pT4 = tTr * rTp * p4;
        xT = floor((pT4[0][0] / (xScale * volumeScale)) + 0.5);
        yT = floor((pT4[1][0] / (yScale * volumeScale)) + 0.5);
        zT = floor((pT4[2][0] / (zScale * volumeScale)) + 0.5);



        xMin = (xT < xMin) ? xT : xMin;
        yMin = (yT < yMin) ? yT : yMin;
        zMin = (zT < zMin) ? zT : zMin;
        xMax = (xT > xMax) ? xT : xMax;
        yMax = (yT > yMax) ? yT : yMax;
        zMax = (zT > zMax) ? zT : zMax;
    }

    static int bounds[6];
    bounds[0] = xMin;
    bounds[1] = xMax;
    bounds[2] = yMin;
    bounds[3] = yMax;
    bounds[4] = zMin;
    bounds[5] = zMax;
    return bounds;
}


// ---------------------------------------------------------------------------

void qSlicerVolumeReconstructionModuleWidget::
BinFilling(vtkImageData * volume, vtkImageData * filledVolume,
        int imageWidth, int imageHeight, int imageSize,
        float xScale, float yScale, float zScale, float volumeScale)
{
    int * extent = volume->GetExtent();

    // get calibrarion matrix as vnl matrix
    vnl_matrix<float> rTp(4, 4);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            rTp[i][j] = this->rTp->GetElement(i, j);
        }
    }

    // create accumulation buffer
    vtkNew<vtkImageData> AccDataVolume;
    AccDataVolume->SetScalarTypeToInt();
    AccDataVolume->SetNumberOfScalarComponents(1);
    AccDataVolume->SetExtent(extent);
    AccDataVolume->AllocateScalars();


    for (int z = 0; z < imageSize; z++)
    {
        // Find Image transforamtion matrix
        vnl_quaternion<float> tTr_Quat(rotations[z][1], rotations[z][2],
                rotations[z][3], rotations[z][0]);
        vnl_matrix<float> tTr = tTr_Quat.rotation_matrix_transpose_4();
        tTr = tTr.transpose();
        tTr.put(0, 3, translations[z][0]);
        tTr.put(1, 3, translations[z][1]);
        tTr.put(2, 3, translations[z][2]);

        vnl_matrix<float> p(4, 1);
        for (int y = 0; y < imageHeight; y++)
        {
            for (int x = 0; x < imageWidth; x++)
            {
                // point p position
                float _p[4] = {x * xScale, y * yScale, 0.0, 1.0};
                p.set_column(0, _p);

                // get transformed point
                vnl_matrix<float> pT = tTr * rTp * p;

                int xT = floor((pT[0][0] / (xScale * volumeScale)) + 0.5);
                int yT = floor((pT[1][0] / (yScale * volumeScale)) + 0.5);
                int zT = floor((pT[2][0] / (zScale * volumeScale)) + 0.5);


                // check bounds
//                if (xT <= extent[1] && xT >= extent[0] &&
//                        yT <= extent[3] && yT >= extent[2] &&
//                        zT <= extent[5] && zT >= extent[4])
//                {

                    // get pointer to the current image pixel 
                    unsigned char * imagePixel = static_cast<unsigned char *> (
                            images->GetScalarPointer(x, y, z));

                    // get pointer to the corresponding reconstructed volume voxel 
                    unsigned char * volumeVoxel = static_cast<unsigned char *> (
                            volume->GetScalarPointer(xT, yT, zT));

                    // get pointer to the current fill volume voxel 
                    unsigned char * fillVoxel = static_cast<unsigned char *> (
                            filledVolume->GetScalarPointer(xT, yT, zT));

                    // get pointer to the corresponding accumator volume voxel 
                    unsigned char * accDataVoxel = static_cast<unsigned char *> (
                            AccDataVolume->GetScalarPointer(xT, yT, zT));


                    // set voxel value with the corresponding pixel value
                    if (fillVoxel[0] == 0)
                    {
                        volumeVoxel[0] = imagePixel[0];
                        accDataVoxel[0] = 1;
                        fillVoxel[0] = 1;
                    }
                    else
                    {
                        float temp = ((volumeVoxel[0] * accDataVoxel[0]) + imagePixel[0]) /
                                (accDataVoxel[0] + 1);
                        volumeVoxel[0] = (unsigned char) temp;
                        accDataVoxel[0]++;
                    }
//                }
//                else
//                {
//                    std::cout << ".";
//                }
            }
        }
    }
}

// ---------------------------------------------------------------------------

void qSlicerVolumeReconstructionModuleWidget::
HoleFilling(vtkImageData * volume, vtkImageData * filledVolume, int windowSize)
{
    int * bounds = volume->GetExtent();
    int * dims = volume->GetDimensions();
    float dMax = (float) sqrt(dims[0] * dims[0] + dims[1] * dims[1] + dims[2] * dims[2]);


    //    int emptyVoxels = fullSize - filledVoxels;
    //    std::cout << "Empty voxels: " << emptyVoxels << std::endl;

    // create an interpolation buffer
    vtkNew<vtkImageData> InterpolationVolume;
    InterpolationVolume->SetScalarTypeToUnsignedChar();
    InterpolationVolume->SetNumberOfScalarComponents(1);
    InterpolationVolume->SetExtent(bounds);
    InterpolationVolume->AllocateScalars();


    // create volume pointers
    unsigned char * voxel = static_cast<unsigned char *>
            (volume->GetScalarPointer(bounds[0], bounds[2], bounds[4]));

    unsigned char * filledVoxel = static_cast<unsigned char *>
            (filledVolume->GetScalarPointer(bounds[0], bounds[2], bounds[4]));

    unsigned char * interpVoxel = static_cast<unsigned char *>
            (InterpolationVolume->GetScalarPointer(bounds[0], bounds[2], bounds[4]));

    float * innerVoxel = static_cast<float *>
            (volume->GetScalarPointer(bounds[0], bounds[2], bounds[4]));


    for (int z = bounds[4]; z <= bounds[5]; z++)
    {
        for (int y = bounds[2]; y <= bounds[3]; y++)
        {
            for (int x = bounds[0]; x <= bounds[1]; x++)
            {
                filledVoxel = static_cast<unsigned char *>
                        (filledVolume->GetScalarPointer(x, y, z));

                if (filledVoxel[0] == 0)
                {
                    // center window point
                    float p_i[3] = {x, y, z};

                    // initial window size
                    int ws = 1;
                    int idx = 0;
                    float acc = 0;

                    while ((idx == 0) && (ws <= windowSize))
                    {
                        // create subvolume extent
                        int subX1 = ((x - ws) < bounds[0]) ? bounds[0] : x - ws;
                        int subX2 = ((x + ws) > bounds[1]) ? bounds[1] : x + ws;
                        int subY1 = ((y - ws) < bounds[2]) ? bounds[2] : y - ws;
                        int subY2 = ((y + ws) > bounds[3]) ? bounds[3] : y + ws;
                        int subZ1 = ((z - ws) < bounds[4]) ? bounds[4] : z - ws;
                        int subZ2 = ((z + ws) > bounds[5]) ? bounds[5] : z + ws;

                        for (int k = subZ1; k < subZ2; k++)
                        {
                            for (int j = subY1; j <= subY2; j++)
                            {
                                for (int i = subX1; i <= subX2; i++)
                                {
                                    if (x == i && y == j && z == k)
                                        continue;

                                    innerVoxel = static_cast<float *> (
                                            volume->GetScalarPointer(i, j, k));

                                    filledVoxel = static_cast<unsigned char *>
                                            (filledVolume->GetScalarPointer(i, j, k));

                                    if (filledVoxel[0] != 0)
                                    {
                                        float p_f[3] = {i, j, k};
                                        float dist = sqrt(vtkMath::Distance2BetweenPoints(p_i, p_f));

                                        // distance ponderation  
                                        float weight = (1 - (dist / dMax));
                                        acc += innerVoxel[0] * weight;
                                        idx++;
                                    }
                                }
                            }
                        }

                        if (idx != 0)
                        {
                            acc /= idx;
                            //                            std::cout << acc << " ";
                            interpVoxel = static_cast<unsigned char*>
                                    (InterpolationVolume->GetScalarPointer(x, y, z));
                            interpVoxel[0] = acc;
                        }
                        else
                        {
                            ws += 1;
                        }
                    }
                }
            }
        }
    }

    std::cout << "fill final volume" << std::endl;

    for (int z = bounds[4]; z <= bounds[5]; z++)
    {
        for (int y = bounds[2]; y <= bounds[3]; y++)
        {
            for (int x = bounds[0]; x <= bounds[1]; x++)
            {                
                 interpVoxel = static_cast<unsigned char*>
                        (InterpolationVolume->GetScalarPointer(x, y, z));
                if (interpVoxel[0] != 0)
                {                   
                    voxel = static_cast<unsigned char *>
                            (volume->GetScalarPointer(x, y, z));
                    voxel[0] = interpVoxel[0];
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------

vtkMatrix4x4 * qSlicerVolumeReconstructionModuleWidget::
getVolumeTransformation(float xScale, float yScale, float zScale, float volumeScale)
{
    return this->rTp.GetPointer();
}
