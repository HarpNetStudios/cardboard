// note: the code style in this file is fucking atrocious. i don't have the patience to deal with it. -Y

#ifdef WIN32
#include <windows.h> // For GetConsoleScreenBufferInfo() to get terminal width.
#else
#include <sys/ioctl.h> // For ioctl to get terminal width.
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <zlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include FT_GLYPH_H
#include FT_TYPE1_TABLES_H

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

// Attribute for printf-style format string checking.
// (Also see http://gcc.gnu.org/onlinedocs/gcc-3.1/gcc/Function-Attributes.html)
#ifdef __GNUC__
#define ATTR_PRINTF(fmtidx, checkidx) \
		__attribute__ ((format (printf, fmtidx, checkidx)))
#else
#define ATTR_PRINTF(fmtidx, checkidx)
#endif

//////////////////////////////////////////////////////////////////////////////
// Strings used for --version.  Update as needed.
// See http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
//////////////////////////////////////////////////////////////////////////////

#define APP_NAME "cbfont"
#define APP_VERSION "1.0"
#define APP_COPYRIGHT "\
Copyright (C) 2024 Quinton Reeves, Lee Salzman, Martin Erik Werner, Tim Thomas, Zsolt Zitting \n\
License: ZLIB (http://www.opensource.org/licenses/zlib-license.php) \n\
There is NO WARRANTY, to the extent permitted by law. \
"

//////////////////////////////////////////////////////////////////////////////
// Utility functions.
//////////////////////////////////////////////////////////////////////////////

void VMessage(FILE* stream, const char* fmt, va_list ap) ATTR_PRINTF(2, 0);
void Info(const char* fmt, ...) ATTR_PRINTF(1, 2);
void Warning(const char* fmt, ...) ATTR_PRINTF(1, 2);
void Fatal(const char* fmt, ...) ATTR_PRINTF(1, 2);

const char* program = NULL;

void
SetProgram(const char* progPath)
{
	const char* progBase = strrchr(progPath, '/');

	program = progBase ? progBase + 1 : progPath;
}

void
VMessage(FILE* stream, const char* fmt, va_list ap)
{
	if (program)
	{
		fprintf(stream, "%s: ", program);
	}

	vfprintf(stream, fmt, ap);
}

void
Info(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	VMessage(stdout, fmt, ap);
	va_end(ap);
	fputc('\n', stdout);
}

void
Warning(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	VMessage(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
}

void
Fatal(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	VMessage(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);

	exit(EXIT_FAILURE);
}

FILE*
FileOpen(const char* filename, const char* mode)
{
	FILE* f = fopen(filename, mode);

	if (!f)
	{
		Fatal("File open failed for '%s'.", filename);
	}

	return f;
}

void*
Allocate(void* curmem, size_t size, const char* what)
{
	void* newmem = realloc(curmem, size);

	if (!newmem)
	{
		Fatal("Allocation failed for %s.", what);
	}

	return newmem;
}

char*
AppFmtStr(char* curstr, const char* fmt, ...)
{
	va_list ap;
	int len;

	int curlen = curstr ? strlen(curstr) : 0;

	va_start(ap, fmt);
	// XXX http://msdn.microsoft.com/en-us/library/w05tbk72%28VS.71%29.aspx
	// _vscprintf()
	// #define _vscprintf(fmt, ap) vsnprintf(NULL, 0, fmt, ap);
	len = curlen + vsnprintf(NULL, 0, fmt, ap) + 1;
	va_end(ap);

	char* newstr = (char*)Allocate(curstr, len, "string");

	va_start(ap, fmt);
	vsnprintf(newstr + curlen, len - curlen, fmt, ap);
	va_end(ap);

	return newstr;
}

int
IMin(int a, int b)
{
	return a < b ? a : b;
}

int
IMax(int a, int b)
{
	return a > b ? a : b;
}

//////////////////////////////////////////////////////////////////////////////
// Color management.
//////////////////////////////////////////////////////////////////////////////

typedef struct
{
	uchar red;
	uchar green;
	uchar blue;
} ColorT, * ColorP;

typedef struct
{
	const char* name;
	ColorT color;
} NamedColorT, * NamedColorP;

// Definitions are for HTML colors:
//    http://en.wikipedia.org/wiki/Web_colors#HTML_color_names
// XXX List is not complete.
NamedColorT ColorList[] = {
	{ "black",      { 0x00, 0x00, 0x00 } },
	{ "white",      { 0xFF, 0xFF, 0xFF } },

	{ "aqua",       { 0x00, 0xFF, 0xFF } },

	{ "fuchsia",    { 0xFF, 0x00, 0xFF } },

	{ "red",        { 0xFF, 0x00, 0x00 } },
	{ "pink",       { 0xFF, 0xC0, 0xCB } },
	{ "orange",     { 0xFF, 0xA5, 0x00 } },
	{ "darkred",    { 0x8B, 0x00, 0x00 } },
	{ "maroon",     { 0x80, 0x00, 0x00 } },

	{ "yellow",     { 0xFF, 0xFF, 0x00 } },
	{ "olive",      { 0x80, 0x80, 0x00 } },

	{ "lime",       { 0x00, 0xFF, 0x00 } },
	{ "lightgreen", { 0x90, 0xEE, 0x90 } },
	{ "green",      { 0x00, 0x80, 0x00 } },
	{ "darkgreen",  { 0x00, 0x64, 0x00 } },

	{ "teal",       { 0x00, 0x80, 0x80 } },

	{ "blue",       { 0x00, 0x00, 0xFF } },
	{ "lightblue",  { 0xAD, 0xD8, 0xE6 } },
	{ "darkblue",   { 0x00, 0x00, 0x8B } },
	{ "navy",       { 0x00, 0x00, 0x80 } },

	{ "purple",     { 0x80, 0x00, 0x80 } },

	{ "gray",       { 0x80, 0x80, 0x80 } },
	{ "lightgray",  { 0xD3, 0xD3, 0xD3 } },
	{ "silver",     { 0xC0, 0xC0, 0xC0 } },
	{ "darkgray",   { 0xA9, 0xA9, 0xA9 } },

	{ NULL }
};

void
StringToColor(const char* colorString, ColorP colorp)
{
	NamedColorP colorlistp;
	int len = 0;
	uint red;
	uint green;
	uint blue;
	uint grayscale;

	for (colorlistp = ColorList; colorlistp->name; colorlistp++)
	{
		if (!strcmp(colorString, colorlistp->name))
		{
			*colorp = colorlistp->color;
			return;
		}
	}

	if (sscanf(colorString, "%02x%02x%02x%n", &red, &green, &blue, &len) == 3
		&& len == 6 && len == (int)strlen(colorString))
	{
		// RGB color.
	}
	else if (sscanf(colorString, "%u%n", &grayscale, &len) == 1
		&& len <= 3 && len == (int)strlen(colorString))
	{
		if (grayscale > UCHAR_MAX)
		{
			Fatal("Grayscale color must be between 0 and %d", UCHAR_MAX);
		}
		red = green = blue = grayscale;
	}
	else
	{
		Fatal("Unrecognized color '%s'", colorString);
	}

	//printf("parsed color %02x,%02x,%02x\n", red, green, blue);

	colorp->red = red;
	colorp->green = green;
	colorp->blue = blue;
}

const char*
ColorToString(ColorP colorp)
{
	static char colorVal[7]; // "rrggbb\0"
	NamedColorP colorlistp;

	for (colorlistp = ColorList; colorlistp->name; colorlistp++)
	{
		if (colorp->red == colorlistp->color.red
			&& colorp->green == colorlistp->color.green
			&& colorp->blue == colorlistp->color.blue
			)
		{
			return colorlistp->name;
		}
	}

	sprintf(colorVal, "%02X%02X%02X", colorp->red, colorp->green, colorp->blue);

	return colorVal;
}

uchar
RGBToGrayscale(ColorP colorp)
{
	// Return average.  Alternative equations are:
	//
	//    Lightness:  (max(R,G,B) + min(R,G,B)) / 2
	//    Luminosity: 0.2126 * R + 0.7152 * G + 0.0722 * B
	//
	// There are many other formulas for calculating luminosity, including
	// faster approximations:
	//    http://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color

	return (colorp->red + colorp->green + colorp->blue) / 3;
}

uchar
LerpColorValue(int from, int to, uchar lerpVal)
{
	int result = from + ((to - from) * lerpVal) / UCHAR_MAX;

	if (result < 0)
	{
		result = 0;
	}
	else if (result > UCHAR_MAX)
	{
		result = UCHAR_MAX;
	}

	return result;
}

#if 0
// Not currently needed.
void
LerpColor(ColorP from, ColorP to, ColorP lerpColor, uchar lerpVal)
{
	lerpColor->red = LerpColorValue(from->red, to->red, lerpVal);
	lerpColor->green = LerpColorValue(from->green, to->green, lerpVal);
	lerpColor->blue = LerpColorValue(from->blue, to->blue, lerpVal);
}
#endif

//////////////////////////////////////////////////////////////////////////////
// Option processing engine.
//////////////////////////////////////////////////////////////////////////////

#define IS_BOOL_OPT(type) ((type) == OptHelp || (type) == OptVersion \
	|| (type) == OptBool0 || (type) == OptBool1)

typedef enum
{
	OptVersion = -1, // Special type for version.
	OptHelp = 0,     // Special type for help.
	OptBool0 = 1,    // Regular option types start from 1.
	OptBool1,
	OptInt,
	OptFloat,
	OptString,
	OptArgv,
	OptColor,
} OptionType;

typedef void (*OptSet)(void* offset, const char* value, int isdefault);
typedef char* (*OptGet)(void* offset);

typedef struct
{
	const char* name;
	OptionType type;
	OptSet set;
	OptGet get;
} OptionTypeT, * OptionTypeP;

typedef struct
{
	const char* name;
	int short_opt;
	OptionType type;
	const char* description;
	const char* defval;
	size_t offset;
} OptionDefT, * OptionDefP;

typedef struct
{
	const char* name;
	const char* description;
	size_t offset;
} OptionArgDefT, * OptionArgDefP;

void
OptSetBool0(void* offset, const char* value, int isdefault)
{
	*(int*)offset = isdefault;
}

char*
OptGetBool0(void* offset)
{
	return *(int*)offset ? NULL : AppFmtStr(NULL, "true");
}

void
OptSetBool1(void* offset, const char* value, int isdefault)
{
	*(int*)offset = !isdefault;
}

char*
OptGetBool1(void* offset)
{
	return *(int*)offset ? AppFmtStr(NULL, "true") : NULL;
}

void
OptSetFloat(void* offset, const char* value, int isdefault)
{
	int len;
	if (sscanf(value, "%f%n", (float*)offset, &len) != 1
		&& len != (int)strlen(value))
	{
		Fatal("Could not convert '%s' to float", value);
	}
}

char*
OptGetFloat(void* offset)
{
	return AppFmtStr(NULL, "%g", *(float*)offset);
}

void
OptSetString(void* offset, const char* value, int isdefault)
{
	*(const char**)offset = value;
}

char*
OptGetString(void* offset)
{
	char* value = *(char**)offset;
	return value ? AppFmtStr(NULL, "%s", value) : NULL;
}

void
OptSetArgv(void* offset, const char* value, int isdefault)
{
	char** argv = *(char***)offset;

	if (isdefault)
	{
		argv = NULL;
	}

	// If we have a value, count the number of elements we currently have,
	// add two empty entries, fill the first empty one with the value, and the
	// last with a terminating NULL.  This isn't very efficient, but it
	// avoids requiring auxiliary data to track the array size.
	if (value)
	{
		int argc = 0;

		if (argv)
		{
			for (argc = 0; argv[argc]; argc++)
			{
				// Count the number of args.
			}
		}

		argc += 2; // Add room for the new entry and the terminating NULL.

		argv = (char**)Allocate(argv, argc * sizeof(char*), "argv");
		*(const char**)(argv + argc - 2) = value;
		*(argv + argc - 1) = NULL;
	}

	*(char***)offset = argv;
}

void
OptSetInt(void* offset, const char* value, int isdefault)
{
	*(int*)offset = atoi(value);
}

char*
OptGetInt(void* offset)
{
	return AppFmtStr(NULL, "%d", *(int*)offset);
}

void
OptSetColor(void* offset, const char* value, int isdefault)
{
	StringToColor(value, (ColorP)offset);
}

char*
OptGetColor(void* offset)
{
	const char* colorName = ColorToString((ColorP)offset);
	return AppFmtStr(NULL, "%s", colorName);
}

// List providing the mapping between option types and get/set functions, as
// well as a string tag used when displaying help for the option type.
OptionTypeT OptionTypeList[] = {
	{ "bool0", OptBool0, OptSetBool0, OptGetBool0 },
	{ "bool1", OptBool1, OptSetBool1, OptGetBool1 },
	{ "float", OptFloat, OptSetFloat, OptGetFloat },
	{ "int", OptInt, OptSetInt, OptGetInt },
	{ "string", OptString, OptSetString, OptGetString },
	{ "value", OptArgv, OptSetArgv, NULL },
	{ "color", OptColor, OptSetColor, OptGetColor },
	{ NULL }
};

const char*
GetTypeString(OptionDefP optp)
{
	OptionTypeP otp;
	for (otp = OptionTypeList; otp->name; otp++)
	{
		if (otp->type == optp->type)
		{
			return otp->name;
		}
	}

	return "value";
}

char*
GetTypeValue(void* prog_opts, OptionDefP optp)
{
	OptionTypeP otp;

	for (otp = OptionTypeList; otp->name; otp++)
	{
		if (otp->type == optp->type)
		{
			return otp->get((char*)prog_opts + optp->offset);
		}
	}

	// Should never occur.
	Fatal("Internal Error:  Option '%s' has unknown option type '%d'",
		optp->name, optp->type);

	return NULL;
}

void
SetOptionValue(void* prog_opts, OptionDefP optp, const char* value,
	int isdefault)
{
	// isdefault indicates if we are setting a default value.

	OptionTypeP otp;
	for (otp = OptionTypeList; otp->name; otp++)
	{
		if (otp->type == optp->type)
		{
			otp->set((char*)prog_opts + optp->offset, value, isdefault);
			return;
		}
	}

	// Should never occur.
	Fatal("Internal Error:  Option '%s' has unknown option type '%d'",
		optp->name, optp->type);
}

int
GetTerminalWidth()
{
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)
		&& csbi.dwSize.X > 0)
	{
		// XXX Apparently the console automatically appends
		// a newline if a line is exactly the width of the console window,
		// so subtract one to avoid the problem.
		return csbi.dwSize.X - 1;
	}
