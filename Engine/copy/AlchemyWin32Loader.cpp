#include "AlchemyWin32Loader.h"

#include "../Graphics/AlchemyARGBColor.h"

#include <Windows.h>
#include <setjmp.h>

extern "C"
{
	#include "jpeglib.h"
	#include "png.h"
	//#include "zlib.h"
};

#pragma comment(lib,"libjpeg.lib")
#pragma comment(lib,"libpng.lib")
#pragma comment(lib,"zlib.lib")

typedef struct 
{
  struct jpeg_error_mgr Public;	/* "public" fields */

  jmp_buf SetJumpBuffer;	/* for return to caller */
}ERRORMESSAGE, * LPERRORMESSAGE;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void) JPGErrorHandler(j_common_ptr pCommonInfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	LPERRORMESSAGE pErrorMessage = reinterpret_cast<LPERRORMESSAGE>(pCommonInfo->err);

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*pCommonInfo->err->output_message)(pCommonInfo);

	/* Return control to the setjmp point */
	longjmp(pErrorMessage->SetJumpBuffer, 1);
}

void PNGErrorHandler(png_structp pPNG, png_const_charp pMessage)
{
	
}

void PNGWarningHandler(png_structp pPNG, png_const_charp pMessage)
{
	ALCHEMY_DEBUG_WARNING(pMessage);
}

using namespace alchemy;

CWin32Loader::CWin32Loader(void)
{
}

CWin32Loader::~CWin32Loader(void)
{
}

