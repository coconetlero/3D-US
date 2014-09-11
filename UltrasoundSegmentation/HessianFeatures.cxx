#include "float.h"

#include "HessianFeatures.h"

// ITK Includes
#include "itkSymmetricSecondRankTensor.h"
#include "itkSymmetricEigenAnalysis.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"

#include "itkImageRegionIterator.h"
#include "itkSpatialOrientationAdapter.h"

#include "itkHessianToObjectnessMeasureImageFilter.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"


#include "itkImageFileWriter.h"

// VNL Includes
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

HessianFeatures::HessianFeatures()
{
}

HessianFeatures::HessianFeatures(const HessianFeatures& orig)
{
}

HessianFeatures::~HessianFeatures()
{
}

// ----------------------------------------------------------------------------

void HessianFeatures::SetImage(const FloatImageType::Pointer inputImage)
{

    this->InputImage = inputImage;

    this->Region = this->InputImage->GetLargestPossibleRegion();
    FloatImageType::SizeType size = this->Region.GetSize();
    this->ImageSize = size[0] * size[1] * size[2];
}

// ----------------------------------------------------------------------------

EigenvalueImageType::Pointer HessianFeatures::
EigenDecomposition(double sigma, bool SetMainEigendecomposition)
{
    typedef itk::SymmetricSecondRankTensor< PixelType, Dimension > TensorPixelType;
    typedef itk::Image< TensorPixelType, Dimension > TensorImageType;
    typedef itk::Vector< PixelType, Dimension > VectorPixelType;
    typedef itk::Vector< VectorPixelType, Dimension > MatrixPixelType;

    // Hessian Matrix
    typedef itk::HessianRecursiveGaussianImageFilter
            < FloatImageType, TensorImageType > HessianFilterType;

    std::cout << "compute hessian matrix at scale = " << sigma << std::endl;


    HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
    hessianFilter->SetSigma(sigma);
    hessianFilter->SetInput(this->InputImage);
    hessianFilter->Update();
    TensorImageType::Pointer hessianImage = hessianFilter->GetOutput();

    // Eigen Analysis 
    typedef itk::SymmetricEigenAnalysis
            < TensorPixelType, VectorPixelType, MatrixPixelType > EigenAnalysisType;

    // Find Eigenvalues and corresponding Eigenvectors
    EigenAnalysisType eigenAnalysis;
    eigenAnalysis.SetDimension(Dimension);
    eigenAnalysis.SetOrderEigenValues(true);


    // Construct output images 
    std::cout << "calculate eigen analysis" << std::endl;
    typedef itk::ImageRegionConstIteratorWithIndex< TensorImageType > TensorIteratorType;
    TensorIteratorType hess_It(hessianImage, hessianImage->GetLargestPossibleRegion());

    TensorImageType::IndexType hessianIndex;
    TensorImageType::PointType hessianPoint;
    TensorPixelType hessianMatrix;

    VectorPixelType currentEigenValues;
    MatrixPixelType currentEigenVectors;

    // Create eigenvalue and eigenvector images     
    this->eigenvalues = EigenvalueImageType::New();
    this->eigenvalues->SetRegions(this->Region);
    this->eigenvalues->Allocate();

    this->eigenvectors = EigenvectorImageType::New();
    this->eigenvectors->SetRegions(this->Region);
    this->eigenvectors->Allocate();



    // create iterators over eigen values an vectors    
    typedef itk::ImageRegionIterator<EigenvalueImageType> EigenvaluesIteratorType;
    EigenvaluesIteratorType eigenvalueIterator(this->eigenvalues, this->Region);
    typedef itk::ImageRegionIterator<EigenvectorImageType> EigenvectorsIteratorType;
    EigenvectorsIteratorType eigenvectorIterator(this->eigenvectors, this->Region);


    // calculating the hessian matrix
    eigenvectorIterator.GoToBegin();
    eigenvalueIterator.GoToBegin();
    while (!hess_It.IsAtEnd())
    {
        hessianIndex = hess_It.GetIndex();
        hessianImage->TransformIndexToPhysicalPoint(hessianIndex, hessianPoint);

        hessianMatrix = hess_It.Get();
        eigenAnalysis.ComputeEigenValuesAndVectors(hessianMatrix, currentEigenValues, currentEigenVectors);

        EigenvaluePixelType newEigenvalues;
        newEigenvalues[0] = currentEigenValues[2];
        newEigenvalues[1] = currentEigenValues[1];
        newEigenvalues[2] = currentEigenValues[0];


        eigenvalueIterator.Set(newEigenvalues);
        eigenvectorIterator.Set(currentEigenVectors);




        ++hess_It;
        ++eigenvectorIterator;
        ++eigenvalueIterator;
    }

    return eigenvalues;
}