#else
	struct winsize w;

	if (ioctl(0, TIOCGWINSZ, &w) != -1)
	{
		return w.ws_col;
	}
#endif

	return 80;
}

void
WrapOut(char* text, int indent, int maxwidth)
{
	char* textp;
	char* startp;
	int len = 0;

	for (startp = textp = text; *textp; textp++)
	{
		if (++len > maxwidth - indent)
		{
			char* endp;

			// We've hit the max column limit.  Back up, looking for a space
			// character we can break on.
			for (endp = textp; endp > startp && *endp != ' '; endp--)
			{
			}

			if (*endp == ' ')
			{
				// We found a character we can break the line on.
				// Indent the line.
				for (len = indent; len > 0; len--)
				{
					putc(' ', stdout);
				}

				// Temporarily replace the space so we can output the line
				// fragment.
				*endp = '\0';
				printf("%s\n", startp);
				*endp = ' ';

				// Skip past the line fragment we just output, reseting the line
				// length counter since we are now back at the beginning of the
				// line.
				startp = endp + 1;
				textp = endp;
				len = 0;
			}
		}
	}

	for (len = indent; len > 0; len--)
	{
		putc(' ', stdout);
	}

	printf("%s\n", startp);
}

void
DisplayUsage(OptionDefP optlistp, OptionArgDefP arglistp)
{
	OptionDefP optp;
	OptionArgDefP optargp;
	char* buffer = NULL;
	int max_col = GetTerminalWidth();

	printf("Usage:\n");

	buffer = AppFmtStr(NULL, "%s", program);

#if 0
	for (optp = optlistp; optp->name; optp++)
	{
		buffer = AppFmtStr(buffer, " [--%s", optp->name);

		if (!IS_BOOL_OPT(optp->type))
		{
			buffer = AppFmtStr(buffer, "=%s", GetTypeString(optp));
		}

		buffer = AppFmtStr(buffer, "]");
	}
#else
	buffer = AppFmtStr(buffer, " [options]");
#endif

	for (optargp = arglistp; optargp->name; optargp++)
	{
		buffer = AppFmtStr(buffer, " %s", optargp->name);
	}

	WrapOut(buffer, 3, max_col);
	free(buffer);

	printf("Arguments and Options:\n");

	for (optargp = arglistp; optargp->name; optargp++)
	{
		printf("   %s\n", optargp->name);
		buffer = AppFmtStr(NULL, "%s.", optargp->description);
		WrapOut(buffer, 6, max_col);
		free(buffer);
	}

	for (optp = optlistp; optp->name; optp++)
	{
		printf("   ");

		if (optp->short_opt)
		{
			printf("-%c, ", optp->short_opt);
		}

		printf("--%s", optp->name);

		if (!IS_BOOL_OPT(optp->type))
		{
			printf("=%s", GetTypeString(optp));
		}

		printf(":\n");
		buffer = AppFmtStr(NULL, "%s. ", optp->description);

		if (optp->defval)
		{
			buffer = AppFmtStr(buffer, "Default value is %s%s%s.",
				optp->type == OptString || optp->type == OptColor ? "'" : "",
				optp->defval,
				optp->type == OptString || optp->type == OptColor ? "'" : "");
		}
		WrapOut(buffer, 6, max_col);
		free(buffer);
	}
}

OptionDefP
FindSelectedLongOption(char* longopt, size_t optlen, OptionDefP optlistp)
{
	OptionDefP optp;

	for (optp = optlistp; optp->name; optp++)
	{
		if (optlen == strlen(optp->name)
			&& !strncmp(longopt, optp->name, optlen))
		{
			return optp;
		}
	}

	return NULL;
}

OptionDefP
FindSelectedOption(int c, OptionDefP optlistp)
{
	OptionDefP optp;

	for (optp = optlistp; optp->name; optp++)
	{
		if (c == optp->short_opt)
		{
			return optp;
		}
	}

	return NULL;
}

int
ProcessOption(void* prog_opts, int islong, OptionDefP optp, char* argval)
{
	char optstring[2]; // String equiv to short opt to ease error reporting.
	*optstring = optp->short_opt;
	*(optstring + 1) = '\0';


	if (IS_BOOL_OPT(optp->type) && argval)
	{
		Fatal("Unexpected argument '%s' specified for option '-%s%s'.",
			argval, islong ? "-" : "", islong ? optp->name : optstring);
	}
	else if (!IS_BOOL_OPT(optp->type) && !argval)
	{
		Fatal("No argument specified for option '-%s%s'.",
			islong ? "-" : "", islong ? optp->name : optstring);
	}

	if (optp->type != OptHelp && optp->type != OptVersion)
	{
		SetOptionValue(prog_opts, optp, argval, 0);
	}

	return optp->type;
}

