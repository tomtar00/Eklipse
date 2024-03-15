#pragma once
#include "Shader.h"
#include "Material.h"

namespace Eklipse
{
    class EK_API ComputeShader : public Asset
    {
    public:
        ComputeShader() = delete;
        ComputeShader(const Path& filePath, const AssetHandle handle = 0);
        static Ref<ComputeShader> Create(const Path& filePath, const AssetHandle handle = 0);

        Ref<Shader> GetShader() const;
        Ref<Material> GetMaterial() const;

        virtual void Dispose() override;

        virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z) const = 0;

        static AssetType GetStaticType() { return AssetType::Shader; }
        virtual AssetType GetType() const override { return GetStaticType(); }

    protected:
        Ref<Shader> m_shader;
        Ref<Material> m_material; // TODO: This is weird
    };
}