#include "engine.h"

#include <ft2build.h>
#include FT_FREETYPE_H

static hashnameset<font> fonts;
// TODO: better font setup
font *curfont = NULL;

FT_Library ft;
// Inits the font library
bool init_fonts()
{
    if(FT_Init_FreeType(&ft))
    {
        return false;
    }
    return true;
}

// cleans up the font library
void cleanup_fonts()
{
    FT_Done_FreeType(ft);
}

// This was instrumental in writing:
// https://learnopengl.com/In-Practice/Text-Rendering
void newfont(char *name, char *fp, int *defaultw, int *defaulth)
{
    font *f = &fonts[name];
    if (!f->name) f->name = newstring(name);

    f->scale = *defaulth;

    FT_Face face;
    if (FT_New_Face(ft, fp, 0, &face))
    {
        conoutf(CON_ERROR, "freetype could not load font %s", fp);
    }

    FT_Set_Pixel_Sizes(face, *(unsigned int*)defaultw, *(unsigned int*)defaulth);

    f->lineheight = (face->size->metrics.ascender - face->size->metrics.descender) >> 6;

    f->chars.shrink(0);
    f->charoffset = 0;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction


    unsigned char c;
    for (c = 0; c < 128; c++) // hehe c++
    {
        // conoutf(CON_DEBUG, "%c", c);
        unsigned long uc = FT_Get_Char_Index(face, c);
        if (FT_Load_Glyph(face, uc, FT_LOAD_RENDER))
        {
            conoutf(CON_WARN, "Font is missing glpyh %c.", uc);
            continue;
        }

        // TODO: glue to the textures system
        unsigned int texid;
        glGenTextures(1, &texid);
        glBindTexture(GL_TEXTURE_2D, texid);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Swizzle to alpha
        GLint swizzlemask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzlemask);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        font::charinfo &cinfo = f->chars.add();
        cinfo.tex = texid;
        cinfo.offsetx = face->glyph->bitmap_left;
        cinfo.offsety = face->glyph->bitmap_top;
        cinfo.w = face->glyph->bitmap.width;
        cinfo.h = face->glyph->bitmap.rows;
        cinfo.advance = face->glyph->advance.x;
    }

    FT_Done_Face(face);
}

COMMANDN(font, newfont, "ssii");

// sets the curfont
bool setfont(const char *name)
{
    font *f = fonts.access(name);
    if(!f) return false;
    curfont = f;
    return true;
}

// A stack to store fonts.
// Ideally you'd push a font to it when switching, so you can go back to the previous font with pop.
static vector<font *> fontstack;

// Pushes the current font to the stack
void pushfont()
{
    fontstack.add(curfont);
}

// Pops the font off of the stack
bool popfont()
{
    if (fontstack.empty()) return false;
    curfont = fontstack.pop();
    return true;
}

// Returns the resolution we render text at(?)
void gettextres(int& w, int& h)
{
    if (w < MINRESW || h < MINRESH)
    {
        if (MINRESW > w * MINRESH / h)
        {
            h = h * MINRESW / w;
            w = MINRESW;
        }
        else
        {
            w = w * MINRESH / h;
            h = MINRESH;
        }
    }
}
// Sets the width, height values to the charinfo associated with the character, according to the current font.
void font_metrics(char c, float& width, float& height, float& offsetx, float& offsety, float& advance)
{
    font::charinfo cinfo = curfont->chars[c - curfont->charoffset];

    width = cinfo.w;
    height = cinfo.h;

    offsetx = cinfo.offsetx;
    offsety = (curfont->lineheight * 0.6) - cinfo.offsety;
    //offsety = (curfont->lineheight * 0.5) - cinfo.offsety;

    // For some reason the value here isn't in pixels.
    // WHY? Anyway, we need to bitshift it.
    advance = (cinfo.advance >> 6);
}

// Returns the width that text will take up to render the string.
float text_widthf(const char *str) 
{
    float w, h;
    text_boundsf(str, w, h);
    return w;
}

// TODO: What does this do? And why?
void draw_textf(const char *fstr, int left, int top, ...)
{
    defvformatstring(str, top, fstr);
    draw_text(str, left, top);
}

const matrix4x3 *textmatrix = NULL;

// Draws the given character, if the font has it.
static float draw_char(char c, float x, float y, float scale = 1.0)
{
    if (!curfont->chars.inrange(c-curfont->charoffset)) return 0.0;
    font::charinfo cinfo = curfont->chars[c-curfont->charoffset];

    float w, h, ox, oy, adv;
    font_metrics(c, w, h, ox, oy, adv);

    float x1 = x + scale * ox;
    float y1 = y + scale * oy;
    float x2 = x1 + scale * w;
    float y2 = y1 + scale * h;

    gle::begin(GL_QUADS);
        glBindTexture(GL_TEXTURE_2D, cinfo.tex);

    if (textmatrix) // 3D TEXT
    {
        gle::attrib(textmatrix->transform(vec2(x1, y1))); gle::attribf(0, 0);
        gle::attrib(textmatrix->transform(vec2(x2, y1))); gle::attribf(1, 0);
        gle::attrib(textmatrix->transform(vec2(x2, y2))); gle::attribf(1, 1);
        gle::attrib(textmatrix->transform(vec2(x1, y2))); gle::attribf(0, 1);
    }
    else // HUD TEXT
    {
        gle::attribf(x1, y1);  gle::attribf(0,0);
        gle::attribf(x2, y1);  gle::attribf(1, 0);
        gle::attribf(x2, y2);  gle::attribf(1, 1);
        gle::attribf(x1, y2);  gle::attribf(0, 1);
    }
    gle::end();

    return scale*adv;
}

