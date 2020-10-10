#pragma once

#include "Image2D.h"
#include "ssbo.h"
#include "FFTAdditionalFunctions.h"
#include "ComputeShader.h"
#include "ShaderSources.h";

class FFT2D
{
public:
	FFT2D(std::string_view path)
		: 
		originalImage(path, true), 
		realPart(nextPoT(originalImage.get_width()), nextPoT(originalImage.get_height()), originalImage.get_channels()),
		imaginaryPart(realPart.get_width(), realPart.get_height(), realPart.get_channels()), 
		fft2d(FFT2DSource, false)
	{
		fft2ddata.input_width = originalImage.get_width();
		fft2ddata.input_height = originalImage.get_height();
	
		fft2ddata.channel_no = originalImage.get_channels();

		fft2ddata.output_width = realPart.get_width();
		fft2ddata.output_height = realPart.get_height();

		fft2ddata.clz_width = fft2d_clz(fft2ddata.output_width) + 1;
		fft2ddata.clz_height = fft2d_clz(fft2ddata.output_height) + 1;

		fft2ddata.logtwo_width = 32 - fft2ddata.clz_width;
		fft2ddata.logtwo_height = 32 - fft2ddata.clz_height;

		originalImage.upload();
		realPart.upload();
		imaginaryPart.upload();

		fft2d.bindUniform("inputImage", 0);
		fft2d.bindUniform("realPart", 1);
		fft2d.bindUniform("imagPart", 2);
		fft2d.bindUniform("img_info", 3);

		originalImage.bind(0);
		realPart.bind(1);
		imaginaryPart.bind(2);
	}

	void foward()
	{
		{
			fft2ddata.stage = 0;
			SSBO tmp(fft2ddata, 3);

			fft2d.invoke(realPart.get_width());
		}

		{
			fft2ddata.stage = 1;
			SSBO tmp(fft2ddata, 3);
			
			fft2d.invoke(realPart.get_height());
		}
	}

	void inverse()
	{
		{
			fft2ddata.stage = 2;
			SSBO tmp(fft2ddata, 3);
			
			fft2d.invoke(realPart.get_height());
		}

		{
			fft2ddata.stage = 3;
			SSBO tmp(fft2ddata, 3);
			
			fft2d.invoke(realPart.get_width());
		}
	}

	Image2D generatePowerSpectrum()
	{
		originalImage.unbind();
		realPart.unbind();
		imaginaryPart.unbind();

		realPart.bind(0);
		imaginaryPart.bind(1);

		Image2D powerSpectrum(realPart.get_width(), realPart.get_height(), realPart.get_channels());
		powerSpectrum.upload();
		powerSpectrum.bind(2);

		ComputeShader spectrumShader(PowerSpectrumSource, false);

		spectrumShader.bindUniform("realPart", 0);
		spectrumShader.bindUniform("imagPart", 1);

		spectrumShader.bindUniform("spectrum", 2);
		SSBO tmp(fft2ddata, 3);
		spectrumShader.invoke(realPart.get_width() / 2, realPart.get_height());

		powerSpectrum.unbind();

		originalImage.bind(0);
		realPart.bind(1);
		imaginaryPart.bind(2);

		return powerSpectrum;
	}

	Image2D originalImage;
	Image2D realPart;
	Image2D imaginaryPart;

private:
	ComputeShader fft2d;

	struct FFT2Ddata
	{
		int input_width;
		int input_height;
		int output_width;
		int output_height;
		int logtwo_width;
		int logtwo_height;
		int clz_width;
		int clz_height;
		int channel_no;
		int stage;
	} fft2ddata;
};
