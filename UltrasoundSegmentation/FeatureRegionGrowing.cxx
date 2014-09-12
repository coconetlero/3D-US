/* 
 * File:   FeatureRegionGrowing.cxx
 * Author: zian
 * 
 * Created on 27 de mayo de 2014, 08:43 PM
 */

#include "FeatureRegionGrowing.h"

#include <vector>
#include <math.h>

// itk includes 
#include "itkStatisticsImageFilter.h"
#include "itkImageToHistogramFilter.hxx"
#include "itkImageIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkSpatialOrientationAdapter.h"


// vnl includes
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include "vnl/vnl_matrix_fixed.h" 
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_determinant.h>

FeatureRegionGrowing::FeatureRegionGrowing()
{
}

FeatureRegionGrowing::FeatureRegionGrowing(const FeatureRegionGrowing& orig)
{
}

FeatureRegionGrowing::~FeatureRegionGrowing()
{
}

// ----------------------------------------------------------------------------

void FeatureRegionGrowing::SetImage(const FloatImageType::Pointer inputImage)
{
    this->InputImage = inputImage;

    FloatImageType::RegionType region = this->InputImage->GetLargestPossibleRegion();
    FloatImageType::SizeType size = region.GetSize();
    this->ImageSize = size[0] * size[1] * size[2];
}

// ----------------------------------------------------------------------------

void FeatureRegionGrowing::SetEnhancedImage(const FloatImageType::Pointer enhancedImage)
{
    this->EnhancedImage = enhancedImage;

    FloatImageType::RegionType region = this->EnhancedImage->GetLargestPossibleRegion();
    FloatImageType::SizeType size = region.GetSize();
    this->ImageSize = size[0] * size[1] * size[2];
}


// ----------------------------------------------------------------------------

