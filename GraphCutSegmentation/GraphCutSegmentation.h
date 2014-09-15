/* 
 * File:   GraphCutSegmentation.h
 * Author: zian
 *
 * Created on 12 de marzo de 2014, 14:24
 */


// itk includes
#include <itkImage.h>

// Boost includes 
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

#ifndef GRAPHCUTSEGMENTATION_H
#define	GRAPHCUTSEGMENTATION_H

using namespace boost;

// -----------------------------     ITK typedefs     --------------------------
// Image dimension
const unsigned int D = 3;
// Pixel Types
//typedef unsigned char PixelType;
typedef unsigned char PixelType;
// Image Types 
typedef itk::Image< PixelType, D > ImageType;


//  --------------------------    Boost C++ typedefs   -------------------------
using namespace boost;

typedef adjacency_list_traits < vecS, vecS, directedS > Traits;

typedef adjacency_list < vecS, vecS, directedS,
property < vertex_name_t, long,
property < vertex_index_t, long,
property < vertex_color_t, default_color_type,
property < vertex_distance_t, long,
property < vertex_predecessor_t, Traits::edge_descriptor > > > > >,
property < edge_capacity_t, double,
property < edge_residual_capacity_t, double,
property < edge_reverse_t, Traits::edge_descriptor > > > > Graph;

/**
 * \brief
 */
class GraphCutSegmentation
{

public:
    GraphCutSegmentation();
    GraphCutSegmentation(const GraphCutSegmentation& orig);
    virtual ~GraphCutSegmentation();

    /**
     * \brief
     */
    ImageType::Pointer segment(ImageType::Pointer inputImage,
            double obj_mean, double obj_stdv, double bkg_mean, double bkg_stdv,
            double im_stdv);




private:

    /**
     * \brief Add an edge to this type of graph
     */
    void AddEdge(long s, long t, property_map < Graph, edge_reverse_t >::type &rev,
            const double capacity,
            Graph &g);

};

#endif	/* GRAPHCUTSEGMENTATION_H */