int
ProcessArguments(void* prog_opts, OptionDefP optlistp, OptionArgDefP arglistp,
	const char* app_name, const char* app_version, const char* app_copyright,
	int argc, char** argv)
{
	OptionDefP optp;
	OptionArgDefP argp;
	int opt_type = OptBool0; // Start with a non-special option type.

	SetProgram(argv[0]);

	argc--;
	argv++;

	// Initialize the defaults.
	for (optp = optlistp; optp->name; optp++)
	{
		if (optp->type != OptHelp && optp->type != OptVersion)
		{
			SetOptionValue(prog_opts, optp, optp->defval, 1);
		}
	}

	// Parse the options, stopping when we hit a non-option or the end.
	for (; opt_type > 0 && argc > 0; argc--, argv++)
	{
		if (**argv != '-')
		{
			// We've run into a non-option argument, so we are done.
			break;
		}

		if (!strcmp(*argv, "--"))
		{
			// The special "--" argument was specified to end option processing.
			argc--;
			argv++;
			break;
		}

		if (!strncmp(*argv, "--", 2))
		{
			// Long option (--name[=value]).
			char* argname = *argv + 2; // Skip "--".
			char* sep = strchr(argname, '=');
			char* argval = sep ? sep + 1 : NULL;
			size_t arglen = sep ? sep - argname : strlen(argname);
			OptionDefP optp;

			optp = FindSelectedLongOption(argname, arglen, optlistp);

			if (!optp)
			{
				Fatal("Invalid option --%s.  Enter '%s --help' for syntax.",
					argname, program);
			}

			if (!IS_BOOL_OPT(optp->type) && !argval && argc - 1 > 0)
			{
				// We need an argument value and none was supplied, so
				// consume the next argument as the value.  If there isn't
				// an available argument we'll just pass the NULL value along
				// and expect a failure.
				argc--;
				argv++;
				argval = *argv;
			}

			opt_type = ProcessOption(prog_opts, 1, optp, argval);
		}
		else
		{
			// Short option.  Scan through the option characters.
			char* optchar;
			for (optchar = *argv + 1; opt_type > 0 && *optchar; optchar++)
			{
				OptionDefP optp = FindSelectedOption(*optchar, optlistp);

				if (!optp)
				{
					Fatal("Invalid option -%c.  Enter '%s --help' for syntax.",
						*optchar, program);
				}

				if (IS_BOOL_OPT(optp->type))
				{
					opt_type = ProcessOption(prog_opts, 0, optp, NULL);
				}
				else if (*(optchar + 1))
				{
					// Short option with arg as part of the same word (-cvalue).
					opt_type = ProcessOption(prog_opts, 0, optp,
						optchar + 1);
					break;
				}
				else if ((argc - 1) > 0)
				{
					// We need an argument value and none was supplied, so
					// consume the next argument as the value.
					argc--;
					argv++;
					opt_type = ProcessOption(prog_opts, 0, optp, *argv);
					break;
				}
				else
				{
					// Argument is missing, so we expect failure.
					opt_type = ProcessOption(prog_opts, 0, optp, NULL);
					break;
				}
			}
		}
	}

	if (opt_type == OptHelp)
	{
		DisplayUsage(optlistp, arglistp);
		return 0;
	}
	else if (opt_type == OptVersion)
	{
		printf("%s %s\n%s\n", app_name, app_version, app_copyright);
		return 0;
	}

	for (argp = arglistp; argc > 0 && argp->name; argp++, argc--, argv++)
	{
		*(char**)((char*)prog_opts + argp->offset) = *argv;
	}

	if (argc > 0)
	{
		char* unexpectedArgs = NULL;
		for (; argc > 0; argc--, argv++)
		{
			unexpectedArgs = AppFmtStr(unexpectedArgs, " %s", *argv);
		}
		Fatal("Unexpected arguments [%s ].  Enter: '%s --help' for syntax.",
			unexpectedArgs, program);
		free(unexpectedArgs);
	}
	else if (argp->name)
	{
		char* missingArgs = NULL;
		for (; argp->name; argp++)
		{
			missingArgs = AppFmtStr(missingArgs, " %s", argp->name);
		}

		Fatal("Missing required arguments [%s ].  Enter: '%s --help' for syntax",
			missingArgs, program);
		free(missingArgs);
	}

	return 1;
}

char*
AddOptionSetting(OptionDefP optp, char* value, char* settings)
{
	if (optp->short_opt)
	{
		settings = AppFmtStr(settings, " -%c", optp->short_opt);
		if (!IS_BOOL_OPT(optp->type))
		{
			settings = AppFmtStr(settings, " '%s'", value);
		}
	}
	else
	{
		settings = AppFmtStr(settings, " --%s", optp->name);
		if (!IS_BOOL_OPT(optp->type))
		{
			settings = AppFmtStr(settings, "='%s'", value);
		}
	}

	return settings;
}

char*
GetArgumentSettings(void* prog_opts, OptionDefP optlistp,
	OptionArgDefP arglistp)
{
	char* settings = NULL;
	OptionDefP optp;
	OptionArgDefP argp;

	for (optp = optlistp; optp->name; optp++)
	{
		char* value;

		if (optp->type == OptHelp || optp->type == OptVersion)
		{
			continue;
		}
		else if (optp->type == OptArgv)
		{
			// XXX Special handling for argv options.
			char** argv = *(char***)((char*)prog_opts + optp->offset);

			if (argv != NULL)
			{
				for (; *argv; argv++)
				{
					settings = AddOptionSetting(optp, *argv, settings);
				}
			}
		}
		else if ((value = GetTypeValue(prog_opts, optp)) != NULL)
		{
			settings = AddOptionSetting(optp, value, settings);
			free(value);
		}
	}

	for (argp = arglistp; argp->name; argp++)
	{
		char* value = *(char**)((char*)prog_opts + argp->offset);
		settings = AppFmtStr(settings, " '%s'", value);
	}

	return settings;
}

//////////////////////////////////////////////////////////////////////////////
// FreeType error message handling.
//////////////////////////////////////////////////////////////////////////////

const char* ft_error(int err_code) {
	#undef FTERRORS_H_
	#define FT_ERROR_START_LIST     switch (error_code) {
	#define FT_ERRORDEF(e, v, s)		case v: return s;
	#define FT_ERROR_END_LIST		}

	return "";
}

void
VFTFatal(FT_Error error, const char* fmt, va_list ap)
{
	VMessage(stderr, fmt, ap);

	fprintf(stderr, ": %s\n", ft_error(error));

	exit(EXIT_FAILURE);
}

void
FTFatal(FT_Error error, const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	VFTFatal(error, fmt, ap);
	va_end(ap);
}

//////////////////////////////////////////////////////////////////////////////
// Option Initialization.
//////////////////////////////////////////////////////////////////////////////

// Program-specific option structure.  The option parsing engine is handed
// this to stash option values.
typedef struct
{
	float outborder;
	float inborder;
	int pad;
	int offset;
	int advance;
	int charwidth;
	int charheight;
	int texwidth;
	int texheight;
	int spacewidth;
	int spaceheight;
	const char* texdir;
	const char* fontname;
	const char* infile;
	const char* outfile;
	char** altfont;
	char** fontmap;
	int colortex;
	int noalpha;
	ColorT bgcolor;
	ColorT fgcolor;
	int digits;
	int missingfatal;
	int fixedwidth;
	int sauerpack;
	int noexec;
} ProgramOptionT, * ProgramOptionP;

// Program-specific option definitions.  The option parsing engine uses this to
// configure itself.
OptionDefT ProgramOptionDefs[] =
{
	{ "help", '?', OptHelp, "Display help", NULL, 0 },
	{ "version", 0, OptVersion, "Show version", NULL, 0 },
	{ "outborder", 'B', OptFloat, "Size of the outside border around each character on the texes", "3", offsetof(ProgramOptionT, outborder) },
	{ "inborder", 'b', OptFloat, "Size of the border between elements inside each character on the texes", "0", offsetof(ProgramOptionT, inborder) },
	{ "pad", 'p', OptInt, "Added padding around characters on the texes, to prevent bleeding into neighboring characters", "19", offsetof(ProgramOptionT, pad) },
	{ "offset", 'o', OptInt, "Added spacing to the left of each character when rendered ingame", "1", offsetof(ProgramOptionT, offset) },
	{ "advance", 'a', OptInt, "Added spacing to the right of each character when rendered ingame", "1", offsetof(ProgramOptionT, advance) },
	{ "charwidth", 'w', OptInt, "Width of generated characters on the texes", "49", offsetof(ProgramOptionT, charwidth) },
	{ "charheight", 'h', OptInt, "Height of generated characters on the texes", "49", offsetof(ProgramOptionT, charheight) },
	{ "texwidth", 'W', OptInt, "Width of generated texes", "512", offsetof(ProgramOptionT, texwidth) },
	{ "texheight", 'H', OptInt, "Height of generated texes", "512", offsetof(ProgramOptionT, texheight) },
	{ "spacewidth", 's', OptInt, "Width of the standard space character when rendered ingame. Automatically set to one third the spaceheight if set to 0", "0", offsetof(ProgramOptionT, spacewidth) },
	{ "spaceheight", 't', OptInt, "Height of the vertical character separation when rendered ingame. Automatically set to the maximum vertical character range if set to 0", "0", offsetof(ProgramOptionT, spaceheight) },
	{ "texdir", 'd', OptString, "Font directory path in the config file", NULL, offsetof(ProgramOptionT, texdir) },
	{ "fontname", 'n', OptString, "Font name in the config file", NULL, offsetof(ProgramOptionT, fontname) },
	{ "map", 0, OptArgv, "Font character mapping for a single character. Can be specified multiple times for different characters. Each entry is of the form '[from] [to]' to map from one character code to another character code. The format of [from] and [to] is either a single ASCII character or a unicode hex character code in the form Uxxxx. For example, to map the letter 'a' to the '@' sign, you could specify --map='a @' or --map='a U0040' or --map='U0061 @' or --map='U0061 U0040'", NULL, offsetof(ProgramOptionT, fontmap) },
	{ "altfont", 0, OptArgv, "Alternate font to use when mapping characters using --map. Can be specified multiple times. Each alternate font is assigned an index number starting from 1 in the order they are specified on the command line. Mappings use the alternate fonts by preceding the 'to' part with a font index number followed by ':', for example --map='a 1:U0040'. The default index for a mapping is 0 (the infile font) if no alternate fonts are specified; otherwise it is 1 (the first alternate font)", NULL, offsetof(ProgramOptionT, altfont) },
	{ "colortex", 0, OptBool1, "Make the texes color instead of grayscale", NULL, offsetof(ProgramOptionT, colortex) },
	{ "noalpha", 0, OptBool1, "Do not include alpha in texes", NULL, offsetof(ProgramOptionT, noalpha) },
	{ "bgcolor", 0, OptColor, "Tex background color. This controls the color of the character", "white", offsetof(ProgramOptionT, bgcolor) },
	{ "fgcolor", 0, OptColor, "Tex foreground color. This controls the color of the border around each character", "black", offsetof(ProgramOptionT, fgcolor) },
	{ "digits", 0, OptBool1, "Digits only", NULL, offsetof(ProgramOptionT, digits) },
	{ "missingfatal", 0, OptBool1, "If specified, missing font characters are treated as a fatal error instead of a warning", NULL, offsetof(ProgramOptionT, missingfatal) },
	{ "fixedwidth", 0, OptBool1, "Make font fixed width.  Generally this should be avoided.  It is better to use a fixed width font rather than try to force a variable width font to be fixed width", NULL, offsetof(ProgramOptionT, fixedwidth) },
	{ "sauerpack", 0, OptBool1, "Use Sauer packing algorithm", NULL, offsetof(ProgramOptionT, sauerpack) },
	{ "noexec", 0, OptBool1, "Validate input only. Do not create files", NULL, offsetof(ProgramOptionT, noexec) },
	{ NULL }
};

