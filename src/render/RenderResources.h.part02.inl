
	class RenderTarget
	{
	public:
		virtual ~RenderTarget() = default;
		[[nodiscard]] virtual RenderTargetDescriptor Descriptor() const noexcept = 0;
		virtual void Release() = 0;
	};

	class RenderSceneInstance
	{
	public:
		virtual ~RenderSceneInstance() = default;
		[[nodiscard]] virtual RenderSceneInstanceDescriptor Descriptor() const noexcept = 0;
		virtual void Release() = 0;
	};

	using RenderTexturePtr = std::unique_ptr<RenderTexture>;
	using RenderMaterialPtr = std::unique_ptr<RenderMaterial>;
	using RenderTargetPtr = std::unique_ptr<RenderTarget>;
	using RenderSceneInstancePtr = std::unique_ptr<RenderSceneInstance>;
}
