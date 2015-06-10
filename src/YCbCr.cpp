//
//  YCbCr.cpp
//  RGB_YCbCr
//
//  Created by Henrique Penteado Kujawski Périgo on 09/06/15.
//
//

#include "YCbCr.h"
#include "cinder/gl/Texture.h"

using namespace ci;


// -------- SurfaceYCbCr

SurfaceYCbCr::SurfaceYCbCr(){
    
    
}

SurfaceYCbCr::~SurfaceYCbCr(){
    
    
}

SurfaceYCbCr::SurfaceYCbCr(const ci::Surface& original, float compressionAmount){
    
    setSurface(original, compressionAmount);
    
}

void SurfaceYCbCr::setSurface(const ci::Surface &original, float compressionAmount){
    
    
    Y =  Channel(original.getWidth(), original.getHeight());
    Cb = Channel(original.getWidth() * compressionAmount, original.getHeight() * compressionAmount);
    Cr = Channel(original.getWidth() * compressionAmount, original.getHeight() * compressionAmount);
    
    Surface::ConstIter iter = original.getIter();
    amt = compressionAmount;
    
    while( iter.line() ){
        while( iter.pixel() ){
            
            Vec2i pos = iter.getPos();
            Vec2i scaledPos = pos * amt;
            
            uint8_t r = iter.r();
            uint8_t g = iter.g();
            uint8_t b = iter.b();
            
            
            uint8_t y = constrain<uint8_t>( 0.299f * r + 0.587f * g + 0.114f*b, 0, 255 );
            uint8_t cb = 128 -  ( 0.168736f *r )  - (0.331264f*g) + ( 0.5f * b );
            uint8_t cr = 128 + (0.5f*r) - (0.418688f*g) - (0.081312f*b);
            
            *Y.getData( pos ) = y;
            *Cb.getData( scaledPos ) = cb;
            *Cr.getData( scaledPos ) = cr;
        }
        
    }
    
    
    
}

Surface SurfaceYCbCr::toSurface(){
    
    Channel::Iter ch_iter = Y.getIter();
    
    Surface8u final(Y.getWidth(), Y.getHeight(), false);
    
    
    while(ch_iter.line()){
        while(ch_iter.pixel()){
            
            auto pos = ch_iter.getPos();
            Vec2i scaledPos = pos * amt;
            
            uint8_t y = ch_iter.v();
            uint8_t cb = *Cb.getData(scaledPos);
            uint8_t cr = *Cr.getData(scaledPos);
            
            // calculate RGB values
            uint8_t r = constrain<int16_t>(y + 1.140f * (cr - 128), 0, 255);
            uint8_t g = constrain<int16_t>(y - 0.34414f * (cb - 128) - 0.71414f * ( cr - 128 ), 0, 255);
            uint8_t b = constrain<int16_t>(y + 1.772f * (cb - 128), 0, 255);
            
            // apply values to final image to pointer p
            uint8_t *p = final.getData( pos );
            p[final.getRedOffset()] = r;
            p[final.getGreenOffset()] = g;
            p[final.getBlueOffset()] = b;
        }
        
    }
    
    return final;
}


//------------ TextureYCbCr

ci::gl::GlslProgRef TextureYCbCr::mShader = nullptr;

TextureYCbCr::TextureYCbCr(const SurfaceYCbCr& surf ){
    
    setSurfaceYCbCr(surf);
    
}


void TextureYCbCr::setSurfaceYCbCr(const SurfaceYCbCr &surf){
    
    YTex = gl::Texture::create( surf.getChannelLuma() );
    CrTex = gl::Texture::create( surf.getChannelChromaR() );
    CbTex = gl::Texture::create( surf.getChannelChromaB() );
    
    initializeShader();
}



void TextureYCbCr::initializeShader(){
    
    if(!mShader){
    
        try {
            mShader = gl::GlslProg::create( app::loadAsset("shaders/passThru_vert.glsl"), app::loadAsset("shaders/YCbCr_frag.glsl") );
        }
        catch( gl::GlslProgCompileExc &exc ) {
            std::cout << "Shader compile error: " << std::endl;
            std::cout << exc.what();
        }
        catch( ... ) {
            std::cout << "Unable to load shader" << std::endl;
        }
    }
    
}

void TextureYCbCr::bindShaderTexture(){
    
    YTex->bind(0);
    CrTex->bind(1);
    CbTex->bind(2);
    
    mShader->bind();
    mShader->uniform( "Y", 0 );  // texunit 0
    mShader->uniform( "Cr", 1 );  // texunit 1
    mShader->uniform( "Cb", 2 );  // texunit 1
    
}

void TextureYCbCr::unbindShaderTexture(){
    
    YTex->disable();
    CrTex->disable();
    CbTex->disable();
    mShader->unbind();
    
}


