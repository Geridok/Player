
#ifndef __IMAGE_FORMATS_H
#define __IMAGE_FORMATS_H

enum
{
	FORMAT_FORWARD			= 0,
	FORMAT_WIDE				= 1,
	FORMAT_WIDE_ALPHA		= 2,
	FORMAT_DOUBLE			= 3,
	FORMAT_DOUBLE_ALPHA		= 4,
	FORMAT_UYVY				= FORMAT_FORWARD,
	FORMAT_YUY2				= 5,
	FORMAT_YVYU				= 6,
	FORMAT_Y16				= FORMAT_WIDE,
	FORMAT_Y16A				= FORMAT_WIDE_ALPHA,
	FORMAT_Y32				= FORMAT_DOUBLE,
	FORMAT_Y32A				= FORMAT_DOUBLE_ALPHA,
	FORMAT_BGR				= 7,
	FORMAT_BGRA				= 8,
	FORMAT_WIDE_ALPHA_RLE	= 9,
	FORMAT_Y16A_RLE			= FORMAT_WIDE_ALPHA_RLE,
};

#ifdef __cplusplus

__inline
DWORD ConvertNumToFcc(DWORD nFormatNumber)
{
	DWORD nToReturn;

	switch (nFormatNumber)
	{
	case FORMAT_UYVY:
		nToReturn = MAKEFOURCC('U','Y','V','Y');
		break;
		
	case FORMAT_YUY2:
		nToReturn = MAKEFOURCC('Y','U','Y','2');
		break;
		
	case FORMAT_YVYU:
		nToReturn = MAKEFOURCC('Y','V','Y','U');
		break;
		
	case FORMAT_Y16:
		nToReturn = MAKEFOURCC('Y','1','6',0);
		break;
		
	case FORMAT_Y16A:
		nToReturn = MAKEFOURCC('Y','1','6','A');
		break;

	case FORMAT_Y32:
		nToReturn = MAKEFOURCC('Y','3','2',0);
		break;
		
	case FORMAT_Y32A:
		nToReturn = MAKEFOURCC('Y','3','2','A');
		break;

	case FORMAT_BGR:
		nToReturn = MAKEFOURCC('B','G','R',0);
		break;

	case FORMAT_BGRA:
		nToReturn = MAKEFOURCC('B','G','R','A');
		break;

	case FORMAT_Y16A_RLE:
		nToReturn = MAKEFOURCC('Y','1','6','R');
		break;

	default:
		nToReturn = nFormatNumber;
		break;
	};

	return nToReturn;

}; //DWORD ConvertNumToFcc(DWORD nFormatNumber)

__inline
DWORD ConvertFccToNum(DWORD nFormatFcc)
{
	DWORD nToReturn;

	/* upper FCC */
	for (int i = 0;i < 4;i++)
	{
		char *lpc = reinterpret_cast<char *> (&nFormatFcc);
		lpc[i] = static_cast<char> ((('a' <= lpc[i]) && ('z' >= lpc[i])) ? (lpc[i] - 'a' + 'A') : (lpc[i]));
	};

	switch (nFormatFcc)
	{
	case MAKEFOURCC('U','Y','V','Y'):
		nToReturn = FORMAT_UYVY;
		break;
		
	case MAKEFOURCC('Y','U','Y','2'):
		nToReturn = FORMAT_YUY2;
		break;
		
	case MAKEFOURCC('Y','V','Y','U'):
		nToReturn = FORMAT_YVYU;
		break;
		
	case MAKEFOURCC('Y','1','6',0):
		nToReturn = FORMAT_Y16;
		break;
		
	case MAKEFOURCC('Y','1','6','A'):
		nToReturn = FORMAT_Y16A;
		break;

	case MAKEFOURCC('Y','3','2',0):
		nToReturn = FORMAT_Y32;
		break;
		
	case MAKEFOURCC('Y','3','2','A'):
		nToReturn = FORMAT_Y32A;
		break;

	case MAKEFOURCC('B','G','R',0):
		nToReturn = FORMAT_BGR;
		break;

	case MAKEFOURCC('B','G','R','A'):
		nToReturn = FORMAT_BGRA;
		break;

	case MAKEFOURCC('Y','1','6','R'):
		nToReturn = FORMAT_Y16A_RLE;
		break;

	default:
		nToReturn = nFormatFcc;
		break;
	};

	return nToReturn;

}; //DWORD ConvertFccToNum(DWORD nFormatFcc)

#endif //__cplusplus

#endif //__IMAGE_FORMATS_H