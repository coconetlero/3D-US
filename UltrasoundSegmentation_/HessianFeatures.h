/* 
 * File:   HessianFeatures.h
 * Author: zian
 *
 * Created on 13 de mayo de 2014, 06:54 PM
 */

#ifndef HESSIANFEATURES_H
#define	HESSIANFEATURES_H

#include <vector>
#include <UltrasoundSegmentationDefs.h>

// ITK Includes
#include "itkImage.h"
#include "itkVector.h"

// VNL Includes
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"


// Image dimension
//const static unsigned int Dimension = 3;

// Pixel Types    
// typedef float PixelType;

typedef itk::Vector<itk::Vector<PixelType, Dimension>, Dimension> EigenvectorValueType;
typedef itk::Vector<PixelType, Dimension> EigenvaluePixelType;

// Image Types 
// typedef itk::Image< PixelType, Dimension > ImageType;
typedef itk::Image< EigenvectorValueType, Dimension > EigenvectorImageType;
typedef itk::Image< EigenvaluePixelType, Dimension > EigenvalueImageType;

class HessianFeatures
{

public:


    HessianFeatures();

    HessianFeatures(const HessianFeatures& orig);

    virtual ~HessianFeatures();

    void SetImage(const FloatImageType::Pointer input);

    EigenvalueImageType::Pointer EigenDecomposition(double sigma = 1,
            bool SetMainEigendecomposition = false);

    FloatImageType::Pointer Surfaceness(EigenvalueImageType::Pointer eigenvalues, double sigma);
    
    FloatImageType::Pointer Tubeness(EigenvalueImageType::Pointer eigenvalues, double sigma);

    FloatImageType::Pointer Blobness(EigenvalueImageType::Pointer eigenvalues, double sigma);

    FloatImageType::Pointer MultiscaleSurfaceness(int NFloatImageTypegmaSteps);

    FloatImageType::Pointer MultiscaleTubeness(int NumberOfSigmaSteps);

    void SetSigmaStepMethodToEquispaced();


    void SetSigmaMin(double _SigmaMin);
    double GetSigmaMin();
    void SetSigmaMax(double _SigmaMax);
    double GetSigmaMax();
    void SetGamma(double gamma);
    double GetGamma() const;
    void SetBeta(double beta);
    double GetBeta() const;
    void SetAlpha(double alpha);
    double GetAlpha() const;

    EigenvectorImageType::Pointer GetEigenvectors() const;
    void SetEigenvalues(EigenvalueImageType::Pointer eigenvalues);
    EigenvalueImageType::Pointer GetEigenvalues() const;




protected:


private:

    double SigmaMin;
    double SigmaMax;

    double alpha;
    double beta;
    double gamma;
    int SigmaStepsType;

    FloatImageType::Pointer InputImage;
    FloatImageType::RegionType Region;

    int ImageSize;

    EigenvalueImageType::Pointer eigenvalues;
    EigenvectorImageType::Pointer eigenvectors;

    float W(float ls, float lt, float alfa, float gamma);

    double Fi(double ls, double lt, double gamma);

    float T(float l1, float l2, float l3, float alpha, float beta, float gamma);

    FloatImageType::Pointer CreateEmptyImage();


};

#endif	/* HESSIANFEATURES_H */

