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

    
};


void RGB_YCrCbApp::setup()
{
    
    original = loadImage(loadAsset("dog.jpg"));
    setWindowSize(original.getSize());
    
    float scale = 0.25f;
    Timer timer;
    
    // Create a YCbCr Surface -----
    timer.start();
    img = SurfaceYCbCr(original, scale); // create an compressed image
    final = img.toSurface(); // get the surface back
    console() << "compression and decompression took: " << timer.getSeconds() << " seconds" << endl;
    
    int size = original.getRowBytes() * original.getHeight();
    int csize =  img.getCompressedSize();
    
    console() << "original image is: " << size << " bytes" << endl;
    console() << "compressed image is: " << csize << " bytes" << endl;
    console() << "rate: " << size/(float)csize << endl;
    
    // Create a YCbCr Texture -----
    
    tex.setSurfaceYCbCr(img);
    
    
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

    tex.bindShaderTexture();
    gl::drawSolidRect(getWindowBounds());
    tex.unbindShaderTexture();


}

CINDER_APP_NATIVE( RGB_YCrCbApp, RendererGl )
