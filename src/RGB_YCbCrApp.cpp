#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"


#include "cinder/ImageIo.h"
#include "cinder/Timer.h"
#include "cinder/Matrix.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SurfaceYCbCr{
public:
    SurfaceYCbCr(){ };
    ~SurfaceYCbCr() {  };
    
    // construct a YCbCr surface and compress it by given amount
    SurfaceYCbCr(const ci::Surface& original, float compressionAmount = 1.0f){
        
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
    
    //decompress to RGB format
    ci::Surface toSurface(){
        
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
    
    
    //TODO: write a method to decrompress texture on the GPU using shaders...
    //reference: https://github.com/paulhoux/Cinder-FFmpeg/blob/master/templates/Basic%20FFmpeg/assets/framerender_frag.glsl
    
    void loadShader(){
        
        
        try {
            mShader = gl::GlslProg::create( loadAsset("passThru_vert.glsl"), loadAsset("YCbCr_frag.glsl") );
        }
        catch( gl::GlslProgCompileExc &exc ) {
            std::cout << "Shader compile error: " << std::endl;
            std::cout << exc.what();
        }
        catch( ... ) {
            std::cout << "Unable to load shader" << std::endl;
        }
        
        YTex = gl::Texture::create( Y );
        CrTex = gl::Texture::create( Cr );
        CbTex = gl::Texture::create( Cb );
        
    }
    
    void bindShaderTexture(){
        
        YTex->bind(0);
        CrTex->bind(1);
        CbTex->bind(2);
        
        mShader->bind();
        mShader->uniform( "Y", 0 );  // texunit 0
        mShader->uniform( "Cr", 1 );  // texunit 1
        mShader->uniform( "Cb", 2 );  // texunit 1
        
    }
    
    void unbindShaderTexture(){
        
        YTex->disable();
        CrTex->disable();
        CbTex->disable();
        mShader->unbind();
    }
    //--
    
    
    Channel getChannelLuma(){ return Y; };
    Channel getChannelChromaB(){ return Cb; };
    Channel getChannelChromaR(){ return Cr; };
    
    Vec2i getlLumaSize(){ return Y.getSize(); };
    Vec2i getChromaBlueSize(){ return Cb.getSize(); };
    Vec2i getChromaRedSize() { return Cr.getSize(); };
    
    float getCompressionAmount() { return amt; };
    
    //returns the number of bytes of all 3 channels combined
    int getCompressedSize() {return  (Y.getRowBytes() * Y.getHeight()) + (Cb.getRowBytes() * Cb.getHeight()) +  (Cr.getRowBytes() * Cr.getHeight() ); }
    
private:
    
    
    
    Channel Y;
    Channel Cb;
    Channel Cr;
    
    float amt;
    
    gl::GlslProgRef mShader;
    gl::TextureRef YTex;
    gl::TextureRef CrTex;
    gl::TextureRef CbTex;
};


class RGB_YCrCbApp : public AppNative {
public:
    void setup();
    void mouseDown( MouseEvent event );
    void update();
    void draw();
    
    
    Surface original;
    Surface final;
    
    SurfaceYCbCr img;
    gl::TextureRef mTexture;
    
};


void RGB_YCrCbApp::setup()
{
    
    original = loadImage(loadAsset("kid.png"));
    setWindowSize(original.getSize());
    float scale = 0.25f;
    
    Timer t;
    
    img = SurfaceYCbCr(original, scale);
    img.loadShader();
    t.start();
    final = img.toSurface();
    console() << "took: " << t.getSeconds() << endl;
    
    int size = original.getRowBytes() * original.getHeight();
    int csize =  img.getCompressedSize();
    
    console() << "original image is: " << size << " bytes" << endl;
    console() << "compressed image is: " << csize << " bytes" << endl;
    console() << "rate: " << size/(float)csize << endl;
    
    mTexture = gl::Texture::create(final);
}

void RGB_YCrCbApp::mouseDown( MouseEvent event )
{
}

void RGB_YCrCbApp::update()
{
}

void RGB_YCrCbApp::draw()
{
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
    
    Timer mTimer;
    mTimer.start();
    
    //    gl::draw(*mTexture);
    
    img.bindShaderTexture();
    gl::drawSolidRect(getWindowBounds());
    img.unbindShaderTexture();
    
    console() << "t: " << mTimer.getSeconds() << endl;
    
    
    
}

CINDER_APP_NATIVE( RGB_YCrCbApp, RendererGl )
