//
//  YCbCr.h
//  RGB_YCbCr
//
//  Created by Henrique Penteado Kujawski Périgo on 09/06/15.
//
//

#ifndef __RGB_YCbCr__YCbCr__
#define __RGB_YCbCr__YCbCr__

#include "cinder/Surface.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"

class SurfaceYCbCr{
public:
    SurfaceYCbCr();
    ~SurfaceYCbCr();
    SurfaceYCbCr(const ci::Surface& original, float compressionAmount = 1.0f);
    
    void setSurface(const ci::Surface& original, float compressionAmount = 1.0f);
    ci::Surface toSurface();
    
    ci::Channel getChannelLuma() const { return Y; };
    ci::Channel getChannelChromaB()const { return Cb; };
    ci::Channel getChannelChromaR() const { return Cr; };
    
    ci::Vec2i getlLumaSize() const { return Y.getSize(); };
    ci::Vec2i getChromaBlueSize() const { return Cb.getSize(); };
    ci::Vec2i getChromaRedSize() const { return Cr.getSize(); };
    
    float getCompressionAmount() const { return amt; };
    
    //returns the number of bytes of all 3 channels combined
    int getCompressedSize() {return  (Y.getRowBytes() * Y.getHeight()) + (Cb.getRowBytes() * Cb.getHeight()) +  (Cr.getRowBytes() * Cr.getHeight() ); }
    
private:
    
    ci::Channel Y;
    ci::Channel Cb;
    ci::Channel Cr;
    
    float amt;
};

class TextureYCbCr{
public:
    TextureYCbCr(){ };
    ~TextureYCbCr() {  };
    TextureYCbCr(const SurfaceYCbCr& surf );
    
    void bindShaderTexture();
    void unbindShaderTexture();
    void setTexture(const SurfaceYCbCr& surf);
    
    ci::gl::TextureRef getYTexture() const  { return YTex;  };
    ci::gl::TextureRef getCbTexture() const { return CbTex; };
    ci::gl::TextureRef getCrTexture() const { return CrTex; };
    
private:
    static ci::gl::GlslProgRef mShader;
    
    void initializeShader();
    ci::gl::TextureRef YTex;
    ci::gl::TextureRef CrTex;
    ci::gl::TextureRef CbTex;
};


#endif /* defined(__RGB_YCbCr__YCbCr__) */