MaskImageType::Pointer FeatureRegionGrowing::Grow(MaskImageType::Pointer seeds,
        float P_bone, float P_tissue, float P_shadow,
        vnl_vector<float> M_bone, vnl_vector<float> M_tissue, vnl_vector<float> M_shadow,
        vnl_matrix<float> C_bone, vnl_matrix<float> C_tissue, vnl_matrix<float> C_shadow)
{
    // Create the output image   
    MaskImageType::Pointer visitedImage = CreateEmptyImage();

    // get some statistics
    // get min and max of the image
    typedef itk::StatisticsImageFilter<FloatImageType> StatisticsImageFilterType;
    StatisticsImageFilterType::Pointer statsImageFilter = StatisticsImageFilterType::New();
    statsImageFilter->SetInput(this->InputImage);
    statsImageFilter->Update();

    double stdv = statsImageFilter->GetSigma();
    double mean = statsImageFilter->GetMean();

    // iterate over the  hole image to obtain the pixels positions who are seeds
    typedef itk::ImageRegionIterator < FloatImageType > RegionIteratorType;
    typedef itk::ImageRegionIterator < MaskImageType > SeedRegionIteratorType;

    FloatImageType::RegionType region = this->InputImage->GetLargestPossibleRegion();
    RegionIteratorType imageIterator(this->InputImage, region);
    SeedRegionIteratorType seedIterator(seeds, region);
    SeedRegionIteratorType visitedIterator(visitedImage, region);

    // average seed value
    float asv = 0;

    // fill list for grow regions and update visited image with the seeds
    std::vector < MaskImageType::IndexType > visitedVoxels;

    int acc = 0;
    seedIterator.GoToBegin();
    visitedIterator.GoToBegin();
    imageIterator.GoToBegin();

    while (!seedIterator.IsAtEnd())
    {
        if (seedIterator.Get() != 0)
        {
            MaskImageType::IndexType currentIndex = visitedIterator.GetIndex();
            visitedVoxels.push_back(currentIndex);
            visitedIterator.Set(1);

            asv += imageIterator.Get();
            acc++;
        }
        ++seedIterator;
        ++visitedIterator;
        ++imageIterator;
    }

    asv /= acc;


    // iterate over the hole image to grow each seed    
    typedef itk::NeighborhoodIterator< FloatImageType > NeighborhoodIteratorType;
    typedef itk::NeighborhoodIterator< MaskImageType > VisitNeighborhoodIteratorType;
    typedef itk::NeighborhoodIterator< EigenvectorImageType > EigenvectorsIteratorType;
    typedef itk::NeighborhoodIterator< EigenvalueImageType > EigenvalueIteratorType;


    NeighborhoodIteratorType::RadiusType radius;
    radius.Fill(1);

    NeighborhoodIteratorType enhancedNeighborIterator(radius, this->EnhancedImage, region);
    NeighborhoodIteratorType inputNeighborIterator(radius, this->InputImage, region);
    VisitNeighborhoodIteratorType visitedNeighborIterator(radius, visitedImage, region);
    EigenvectorsIteratorType eigenvectorsIterator(radius, this->eigenvectors, region);
    EigenvalueIteratorType eigenvaluesIterator(radius, this->eigenvalues, region);

    std::cout << "size seeds = " << visitedVoxels.size() << std::endl;
    int seedSize = visitedVoxels.size();
    int sizeidx = 0;

    typedef itk::Vector< float, Dimension > EigenvectorValueType;


    // -------   bayes clasification

    float bone_const = -0.5f * std::log(vnl_determinant(C_bone)) + std::log(P_bone);
    float tissue_const = -0.5f * std::log(vnl_determinant(C_tissue)) + std::log(P_tissue);
    float shadow_const = -0.5f * std::log(vnl_determinant(C_shadow)) + std::log(P_shadow);

    vnl_matrix<float> C_bone_inv = vnl_matrix_inverse<float>(C_bone);
    vnl_matrix<float> C_tissue_inv = vnl_matrix_inverse<float>(C_tissue);
    vnl_matrix<float> C_shadow_inv = vnl_matrix_inverse<float>(C_shadow);

    // -------   bayes clasification




    while (!visitedVoxels.empty())
    {
        MaskImageType::IndexType currentIndex = visitedVoxels.front();

        enhancedNeighborIterator.SetLocation(currentIndex);
        inputNeighborIterator.SetLocation(currentIndex);
        visitedNeighborIterator.SetLocation(currentIndex);
        eigenvectorsIterator.SetLocation(currentIndex);
        eigenvaluesIterator.SetLocation(currentIndex);

        // seed value from the list
        float sv = inputNeighborIterator.GetCenterPixel();

        // decide which neighbor to grow

        EigenvectorValueType e_1 = eigenvectorsIterator.GetCenterPixel()[0];
        EigenvectorValueType e_2 = eigenvectorsIterator.GetCenterPixel()[1];
        EigenvectorValueType e_3 = eigenvectorsIterator.GetCenterPixel()[2];








        //
        //                std::cout << "e2 = " << e_2 << std::endl;
        //                std::cout << "e2 = " << e_2[0] << ", " << e_2[1] << ", " << e_2[2] << std::endl;
        //        std::cout << "pos- = " << currentIndex << std::endl;
        //        std::cout << "e2*c = " << (e_2 + current) << std::endl;
        //        std::cout << "e2*i = " << (e_2 + current_inv) << std::endl;
        //
        //        float th = 2;
        //
        //                ImageType::IndexType v1;
        //                v1[0] = (int) std::floor(currentIndex[0] + e_2[0] + 0.5);
        //                v1[1] = (int) std::floor(currentIndex[1] + e_2[1] + 0.5);
        //                v1[2] = (int) std::floor(currentIndex[2] + e_2[2] + 0.5);
        //        
        //                EigenvectorValueType C1_E3 = this->eigenvectors->GetPixel(v1)[2];
        //                EigenvectorValueType::ComponentType dotproduct = std::abs(e_3 * C1_E3);
        //        
        //                if (dotproduct > 0.3 && visitedImage->GetPixel(v1) == 0 && region.IsInside(v1))
        //                {
        //                    visitedVoxels.push_back(v1);
        //                    visitedImage->SetPixel(v1, 1);
        //                    std::cout << "pos v1 = " << v1 << std::endl;
        //                }
        //
        //        std::cout << "new E3 = " << C1_E3 << std::endl;
        //        std::cout << "dot(e3,ce3): " << dotproduct << std::endl;



        //        if (this->inputImage->GetPixel(v1) > th &&
        //                visitedImage->GetPixel(v1) == 0 && region.IsInside(v1))
        //        {
        //            visitedVoxels.push_back(v1);
        //            visitedImage->SetPixel(v1, 1);
        //            //            std::cout << "add v1 = " << v1 << std::endl;
        //        }
        //

        //
        //        ImageType::IndexType v2;
        //        v2[0] = (int) std::floor(currentIndex[0] + (e_2[0] * -1) + 0.5);
        //        v2[1] = (int) std::floor(currentIndex[1] + (e_2[1] * -1) + 0.5);
        //        v2[2] = (int) std::floor(currentIndex[2] + (e_2[2] * -1) + 0.5);
        //
        //        EigenvectorValueType C2_E3 = this->eigenvectors->GetPixel(v2)[2];
        //        EigenvectorValueType::ComponentType dp2 = std::abs(e_3 * C2_E3);
        //
        //
        //
        //
        //        if (dp2 > 0.8 && visitedImage->GetPixel(v2) == 0 && region.IsInside(v2))
        //        {
        //            visitedVoxels.push_back(v2);
        //            visitedImage->SetPixel(v2, 1);
        //            std::cout << "pos v2 = " << v2 << std::endl;
        //        }
        //        std::cout << "new E3 = " << C2_E3 << std::endl;
        //        std::cout << "dot(e3,ce3): " << dp2 << std::endl;




        //        ImageType::IndexType v3;
        //        v3[0] = (int) std::floor(currentIndex[0] + e_3[0] + 0.5);
        //        v3[1] = (int) std::floor(currentIndex[1] + e_3[1] + 0.5);
        //        v3[2] = (int) std::floor(currentIndex[2] + e_3[2] + 0.5);
        //        
        //        
        //        EigenvectorValueType C2_E3 = this->eigenvectors->GetPixel(v3)[2];
        //        EigenvectorValueType::ComponentType dp2 = std::abs(e_3 * C2_E3);
        //
        //        if (dp2 > 0.8 && visitedImage->GetPixel(v3) == 0 && region.IsInside(v3))
        //        {
        //            visitedVoxels.push_back(v3);
        //            visitedImage->SetPixel(v3, 1);
        //        }


        //
        ////        ImageType::IndexType v4;
        ////        v4[0] = (int) std::floor(current[0] + (e_2[0] * -1) + 0.5);
        ////        v4[1] = (int) std::floor(current[1] + (e_2[1] * -1) + 0.5);
        ////        v4[2] = (int) std::floor(current[2] + (e_2[2] * -1) + 0.5);
        ////
        ////        if (this->inputImage->GetPixel(v4) > th &&
        ////                visitedImage->GetPixel(v4) == 0)
        ////        {
        ////
        ////            visitedVoxels.push_back(v4);
        ////            visitedImage->SetPixel(v4, 1);
        ////        }
        //
        //
        //
        //
        //



        // ----------------------------------------------------------------------------        
        // grow for each neighbor
        //        for (unsigned int i = 0; i < 27; i++)
        //        {
        //            FloatImageType::IndexType imageIndex = neighborIterator.GetIndex(i);
        //            MaskImageType::IndexType visitedIndex = visitedNeighborIterator.GetIndex(i);
        //
        //
        //            // current image value            
        //            float cv = neighborIterator.GetPixel(i);
        //
        //            bool IsInBounds;
        //            unsigned char visited = visitedNeighborIterator.GetPixel(i, IsInBounds);
        //
        //            if (visited == 0 && IsInBounds)
        //            {
        //                EigenvaluePixelType current_eigenvalues = eigenvaluesIterator.GetPixel(i);
        //                float l1 = std::abs(current_eigenvalues[0]);
        //                float l2 = std::abs(current_eigenvalues[1]);
        //                float l3 = std::abs(current_eigenvalues[2]);
        //
        //                //                std::cout << "evs = " << current_eigenvalues << std::endl;
        //
        //                // condition to grow 
        //                float weight_vw = std::exp(-(((asv - cv) * (asv - cv))) / (2.0f * stdv * stdv));
        //
        //
        //                if (weight_vw > 0.01)
        //                {
        //                    visitedNeighborIterator.SetPixel(i, 1);
        //                    visitedVoxels.push_back(visitedNeighborIterator.GetIndex(i));
        //
        //                }
        //            }
        //        }

        // ----------------------------------------------------------------------------






        // ----------------------------------------------------------------------------




        


        for (unsigned int i = 0; i < 27; i++)
        {
            FloatImageType::IndexType imageIndex = inputNeighborIterator.GetIndex(i);
            MaskImageType::IndexType visitedIndex = visitedNeighborIterator.GetIndex(i);

            // current image value            
            float cv = inputNeighborIterator.GetPixel(i);
            float fv = enhancedNeighborIterator.GetPixel(i);

            bool IsInBounds;
            unsigned char visited = visitedNeighborIterator.GetPixel(i, IsInBounds);

            if (visited == 0 && IsInBounds)
            {
                std::cout << "classify voxel (" <<
                currentIndex[0] << ", " <<
                currentIndex[1] << ", " <<
                currentIndex[2] << ") " << std::endl;
                
                
                EigenvaluePixelType current_eigenvalues = eigenvaluesIterator.GetPixel(i);
                float l1 = std::abs(current_eigenvalues[0]);
                float l2 = std::abs(current_eigenvalues[1]);
                float l3 = std::abs(current_eigenvalues[2]);

                vnl_matrix<float> V_bone(M_bone.size(), 1);               
//                V_bone(0, 0) = (float) imageIndex[0] - M_bone(0);
//                V_bone(1, 0) = (float) imageIndex[1] - M_bone(1);
//                V_bone(2, 0) = (float) imageIndex[2] - M_bone(2);
//                V_bone(3, 0) = (float) cv - M_bone(3);
                V_bone(0, 0) = (float) cv - M_bone(0);
                V_bone(1, 0) = (float) fv - M_bone(1);

                vnl_matrix<float> V_tissue(M_bone.size(), 1);
//                V_tissue(0, 0) = (float) imageIndex[0] - M_tissue(0);
//                V_tissue(1, 0) = (float) imageIndex[1] - M_tissue(1);
//                V_tissue(2, 0) = (float) imageIndex[2] - M_tissue(2);
//                V_tissue(3, 0) = (float) cv - M_tissue(3);
                V_tissue(0, 0) = (float) cv - M_tissue(0);
                V_tissue(1, 0) = (float) fv - M_tissue(1);

                vnl_matrix<float> V_shadow(M_bone.size(), 1);
//                V_shadow(0, 0) = (float) imageIndex[0] - M_shadow(0);
//                V_shadow(1, 0) = (float) imageIndex[1] - M_shadow(1);
//                V_shadow(2, 0) = (float) imageIndex[2] - M_shadow(2);
//                V_shadow(3, 0) = (float) cv - M_shadow(3);
                V_shadow(0, 0) = (float) cv - M_shadow(0);
                V_shadow(1, 0) = (float) fv - M_shadow(1);

                vnl_matrix<float> bone_index = -0.5f * V_bone.transpose() *
                        C_bone_inv * V_bone + bone_const;
                vnl_matrix<float> tissue_index = -0.5f * V_tissue.transpose() *
                        C_tissue_inv * V_tissue + tissue_const;
                vnl_matrix<float> shadow_index = -0.5f * V_shadow.transpose() *
                        C_shadow_inv * V_shadow + shadow_const;


                std::cout << bone_index(0, 0) << ", " <<
                        tissue_index(0, 0) << ", " <<
                        shadow_index(0, 0) << ") " << std::endl;

                if (bone_index(0, 0) > tissue_index(0, 0) && bone_index(0, 0) > shadow_index(0, 0))
                {
                    visitedNeighborIterator.SetPixel(i, 1);
                    visitedVoxels.push_back(visitedNeighborIterator.GetIndex(i));
                }
            }
        }





        visitedVoxels.erase(visitedVoxels.begin());
    }
    return visitedImage;
}