void CWin32Loader::Load(const alchemy::CHAR* pcPath)
{
	/* More stuff */
	FILE * pInFile;		/* source file */
	
	/* In this example we want to open the input file before doing anything else,
	* so that the setjmp() error recovery below can assume the file is open.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to read binary files.
	*/

	char* pcCurrentPath = ALCHEMY_NULL;
#ifdef _UNICODE
	UINT uLength = WideCharToMultiByte( 
		CP_ACP,
		0,
		pcPath,
		- 1,
		ALCHEMY_NULL,
		0,
		ALCHEMY_NULL,
		ALCHEMY_NULL);

	ALCHEMY_DEBUG_NEW(pcCurrentPath, char[uLength + 1]);

	WideCharToMultiByte(
		CP_ACP,
		0,
		pcPath,
		- 1,
		pcCurrentPath,
		uLength,
		ALCHEMY_NULL,
		ALCHEMY_NULL);
#else
	pcCurrentPath = const_cast<char*>(pcPath);
#endif

	if( (pInFile = fopen(pcCurrentPath, "rb") ) == NULL) 
	{
		fprintf(stderr, "can't open %s\n", pcCurrentPath);

		return;
	}

	CLoader::DATA Data;
	Data.pData = ALCHEMY_NULL;

	STRING Extension;
	GetPathInfo(pcPath, ALCHEMY_NULL, ALCHEMY_NULL, &Extension);
	if( Extension == ALCHEMY_TEXT("jpg") || Extension == ALCHEMY_TEXT("jpeg") )
	{
		/* Step 1: allocate and initialize JPEG decompression object */

		/* This struct contains the JPEG decompression parameters and pointers to
		* working space (which is allocated as needed by the JPEG library).
		*/
		struct jpeg_decompress_struct CommonInfo;

		/* We use our private extension JPEG error handler.
		* Note that this struct must live as long as the main JPEG parameter
		* struct, to avoid dangling-pointer problems.
		*/
		ERRORMESSAGE ErrorMessage;
		/* We set up the normal JPEG error routines, then override error_exit. */
		CommonInfo.err = jpeg_std_error(&ErrorMessage.Public);
		ErrorMessage.Public.error_exit = JPGErrorHandler;
		/* Establish the setjmp return context for my_error_exit to use. */
		if (setjmp(ErrorMessage.SetJumpBuffer)) 
		{
			/* If we get here, the JPEG code has signaled an error.
			* We need to clean up the JPEG object, close the input file, and return.
			*/
			jpeg_destroy_decompress(&CommonInfo);

			fclose(pInFile);

			return;
		}

		/* Now we can initialize the JPEG decompression object. */
		jpeg_create_decompress(&CommonInfo);

		/* Step 2: specify data source (eg, a file) */

		jpeg_stdio_src(&CommonInfo, pInFile);

		/* Step 3: read file parameters with jpeg_read_header() */

		(void) jpeg_read_header(&CommonInfo, TRUE);
		/* We can ignore the return value from jpeg_read_header since
		*   (a) suspension is not possible with the stdio data source, and
		*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
		* See libjpeg.txt for more info.
		*/

		/* Step 4: set parameters for decompression */

		/* In this example, we don't need to change any of the defaults set by
		* jpeg_read_header(), so we do nothing here.
		*/

		/* Step 5: Start decompressor */

		(void) jpeg_start_decompress(&CommonInfo);
		/* We can ignore the return value since suspension is not possible
		* with the stdio data source.
		*/

		/* We may need to do some setup of our own at this point before reading
		* the data.  After jpeg_start_decompress() we have the correct scaled
		* output image dimensions available, as well as the output colormap
		* if we asked for color quantization.
		* In this example, we need to make an output work buffer of the right size.
		*/ 
		/* JSAMPLEs per row in output buffer */

		alchemy::UINT RowStride = CommonInfo.output_width * CommonInfo.output_components; /* physical row width in output buffer */
		/* Make a one-row-high sample array that will go away when done with image */
		JSAMPARRAY Buffer = (*CommonInfo.mem->alloc_sarray) /* Output row buffer */
		((j_common_ptr) &CommonInfo, JPOOL_IMAGE, RowStride, 1);

		Data.uAlpha = CommonInfo.image_width;
		Data.uBeta  = CommonInfo.image_height;
		ALCHEMY_DEBUG_NEW(Data.pData, UINT8[4 * CommonInfo.image_width * CommonInfo.image_height]);
		ARGBCOLOR* pColor = reinterpret_cast<ARGBCOLOR*>(Data.pData);
		/* Step 6: while (scan lines remain to be read) */
		/*           jpeg_read_scanlines(...); */

		/* Here we use the library's state variable cinfo.output_scanline as the
		* loop counter, so that we don't have to keep track ourselves.
		*/
		alchemy::UINT i, uIndex;
		JSAMPROW Row;
		while(CommonInfo.output_scanline < CommonInfo.output_height) 
		{
			/* jpeg_read_scanlines expects an array of pointers to scanlines.
			* Here the array is only one element long, but you could ask for
			* more than one scanline at a time if that's more convenient.
			*/
			(void) jpeg_read_scanlines(&CommonInfo, Buffer, 1);
			/* Assume put_scanline_someplace wants a pointer and sample count. */
			Row = Buffer[0];
			uIndex = 0;
			for(i = 0; i < CommonInfo.image_width; ++ i)
			{
				*(pColor ++) = ALCHEMY_ARGBCOLOR_255(0xff, Row[uIndex], Row[uIndex + 1], Row[uIndex + 2]);

				uIndex += CommonInfo.output_components;
			}
		}

		/* Step 7: Finish decompression */

		(void) jpeg_finish_decompress(&CommonInfo);
		/* We can ignore the return value since suspension is not possible
		* with the stdio data source.
		*/

		/* Step 8: Release JPEG decompression object */

		/* This is an important step since it will release a good deal of memory. */
		jpeg_destroy_decompress(&CommonInfo);

		/* At this point you may want to check to see whether any corrupt-data
		* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
		*/

		/* And we're done! */
	}
	else if( Extension == ALCHEMY_TEXT("png") )
	{
		png_structp pPNG;

		/* Create and initialize the png_struct with the desired error handler
		* functions.  If you want to use the default stderr and longjump method,
		* you can supply NULL for the last three parameters.  We also supply the
		* the compiler header file version, so that we know if the application
		* was compiled with a compatible version of the library.  REQUIRED
		*/
		pPNG = png_create_read_struct(PNG_LIBPNG_VER_STRING, ALCHEMY_NULL, PNGErrorHandler, PNGWarningHandler);

		if(pPNG == ALCHEMY_NULL)
		{
			fclose(pInFile);

			return;
		}

		png_infop pInfo;
		/* Allocate/initialize the memory for image information.  REQUIRED. */
		pInfo = png_create_info_struct(pPNG);
		if(pInfo == ALCHEMY_NULL)
		{
			fclose(pInFile);

			png_destroy_read_struct(&pPNG, ALCHEMY_NULL, ALCHEMY_NULL);

			return;
		}

		/* Set error handling if you are using the setjmp/longjmp method (this is
		* the normal method of doing things with libpng).  REQUIRED unless you
		* set up your own error handlers in the png_create_read_struct() earlier.
		*/

		if( setjmp( png_jmpbuf(pPNG) ) )
		{
			/* Free all of the memory associated with the png_ptr and info_ptr */
			png_destroy_read_struct(&pPNG, &pInfo, ALCHEMY_NULL);

			fclose(pInFile);
			/* If we get here, we had a problem reading the file */
			return;
		}

		/* One of the following I/O initialization methods is REQUIRED */
		png_init_io(pPNG, pInFile);

		/* If we have already read some of the signature */
		//png_set_sig_bytes(pPNG, sig_read);

#ifdef hilevel
		/*
		* If you have enough memory to read in the entire image at once,
		* and you need to specify only transforms that can be controlled
		* with one of the PNG_TRANSFORM_* bits (this presently excludes
		* quantizing, filling, setting background, and doing gamma
		* adjustment), then you can read the entire image (including
		* pixels) into the info structure with this call:
		*/
		png_read_png(pPNG, pInfo, png_transforms, ALCHEMY_NULL);

#else
		/* OK, you're doing it the hard way, with the lower-level functions */

		/* The call to png_read_info() gives us all of the information from the
		* PNG file before the first IDAT (image data chunk).  REQUIRED
		*/
		png_read_info(pPNG, pInfo);

		INT nBitdepth, nColorType, nInterlaceType;
		png_get_IHDR(pPNG, pInfo, &Data.uAlpha, &Data.uBeta, &nBitdepth, &nColorType, &nInterlaceType, ALCHEMY_NULL, ALCHEMY_NULL);

		/* Set up the data transformations you want.  Note that these are all
		* optional.  Only call them if you want/need them.  Many of the
		* transformations only work on specific types of images, and many
		* are mutually exclusive.
		*/

		/* Tell libpng to strip 16 bit/color files down to 8 bits/color.
		* Use accurate scaling if it's available, otherwise just chop off the
		* low byte.
		*/
#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
		png_set_scale_16(pPNG);
#else
		png_set_strip_16(pPNG);
#endif

		/* Strip alpha bytes from the input data without combining with the
		* background (not recommended).
		*/
		png_set_strip_alpha(pPNG);

		/* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
		* byte into separate bytes (useful for paletted and grayscale images).
		*/
		png_set_packing(pPNG);

		/* Change the order of packed pixels to least significant bit first
		* (not useful if you are using png_set_packing). */
		png_set_packswap(pPNG);

		/* Expand paletted colors into true RGB triplets */
		if(nColorType == PNG_COLOR_TYPE_PALETTE)
		  png_set_palette_to_rgb(pPNG);

		/* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
		if(nColorType == PNG_COLOR_TYPE_GRAY && nBitdepth < 8)
		  png_set_expand_gray_1_2_4_to_8(pPNG);

		/* Expand paletted or RGB images with transparency to full alpha channels
		* so the data will be available as RGBA quartets.
		*/
		if(png_get_valid(pPNG, pInfo, PNG_INFO_tRNS) )
		  png_set_tRNS_to_alpha(pPNG);

		/* Set the background color to draw transparent and alpha images over.
		* It is possible to set the red, green, and blue components directly
		* for paletted images instead of supplying a palette index.  Note that
		* even if the PNG file supplies a background, you are not required to
		* use it - you should use the (solid) application background if it has one.
		*/

		const png_color_16 BACKGRAOUND = 
		{
			0, 
			0, 
			0, 
			0, 
			0
		};

		png_color_16* pImageBackground;

		if( png_get_bKGD(pPNG, pInfo, &pImageBackground) )
			png_set_background(pPNG, pImageBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
		else
			png_set_background(pPNG, &BACKGRAOUND, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);

		/* Some suggestions as to how to get a screen gamma value
		*
		* Note that screen gamma is the display_exponent, which includes
		* the CRT_exponent and any correction for viewing conditions
		*/
		//const FLOAT SCREEN_GAMMA = 1.0f;

		/* Tell libpng to handle the gamma conversion for you.  The final call
		* is a good guess for PC generated images, but it should be configurable
		* by the user at run time by the user.  It is strongly suggested that
		* your application support gamma correction.
		*/

		/*alchemy::INT nIntent;

		if (png_get_sRGB(pPNG, pInfo, &nIntent))
		  png_set_gamma(pPNG, SCREEN_GAMMA, 0.45455);
		else
		{
		  double ImageGamma;
		  if (png_get_gAMA(pPNG, pInfo, &ImageGamma))
			 png_set_gamma(pPNG, SCREEN_GAMMA, ImageGamma);
		  else
			 png_set_gamma(pPNG, SCREEN_GAMMA, 0.45455);
		}*/

#ifdef PNG_READ_QUANTIZE_SUPPORTED
		/* Quantize RGB files down to 8 bit palette or reduce palettes
		* to the number of colors available on your screen.
		*/
		/*if(nColorType & PNG_COLOR_MASK_COLOR)
		{
			alchemy::INT nPaletteCount;
			png_colorp pPalette;

			if(png_get_PLTE(pPNG, pInfo, &pPalette, &nPaletteCount))
			{
				png_uint_16p pHistogram = ALCHEMY_NULL;

				png_get_hIST(pPNG, pInfo, &pHistogram);

				png_set_quantize(pPNG, pPalette, nPaletteCount, max_screen_colors, pHistogram, 0);
			}
		}*/
#endif /* PNG_READ_QUANTIZE_SUPPORTED */

		/* Invert monochrome files to have 0 as white and 1 as black */
		png_set_invert_mono(pPNG);

		/* If you want to shift the pixel values from the range [0,255] or
		* [0,65535] to the original [0,7] or [0,31], or whatever range the
		* colors were originally in:
		*/
		if( png_get_valid(pPNG, pInfo, PNG_INFO_sBIT) )
		{
			png_color_8p sig_bit_p;

			png_get_sBIT(pPNG, pInfo, &sig_bit_p);
			png_set_shift(pPNG, sig_bit_p);
		}

		/* Flip the RGB pixels to BGR (or RGBA to BGRA) */
		if(nColorType & PNG_COLOR_MASK_COLOR)
			png_set_bgr(pPNG);

		/* Swap the RGBA or GA data to ARGB or AG (or BGRA to ABGR) */
		png_set_swap_alpha(pPNG);

		/* Swap bytes of 16 bit files to least significant byte first */
		png_set_swap(pPNG);

		/* Add filler (or alpha) byte (before/after each RGB triplet) */
		png_set_filler(pPNG, 0xff, PNG_FILLER_AFTER);

		/* Optional call to gamma correct and add the background to the palette
		* and update info structure.  REQUIRED if you are expecting libpng to
		* update the palette for you (ie you selected such a transform above).
		*/
		png_read_update_info(pPNG, pInfo);

		/* Allocate the memory to hold the image using the fields of info_ptr. */

		/* The easiest way to read the image: */
		png_bytepp ppRow;
		ALCHEMY_DEBUG_NEW(ppRow, png_bytep[Data.uBeta]);

		alchemy::UINT i;
		/* Clear the pointer array */
		for(i = 0; i < Data.uBeta; ++ i)
			ppRow[i] = ALCHEMY_NULL;

		UINT uRowBytes = png_get_rowbytes(pPNG, pInfo);
		for(i = 0; i < Data.uBeta; ++ i)
			ppRow[i] = reinterpret_cast<png_bytep>( png_malloc(pPNG, uRowBytes) );

		png_read_image(pPNG, ppRow);

		ALCHEMY_DEBUG_NEW(Data.pData, UINT8[Data.uAlpha * Data.uBeta * sizeof(ARGBCOLOR)]);

		alchemy::UINT j, uBlue, uGreen, uRed, uAlpha;
		png_bytep pRow;
		ARGBCOLOR* pColor = reinterpret_cast<ARGBCOLOR*>(Data.pData);
		for(i = 0; i < Data.uBeta; ++ i)
		{
			pRow = ppRow[i];
			for(j = 0; j < Data.uAlpha; ++ j)
			{
				uBlue = *(pRow ++);
				uGreen = *(pRow ++);
				uRed = *(pRow ++);
				uAlpha = *(pRow ++);
				*(pColor ++) = ALCHEMY_ARGBCOLOR_255(uAlpha, uRed, uGreen, uBlue);
			}
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(ppRow);

		/* Read rest of file, and get additional chunks in info_ptr - REQUIRED */
		png_read_end(pPNG, pInfo);
#endif hilevel

		/* At this point you have read the entire image */

		/* Clean up after the read, and free any memory allocated - REQUIRED */
		png_destroy_read_struct(&pPNG, &pInfo, ALCHEMY_NULL);
	}

	fclose(pInFile);

	alchemy::UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_COMPLETE, CLoader) )
	{
		CLoader::CLoadEvent Event(Data, pcPath, uType);

		DispatchEvent(Event);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(Data.pData);
}