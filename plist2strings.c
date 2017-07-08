/* https://stackoverflow.com/questions/27672815/parsing-xml-file-with-expat-library-in-c */

#include <expat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define SQUELCHNULL(x) (((x) != NULL) ? (x) : "")

static XML_Parser parser;
static char *current_key		 = NULL;
static char *product_build_version = NULL;
static char *product_name		 = NULL;
static char *product_version	 = NULL;

void XMLCALL
handler_key(void *userData, const XML_Char *s, int len)
{
    if(current_key) free(current_key);

    if(len == 0) {
        current_key = NULL;
        return;
    }

    current_key = strndup(s, len);
}

void XMLCALL
handler_build_version(void *userData, const XML_Char *s, int len)
{
    if(len == 0) {
        return;
    }

    product_build_version = strndup(s, len);
}

void XMLCALL
handler_name(void *userData, const XML_Char *s, int len)
{
    if(len == 0) {
        return;
    }

    product_name = strndup(s, len);
}

void XMLCALL
handler_version(void *userData, const XML_Char *s, int len)
{
    if(len == 0) {
        return;
    }

    product_version = strndup(s, len);
}

static void XMLCALL
startElement(void *userData, const char *name, const char **atts)
{
    int n;
    n = strcmp("key", name);
    if(n > 0) return;
    if(n == 0) {
        XML_SetCharacterDataHandler(parser, handler_key);
        return;
    }
    n = strcmp("string", name);
    if(n > 0) return;
    if(n == 0) {
        n = strcmp("ProductBuildVersion", current_key);
        if(n > 0) return;
        if(n == 0) {
            XML_SetCharacterDataHandler(parser, handler_build_version);
            return;
        }
        n = strcmp("ProductName", current_key);
        if(n > 0) return;
        if(n == 0) {
            XML_SetCharacterDataHandler(parser, handler_name);
            return;
        }
        n = strcmp("ProductVersion", current_key);
        if(n > 0) return;
        if(n == 0) {
            XML_SetCharacterDataHandler(parser, handler_version);
            return;
        }
        return;
    }
}

static void XMLCALL
endElement(void *userData, const char *name)
{
    XML_SetCharacterDataHandler(parser, NULL);
}

int
main(int argc, char *argv[])
{
    char buf[BUFSIZ];

    if(argc < 2) return 1;

    parser = XML_ParserCreate(NULL);

    int done;

    XML_SetElementHandler(parser, startElement, endElement);

    FILE *fp = (strncmp("-", argv[1], 1) == 0) ? stdin : fopen(argv[1], "r");

    if(fp == NULL) {
        perror(argv[1]);
        return 2;
    }

    do {
        int len = (int)fread(buf, 1, sizeof(buf), fp);
        done = len < sizeof(buf);
        if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
            fprintf(stderr,
                    "%s at line %" XML_FMT_INT_MOD "u\n",
                    XML_ErrorString(XML_GetErrorCode(parser)),
                    XML_GetCurrentLineNumber(parser));
            return 1;
        }
    } while (!done);

    XML_ParserFree(parser);

    printf("%s\t%s\t%s\n",
		    SQUELCHNULL(product_build_version),
		    SQUELCHNULL(product_name),
		    SQUELCHNULL(product_version));

    return 0;
}
