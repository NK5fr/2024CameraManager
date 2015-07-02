
#ifndef TEXTURE_H
#define TEXTURE_H

//#include <FreeImage.h>
#include <QtOpenGL/qgl.h>
#include "databuffer.h"

namespace OpenGL {
    enum PixelFormat {
        ALPHA,
        RGBA,
        RGB,
        LUMINANCE
    };

    enum TextureWrap {
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
        REPEAT,
        MIRRORED_REPEAT
    };

    enum TextureFilter {
        NEAREST,
        LINEAR
    };

    class Texture {
    public:
        Texture();
        ~Texture();

        //bool loadTextureFromFile(const char* filepath, PixelFormat pixelFormat = PixelFormat::LUMINANCE);
        bool updateTexture(unsigned char* imageBuffer, unsigned int bufferSize);
        void createEmptyTexture(int imageWidth, int imageHeight, PixelFormat pixelFormat = PixelFormat::LUMINANCE);
        void setTextureFilter(TextureFilter texMinFilter, TextureFilter texMagFilter);
        void setTextureWrap(TextureWrap texWrap);

        inline void bind() { glBindTexture(GL_TEXTURE_2D, textureId); };
        inline void unbind() { glBindTexture(GL_TEXTURE_2D, 0); };

        inline GLuint getTextureId() const { return this->textureId; }
        inline GLuint getTextureWidth() const { return this->textureWidth; }
        inline GLuint getTextureHeight() const { return this->textureHeight; }
        inline TextureFilter getTextureMagFilter() { return this->textureMagFilter; }
        inline TextureFilter getTextureMinFilter() { return this->textureMinFilter; }
        inline TextureWrap getTextureWrap() { return this->textureWrap; }
        inline PixelFormat getPixelFormat() { return this->pixelFormat; }
        inline DataBuffer* getImageBuffer() { return &this->imageBuffer; }
        inline unsigned int getTextureSize() const { return this->imageBuffer.getUsedBuffer(); }

    private:
        GLuint textureId;
        GLuint textureWidth;
        GLuint textureHeight;
        TextureFilter textureMinFilter;
        TextureFilter textureMagFilter;
        TextureWrap textureWrap;
        PixelFormat pixelFormat;
        DataBuffer imageBuffer;
    };
}

#endif