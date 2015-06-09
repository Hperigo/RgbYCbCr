#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Timer.h"
#include "cinder/gl/Texture.h"

#include "YCbCr.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class RGB_YCrCbApp : public AppNative {
public:
    void setup();
    void mouseDown( MouseEvent event );
    void update();
    void draw();
    
    
    Surface original;
    Surface final;
    
    SurfaceYCbCr img;
    TextureYCbCr tex;
    
    
    gl::TextureRef mTexture;
    
};


void RGB_YCrCbApp::setup()
{
    
    original = loadImage(loadAsset("kid.png"));
    
    float scale = 0.25f;
    Timer t;
    
    // Create a YCbCr Surface -----
    img = SurfaceYCbCr(original, scale);
    t.start();
    final = img.toSurface();
    
    console() << " compression and decompression took: " << t.getSeconds() << endl;
    
    int size = original.getRowBytes() * original.getHeight();
    int csize =  img.getCompressedSize();
    
    console() << "original image is: " << size << " bytes" << endl;
    console() << "compressed image is: " << csize << " bytes" << endl;
    console() << "rate: " << size/(float)csize << endl;
    
    mTexture = gl::Texture::create(final);
    
    // Create a YCbCr Texture -----
    
    tex.setTexture(img);
    
    
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
    
    tex.bindShaderTexture();
    gl::drawSolidRect(getWindowBounds());
    tex.unbindShaderTexture();
    
    console() << "t: " << mTimer.getSeconds() << endl;

}

CINDER_APP_NATIVE( RGB_YCrCbApp, RendererGl )
