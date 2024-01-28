#pragma once
#include <Eklipse/Renderer/Shader.h>
#include <Eklipse/Renderer/Texture.h>

namespace Eklipse
{
    struct PushConstantData
    {
        void* data;
        size_t size;
        ShaderDataType type;
    };

    using PushConstantDataMap = std::unordered_map<String, PushConstantData>;

    struct PushConstant
    {
        PushConstant() = default;

        PushConstant(const PushConstant& other);
        PushConstant& operator=(const PushConstant& other);
        void Copy(const PushConstant& other);

        PushConstantDataMap dataPointers;
        Unique<char[]> pushConstantData;
        size_t pushConstantSize;
    };
    struct Sampler2D
    {
        uint32_t binding;
        AssetHandle textureHandle;
        Ref<Texture2D> texture;
    };

    using PushConstantMap = std::map<String, PushConstant>;
    using Sampler2DMap = std::map<String, Sampler2D>;

    class Material : public Asset
    {
    public:
        Material() = delete;
        Material(const Path& path);
        Material(const Path& path, AssetHandle shaderHandle);
        static Ref<Material> Create(const Path& path);
        static Ref<Material> Create(const Path& path, AssetHandle shaderHandle);

        template <typename T>
        void SetConstant(const String& constantName, const String& memberName, const T* data, size_t size);

        virtual void Bind();
        virtual void ApplyChanges();

        void SetShader(AssetHandle shaderHandle);
        void OnShaderReloaded();
        bool Serialize(const Path& path);
        bool Deserialize(const Path& path);

        const String& GetName() const;
        const Ref<Shader> GetShader() const;
        AssetHandle& GetShaderHandle();
        const PushConstantMap& GetPushConstants() const;
        Sampler2DMap& GetSamplers();
        bool IsValid() const;

        static AssetType GetStaticType() { return AssetType::Material; }
        virtual AssetType GetType() const override { return GetStaticType(); }

        virtual void Dispose() = 0;

    protected:
        String m_name;
        PushConstantMap m_pushConstants{};
        Sampler2DMap m_samplers{};

        AssetHandle m_shaderHandle;
        Ref<Shader> m_shader;
    };

    template <typename T>
    inline void Material::SetConstant(const String& constantName, const String& memberName, const T* data, size_t size)
    {
        EK_PROFILE();

        EK_ASSERT(m_pushConstants.find(constantName) != m_pushConstants.end(), "({0}) Push constant '{1}' not found", m_name, constantName);
        auto& pushConstant = m_pushConstants.at(constantName);
        EK_ASSERT(pushConstant.dataPointers.find(memberName) != pushConstant.dataPointers.end(), "({0}) Push constant '{1}' member '{2}' not found", m_name, constantName, memberName);

        auto& dataPointer = pushConstant.dataPointers.at(memberName);
        EK_ASSERT(dataPointer.size == size, "({0}) Push constant '{1}' member '{2}' size mismatch. Required = {3} Given = {4}", m_name, constantName, memberName, dataPointer.size, size);

        std::memcpy(dataPointer.data, data, size);
    }
}