// ----------------------------------------------------------------------------

std::vector<int> FeatureRegionGrowing::FindSeedPoints()
{
    // get min and max of the image
    typedef itk::StatisticsImageFilter<FloatImageType> StatisticsImageFilterType;
    StatisticsImageFilterType::Pointer statisticsImageFilter = StatisticsImageFilterType::New();
    statisticsImageFilter->SetInput(this->EnhancedImage);
    statisticsImageFilter->Update();

    float minPixelValue = statisticsImageFilter->GetMinimum();
    float maxPixelValue = statisticsImageFilter->GetMaximum();

    std::cerr << "Min val: " << minPixelValue << std::endl;
    std::cerr << "Max val: " << maxPixelValue << std::endl;

    // find image histogram 
    const unsigned int MeasurementVectorSize = 1; // Grayscale
    const unsigned int binsPerDimension =
            static_cast<unsigned int> (maxPixelValue - minPixelValue);
    typedef itk::Statistics::ImageToHistogramFilter< FloatImageType > ImageToHistogramFilterType;

    ImageToHistogramFilterType::HistogramType::MeasurementVectorType
    lowerBound(binsPerDimension);
    lowerBound.Fill((int) minPixelValue);

    ImageToHistogramFilterType::HistogramType::MeasurementVectorType
    upperBound(binsPerDimension);
    upperBound.Fill((int) maxPixelValue);

    ImageToHistogramFilterType::HistogramType::SizeType
    size(MeasurementVectorSize);
    size.Fill(binsPerDimension);

    ImageToHistogramFilterType::Pointer imageToHistogramFilter =
            ImageToHistogramFilterType::New();
    imageToHistogramFilter->SetInput(this->EnhancedImage);
    imageToHistogramFilter->SetHistogramBinMinimum(lowerBound);
    imageToHistogramFilter->SetHistogramBinMaximum(upperBound);
    imageToHistogramFilter->SetHistogramSize(size);



    try
    {
        imageToHistogramFilter->Update();
    }
    catch (itk::ExceptionObject & error)
    {
        std::cerr << "Catch Histogram Error: " << error << std::endl;
    }

    ImageToHistogramFilterType::HistogramType* histogram =
            imageToHistogramFilter->GetOutput();

    //get the 1% greates values  
    int percent = ImageSize * 0.001;

    int acc = 0;
    int threshold = 0;

    for (int i = histogram->GetSize()[0] - 1; i >= 0; i--)
    {
        if (acc < percent)
        {
            acc += (int) histogram->GetFrequency(i);
        }
        else
        {
            threshold = i + 1;
            percent = acc;
            break;
        }
    }
    std::cout << "threshold = " << threshold << std::endl;


    // iterate over the  hole image to obtain the pixels positions who are seeds
    typedef itk::ImageRegionIterator <FloatImageType> RegionIteratorType;

    FloatImageType::RegionType region = this->EnhancedImage->GetLargestPossibleRegion();
    RegionIteratorType imageIt(this->EnhancedImage, region);

    /// eigenvalues iteration
    typedef itk::ImageRegionIterator<EigenvalueImageType> EigenvalueIteratorType;
    EigenvalueIteratorType eigenvaluesIterator(this->eigenvalues, region);


    std::vector<int> positions;

    int imageIdx = 0;
    imageIt.GoToBegin();
    while (!imageIt.IsAtEnd())
    {
        float value = imageIt.Get();
        if (std::floor(value) > threshold)
        {
            positions.push_back(imageIdx);

            //            EigenvaluePixelType ev = eigenvaluesIterator.Get();
            //            std::cout << std::abs(ev[0]) << "\t" << std::abs(ev[1]) << "\t" <<
            //                    std::abs(ev[2]) << "\t" << imageIt.Get() << std::endl;
        }
        ++eigenvaluesIterator;
        ++imageIdx;
        ++imageIt;
    }

    std::cout << "finsh seeds" << std::endl;

    return positions;
}

