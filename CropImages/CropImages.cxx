#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"

#include "itkPluginUtilities.h"
#include "CropImagesCLP.h"

// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace
{

    template <class T>
    int DoIt(int argc, char * argv[], T)
    {
        PARSE_ARGS;

        typedef T InputPixelType;
        typedef T OutputPixelType;

        typedef itk::Image<InputPixelType, 3 > InputImageType;
        typedef itk::Image<OutputPixelType, 3 > OutputImageType;

        typedef itk::ImageFileReader<InputImageType> ReaderType;
        typedef itk::ImageFileWriter<OutputImageType> WriterType;

        typename ReaderType::Pointer reader = ReaderType::New();

        itk::PluginFilterWatcher watchReader(reader, "Read Sequence", CLPProcessInformation);

        reader->SetFileName(inputSequence.c_str());
        reader->Update();

        typename InputImageType::Pointer image = reader->GetOutput();
        typename InputImageType::SizeType dims =
                image->GetLargestPossibleRegion().GetSize();


        typename InputImageType::IndexType cStart;
        typename InputImageType::SizeType cSize;

        if (size == 4)
        {
            cStart[0] = 67;
            cStart[1] = 38;
            cStart[2] = 0;

            cSize[0] = 397;
            cSize[1] = 397;
            cSize[2] = dims[2];
        }
        else if (size == 5)
        {
            cStart[0] = 128;
            cStart[1] = 49;
            cStart[2] = 0;

            cSize[0] = 313;
            cSize[1] = 397;
            cSize[2] = dims[2];
        }
        else if (size == 6)
        {
            cStart[0] = 155;
            cStart[1] = 49;
            cStart[2] = 0;

            cSize[0] = 261;
            cSize[1] = 397;
            cSize[2] = dims[2];
        }
        else if (size == 8)
        {
            cStart[0] = 187;
            cStart[1] = 49;
            cStart[2] = 0;

            cSize[0] = 197;
            cSize[1] = 397;
            cSize[2] = dims[2];
        }

        typename InputImageType::RegionType cRegion(cStart, cSize);


        typedef itk::ExtractImageFilter< InputImageType, OutputImageType > FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        itk::PluginFilterWatcher watchFilter(filter, "Cropping image", CLPProcessInformation);

        filter->SetExtractionRegion(cRegion);
        filter->SetInput(image);
        filter->Update();

        typename WriterType::Pointer writer = WriterType::New();
        itk::PluginFilterWatcher watchWriter(writer, "Write Volume", CLPProcessInformation);
        writer->SetFileName(outputSequence.c_str());
        writer->SetInput(filter->GetOutput());
        writer->SetUseCompression(1);
        writer->Update();

        return EXIT_SUCCESS;
    }

} // end of anonymous namespace

int main(int argc, char * argv[])
{
    PARSE_ARGS;

    itk::ImageIOBase::IOPixelType pixelType;
    itk::ImageIOBase::IOComponentType componentType;

    try
    {
        itk::GetImageType(inputSequence, pixelType, componentType);

        // This filter handles all types on input, but only produces
        // signed types
        switch (componentType)
        {
            case itk::ImageIOBase::UCHAR:
                return DoIt(argc, argv, static_cast<unsigned char> (0));
                break;
            case itk::ImageIOBase::CHAR:
                return DoIt(argc, argv, static_cast<char> (0));
                break;
            case itk::ImageIOBase::USHORT:
                return DoIt(argc, argv, static_cast<unsigned short> (0));
                break;
            case itk::ImageIOBase::SHORT:
                return DoIt(argc, argv, static_cast<short> (0));
                break;
            case itk::ImageIOBase::UINT:
                return DoIt(argc, argv, static_cast<unsigned int> (0));
                break;
            case itk::ImageIOBase::INT:
                return DoIt(argc, argv, static_cast<int> (0));
                break;
            case itk::ImageIOBase::ULONG:
                return DoIt(argc, argv, static_cast<unsigned long> (0));
                break;
            case itk::ImageIOBase::LONG:
                return DoIt(argc, argv, static_cast<long> (0));
                break;
            case itk::ImageIOBase::FLOAT:
                return DoIt(argc, argv, static_cast<float> (0));
                break;
            case itk::ImageIOBase::DOUBLE:
                return DoIt(argc, argv, static_cast<double> (0));
                break;
            case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
            default:
                std::cout << "unknown component type" << std::endl;
                break;
        }
    }

    catch (itk::ExceptionObject & excep)
    {
        std::cerr << argv[0] << ": exception caught !" << std::endl;
        std::cerr << excep << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
