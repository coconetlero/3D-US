/* 
 * File:   FeatureRegionGrowing.h
 * Author: zian
 *
 * Created on 27 de mayo de 2014, 08:43 PM
 */

#ifndef FEATUREREGIONGROWING_H
#define	FEATUREREGIONGROWING_H

#include <vector>
#include <UltrasoundSegmentationDefs.h>


// itk Includes
#include "itkImage.h"
#include "itkVector.h"

// vnl includes
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

// Image dimension
//const static unsigned int Dimension = 3;

// Pixel Types    
//typedef float PixelType;
//typedef unsigned char MaskPixelType;
typedef itk::Vector<PixelType, Dimension> EigenvaluePixelType;
typedef itk::Vector<itk::Vector<PixelType, Dimension>, Dimension> EigenvectorValueType;

// Image Types 
//typedef itk::Image< PixelType, Dimension > ImageType;
//typedef itk::Image< MaskPixelType, Dimension > MaskImageType;
typedef itk::Image< EigenvectorValueType, Dimension > EigenvectorImageType;
typedef itk::Image< EigenvaluePixelType, Dimension > EigenvalueImageType;

class FeatureRegionGrowing
{

public:
    FeatureRegionGrowing();

    FeatureRegionGrowing(const FeatureRegionGrowing& orig);

    virtual ~FeatureRegionGrowing();

    void SetImage(const FloatImageType::Pointer input);

    void SetEnhancedImage(const FloatImageType::Pointer enhancedImage);

    MaskImageType::Pointer Grow(MaskImageType::Pointer seeds,
            float P_bone, float P_tissue, float P_shadow,
            vnl_vector<float> M_bone, vnl_vector<float> M_tissue, vnl_vector<float> M_shadow,
            vnl_matrix<float> C_bone, vnl_matrix<float> C_tissue, vnl_matrix<float> C_shadow);

    /**
     * each row are the x,y,z coordinates of the seed points
     */
    std::vector<int> FindSeedPoints();

    /**
     * 
     * @param 
     * @return 
     */
    MaskImageType::Pointer SeedPointsToImage(std::vector< int > seedPoints);


    /** Get and Set Methods */
    void SetEigenvectors(EigenvectorImageType::Pointer eigenvectors);

    EigenvectorImageType::Pointer GetEigenvectors() const;
    void SetEigenvalues(EigenvalueImageType::Pointer eigenvalues);
    /** Get and Set Methods */

private:

    int ImageSize;

    FloatImageType::Pointer EnhancedImage;

    FloatImageType::Pointer InputImage;

    EigenvectorImageType::Pointer eigenvectors;

    EigenvalueImageType::Pointer eigenvalues;

    MaskImageType::Pointer CreateEmptyImage();

    int whichClass(float P_bone, float P_tissue, float P_shadow,
            vnl_vector<float> M_bone, vnl_vector<float> M_tissue, vnl_vector<float> M_shadow,
            vnl_matrix<float> C_bone, vnl_matrix<float> C_tissue, vnl_matrix<float> C_shadow,
            vnl_vector<float> V);


};

#endif	/* FEATUREREGIONGROWING_H */