// Program-specific argument definitions.  The option parsing engine uses this
// to configure itself.
OptionArgDefT ProgramArgDefs[] =
{
	{ "infile", "Path to input file in truetype format", offsetof(ProgramOptionT, infile) },
	{ "outfile", "Sets the font name in the config file, and is also used as an output filename template. A number and .png extension will be appended to the tex files, and a .cfg extension will be appended to the coordinate file", offsetof(ProgramOptionT, outfile) },
	{ NULL }
};

//////////////////////////////////////////////////////////////////////////////
// Code Page Definition.
//////////////////////////////////////////////////////////////////////////////

const uint CardboardUnicharList[] =
{
	0, 192, 193, 194, 195, 196, 197, 198, 199, 9, 10, 11, 12, 13, 200, 201,
	202, 203, 204, 205, 206, 207, 209, 210, 211, 212, 213, 214, 216, 217, 218, 219,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 220,
	221, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237,
	238, 239, 241, 242, 243, 244, 245, 246, 248, 249, 250, 251, 252, 253, 255, 0x104,
	0x105, 0x106, 0x107, 0x10C, 0x10D, 0x10E, 0x10F, 0x118, 0x119, 0x11A, 0x11B, 0x11E, 0x11F, 0x130, 0x131, 0x141,
	0x142, 0x143, 0x144, 0x147, 0x148, 0x150, 0x151, 0x152, 0x153, 0x158, 0x159, 0x15A, 0x15B, 0x15E, 0x15F, 0x160,
	0x161, 0x164, 0x165, 0x16E, 0x16F, 0x170, 0x171, 0x178, 0x179, 0x17A, 0x17B, 0x17C, 0x17D, 0x17E, 0x404, 0x411,
	0x413, 0x414, 0x416, 0x417, 0x418, 0x419, 0x41B, 0x41F, 0x423, 0x424, 0x426, 0x427, 0x428, 0x429, 0x42A, 0x42B,
	0x42C, 0x42D, 0x42E, 0x42F, 0x431, 0x432, 0x433, 0x434, 0x436, 0x437, 0x438, 0x439, 0x43A, 0x43B, 0x43C, 0x43D,
	0x43F, 0x442, 0x444, 0x446, 0x447, 0x448, 0x449, 0x44A, 0x44B, 0x44C, 0x44D, 0x44E, 0x44F, 0x454, 0x490, 0x491
};

#define CODE_PAGE_SIZE (sizeof(CardboardUnicharList)/sizeof(CardboardUnicharList[0]))

enum
{
	CT_PRINT = 1 << 0,
	CT_SPACE = 1 << 1,
	CT_DIGIT = 1 << 2,
	CT_ALPHA = 1 << 3,
	CT_LOWER = 1 << 4,
	CT_UPPER = 1 << 5,
	CT_UNICODE = 1 << 6
};

#define CBCTYPE(s, p, d, a, A, u, U) \
	0, U, U, U, U, U, U, U, U, s, s, s, s, s, U, U, \
	U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, \
	s, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, \
	d, d, d, d, d, d, d, d, d, d, p, p, p, p, p, p, \
	p, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, \
	A, A, A, A, A, A, A, A, A, A, A, p, p, p, p, p, \
	p, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, \
	a, a, a, a, a, a, a, a, a, a, a, p, p, p, p, U, \
	U, u, u, u, u, u, u, u, u, u, u, u, u, u, u, u, \
	u, u, u, u, u, u, u, u, u, u, u, u, u, u, u, U, \
	u, U, u, U, u, U, u, U, u, U, u, U, u, U, u, U, \
	u, U, u, U, u, U, u, U, u, U, u, U, u, U, u, U, \
	u, U, u, U, u, U, u, U, U, u, U, u, U, u, U, U, \
	U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, U, \
	U, U, U, U, u, u, u, u, u, u, u, u, u, u, u, u, \
	u, u, u, u, u, u, u, u, u, u, u, u, u, u, U, u

const uchar CardboardCharTypeList[CODE_PAGE_SIZE] =
{
	CBCTYPE(CT_SPACE,
			CT_PRINT,
			CT_PRINT | CT_DIGIT,
			CT_PRINT | CT_ALPHA | CT_LOWER,
			CT_PRINT | CT_ALPHA | CT_UPPER,
			CT_PRINT | CT_UNICODE | CT_ALPHA | CT_LOWER,
			CT_PRINT | CT_UNICODE | CT_ALPHA | CT_UPPER)
};


int
IsCardboardPrint(uchar c)
{
	return CardboardCharTypeList[c] & CT_PRINT;
}

#if 0
int
IsCardboardSpace(uchar c)
{
	return CardboardCharTypeList[c] & CT_SPACE;
}

int
IsCardboardAlpha(uchar c)
{
	return CardboardCharTypeList[c] & CT_ALPHA;
}
#endif

int
IsCardboardNum(uchar c)
{
	return CardboardCharTypeList[c] & CT_DIGIT;
}

#if 0
int
IsCardboardAlphaNum(uchar c)
{
	return CardboardCharTypeList[c] & (CT_ALPHA | CT_DIGIT);
}

int
IsCardboardLower(uchar c)
{
	return CardboardCharTypeList[c] & CT_LOWER;
}

int
IsCardboardUpper(uchar c)
{
	return CardboardCharTypeList[c] & CT_UPPER;
}
#endif

int
CardboardUni(uchar c)
{
	return CardboardUnicharList[c];
}

//////////////////////////////////////////////////////////////////////////////
// Code Page Construction.
//////////////////////////////////////////////////////////////////////////////

// Sentinel value used to indicate when we've hit the end of the mapping list.
// Note:  This must be an invalid unicode value.
#define UNICODE_SENTINEL ((uint) -1)

// Structure defining each character in the code page.
typedef struct
{
	uint code;
	uint uni;
	uint mapped_uni;
	int tex;
	int x;
	int y;
	int w;
	int h;
	int offx;
	int offy;
	int offset;
	int advance;
	FT_BitmapGlyph color;
	FT_BitmapGlyph alpha;
} FontCharT, * FontCharP;

// Structure defining a character mapping.
typedef struct
{
	uint from_uni; // Unicode char code we are mapping from.
	uint to_uni;   // Unicode char code we are mapping to.
	uint to_font;  // Index of font we are mapping to.  (0 is the infile font.)
} CharMapT, * CharMapP;

void
ParseCharacterMapPart(const char* mapstring, const char* part,
	uint* out_unip, uint* out_fip)
{
	char uni_char;
	int len;

	if (sscanf(part, "U%04x%n", out_unip, &len) == 1
		&& len == 5 && len == (int)strlen(part))
	{
		// 'Uxxxx' (Hex unicode char code)
	}
	else if (sscanf(part, "%c%n", &uni_char, &len) == 1
		&& len == 1 && len == (int)strlen(part))
	{
		// 'c' (single ASCII character)
		*out_unip = uni_char;
	}
	else if (out_fip && sscanf(part, "%u:U%04x%n", out_fip, out_unip, &len) == 2
		&& len == (int)strlen(part))
	{
		// 'd:Uxxxx' (Hex unicode char code with font index)
	}
	else if (out_fip && sscanf(part, "%u:%c%n", out_fip, &uni_char, &len) == 2
		&& len == (int)strlen(part))
	{
		// 'dc' (single ASCII character with font index)
		*out_unip = uni_char;
	}
	else
	{
		Fatal("Could not parse '%s' part of character mapping '%s'. Enter '%s --help' for syntax.",
			part, mapstring, program);
	}
}

void
InitCharacterMapEntry(const char* mapstring, CharMapP map_entry_p,
	uint face_count)
{
	char* map_from = AppFmtStr(NULL, "%s", mapstring); // Expensive strdup()
	char* map_to;
	char* sep = strchr(map_from, ' ');

	if (!sep)
	{
		Fatal("Missing ' ' separator in character mapping '%s'. Enter '%s --help' for syntax.",
			mapstring, program);
	}

	*sep = '\0';
	map_to = sep + 1;

	// If alternate fonts have been provided then the default font is the
	// first alternate font.  Otherwise it is the input font.
	map_entry_p->to_font = face_count > 1 ? 1 : 0;

	ParseCharacterMapPart(mapstring, map_from, &map_entry_p->from_uni, NULL);
	ParseCharacterMapPart(mapstring, map_to, &map_entry_p->to_uni,
		&map_entry_p->to_font);

	if (map_entry_p->to_font >= face_count)
	{
		Fatal("Font index %d for mapping '%s' is out of range (%d max)",
			map_entry_p->to_font, mapstring, face_count - 1);
	}

	free(map_from);
}

CharMapP
CreateCharacterMapping(char** fontmap, FT_Face* face_list)
{
	CharMapP charmap_list = NULL;
	if (fontmap)
	{
		int face_count = 0;
		int count = 0;
		CharMapP cmap;

		// Count how many font entries we have.
		for (; face_list[face_count]; face_count++)
		{
		}

		// Count how many char map entries we have.
		for (; fontmap[count]; count++)
		{
		}

		charmap_list = (CharMapP)Allocate(NULL, (count + 1) * sizeof(CharMapT),
			"char map");

		// Initialize the char map entries.
		for (cmap = charmap_list; *fontmap; fontmap++, cmap++)
		{
			InitCharacterMapEntry(*fontmap, cmap, face_count);
		}

		// Set sentinel values so when scanning we know we've hit the end.
		cmap->from_uni = UNICODE_SENTINEL;
		cmap->to_uni = UNICODE_SENTINEL;
	}

	return charmap_list;
}

CharMapP
FindCharacterMap(CharMapP charmap, uint* uni_char)
{
	CharMapP cmap;
	if (charmap)
	{
		for (cmap = charmap; cmap->from_uni != UNICODE_SENTINEL; cmap++)
		{
			if (*uni_char == cmap->from_uni)
			{
				return cmap;
			}
		}
	}

	return NULL;
}

