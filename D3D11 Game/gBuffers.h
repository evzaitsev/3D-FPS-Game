#ifdef _USE_DEFERRED_SHADING_

#ifndef G_BUFFERS_H
#define G_BUFFERS_H

#define NUM_GBUFFERS 3

class gBuffers
{
public:
	gBuffers(ID3D11Device* device, ID3D11DeviceContext* pDeviceContext, D3D11_VIEWPORT viewport, UINT width, UINT height);
	~gBuffers();
	
	void SetRenderTargetView(ID3D11DepthStencilView* depthStencilView);

	ID3D11ShaderResourceView* GetGBuffersSRV(UINT Array);

private:

private:
	UINT mWidth;
	UINT mHeight;
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDevContext;
	D3D11_VIEWPORT mViewPort;
	ID3D11Texture2D* mRenderTargetTexture[NUM_GBUFFERS];
	ID3D11RenderTargetView* mRenderTargetView[NUM_GBUFFERS];
	ID3D11ShaderResourceView* mRenderTargetViewSRV[NUM_GBUFFERS];

};

#endif //G_BUFFERS_H

#endif //_USE_DEFERRED_SHADING_