// Sets the gle::color to the text colour associated with the code.
static void text_color(char c, char* stack, int size, int& sp, bvec color, int a)
{
    if (c == 's') // save color
    {
        c = stack[sp];
        if (sp < size - 1) stack[++sp] = c;
    }
    else
    {
        xtraverts += gle::end();
        if (c == 'r') { if (sp > 0) --sp; c = stack[sp]; } // restore color
        else stack[sp] = c;
        switch (c)
        {
        case 'g': case '0': color = bvec(64, 255, 128); break; // green
        case 'b': case '1': color = bvec(46, 130, 255); break; // blue
        case 'y': case '2': color = bvec(255, 192, 64); break; // yellow
        /*case 'r':*/ case '3': color = bvec(237, 43, 44); break; // red
        case 'a': case '4': color = bvec(128, 128, 128); break; // grey
        case 'm': case '5': color = bvec(192, 64, 192); break; // magenta
        case 'o': case '6': color = bvec(255, 128, 0); break; // orange
        case 'w': case '7': color = bvec(255, 255, 255); break; // white
        case 'k': case '8': color = bvec(0, 0, 0); break; // black
        /*case 'c':*/ case '9': color = bvec(64, 255, 255); break; // cyan
        case 'v': color = bvec(192, 96, 255); break; // violet
        case 'p': color = bvec(224, 64, 224); break; // purple
        case 'n': color = bvec(164, 72, 56); break; // brown
        case 'G': color = bvec(86, 164, 56); break; // dark green
        case 'B': color = bvec(56, 64, 172); break; // dark blue
        case 'Y': color = bvec(172, 172, 0); break; // dark yellow
        case 'R': color = bvec(172, 56, 56); break; // dark red
        case 'M': color = bvec(172, 72, 172); break; // dark magenta
        case 'O': color = bvec(172, 56, 0); break; // dark orange
        case 'C': color = bvec(48, 172, 172); break; // dark cyan
        case 'A': case 'd': color = bvec(102, 102, 102); break; // dark grey
        case 'h': color = bvec(255, 36, 0); break;   // hn scarlet
        case 'l': color = bvec(0, 148, 255); break;  // celeb blue
        // provided color: everything else
        }
        gle::color(color, a);
    }
}

// The character visible in text(?)
int text_visible(const char *str, float hitx, float hity, int maxwidth)
{
    return 0;
}

//inverse of text_visible
void text_posf(const char *str, int cursor, float &cx, float &cy, int maxwidth) 
{
}

// Sets the width, height variables to the amount of space that the string would occupy, if rendered.
void text_boundsf(const char *str, float &width, float &height, int maxwidth)
{
    width = 0.0;
    float usewidth = 0.0;
    height = curfont->lineheight;

    int i;
    for (i = 0; str[i]; i++)
    {
        char c = uchar(str[i]);

        float w, h, ox, oy, adv;
        font_metrics(c, w, h, ox, oy, adv);

        // Test for special characters, like newlines, as they don't take up space.
        if (c == '\r' || c == '\n')
        {
            height += (h + oy);


            if (usewidth > width)
            {
                width = usewidth;
            }
            usewidth = 0.0;

            continue;
        }
        else if (c == '\t') // This doesn't do anything yet.
        {
            continue;
        }
        else if (c == '\f') // Colour code doesn't take up any space.
        {
            continue;
        }

        width += (adv);
    }

    if (usewidth > width)
    {
        width = usewidth;
    }
    
    if (maxwidth != -1)
        width = maxwidth;
        
}

// Draws the given string at the x (left), y (top) coords
void draw_text(const char *str, int left, int top, int r, int g, int b, int a, int cursor, int maxwidth) 
{
    bvec color(r,g,b);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Gah! Damn you cube 2!
    if (a < 0)
    {
        a = -a;
    }

    gle::color(color, a);
    gle::defvertex(2);

    gle::deftexcoord0();

    float x = left;
    float y = top;

    float dontgoover;

    if (maxwidth != -1)
        dontgoover = x + maxwidth;
    else
        dontgoover = screenw;

    char colorstack[10];
    colorstack[0] = 'c'; //indicate user color

    int i;
    for(i = 0; str[i]; i++)
    {

        char c = uchar(str[i]);
        
        // Test for special characters, like newlines
        if (c == '\r' || c == '\n' || x > dontgoover)
        {
            x = left;
            y += curfont->lineheight;
            continue;
        }
        else if (c == '\t') // Tab
        {
            continue;
        }
        else if (c == '\f') // Colouring
        {
            int cpos = 0;
            if (str[i+1]) {
                text_color(str[i+1], colorstack, sizeof(colorstack), cpos, color, a);
                i++; // skip colour number
            }
            continue;
        }

        x += (int)draw_char(c, x, y, 1.0);
    }
}

// Unimplemented.
// Reloads fonts from the file.
void reloadfonts()
{
}