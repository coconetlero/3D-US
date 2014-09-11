/* 
 * File:   UltrasoundSegmentationDefs.h
 * Author: zian
 *
 * Created on 4 de julio de 2014, 02:35 PM
 */

#ifndef ULTRASOUNDSEGMENTATIONDEFS_H
#define	ULTRASOUNDSEGMENTATIONDEFS_H

// ITK Includes
#include "itkImage.h"

// Image dimension
const static unsigned int Dimension = 3;

// Pixel Types    
typedef float PixelType;
typedef unsigned char MaskPixelType;

// Image Types 
typedef itk::Image< PixelType, Dimension > FloatImageType;
typedef itk::Image< MaskPixelType, Dimension > UnsignedCharImageType;
typedef itk::Image< MaskPixelType, Dimension > MaskImageType;


#endif	/* ULTRASOUNDSEGMENTATIONDEFS_H */