// ----------------------------------------------------------------------------

FloatImageType::Pointer HessianFeatures::Surfaceness(EigenvalueImageType::Pointer eigenvalues, double sigma)
{
    FloatImageType::Pointer surfaceness = this->CreateEmptyImage();

//    std::cout << "saving enhanced volume" << std::endl;
//    typedef itk::ImageFileWriter< FloatImageType > ImageWriterType;
//    ImageWriterType::Pointer writer = ImageWriterType::New();
//    writer->SetFileName("/Users/zian/Pictures/regionGrowing_tests/empty.mha");
//    writer->SetInput(surfaceness);
//    writer->Update();

    // create iterators over eigenvalues
    typedef itk::ImageRegionIterator<EigenvalueImageType> EigenvaluesIteratorType;
    EigenvaluesIteratorType eigenvalueIterator(eigenvalues, this->Region);
    
    typedef itk::ImageRegionIterator<FloatImageType> SurfacenessIteratorType;
    SurfacenessIteratorType surfacenessIterator(surfaceness, this->Region);

    eigenvalueIterator.GoToBegin();
    surfacenessIterator.GoToBegin();
    while (!eigenvalueIterator.IsAtEnd())
    {
        EigenvaluePixelType values = eigenvalueIterator.Get();
        double l1 = values[0];
        double l2 = values[1];
        double l3 = values[2];

        if (l3 < 0)
        {
            FloatImageType::PixelType s = std::pow(sigma, 2) * std::abs(l3) *
                    W(l2, l3, alpha, gamma) * W(l1, l2, alpha, gamma);

            surfacenessIterator.Set(s);
        }

        ++eigenvalueIterator;
        ++surfacenessIterator;
    }

    return surfaceness;
}

// ----------------------------------------------------------------------------

FloatImageType::Pointer HessianFeatures::Tubeness(EigenvalueImageType::Pointer eigenvalues, double sigma)
{
    // Create surfaceness image     
    FloatImageType::Pointer tubeness = FloatImageType::New();
    tubeness = FloatImageType::New();
    tubeness->SetRegions(this->Region);
    tubeness->Allocate();

    // create iterators over eigen values
    typedef itk::ImageRegionIterator<EigenvalueImageType> EigenvaluesIteratorType;
    EigenvaluesIteratorType eigenvalueIterator(eigenvalues, this->Region);
    typedef itk::ImageRegionIterator<FloatImageType> TubenessIteratorType;
    TubenessIteratorType tubenessIterator(tubeness, this->Region);

    eigenvalueIterator.GoToBegin();
    tubenessIterator.GoToBegin();
    while (!eigenvalueIterator.IsAtEnd())
    {
        EigenvaluePixelType values = eigenvalueIterator.Get();
        double l1 = values[0];
        double l2 = values[1];
        double l3 = values[2];

        if (l3 < 0)
        {
            double t = std::pow(sigma, 2) *
                    std::abs(l3) * Fi(l2, l3, gamma) * W(l1, l2, alpha, gamma);
            tubenessIterator.Set(t);
        }

        ++eigenvalueIterator;
        ++tubenessIterator;
    }
    return tubeness;
}

// ----------------------------------------------------------------------------

FloatImageType::Pointer HessianFeatures::Blobness(EigenvalueImageType::Pointer eigenvalues, double sigma)
{
    // Create surfaceness image     
    FloatImageType::Pointer blobness = FloatImageType::New();
    blobness = FloatImageType::New();
    blobness->SetRegions(this->Region);
    blobness->Allocate();

    // create iterators over eigen values
    typedef itk::ImageRegionIterator<EigenvalueImageType> EigenvaluesIteratorType;
    EigenvaluesIteratorType eigenvalueIterator(eigenvalues, this->Region);
    typedef itk::ImageRegionIterator<FloatImageType> BlobnessIteratorType;
    BlobnessIteratorType blobnessIterator(blobness, this->Region);

    eigenvalueIterator.GoToBegin();
    blobnessIterator.GoToBegin();
    while (!eigenvalueIterator.IsAtEnd())
    {
        EigenvaluePixelType values = eigenvalueIterator.Get();
        double l1 = values[0];
        double l2 = values[1];
        double l3 = values[2];

        if (l3 < 0)
        {
            double b = std::pow(sigma, 2) *
                    std::abs(l3) * Fi(l2, l3, gamma) * Fi(l1, l2, gamma);
            blobnessIterator.Set(b);
        }

        ++eigenvalueIterator;
        ++blobnessIterator;
    }

    return blobness;
}


// ----------------------------------------------------------------------------

