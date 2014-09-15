/* 
 * File:   GraphCutSegmentation.cxx
 * Author: zian
 * 
 * Created on 12 de marzo de 2014, 14:24
 */

#include "GraphCutSegmentation.h"

// itk imports
#include <itkNeighborhoodIterator.h>
#include <itkImageRegionIterator.h>

GraphCutSegmentation::GraphCutSegmentation()
{
}

GraphCutSegmentation::GraphCutSegmentation(const GraphCutSegmentation& orig)
{
}

GraphCutSegmentation::~GraphCutSegmentation()
{
}

// ---------------------------------------------------------------------------

ImageType::Pointer GraphCutSegmentation::segment(ImageType::Pointer inputImage,
        double obj_mean, double obj_stdv, double bkg_mean, double bkg_stdv,
        double im_stdv)
{
    // extract image caracteristics
    ImageType::PointType origin = inputImage->GetOrigin();
    ImageType::SpacingType spacing = inputImage->GetSpacing();
    ImageType::RegionType region = inputImage->GetLargestPossibleRegion();

    ImageType::SizeType size = region.GetSize();
    int width = size[0];
    int height = size[1];
    int deept = size[2];


    std::cout << "Volume size: " << "x:" << width << " y:" << height << " z:" << deept << std::endl;
    std::cout << "Origen: " << origin << std::endl;
    std::cout << "Spacing " << spacing << std::endl;

    typedef itk::NeighborhoodIterator<ImageType> NeighborhoodIteratorType;

    // Neighborhood size
    NeighborhoodIteratorType::RadiusType radius;
    radius.Fill(1);

    NeighborhoodIteratorType iterator(radius, inputImage, region);

    // create the offsets
    NeighborhoodIteratorType::OffsetType offset0 = {{0, 0, 0}};
    NeighborhoodIteratorType::OffsetType offset1 = {{1, 0, 0}};
    NeighborhoodIteratorType::OffsetType offset2 = {{0, 1, 0}};
    NeighborhoodIteratorType::OffsetType offset3 = {{-1, 0, 0}};
    NeighborhoodIteratorType::OffsetType offset4 = {{0, -1, 0}};
    NeighborhoodIteratorType::OffsetType offset5 = {{0, 0, -1}};
    NeighborhoodIteratorType::OffsetType offset6 = {{0, 0, 1}};


    // build graph with boost BGL


    //a graph with n + 2 vertices
    long source = width * height * deept;
    long target = width * height * deept + 1;

    // start mesuring time
    std::cout << "start counting time " << std::endl;
    clock_t begin = clock();
    // -------- mesuring time ----------


    Graph g((width * height * deept) + 2);

    property_map <Graph, edge_reverse_t>::type rev = get(edge_reverse, g);

    // add edges from source to image vertexes by iterating over the image
    for (iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator)
    {
        // Get the value of the current pixel
        double pv = iterator.GetCenterPixel();

        //        double d1 = (2.0 * obj_stdv * obj_stdv);
        //        double d2 = (pv - obj_mean) * (pv - obj_mean);
        //        double d3 = d2 / d1;
        //        double d4 = std::exp(-d3);


        double weight_sp = std::exp(-(((pv - obj_mean) * (pv - obj_mean))) /
                (2.0 * obj_stdv * obj_stdv));

        double weight_pt = std::exp(-(((pv - bkg_mean) * (pv - bkg_mean))) /
                (2.0 * bkg_stdv * bkg_stdv));

        ImageType::IndexType imageIndex = iterator.GetIndex();
        long x = imageIndex[0];
        long y = imageIndex[1];
        long z = imageIndex[2];

        long edgeSource = width * (y + (height * z)) + x;

        AddEdge(source, edgeSource, rev, weight_sp, g);
        AddEdge(edgeSource, target, rev, weight_pt, g);

        /// terminar de agregar las aristas para cada pixel

        bool IsInBounds;
        float pw = iterator.GetPixel(offset1, IsInBounds);
        float weight_vw = 1.0;
        if (IsInBounds)
        {
            if (pv > pw)
            {
                weight_vw = std::exp(-(((pv - pw) * (pv - pw))) / (2.0f * im_stdv * im_stdv));
            }
            long edgeTarget = width * ((y + offset1[1]) + (height * (z + offset1[2]))) + (x + offset1[0]);
            AddEdge(edgeSource, edgeTarget, rev, weight_vw, g);
        }

        pw = iterator.GetPixel(offset2, IsInBounds);
        weight_vw = 1.0;
        if (IsInBounds)
        {
            if (pv > pw)
            {
                weight_vw = std::exp(-(((pv - pw) * (pv - pw))) / (2.0f * im_stdv * im_stdv));
            }
            long edgeTarget = width * ((y + offset2[1]) + (height * (z + offset2[2]))) + (x + offset2[0]);
            AddEdge(edgeSource, edgeTarget, rev, weight_vw, g);
        }

        pw = iterator.GetPixel(offset3, IsInBounds);
        weight_vw = 1.0;
        if (IsInBounds)
        {
            if (pv > pw)
            {
                weight_vw = std::exp(-(((pv - pw) * (pv - pw))) / (2.0f * im_stdv * im_stdv));
            }
            long edgeTarget = width * ((y + offset3[1]) + (height * (z + offset3[2]))) + (x + offset3[0]);
            AddEdge(edgeSource, edgeTarget, rev, weight_vw, g);
        }

        pw = iterator.GetPixel(offset4, IsInBounds);
        weight_vw = 1.0;
        if (IsInBounds)
        {
            if (pv > pw)
            {
                weight_vw = std::exp(-(((pv - pw) * (pv - pw))) / (2.0f * im_stdv * im_stdv));
            }
            long edgeTarget = width * ((y + offset4[1]) + (height * (z + offset4[2]))) + (x + offset4[0]);
            AddEdge(edgeSource, edgeTarget, rev, weight_vw, g);
        }

        pw = iterator.GetPixel(offset5, IsInBounds);
        weight_vw = 1.0;
        if (IsInBounds)
        {
            if (pv > pw)
            {
                weight_vw = std::exp(-(((pv - pw) * (pv - pw))) / (2.0f * im_stdv * im_stdv));
            }
            long edgeTarget = width * ((y + offset5[1]) + (height * (z + offset5[2]))) + (x + offset5[0]);
            AddEdge(edgeSource, edgeTarget, rev, weight_vw, g);
        }

        pw = iterator.GetPixel(offset6, IsInBounds);
        weight_vw = 1.0;
        if (IsInBounds)
        {
            if (pv > pw)
            {
                weight_vw = std::exp(-(((pv - pw) * (pv - pw))) / (2.0f * im_stdv * im_stdv));
            }
            long edgeTarget = width * ((y + offset6[1]) + (height * (z + offset6[2]))) + (x + offset6[0]);
            AddEdge(edgeSource, edgeTarget, rev, weight_vw, g);
        }

        if (weight_sp <= 0 || weight_pt <= 0 || weight_vw <= 0)
        {
//            std::cout << "sp: " << weight_sp << " pt:" << weight_pt << " vw: " << weight_vw << std::endl;
//            std::cout << "x: " << x << " y: " << y << " z: " << z << std::endl;
        }


    }

    // count partial time
    clock_t partial = clock();
    double diffticks = partial - begin;
    double diffms = diffticks / CLOCKS_PER_SEC;
    std::cout << "Elapsed time to build the graph: " << double(diffms) << " seconds" << std::endl;


    begin = clock();



    //find min cut
    // a list of sources will be returned in s, and a list of sinks will be returned in t
    double flow = boykov_kolmogorov_max_flow(g, source, target);
    std::cout << "Max flow is: " << flow << std::endl;


    // count partial time
    partial = clock();
    diffticks = partial - begin;
    diffms = diffticks / CLOCKS_PER_SEC;
    std::cout << "Elapsed time to find the mincut: " << double(diffms) << " seconds" << std::endl;





    begin = clock();
    
    // create segmented output image
    ImageType::Pointer segmentedImage = ImageType::New();
    segmentedImage->SetRegions(region);
    segmentedImage->Allocate();

    segmentedImage->SetSpacing(spacing);
    segmentedImage->SetOrigin(origin);

    graph_traits<Graph>::vertex_iterator u_iter, u_end;
    tie(u_iter, u_end) = vertices(g);

    typedef itk::ImageRegionIterator<ImageType> IteratorImageType;
    IteratorImageType outImageIterator(segmentedImage, region);


    std::cout << "Creating final volume DONE" << std::endl;

    while (!outImageIterator.IsAtEnd())
    {

        int pixelColor = get(vertex_color, g, *u_iter);

        if (pixelColor > 0)
            outImageIterator.Set(255);
        else
            outImageIterator.Set(0);

        ++outImageIterator;
        ++u_iter;
    }

    
    // count partial time
    partial = clock();
    diffticks = partial - begin;
    diffms = diffticks / CLOCKS_PER_SEC;
    std::cout << "Elapsed time building final volume: " << double(diffms) << " seconds" << std::endl;


    return segmentedImage;
}



// ---------------------------------------------------------------------------

void GraphCutSegmentation::AddEdge(long s, long t, property_map < Graph, edge_reverse_t >::type &rev,
        const double capacity,
        Graph &g)
{
    Traits::edge_descriptor e1 = add_edge(s, t, g).first;
    Traits::edge_descriptor e2 = add_edge(t, s, g).first;
    put(edge_capacity, g, e1, capacity);
    put(edge_capacity, g, e2, 0);


    rev[e1] = e2;
    rev[e2] = e1;
}
