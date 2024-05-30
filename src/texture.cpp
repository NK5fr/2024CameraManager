#include "texture.h"

using namespace OpenGL;

Texture::Texture() {
    glGenTextures(1, &textureId);
    textureWidth = 0;
    textureHeight = 0;
    textureMagFilter = TextureFilter::NEAREST;
    textureMinFilter = TextureFilter::NEAREST;
    textureWrap = TextureWrap::CLAMP_TO_BORDER;
    pixelFormat = PixelFormat::RGBA;
}

Texture::~Texture() {
    glDeleteTextures(1, &textureId);
}

/* // Uses FreeImage-library
bool Texture::loadTextureFromFile(const char* filepath, PixelFormat pixelFormat) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    this->pixelFormat = pixelFormat;
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filepath, 0);
    FIBITMAP* image = FreeImage_Load(format, filepath);
    if (image == nullptr) {
        //printf("Error loading image from file: \'%s\'\n", filepath);
        return false;
    }
    int numchannels = 0;
    if (pixelFormat == PixelFormat::ALPHA || pixelFormat == PixelFormat::LUMINANCE) {
        image = FreeImage_ConvertTo8Bits(image);
        numchannels = 1;
    } else if (pixelFormat == PixelFormat::RGB) {
        image = FreeImage_ConvertTo24Bits(image);
        numchannels = 3;
    } else if (pixelFormat == PixelFormat::RGBA) {
        image = FreeImage_ConvertTo32Bits(image);
        numchannels = 4;
    } else return false;
    textureWidth = FreeImage_GetWidth(image);
    textureHeight = FreeImage_GetHeight(image);
    
    GLuint textureFormat = GL_LUMINANCE;
    if (pixelFormat == PixelFormat::ALPHA) {
        textureFormat = GL_ALPHA;
    } else if (pixelFormat == PixelFormat::RGB) {
        textureFormat = GL_RGB;
    } else if (pixelFormat == PixelFormat::RGBA) {
        textureFormat = GL_RGBA;
    }
    setTextureFilter(textureMinFilter, textureMagFilter);
    setTextureWrap(textureWrap);
    
    imageBuffer.copyIntoBuffer(FreeImage_GetBits(image), textureWidth * textureHeight * numchannels);
    glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, textureWidth, textureHeight, 0, textureFormat, GL_UNSIGNED_BYTE, FreeImage_GetBits(image));
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    FreeImage_Unload(image);
    return true;
}
*/

bool Texture::updateTexture(unsigned char* buffer, unsigned int bufferSize) {
    if (imageBuffer.getUsedBuffer() < bufferSize) return false;
    GLuint textureFormat = 0;
    if (pixelFormat == PixelFormat::LUMINANCE) {
        textureFormat = GL_LUMINANCE;
    } else if (pixelFormat == PixelFormat::ALPHA) {
        textureFormat = GL_ALPHA;
    } else if (pixelFormat == PixelFormat::RGB) {
        textureFormat = GL_RGB;
    } else {
        textureFormat = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, textureFormat, GL_UNSIGNED_BYTE, buffer);
    //glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, textureWidth, textureHeight, 0, textureFormat, GL_UNSIGNED_BYTE, buffer);
    imageBuffer.copyIntoBuffer(buffer, bufferSize);
    return true;
}

void Texture::createEmptyTexture(int imageWidth, int imageHeight, PixelFormat pixelFormat) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    this->pixelFormat = pixelFormat;
    int numchannels = 0;
    if (pixelFormat == PixelFormat::ALPHA || pixelFormat == PixelFormat::LUMINANCE) {
        numchannels = 1;
    } else if (pixelFormat == PixelFormat::RGB) {
        numchannels = 3;
    } else if (pixelFormat == PixelFormat::RGBA) {
        numchannels = 4;
    }
    GLuint textureFormat = GL_LUMINANCE;
    if (pixelFormat == PixelFormat::ALPHA) {
        textureFormat = GL_ALPHA;
    } else if (pixelFormat == PixelFormat::RGB) {
        textureFormat = GL_RGB;
    } else if (pixelFormat == PixelFormat::RGBA) {
        textureFormat = GL_RGBA;
    }
    setTextureFilter(textureMinFilter, textureMagFilter);
    setTextureWrap(textureWrap);
    textureWidth = imageWidth;
    textureHeight = imageHeight;
    imageBuffer.createBuffer(imageWidth * imageHeight * numchannels);
    imageBuffer.zeroBuffer();

    glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, textureWidth, textureHeight, 0, textureFormat, GL_UNSIGNED_BYTE, imageBuffer.getDataBuffer());
}

void Texture::setTextureFilter(TextureFilter texMinFilter, TextureFilter texMagFilter) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    textureMinFilter = texMinFilter;
    textureMagFilter = texMagFilter;
    if (texMinFilter == TextureFilter::LINEAR) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else if (texMinFilter == TextureFilter::NEAREST) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    if (texMagFilter == TextureFilter::LINEAR) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (texMagFilter == TextureFilter::NEAREST) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}

void Texture::setTextureWrap(TextureWrap texWrap) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    textureWrap = texWrap;
    if (texWrap == TextureWrap::CLAMP_TO_EDGE) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else if (texWrap == TextureWrap::CLAMP_TO_BORDER) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    } else if (texWrap == TextureWrap::MIRRORED_REPEAT) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    } else if (texWrap == TextureWrap::REPEAT) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
}