FloatImageType::Pointer HessianFeatures::MultiscaleSurfaceness(int NumberOfSigmaSteps)
{
    FloatImageType::Pointer surfaceness = this->CreateEmptyImage();
    surfaceness->FillBuffer(-DBL_MIN);
    
    typedef itk::ImageRegionIterator<FloatImageType> ImageIteratorType;
    typedef itk::ImageRegionIterator<EigenvalueImageType> EigenvalueIteratorType;

    int scaleLevel = 1;
    double sigma = this->SigmaMin;


    while (sigma < SigmaMax)
    {
        EigenvalueImageType::Pointer current_eigenvalues = this->EigenDecomposition(sigma, false);
        FloatImageType::Pointer currentSurfaceness = this->Surfaceness(current_eigenvalues, sigma);

        ImageIteratorType surfacenessIterator(surfaceness, this->Region);
        ImageIteratorType currentSurfacenessIterator(currentSurfaceness, this->Region);
        EigenvalueIteratorType mainEigenvaluesIterator(this->eigenvalues, this->Region);
        EigenvalueIteratorType eigenvaluesIterator(current_eigenvalues, this->Region);

        surfacenessIterator.GoToBegin();
        currentSurfacenessIterator.GoToBegin();
        mainEigenvaluesIterator.GoToBegin();
        eigenvaluesIterator.GoToBegin();
        while (!surfacenessIterator.IsAtEnd())
        {
            // surfaceness value (sv)
            float sv = surfacenessIterator.Get();
            // current surfaceness value (csv)
            float current_sv = currentSurfacenessIterator.Get();

            if (current_sv > sv)
            {
                surfacenessIterator.Set(current_sv);
                mainEigenvaluesIterator.Set(eigenvaluesIterator.Get());
                //                std::cout << "current = " << current_sv << ", sv = " << sv << std::endl;
            }

            ++eigenvaluesIterator;
            ++mainEigenvaluesIterator;
            ++surfacenessIterator;
            ++currentSurfacenessIterator;
        }

        // update the sigma
        switch (this->SigmaStepsType)
        {
            case 0: // equispaced
            {
                const double stepSize = std::max(1e-10, (SigmaMax - SigmaMin) / NumberOfSigmaSteps);
                sigma = SigmaMin + stepSize * scaleLevel;
                break;
            }
            case 1: // logarithmic spaced
            {
                const double stepSize = std::max(1e-10, (vcl_log(SigmaMax) - vcl_log(SigmaMin)) / NumberOfSigmaSteps);
                sigma = vcl_exp(vcl_log(SigmaMin) + stepSize * scaleLevel);
                break;
            }
            default:
                std::cout << "Invalid SigmaStepMethod." << std::endl;
                //                throw ExceptionObject(__FILE__, __LINE__, "Invalid SigmaStepMethod.", ITK_LOCATION);

                break;
        }
        scaleLevel++;
    }
    


    return surfaceness;
}


// ----------------------------------------------------------------------------

FloatImageType::Pointer HessianFeatures::MultiscaleTubeness(int NumberOfSigmaSteps)
{
    FloatImageType::Pointer tubeness = this->CreateEmptyImage();
    //    tubeness->FillBuffer((float) DBL_MIN);

    typedef itk::ImageRegionIterator<FloatImageType> ImageIteratorType;
    int scaleLevel = 1;
    double sigma = this->SigmaMin;



    while (sigma < SigmaMax)
    {
        EigenvalueImageType::Pointer eigenvalues = this->EigenDecomposition(sigma, false);
        FloatImageType::Pointer currentTubeness = this->Surfaceness(eigenvalues, sigma);

        ImageIteratorType tubenessIterator(tubeness, this->Region);
        ImageIteratorType currentTubenessIterator(currentTubeness, this->Region);



        tubenessIterator.GoToBegin();
        while (!tubenessIterator.IsAtEnd())
        {
            // surfaceness value (sv)
            float sv = tubenessIterator.Get();
            // current surfaceness value (csv)
            float current_sv = currentTubenessIterator.Get();

            if (current_sv > sv)
            {
                tubenessIterator.Set(current_sv);
            }
            ++tubenessIterator;
            ++currentTubenessIterator;
        }

        // update the sigma
        switch (this->SigmaStepsType)
        {
            case 0: // equispaced
            {
                const double stepSize = std::max(1e-10, (SigmaMax - SigmaMin) / NumberOfSigmaSteps);
                sigma = SigmaMin + stepSize * scaleLevel;
                break;
            }
            case 1: // logarithmic spaced
            {
                const double stepSize = std::max(1e-10, (vcl_log(SigmaMax) - vcl_log(SigmaMin)) / NumberOfSigmaSteps);
                sigma = vcl_exp(vcl_log(SigmaMin) + stepSize * scaleLevel);
                break;
            }
            default:
                std::cout << "Invalid SigmaStepMethod." << std::endl;
                //                throw ExceptionObject(__FILE__, __LINE__, "Invalid SigmaStepMethod.", ITK_LOCATION);

                break;
        }
        scaleLevel++;
    }

    return tubeness;
}