const char*
EncodeUTF8(int uni)
{
	static char buf[7];
	char* dst = buf;

	if (uni <= 0x7F)
	{
		*dst++ = uni;
		goto uni1;
	}
	else if (uni <= 0x7FF)
	{
		*dst++ = 0xC0 | (uni >> 6);
		goto uni2;
	}
	else if (uni <= 0xFFFF)
	{
		*dst++ = 0xE0 | (uni >> 12);
		goto uni3;
	}
	else if (uni <= 0x1FFFFF)
	{
		*dst++ = 0xF0 | (uni >> 18);
		goto uni4;
	}
	else if (uni <= 0x3FFFFFF)
	{
		*dst++ = 0xF8 | (uni >> 24);
		goto uni5;
	}
	else if (uni <= 0x7FFFFFFF)
	{
		*dst++ = 0xFC | (uni >> 30);
		goto uni6;
	}
	else
	{
		goto uni1;
	}

uni6: *dst++ = 0x80 | ((uni >> 24) & 0x3F);
uni5: *dst++ = 0x80 | ((uni >> 18) & 0x3F);
uni4: *dst++ = 0x80 | ((uni >> 12) & 0x3F);
uni3: *dst++ = 0x80 | ((uni >> 6) & 0x3F);
uni2: *dst++ = 0x80 | (uni & 0x3F);
uni1: *dst++ = '\0';

	return buf;
}

FT_Face
InitializeFontFace(FT_Library type_library, const char* font_file,
	int charwidth, int charheight)
{
	FT_Face font_face;
	FT_Error error;

	if ((error = FT_New_Face(type_library, font_file, 0, &font_face)))
	{
		FTFatal(error, "Could not initialize font face '%s'", font_file);
	}
	else if ((error = FT_Set_Charmap(font_face, font_face->charmaps[0])))
	{
		FTFatal(error, "Could not set font face char map");
	}
	else if ((error = FT_Set_Pixel_Sizes(font_face, charwidth, charheight)))
	{
		FTFatal(error,
			"Could not set font face pixel (width,height) to (%d,%d)",
			charwidth, charheight);
	}

	return font_face;
}

FT_Face*
InitializeFontFaces(const char* infile, char** altfont,
	FT_Library type_library, int charwidth, int charheight)
{
	FT_Face* facep;
	int count = 0;
	FT_Face* facelist;

	if (altfont)
	{
		// If alternate fonts have been specified then count them.
		for (; altfont[count]; count++)
		{
		}
	}

	count++; // Add one for the input font.

	facep = facelist = (FT_Face*)Allocate(NULL, (count + 1) * sizeof(FT_Face),
		"font list");

	// The first font is always infile (the input font).
	*facep++ = InitializeFontFace(type_library, infile, charwidth, charheight);

	if (altfont)
	{
		// If alternate fonts have been initialized then add them to the font
		// list after the input font.
		for (; *altfont; altfont++, facep++)
		{
			*facep = InitializeFontFace(type_library, *altfont,
				charwidth, charheight);
		}
	}

	*facep = NULL; // NULL terminate the font list.

	return facelist;
}

void
FreeFontFaces(FT_Face* facelist)
{
	FT_Face* facep;

	for (facep = facelist; *facep; facep++)
	{
		FT_Done_Face(*facep);
	}
}

