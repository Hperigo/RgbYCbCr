# RgbYCbCr
Implements YCbCr ( YUV ) image compression using the Cinder library

There are two types of classes, SurfaceYCbCr and TextureYCbCr. The first one converts a ci::Surface8u to YCbCr color space and compress the chroma channels, the second uses a shader to decompress the image on the GPU


###SurfaceYCbCr

```
SurfaceYCbCr img(original, 0.25f); // creates an compressed image 
Surface8u result = img.toSurface(); // get's the surface back.
```
This method is a lossy compression, depending how much you compress you can get some ugly results


###TextureYCbCr

to use a TextureYCbCr you first need to compress a surface using SurfaceYCbCr, then bind the TextureYCbCr to a geometry

```
SurfaceYCbCr img(original, 0.25f); // creates an compressed image 
TextureYCbCr texture(img) // creates a texture

//in draw loop:
tex.bindShaderTexture();
gl::drawSolidRect(getWindowBounds());
tex.unbindShaderTexture();