// ----------------------------------------------------------------------------

float HessianFeatures::W(float ls, float lt, float alfa, float gamma)
{
    float a_lt = std::abs(lt);

    if (lt <= ls)
    {
        return std::pow(1 + (ls / a_lt), gamma);
    }
    else if ((a_lt / alfa) > ls)
    {
        return std::pow(1 - ((alfa) * (ls / a_lt)), gamma);
    }
    else
    {

        return 0.0;
    }
}

// ----------------------------------------------------------------------------

double HessianFeatures::Fi(double ls, double lt, double gamma)
{

    return std::pow((ls / lt), gamma);
}

// ----------------------------------------------------------------------------

float HessianFeatures::T(float l1, float l2, float l3, float alfa, float beta, float gamma)
{

    float l1_a = std::abs(l1);
    float l2_a = std::abs(l2);
    float l3_a = std::abs(l3);
    float ll1 = 0;
    float ll2 = 0;
    float ll3 = 0;

    if ((l1_a < l2_a) && (l1_a < l3_a))
    {
        ll1 = l1;
        if (l2_a < l3_a)
        {
            ll2 = l2;
            ll3 = l3;
        }
        else
        {
            ll2 = l3;
            ll3 = l2;
        }
    }
    else if ((l2_a < l1_a) && (l2_a < l3_a))
    {
        ll1 = l2;
        if (l1_a < l3_a)
        {
            ll2 = l1;
            ll3 = l3;
        }
        else
        {
            ll2 = l3;
            ll3 = l1;
        }
    }
    else
    {
        ll1 = l3;
        if (l1_a < l2_a)
        {
            ll2 = l1;
            ll3 = l2;
        }
        else
        {
            ll2 = l2;
            ll3 = l1;
        }
    }

    if (ll3 < 0)
    {

        // R sheet
        float Ra = std::abs(ll2) / std::abs(ll3);
        // R blob
        float Rb = std::abs((2 * ll3) - ll2 - ll1) / std::abs(ll3);
        // R noise
        float Rc = std::sqrt((ll1 * ll1) + (ll2 * ll2) + (ll3 * ll3));

        float T = std::exp(-(Ra * Ra) / (2 * alfa * alfa)) *
                (1 - std::exp(-(Rb * Rb) / (2 * beta * beta))) *
                (1 - std::exp(-(Rc * Rc) / (2 * gamma * gamma)));
        return T;
    }

    else
        return 0;
}


// ----------------------------------------------------------------------------

FloatImageType::Pointer HessianFeatures::CreateEmptyImage()
{    
    FloatImageType::Pointer emptyImage = FloatImageType::New();
    FloatImageType::PointType origin = this->InputImage->GetOrigin();
    FloatImageType::SpacingType spacing = this->InputImage->GetSpacing();
    
    emptyImage->SetRegions(this->Region);
    emptyImage->Allocate();

    emptyImage->SetOrigin(origin);
    emptyImage->SetSpacing(spacing);
    emptyImage->FillBuffer(0.0);

    return emptyImage;
}


// ----------------------------------------------------------------------------

void HessianFeatures::SetSigmaMin(double _SigmaMin)
{
    this->SigmaMin = _SigmaMin;
}

// ----------------------------------------------------------------------------

void HessianFeatures::SetSigmaMax(double _SigmaMax)
{
    this->SigmaMax = _SigmaMax;
}

// ----------------------------------------------------------------------------

void HessianFeatures::SetGamma(double gamma)
{
    this->gamma = gamma;
}

double HessianFeatures::GetGamma() const
{
    return gamma;
}

// ----------------------------------------------------------------------------

void HessianFeatures::SetBeta(double beta)
{
    this->beta = beta;
}

double HessianFeatures::GetBeta() const
{
    return beta;
}

// ----------------------------------------------------------------------------

void HessianFeatures::SetAlpha(double alpha)
{
    this->alpha = alpha;
}

double HessianFeatures::GetAlpha() const
{
    return alpha;
}

// ----------------------------------------------------------------------------

void HessianFeatures::SetSigmaStepMethodToEquispaced()
{
    this->SigmaStepsType = 0;
}

// ----------------------------------------------------------------------------

EigenvectorImageType::Pointer HessianFeatures::GetEigenvectors() const
{
    return eigenvectors;
}

void HessianFeatures::SetEigenvalues(EigenvalueImageType::Pointer eigenvalues)
{
    this->eigenvalues = eigenvalues;
}

EigenvalueImageType::Pointer HessianFeatures::GetEigenvalues() const
{
    return this->eigenvalues;
}