int
LoadFontGlyphs(ProgramOptionP prog_opts_p,
	FT_Library type_library,
	FontCharP chars, FontCharP* order, size_t codePageSize)
{
	size_t code_page_idx;
	int numchars = 0;

	FT_Face* face_list;
	CharMapP charmap_list;
	FT_Stroker stroker_out_border;
	FT_Stroker stroker_in_border;

	FT_Error error;
	uint missing_glyph = 0;

	face_list = InitializeFontFaces(prog_opts_p->infile, prog_opts_p->altfont,
		type_library,
		prog_opts_p->charwidth, prog_opts_p->charheight);

	charmap_list = CreateCharacterMapping(prog_opts_p->fontmap, face_list);

	error = FT_Stroker_New(type_library, &stroker_out_border);
	error = FT_Stroker_New(type_library, &stroker_in_border);

	if (error)
	{
		FTFatal(error, "Failed to create font strokers");
	}

	if (prog_opts_p->outborder != 0)
	{
		// The stroker radius is expressed as 1/64th of a pixel, so scale the
		// value up to a pixel unit.
		long outborder = prog_opts_p->outborder * 64;
		if (outborder < 0)
		{
			// Negative outborders are valid, but change which border is
			// returned after the stroke.
			outborder = -outborder;
		}

		FT_Stroker_Set(stroker_out_border, (FT_Fixed)outborder,
			FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
	}

	if (prog_opts_p->inborder != 0)
	{
		// The stroker radius is expressed as 1/64th of a pixel, so scale the
		// value up to a pixel unit.
		long inborder = prog_opts_p->inborder * 64;
		if (inborder < 0)
		{
			// Negative inborders are valid, but change which border is returned
			// after the stroke.
			inborder = -inborder;
		}

		FT_Stroker_Set(stroker_in_border, (FT_Fixed)inborder,
			FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
	}

	for (code_page_idx = 0; code_page_idx < codePageSize; code_page_idx++)
	{
		FT_Glyph alpha_glyph;
		FT_Glyph color_glyph;
		FT_BitmapGlyph alpha_bitmap;
		FT_BitmapGlyph color_bitmap;
		FT_Face selected_font;
		FT_UInt glyph_index;

		FontCharP dest_char = &chars[numchars];
		const char* utf8char;

		if (!IsCardboardPrint(code_page_idx))
		{
			continue;
		}

		if (prog_opts_p->digits && !IsCardboardNum(code_page_idx))
		{
			continue;
		}

		dest_char->code = code_page_idx;
		dest_char->uni = CardboardUni(code_page_idx);

		dest_char->mapped_uni = dest_char->uni;
		CharMapP cmap = FindCharacterMap(charmap_list, &dest_char->mapped_uni);

		if (cmap)
		{
			dest_char->mapped_uni = cmap->to_uni;
			selected_font = face_list[cmap->to_font];
		}
		else
		{
			dest_char->mapped_uni = dest_char->uni;
			selected_font = face_list[0]; // Always exists.
		}

		utf8char = EncodeUTF8(dest_char->uni);

		if (cmap)
		{
			// Note: We always map from the input font.
			const char* font_name = FT_Get_Postscript_Name(face_list[0]);
			const char* alt_font_name = FT_Get_Postscript_Name(selected_font);
			Info("Mapping %s U%04X ('%s') to %s U%04X",
				font_name ? font_name : "unicode",
				dest_char->uni, utf8char,
				alt_font_name ? alt_font_name : "unicode",
				dest_char->mapped_uni);
		}

		glyph_index = FT_Get_Char_Index(selected_font,
			(FT_ULong)dest_char->mapped_uni);

		if (!glyph_index)
		{
			Warning("Missing glyph for character '%s' (U%04X)",
				utf8char, dest_char->mapped_uni);
			missing_glyph++;
		}

		// Load the given character into the font face's glyph slot.
		if ((error = FT_Load_Glyph(selected_font, glyph_index,
			FT_LOAD_DEFAULT)))
		{
			FTFatal(error, "Failed loading character '%s'", utf8char);
		}

#if 0
		if (FT_Has_PS_Glyph_Names(selected_font))
		{
			char nbuf[40];
			FT_Get_Glyph_Name(selected_font, glyph_index, nbuf, sizeof(nbuf));
			if (*nbuf) {
				printf("got glyph name: %s\n", nbuf);
			}
		}
#endif

		// Get the glyph for the character just loaded.
		if ((error = FT_Get_Glyph(selected_font->glyph, &alpha_glyph)))
		{
			FTFatal(error, "Failed to get glyph for character '%s'", utf8char);
		}

		if ((error = FT_Glyph_Copy(alpha_glyph, &color_glyph)))
		{
			FTFatal(error, "Character '%s' outer to inner glyph copy failed",
				utf8char);
		}

		if (prog_opts_p->outborder != 0
			&& (error = FT_Glyph_StrokeBorder(&alpha_glyph, stroker_out_border,
				prog_opts_p->outborder < 0, 1)))
		{
			FTFatal(error, "Stroke of outer border of character '%s' failed",
				utf8char);
		}

		if (prog_opts_p->inborder != 0
			&& (error = FT_Glyph_StrokeBorder(&color_glyph, stroker_in_border,
				prog_opts_p->inborder > 0, 1)))
		{
			FTFatal(error, "Stroke of inner border of character '%s' failed",
				utf8char);
		}

		if ((error = FT_Glyph_To_Bitmap(&alpha_glyph, FT_RENDER_MODE_NORMAL,
			NULL, 1)))
		{
			FTFatal(error, "Outer bitmap conversion for character '%s' failed",
				utf8char);
		}
		if ((error = FT_Glyph_To_Bitmap(&color_glyph, FT_RENDER_MODE_NORMAL,
			NULL, 1)))
		{
			FTFatal(error, "Inner bitmap conversion for character '%s' failed",
				utf8char);
		}

		alpha_bitmap = (FT_BitmapGlyph)alpha_glyph;
		color_bitmap = (FT_BitmapGlyph)color_glyph;

#if 0
		// XXX
		printf("'%s' Outer(left=%d,top=%d, width=%d, height=%d, advance=%ld)\n",
			utf8char,
			alpha_bitmap->left, alpha_bitmap->top,
			alpha_bitmap->bitmap.width, alpha_bitmap->bitmap.rows,
			alpha_glyph->advance.x
		);
		printf("'%s' Inner(left=%d,top=%d, width=%d, height=%d, advance=%ld)\n",
			utf8char,
			color_bitmap->left, color_bitmap->top,
			color_bitmap->bitmap.width, color_bitmap->bitmap.rows,
			color_glyph->advance.x
		);
		printf("\n");
		// XXX
#endif

		dest_char->tex = -1;
		dest_char->x = INT_MIN;
		dest_char->y = INT_MIN;
		dest_char->offx = IMin(alpha_bitmap->left, color_bitmap->left);
		dest_char->offy = IMax(alpha_bitmap->top, color_bitmap->top);
		dest_char->offset = prog_opts_p->offset;
		dest_char->advance = prog_opts_p->offset
			+ ((alpha_glyph->advance.x + 0xFFFF) >> 16) + prog_opts_p->advance;
		dest_char->w = IMax(alpha_bitmap->left + alpha_bitmap->bitmap.width,
			color_bitmap->left + color_bitmap->bitmap.width) - dest_char->offx;
		dest_char->h = dest_char->offy
			- IMin(alpha_bitmap->top - alpha_bitmap->bitmap.rows,
				color_bitmap->top - color_bitmap->bitmap.rows);
		dest_char->alpha = alpha_bitmap;
		dest_char->color = color_bitmap;

		order[numchars++] = dest_char;
	}

	if (missing_glyph && prog_opts_p->missingfatal)
	{
		Fatal("Missing %u glyphs", missing_glyph);
	}

	FT_Stroker_Done(stroker_out_border);
	FT_Stroker_Done(stroker_in_border);

	FreeFontFaces(face_list);
	free(charmap_list);

	return numchars;
}

//////////////////////////////////////////////////////////////////////////////
// Shelf Packing Management.
//////////////////////////////////////////////////////////////////////////////

typedef struct
{
	int tex;
	int offsety;
	int width;
	int height;
} ShelfT, * ShelfP;

size_t ShelfCount = 0;
ShelfP ShelfList = NULL;

ShelfP
AddShelf()
{
	ShelfP shelfp;
	ShelfList = (ShelfP)Allocate(ShelfList, ++ShelfCount * sizeof(ShelfT),
		"shelf");
	shelfp = &ShelfList[ShelfCount - 1];

	shelfp->tex = 0;
	shelfp->offsety = 0;
	shelfp->width = 0;
	shelfp->height = 0;

	return shelfp;
}

void
ClearShelves()
{
	free(ShelfList);
	ShelfList = NULL;
	ShelfCount = 0;
}

int
MaxShelfTex()
{
	int max_tex = -1;
	size_t i;

	for (i = 0; i < ShelfCount; i++)
	{
		ShelfP shelfp = &ShelfList[i];
		if (shelfp->height != 0 && shelfp->tex > max_tex)
		{
			max_tex = shelfp->tex;
		}
	}

	return max_tex + 1;
}

ShelfP
FindShelf(int texwidth, int texheight, int pad, int width, int height)
{
	size_t i;
	ShelfP shelfp;

	// Find a shelf that has room for the new char.
	for (i = 0; i < ShelfCount; i++)
	{
		shelfp = &ShelfList[i];
		if (
			(
				(shelfp->height == 0 && shelfp->offsety + height <= texheight)
				|| (shelfp->height >= height)
				) && shelfp->width + width <= texwidth
			)
		{
			return shelfp;
		}
	}

	shelfp = AddShelf();

	if (ShelfCount != 1)
	{
		// This isn't the first shelf, so stick the new shelf above the
		// previous shelf.
		ShelfP prev_shelfp = &ShelfList[ShelfCount - 2];

		shelfp->tex = prev_shelfp->tex;
		shelfp->offsety = prev_shelfp->offsety + prev_shelfp->height + pad;

		// If the new shelf doesn't fit on the tex, make the new shelf the
		// first shelf of the next tex.
		if (shelfp->offsety + height > texheight)
		{
			shelfp = AddShelf();
			shelfp->tex = prev_shelfp->tex + 1;
			shelfp->offsety = 0;
		}
	}

	return shelfp;
}

//////////////////////////////////////////////////////////////////////////////
// Glyph Processing.
//////////////////////////////////////////////////////////////////////////////

// Structure for recording global glyph metrics.
typedef struct
{
	int offsetx_left;
	int offsetx_right;
	int offsety_bottom;
	int offsety_top;
	int char_width_max;
	int char_height_max;
	int numtex;
} CharMetricsT, * CharMetricsP;

int
GroupChar(int c)
{
	// Group characters so that ASCII characters are grouped before
	// others.  This helps arrange for those characters to all show up on
	// the first tex page.  This helps in rendering since most text will only
	// require loading a single tex page.

	switch (c)
	{
	case 0x152:
	case 0x153:
	case 0x178:
		return 1;
	}

	if ((c < 127) || (c >= 0x2000))
	{
		return 0;
	}

	if (c < 0x100)
	{
		return 1;
	}

	if (c < 0x400)
	{
		return 2;
	}

	return 3;
}

int
SimpleFontCharCmp(const void* x, const void* y)
{
	const FontCharP xc = *(const FontCharP*)x;
	const FontCharP yc = *(const FontCharP*)y;

	// Sort in descending height order.
	if (xc->h != yc->h)
	{
		return yc->h - xc->h;
	}

	// Sort in descending width order.
	if (xc->w != yc->w)
	{
		return yc->w - xc->w;
	}

	return xc->uni - yc->uni;
	//return yc->uni - xc->uni;
}

int
FontCharCmp(const void* x, const void* y)
{
	const FontCharP xc = *(const FontCharP*)x;
	const FontCharP yc = *(const FontCharP*)y;

	int xg = GroupChar(xc->uni);
	int yg = GroupChar(yc->uni);

	if (xg < yg)
	{
		return -1;
	}

	if (xg > yg)
	{
		return 1;
	}

	// Sort in descending height order.
	if (xc->h != yc->h)
	{
		return yc->h - xc->h;
	}

	// Sort in descending width order.
	if (xc->w != yc->w)
	{
		return yc->w - xc->w;
	}

	return yc->uni - xc->uni;
}

int
ScoreChar(FontCharP font_char, int pad, int tex_width, int tex_height,
	int rw, int rh, int ry)
{
	int score = 0;

	if (rw + font_char->w > tex_width)
	{
		ry += rh + pad;
		score = 1;
	}

	if (ry + font_char->h > tex_height)
	{
		score = 2;
	}

	return score;
}

void
PackFontChars(ProgramOptionP prog_opts_p,
	CharMetricsP char_metrics_p,
	FT_Library type_library, FontCharP* order, int numchars)
{
	int char_idx;
	int trial = -2;
	int total_row_width = 0;
	int row_height = 0;
	int total_row_height = 0;

	for (char_idx = 0; char_idx < numchars;)
	{
		FontCharP dst;

		int k;
		int trial0;
		int prevscore;
		int dstscore;
		int fitscore;

		for (trial0 = trial, prevscore = -1; (trial -= 2) >= trial0 - 512;)
		{
			int j;
			int g;
			int fw = total_row_width;
			int fh = row_height;
			int fy = total_row_height;
			int curscore = 0;
			int reused = 0;

			for (j = char_idx; j < numchars; j++)
			{
				dst = order[j];

				if ((dst->tex >= 0) || (dst->tex <= trial))
				{
					continue;
				}

				g = GroupChar(dst->uni);
				dstscore = ScoreChar(dst, prog_opts_p->pad, prog_opts_p->texwidth, prog_opts_p->texheight, fw, fh, fy);

				for (k = j; k < numchars; k++)
				{
					FontCharP fit = order[k];

					if ((fit->tex >= 0) || (fit->tex <= trial))
					{
						continue;
					}

					if ((fit->tex >= trial0) && (GroupChar(fit->uni) != g))
					{
						break;
					}

					fitscore = ScoreChar(fit, prog_opts_p->pad, prog_opts_p->texwidth, prog_opts_p->texheight, fw, fh, fy);

					if ((fitscore < dstscore) || ((fitscore == dstscore) && (fit->h > dst->h)))
					{
						dst = fit;
						dstscore = fitscore;
					}
				}

				if (fw + dst->w > prog_opts_p->texwidth)
				{
					fy += fh + prog_opts_p->pad;
					fw = fh = 0;
				}

				if (fy + dst->h > prog_opts_p->texheight)
				{
					fy = fw = fh = 0;

					if (curscore > 0)
					{
						break;
					}
				}

				if ((dst->tex >= trial + 1) && (dst->tex <= trial + 2))
				{
					dst->tex = trial;
					reused++;
				}
				else
				{
					dst->tex = trial;
				}

				fw += dst->w + prog_opts_p->pad;
				fh = IMax(fh, dst->h);

				if (dst != order[j])
				{
					--j;
				}

				curscore++;
			}

			if ((reused < prevscore) || (curscore <= prevscore))
			{
				break;
			}

			prevscore = curscore;
		}

		for (; char_idx < numchars; char_idx++)
		{
			int j;
			dst = order[char_idx];

			if (dst->tex >= 0)
			{
				continue;
			}

			dstscore = ScoreChar(dst, prog_opts_p->pad, prog_opts_p->texwidth, prog_opts_p->texheight, total_row_width, row_height, total_row_height);

			for (j = char_idx; j < numchars; j++)
			{
				FontCharP fit = order[j];

				if ((fit->tex < trial) || (fit->tex > trial + 2))
				{
					continue;
				}

				fitscore = ScoreChar(fit, prog_opts_p->pad, prog_opts_p->texwidth, prog_opts_p->texheight, total_row_width, row_height, total_row_height);

				if ((fitscore < dstscore) || ((fitscore == dstscore) && (fit->h > dst->h)))
				{
					dst = fit;
					dstscore = fitscore;
				}
			}

			if ((dst->tex < trial) || (dst->tex > trial + 2))
			{
				break;
			}

			// If the dest char width would not fit in the remaining portion of
			// the tex row, move to the next row.
			if (total_row_width + dst->w > prog_opts_p->texwidth)
			{
				total_row_height += row_height + prog_opts_p->pad;
				total_row_width = row_height = 0;
			}

			// If the dest char height would not fit in the remaining portion of
			// the tex height, move to the next tex.
			if (total_row_height + dst->h > prog_opts_p->texheight)
			{
				total_row_height = total_row_width = row_height = 0;
				char_metrics_p->numtex++;
			}

			// Place the dest char on the indicated tex.
			dst->tex = char_metrics_p->numtex;
			dst->x = total_row_width;
			dst->y = total_row_height;

			// Account for the dest char width.
			total_row_width += dst->w + prog_opts_p->pad;

			// Increase the row height if the dest char is the tallest.
			row_height = IMax(row_height, dst->h);

			char_metrics_p->offsety_bottom = IMin(char_metrics_p->offsety_bottom, dst->offy - dst->h);
			char_metrics_p->offsety_top = IMax(char_metrics_p->offsety_top, dst->offy);

			char_metrics_p->offsetx_left = IMin(char_metrics_p->offsetx_left, dst->offx);
			char_metrics_p->offsetx_right = IMax(char_metrics_p->offsetx_right, dst->offx + dst->w);

			char_metrics_p->char_width_max = IMax(char_metrics_p->char_width_max, dst->w);
			char_metrics_p->char_height_max = IMax(char_metrics_p->char_height_max, dst->h);

			if (dst != order[char_idx])
			{
				--char_idx;
			}
		}
	}

	if (row_height > 0)
	{
		char_metrics_p->numtex++;
	}
}

void
SimplePackFontChars(ProgramOptionP prog_opts_p,
	CharMetricsP char_metrics_p,
	FT_Library type_library, FontCharP* order, int numchars)
{
	int group;
	int char_idx;
	ShelfP shelfp = NULL;
	int placed_chars = 0;

	// Characters are grouped by prefered tex, eg. ASCII wants to be on tex 1.
	// So increment through groups, and then all characters (skipping the chars
	// in groups we aren't yet interested in), until all characters have been
	// placed on the texes.
	for (group = 0; placed_chars < numchars; group++)
	{
		for (char_idx = 0; char_idx < numchars; char_idx++)
		{
			FontCharP dst = order[char_idx];

			// Classify the char by group (the prefered tex).
			int char_group = GroupChar(dst->uni);

			if (dst->tex >= 0)
			{
				continue; // Character has already been placed.
			}

			if (char_group > group)
			{
				continue; // We aren't yet interested in the char's group.
			}

			// Find a shelf with empty space on it that fits the char.
			shelfp = FindShelf(prog_opts_p->texwidth, prog_opts_p->texheight,
				prog_opts_p->pad, dst->w, dst->h);

			placed_chars++;

			// Place the dest char on the indicated tex.
			dst->tex = shelfp->tex;
			dst->x = shelfp->width;
			dst->y = shelfp->offsety;

			// Account for the dest char width.
			shelfp->width += dst->w + prog_opts_p->pad;

			// Increase the shelf height if the dest char is the tallest.
			shelfp->height = IMax(shelfp->height, dst->h);

			// Record statistics for the newly placed char.
			char_metrics_p->offsety_bottom =
				IMin(char_metrics_p->offsety_bottom, dst->offy - dst->h);
			char_metrics_p->offsety_top =
				IMax(char_metrics_p->offsety_top, dst->offy);
			char_metrics_p->offsetx_left =
				IMin(char_metrics_p->offsetx_left, dst->offx);
			char_metrics_p->offsetx_right =
				IMax(char_metrics_p->offsetx_right, dst->offx + dst->w);
			char_metrics_p->char_width_max =
				IMax(char_metrics_p->char_width_max, dst->w);
			char_metrics_p->char_height_max =
				IMax(char_metrics_p->char_height_max, dst->h);
		}
	}

	char_metrics_p->numtex = MaxShelfTex();

	ClearShelves();
}

//////////////////////////////////////////////////////////////////////////////
// PNG File Handling.
//////////////////////////////////////////////////////////////////////////////

typedef struct
{
	uint width;
	uint height;

	uchar bitdepth;
	uchar colortype;
	uchar compress;
	uchar filter;
	uchar interlace;
} PNGHeaderT;

uint
BigSwap(uint n)
{
	const int is_little_endian = 1;

	return *(const uchar*)&is_little_endian
		? (n << 24) | (n >> 24) | ((n >> 8) & 0xFF00) | ((n << 8) & 0xFF0000)
		: n;
}

size_t
WriteBig(FILE* f, uint n)
{
	n = BigSwap(n);
	return fwrite(&n, 1, sizeof(n), f);
}

void
WritePNGChunk(FILE* f, const char* type, uchar* data, uint len)
{
	uint crc;

	WriteBig(f, len);
	fwrite(type, 1, 4, f);
	fwrite(data, 1, len, f);

	crc = crc32(0, Z_NULL, 0);
	crc = crc32(crc, (const Bytef*)type, 4);

	if (data)
	{
		crc = crc32(crc, data, len);
	}

	WriteBig(f, crc);
}

int
SavePNGData(FILE* fp, z_streamp zstrmp, int w, int h,
	int bytes_per_pixel, uchar* data, int flip, uint* lenp, uint* crcp)
{
	uchar buf[1 << 12];

	int i;
	int j;
	int zret;

#define FLUSHZ \
	do \
	{ \
		int flush = sizeof(buf) - zstrmp->avail_out; \
		*crcp = crc32(*crcp, buf, flush); \
		(*lenp) += flush; \
		fwrite(buf, 1, flush, fp); \
		zstrmp->next_out = (Bytef *)buf; \
		zstrmp->avail_out = sizeof(buf); \
	} \
	while (0)

	zstrmp->next_out = (Bytef*)buf;

	zstrmp->avail_out = sizeof(buf);

	for (i = 0; i < h; i++)
	{
		uchar filter = 0;

		for (j = 0; j < 2; j++)
		{
			zstrmp->next_in = j
				? (Bytef*)data + (flip ? h - i - 1 : i) * w * bytes_per_pixel
				: (Bytef*)&filter;
			zstrmp->avail_in = j ? w * bytes_per_pixel : 1;

			while (zstrmp->avail_in > 0)
			{
				if ((zret = deflate(zstrmp, Z_NO_FLUSH)) != Z_OK)
				{
					return zret;
				}

				FLUSHZ;
			}
		}
	}

	// Finish the stream.  This requires us to continually call Z_FINISH until
	// Z_STREAM_END is returned.
	for (;;)
	{
		zret = deflate(zstrmp, Z_FINISH);

		if ((zret != Z_OK) && (zret != Z_STREAM_END))
		{
			return zret;
		}

		FLUSHZ;

		if (zret == Z_STREAM_END)
		{
			break;
		}
	}

	return Z_OK;
}

void
SavePNG(const char* filename, uchar* data, int w, int h, int bytes_per_pixel, int flip)
{
	const uchar signature[] =
	{
		137, 80, 78, 71, 13, 10, 26, 10
	};

	FILE* fp;
	PNGHeaderT ihdr;

	uint len;
	uint crc;
	long idat;

	z_stream zstrm;
	int zret;

	// Initialize the png header.
	memset(&ihdr, 0, sizeof(ihdr));

	switch (bytes_per_pixel)
	{
	case 1:
		// Each pixel is a grayscale sample.
		ihdr.colortype = 0;
		break;
	case 2:
		// Each pixel is a grayscale sample, followed by an alpha sample.
		ihdr.colortype = 4;
		break;
	case 3:
		// Each pixel is an R,G,B triple.
		ihdr.colortype = 2;
		break;
	case 4:
		// Each pixel is an R,G,B triple, followed by an alpha sample.
		ihdr.colortype = 6;
		break;
	default:
		Fatal("Internal error: bad bytes_per_pixel %d", bytes_per_pixel);
		return;
	}

	ihdr.width = BigSwap(w);
	ihdr.height = BigSwap(h);
	ihdr.bitdepth = 8;

	// Open the png file.
	fp = FileOpen(filename, "wb");

	// Initialize the zlib stream.
	zstrm.zalloc = NULL;
	zstrm.zfree = NULL;
	zstrm.opaque = NULL;

	if ((zret = deflateInit(&zstrm, Z_BEST_COMPRESSION)) == Z_OK)
	{
		// Initialize the png file (write the png file header).
		fwrite(signature, 1, sizeof(signature), fp);
		WritePNGChunk(fp, "IHDR", (uchar*)&ihdr, 13);
		idat = ftell(fp);
		len = 0;
		fwrite("\0\0\0\0IDAT", 1, 8, fp);
		crc = crc32(0, Z_NULL, 0);
		crc = crc32(crc, (const Bytef*)"IDAT", 4);

		// Write the font glyphs to the zlib stream.
		if ((zret = SavePNGData(fp, &zstrm, w, h, bytes_per_pixel, data, flip,
			&len, &crc)) == Z_OK)
		{
			// Exitialize the zlib stream.
			zret = deflateEnd(&zstrm);
		}

		if (zret == Z_OK)
		{
			// Exitialize the png file.
			fseek(fp, idat, SEEK_SET);
			WriteBig(fp, len);
			fseek(fp, 0, SEEK_END);
			WriteBig(fp, crc);

			WritePNGChunk(fp, "IEND", NULL, 0);
		}
	}

	fclose(fp);

	if (zret != Z_OK)
	{
		Fatal("Failed saving PNG to '%s': %s.", filename, zError(zret));
	}
}

//////////////////////////////////////////////////////////////////////////////
// Config and Tex File Creation.
//////////////////////////////////////////////////////////////////////////////

char*
GetTexFileName(const char* name, int texnum)
{
	return AppFmtStr(NULL, "%s%d.png", name, texnum);
}

char*
GetTexPath(const char* name, const char* texdir, int texnum)
{
	char* texFileName;
	char* filename;

	if (texdir && *texdir)
	{
		const char* baseName = strrchr(name, '/');
		baseName = baseName ? baseName + 1 : name;

		texFileName = GetTexFileName(baseName, texnum);
		filename = AppFmtStr(NULL, "<grey>%s/%s", texdir, texFileName);
	}
	else
	{
		texFileName = GetTexFileName(name, texnum);
		filename = AppFmtStr(NULL, "<grey>%s", texFileName);
	}

	free(texFileName);

	return filename;
}

void
WriteConfigFile(ProgramOptionP prog_opts_p, CharMetricsP char_metrics_p,
	FontCharP chars, int numchars, char* argument_settings)
{
	FILE* f;
	char* filename;
	char* texPath;
	int i;
	uint lastcode = 0;
	int lasttex = 0;

	filename = AppFmtStr(NULL, "%s.cfg", prog_opts_p->outfile);

	Info("Writing %d chars to %s", numchars, filename);

	f = FileOpen(filename, "w");

	// Write header comment.
	fprintf(f, "// %s%s\n", APP_NAME, argument_settings);

	// Write font description line.
	texPath = GetTexPath(prog_opts_p->outfile, prog_opts_p->texdir, 0);
	if (!prog_opts_p->fontname || !*(prog_opts_p->fontname))
	{
		prog_opts_p->fontname = prog_opts_p->outfile;
	}
	fprintf(f, "font \"%s\" \"%s\" %d %d\n", prog_opts_p->fontname, texPath,
		prog_opts_p->spacewidth,
		prog_opts_p->spaceheight);
	free(texPath);

	// Write the fontchar description comment.
	fprintf(f, "//fontchar x y width height offsetx offsety advance\n");

	for (i = 0; i < numchars; i++)
	{
		FontCharP c = &chars[i];
		const char* utf8char = EncodeUTF8(c->uni);

		if (!lastcode && (lastcode < c->code))
		{
			fprintf(f, "fontoffset \"%s\"\n", utf8char);
			lastcode = c->code;
		}
		else if (lastcode < c->code)
		{
			if (lastcode + 1 == c->code)
			{
				fprintf(f, "fontskip // %d\n", lastcode);
			}
			else
			{
				fprintf(f, "fontskip %d // %d .. %d\n",
					c->code - lastcode, lastcode, c->code - 1);
			}

			lastcode = c->code;
		}

		if (lasttex != c->tex)
		{
			texPath = GetTexPath(prog_opts_p->outfile, prog_opts_p->texdir,
				c->tex);
			fprintf(f, "\nfonttex \"%s\"\n", texPath);
			free(texPath);

			lasttex = c->tex;
		}

		//fontchar xcoord ycoord width height offsetx offsety advance
		//   utf8char (code [ -> unicode][, mapped to unicode])
		fprintf(f, "fontchar %d %d %d %d %d %d %d // %s (%d",
			c->x, c->y, c->w, c->h,
			prog_opts_p->fixedwidth
			? (prog_opts_p->spacewidth - c->w) / 2
			: c->offx + c->offset,
			char_metrics_p->offsety_top - c->offy,
			prog_opts_p->fixedwidth ? prog_opts_p->spacewidth : c->advance,
			utf8char, c->code);

		// If the code page index doesn't match the actual unicode code,
		// display the unicode code.
		if (c->code != c->uni)
		{
			fprintf(f, " -> 0x%X", c->uni);
		}

		if (c->uni != c->mapped_uni)
		{
			fprintf(f, ", mapped to U%04X", c->mapped_uni);
		}

		fprintf(f, ")\n");

		lastcode++;
	}

	free(filename);

	fclose(f);
}

void
WriteTexFiles(ProgramOptionP prog_opts_p, FontCharP chars, int numchars,
	int numtexs)
{
	int tex;

	uchar bgGrayscale = RGBToGrayscale(&prog_opts_p->bgcolor);
	uchar fgGrayscale = RGBToGrayscale(&prog_opts_p->fgcolor);

	// RGB+Alpha = 4, RGB-Alpha = 3, Grayscale+Alpha = 2, Grayscale-Alpha = 1
	int bytes_per_pixel = prog_opts_p->colortex ? 4 : 2;

	if (prog_opts_p->noalpha)
	{
		bytes_per_pixel--;
	}

	uchar* pixels = (uchar*)Allocate(NULL,
		prog_opts_p->texwidth * prog_opts_p->texheight * bytes_per_pixel,
		"textures");

	for (tex = 0; tex < numtexs; tex++)
	{
		char* file = GetTexFileName(prog_opts_p->outfile, tex);
		int texchars = 0, i;
		uchar* dst, * src;

		memset(pixels, 0,
			prog_opts_p->texwidth * prog_opts_p->texheight * bytes_per_pixel);

		for (i = 0; i < numchars; i++)
		{
			FontCharP c = &chars[i];
			int x;
			int y;

			if (c->tex != tex)
			{
				continue;
			}

			texchars++;

			dst = &pixels[
				bytes_per_pixel * (
					(c->y + c->offy - c->alpha->top) * prog_opts_p->texwidth
					+ c->x + c->alpha->left - c->offx
					)];
			src = (uchar*)c->alpha->bitmap.buffer;

			for (y = 0; y < c->alpha->bitmap.rows; y++)
			{
				for (x = 0; x < c->alpha->bitmap.width; x++)
				{
					int pidx = bytes_per_pixel * x;

					if (bytes_per_pixel == 2 || bytes_per_pixel == 4)
					{
						// Alpha is included.
						dst[pidx + (bytes_per_pixel - 1)] = src[x];
					}

					// Ensure the pixels that are outside the color bitmap's
					// bounds are set to the foreground color.  (The alpha
					// bitmap is typically bigger than the color bitmap.)
					switch (bytes_per_pixel)
					{
					case 1:
					case 2:
						dst[pidx] = fgGrayscale;
						break;
					case 3:
					case 4:
						dst[pidx + 0] = prog_opts_p->fgcolor.red;
						dst[pidx + 1] = prog_opts_p->fgcolor.green;
						dst[pidx + 2] = prog_opts_p->fgcolor.blue;
						break;
					default:
						break;
					}
				}

				src += c->alpha->bitmap.pitch;
				dst += bytes_per_pixel * prog_opts_p->texwidth;
			}

			dst = &pixels[
				bytes_per_pixel * (
					(c->y + c->offy - c->color->top) * prog_opts_p->texwidth
					+ c->x + c->color->left - c->offx
					)];
			src = (uchar*)c->color->bitmap.buffer;

			for (y = 0; y < c->color->bitmap.rows; y++)
			{
				for (x = 0; x < c->color->bitmap.width; x++)
				{
					int pidx = bytes_per_pixel * x;

					switch (bytes_per_pixel)
					{
					case 1:
					case 2:
						dst[pidx] =
							LerpColorValue(fgGrayscale, bgGrayscale, src[x]);
						break;
					case 3:
					case 4:
						dst[pidx + 0] =
							LerpColorValue(prog_opts_p->fgcolor.red, prog_opts_p->bgcolor.red, src[x]);
						dst[pidx + 1] =
							LerpColorValue(prog_opts_p->fgcolor.green, prog_opts_p->bgcolor.green, src[x]);
						dst[pidx + 2] =
							LerpColorValue(prog_opts_p->fgcolor.blue, prog_opts_p->bgcolor.blue, src[x]);
						break;
					default:
						break;
					}
				}

				src += c->color->bitmap.pitch;
				dst += bytes_per_pixel * prog_opts_p->texwidth;
			}
		}

		Info("Writing %d chars to %s", texchars, file);

		SavePNG(file, pixels, prog_opts_p->texwidth, prog_opts_p->texheight,
			bytes_per_pixel, 0);

		free(file);
	}

	free(pixels);
}

void
ProcessFontData(ProgramOptionP prog_opts_p,
	CharMetricsP char_metrics_p,
	FontCharP chars, int numchars, char* argument_settings)
{

#if 0
	if (sw <= 0)
	{
		if (FT_Load_Char(f, ' ', FT_LOAD_DEFAULT))
		{
			Fatal("Failed loading space character.");
		}

		sw = (f->glyph->advance.x + 0x3F) >> 6;
	}
#endif

	if (prog_opts_p->spaceheight <= 0)
	{
		prog_opts_p->spaceheight = char_metrics_p->offsety_top - char_metrics_p->offsety_bottom;
	}

	if (prog_opts_p->spacewidth <= 0)
	{
		prog_opts_p->spacewidth = prog_opts_p->spaceheight / 3;
	}

	if (!prog_opts_p->noexec)
	{
		WriteTexFiles(prog_opts_p, chars, numchars, char_metrics_p->numtex);

		WriteConfigFile(prog_opts_p, char_metrics_p, chars, numchars,
			argument_settings);
	}

	Info("(%d, %d) .. (%d, %d) = (%d, %d) / (%d, %d), %d texs",
		char_metrics_p->offsetx_left, char_metrics_p->offsety_bottom,
		char_metrics_p->offsetx_right, char_metrics_p->offsety_top,
		char_metrics_p->offsetx_right - char_metrics_p->offsetx_left,
		char_metrics_p->offsety_top - char_metrics_p->offsety_bottom,
		char_metrics_p->char_width_max, char_metrics_p->char_height_max,
		char_metrics_p->numtex);
}

//////////////////////////////////////////////////////////////////////////////
// Main.
//////////////////////////////////////////////////////////////////////////////

int
main(int argc, char** argv)
{
	ProgramOptionT prog_opts;
	char* argument_settings;
	FT_Error error;

	FT_Library type_library;

	FontCharT chars[CODE_PAGE_SIZE];
	FontCharP order[CODE_PAGE_SIZE];
	int numchars = 0;

	CharMetricsT char_metrics;
	int char_idx;

	char_metrics.offsetx_left = INT_MAX;
	char_metrics.offsetx_right = INT_MIN;
	char_metrics.offsety_bottom = INT_MAX;
	char_metrics.offsety_top = INT_MIN;
	char_metrics.char_width_max = 0;
	char_metrics.char_height_max = 0;
	char_metrics.numtex = 0;

	// Option processing sets the program name.
	if (!ProcessArguments(&prog_opts, ProgramOptionDefs, ProgramArgDefs,
		APP_NAME, APP_VERSION, APP_COPYRIGHT, argc, argv))
	{
		// Argument processing is benignly terminating execution, usually due
		// to the display of help or the version.
		return EXIT_SUCCESS;
	}

	// Get the argument settings so we can save them in the config file.
	argument_settings = GetArgumentSettings(&prog_opts, ProgramOptionDefs,
		ProgramArgDefs);

	if ((error = FT_Init_FreeType(&type_library)))
	{
		FTFatal(error, "Failed initing freetype.");
	}

	numchars = LoadFontGlyphs(&prog_opts, type_library,
		chars, order, CODE_PAGE_SIZE);

	if (prog_opts.sauerpack)
	{
		qsort(order, numchars, sizeof(order[0]), FontCharCmp);
		PackFontChars(&prog_opts, &char_metrics, type_library, order, numchars);
	}
	else
	{
		qsort(order, numchars, sizeof(order[0]), SimpleFontCharCmp);
		SimplePackFontChars(&prog_opts, &char_metrics, type_library,
			order, numchars);
	}

	ProcessFontData(&prog_opts, &char_metrics, chars, numchars,
		argument_settings);

	// We are finished, so free allocated resources.

	for (char_idx = 0; char_idx < numchars; char_idx++)
	{
		FT_Done_Glyph((FT_Glyph)chars[char_idx].alpha);
		FT_Done_Glyph((FT_Glyph)chars[char_idx].color);
	}

	FT_Done_FreeType(type_library);

	free(argument_settings);
	free(prog_opts.fontmap);
	free(prog_opts.altfont);

	return EXIT_SUCCESS;
}