// -----------------------------------------------------------------------------

MaskImageType::Pointer FeatureRegionGrowing::SeedPointsToImage(std::vector< int > seedPoints)
{
    // Create the output image
    MaskImageType::Pointer outputImage = this->CreateEmptyImage();

    // fill output image
    typedef itk::ImageRegionIterator<MaskImageType> IteratorImageType;
    IteratorImageType imageIterator(outputImage, outputImage->GetLargestPossibleRegion();
    int idx = 0;
    int vectorIdx = 0;
    while (!imageIterator.IsAtEnd())
    {
        if (vectorIdx < seedPoints.size() && seedPoints[vectorIdx] == idx)
        {
            imageIterator.Set(1);
            vectorIdx++;
        }
        else
        {
            imageIterator.Set(0);
        }
        ++idx;
        ++imageIterator;
    }
    return outputImage;
}

// -----------------------------------------------------------------------------

MaskImageType::Pointer FeatureRegionGrowing::CreateEmptyImage()
{
    MaskImageType::Pointer image = MaskImageType::New();
    MaskImageType::RegionType region = this->InputImage->GetLargestPossibleRegion();
    MaskImageType::PointType origin = this->InputImage->GetOrigin();
    MaskImageType::SpacingType spacing = this->InputImage->GetSpacing();

    image->SetRegions(region);
    image->Allocate();

    image->SetOrigin(origin);
    image->SetSpacing(spacing);
    image->FillBuffer(0.0);

    return image;
}


// -----------------------------------------------------------------------------



// -------------------------   Get and Set Methods -----------------------------

void FeatureRegionGrowing::SetEigenvectors(EigenvectorImageType::Pointer eigenvectors)
{

    this->eigenvectors = eigenvectors;
}

EigenvectorImageType::Pointer FeatureRegionGrowing::GetEigenvectors() const
{
    return eigenvectors;
}

void FeatureRegionGrowing::SetEigenvalues(EigenvalueImageType::Pointer eigenvalues)
{
    this->eigenvalues = eigenvalues;
}


