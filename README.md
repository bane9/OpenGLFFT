# OpenGLFFT

![showcase](showcase.png)

Features
--------

- Foward and inverse fast fourier transform
- Can do FFT on an image of any resolution (depends on the configuration, restrictions listed below)
- Can load/save images of .jpg and .png format
- Will work with R, RG, RGB and RGBA images
- Power spectrum generation
- CLI interface
- Runs on optimized GPU Compute shaders

How to use
----------

`OpenGLFFT -input "path_to_image" [-real "path_to_image"] [-imaginary "path_to_image"] [-spectrum "path_to_image"] [-inverse "path_to_image"]`

- `-input` path to the image you want to do FFT on
- `-real` path where the real component will be saved (optional)
- `-imaginary` path where the imaginary component will be saved (optional)
- `-spectrum` path where the power spectrum will be saved (optional)
- `-inverse` path where IFFT will be saved (optional)

All input and output images must have either `.jpg` or `.png` extension

Configuration and restrictions
------------------------------

All of the restrictions are tied to following configurations:

- `WORKGROUP_SIZE_X` this must be power of two and not bigger than `N / 2` (where in this case `N` would be `max(roundUpToPoT(width), roundUpToPoT(height))`)
- `SHARED_BUFFER_SIZE` size of the shared buffer's. This affects the maximum resoultion of an image you can do FFT on
- `PIXEL_BUFFER_SIZE` local array that stores pixel data. This cannot be smaller than `N / 2 / WORKGROUP_SIZE_X`

All of these defines are in [FFT2D.comp](https://github.com/bane9/OpenGLFFT/blob/main/OpenGLFFT/FFT2D.comp) (in this configuration the shader code is located [here](https://github.com/bane9/OpenGLFFT/blob/main/OpenGLFFT/ShaderSources.h#L7))

Dependencies
------------

- [GLFW](https://www.glfw.org/)
- [GLAD](https://glad.dav1d.de/) (included)
- [stbi_image](https://github.com/nothings/stb/blob/master/stb_image.h) (included)
- [stbi_image_write](https://github.com/nothings/stb/blob/master/stb_image_write.h) (included)
