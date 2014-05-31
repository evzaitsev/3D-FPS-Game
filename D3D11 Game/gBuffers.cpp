#include "stdafx.h"

#ifdef _USE_DEFERRED_SHADING_

gBuffers::gBuffers(ID3D11Device* dev, ID3D11DeviceContext* devcontext, D3D11_VIEWPORT viewport, UINT w, UINT h)
	: mDevice(dev),
	mDevContext(devcontext),
	mWidth(w),
	mHeight(h),
	mViewPort(viewport)
{
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;

	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1; 
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	SRVDesc.Format = texDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture1D.MipLevels = 1;

	for (int i = 0; i < NUM_GBUFFERS; ++i)
	{
		HR(mDevice->CreateTexture2D(&texDesc, 0, &mRenderTargetTexture[i]));
		HR(mDevice->CreateRenderTargetView(mRenderTargetTexture[i], &rtvDesc, &mRenderTargetView[i]));
		HR(mDevice->CreateShaderResourceView(mRenderTargetTexture[i], &SRVDesc,  &mRenderTargetViewSRV[i]));
	}
}

gBuffers::~gBuffers()
{
	for (int i = 0; i < NUM_GBUFFERS; ++i)
	{
		ReleaseCOM(mRenderTargetTexture[i]);
		ReleaseCOM(mRenderTargetView[i]);
		ReleaseCOM(mRenderTargetViewSRV[i]);

	}
}


ID3D11ShaderResourceView* gBuffers::GetGBuffersSRV(UINT Array)
{
	return mRenderTargetViewSRV[Array];
}

void gBuffers::SetRenderTargetView(ID3D11DepthStencilView* depthstencilview)
{
	mDevContext->OMSetRenderTargets(NUM_GBUFFERS, mRenderTargetView, depthstencilview);
	mDevContext->RSSetViewports(1, &mViewPort);	

	const float color[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

	for (int i = 0; i < NUM_GBUFFERS; ++i)
	{
		mDevContext->ClearRenderTargetView(mRenderTargetView[i], color);
	}
	mDevContext->ClearDepthStencilView(depthstencilview, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

#endif