#include "RTLayer.h"

namespace Eklipse
{
    static float time = 0.0f;

    void RTLayer::OnAttach()
    {
        m_cameraTransform.position = { 0.0f, 0.0f, 1.0f };
        m_cameraTransform.rotation = { 0.0f, 0.0f, 0.0f };
        m_camera.m_fov = 90.0f;

        m_shaderPath = "Assets/Shaders/RT_accum.glsl";
    }

    void RTLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
        {
            //glm::vec2 screenSize = { e.GetWidth(), e.GetHeight() };
            //m_rayMaterial->SetConstant("pushConstants", "uResolution", &screenSize, sizeof(glm::vec2));

            Renderer::WaitDeviceIdle();

            ResetPixelBuffer();
            time = 0.0f;

            m_rayMaterial->Dispose();
            InitMaterial();

        }, false);
    }
    void RTLayer::OnGUI(float deltaTime)
    {
        ImGui::Begin("Ray Tracing");
        ImGui::Text("FPS: %f", Stats::Get().fps);
        ImGui::Separator();
        static int shaderIndex = 1;
        if (ImGui::Combo("Shader", &shaderIndex, "RT_basic\0RT_accum"))
        {
            Renderer::WaitDeviceIdle();
            m_rayShader->Dispose();
            m_rayMaterial->Dispose();
            ResetPixelBuffer();
            switch (shaderIndex)
            {
                case 0: m_shaderPath = "Assets/Shaders/RT_basic.glsl"; break;
                case 1: m_shaderPath = "Assets/Shaders/RT_accum.glsl"; break;
            }
            InitShader();
            InitMaterial();
        }
        if (ImGui::Button("Recompile Shader"))
        {
            Renderer::WaitDeviceIdle();
            ResetPixelBuffer();
            if (m_rayShader->Compile(m_shaderPath, true))
                m_rayMaterial->OnShaderReloaded();
        }
        ImGui::Separator();
        ImGui::DragFloat3("Camera Position", &m_cameraTransform.position[0], 0.1f);
        ImGui::DragFloat3("Camera Rotation", &m_cameraTransform.rotation[0], 0.1f);
        if (ImGui::SliderFloat("Camera FOV", &m_camera.m_fov, 1.0f, 120.0f))
        {
            ResetPixelBuffer();
        }
        ImGui::End();
    }
    void RTLayer::OnRender(float deltaTime)
    {
        time += deltaTime;
        m_camera.UpdateViewProjectionMatrix(m_cameraTransform, g_defaultFramebuffer->GetAspectRatio());
        m_rayMaterial->SetConstant("pushConstants", "uCameraPos", &m_cameraTransform.position[0], sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pushConstants", "uTime", &time, sizeof(float));
        
        Renderer::UpdateViewProjection(m_camera, m_cameraTransform);
        RenderCommand::DrawIndexed(m_fullscreenVA, m_rayMaterial.get());
    }
    
    void RTLayer::OnAPIHasInitialized(ApiType api)
    {
        InitQuad();
        InitShader();
        InitMaterial();
    }
    void RTLayer::OnShutdownAPI(bool quit)
    {
        m_fullscreenVA->Dispose();
        m_rayShader->Dispose();
        m_rayMaterial->Dispose();
    }

    void RTLayer::InitQuad()
    {
        std::vector<float> vertices = {
             1.0f,  1.0f,  // top right
             1.0f, -1.0f,  // bottom right
            -1.0f, -1.0f,  // bottom left
            -1.0f,  1.0f,  // top left
        };
        std::vector<uint32_t> indices = {
            0, 1, 3,
            1, 2, 3
        };

        Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices);
        BufferLayout layout = {
            { "inPos", ShaderDataType::FLOAT2, false }
        };
        vertexBuffer->SetLayout(layout);

        m_fullscreenVA = VertexArray::Create();
        m_fullscreenVA->AddVertexBuffer(vertexBuffer);
        m_fullscreenVA->SetIndexBuffer(IndexBuffer::Create(indices));
    }
    void RTLayer::InitShader()
    {
        m_rayShader = Shader::Create(m_shaderPath);
    }
    void RTLayer::InitMaterial()
    {
        glm::vec2 screenSize = { Application::Get().GetInfo().windowWidth, Application::Get().GetInfo().windowHeight };

        uint32_t numChannels = 4;
        uint32_t floatSize = sizeof(float);
        size_t bufferSize = screenSize.x * screenSize.y * numChannels * floatSize;
        m_pixelBuffer = Renderer::CreateStorageBuffer("pixels", bufferSize, 1);

        m_rayMaterial = Material::Create(m_rayShader);
        m_rayMaterial->SetConstant("pushConstants", "uResolution", &screenSize, sizeof(glm::vec2));
    }
    void RTLayer::ResetPixelBuffer()
    {
        glm::vec2 screenSize = { Application::Get().GetInfo().windowWidth, Application::Get().GetInfo().windowHeight };
        size_t size = screenSize.x * screenSize.y * 4 * sizeof(float);
        uint8_t* data = new uint8_t[size];

        for (int i = 0; i < size; i += 4)
        {
            data[i]        = 0;
            data[i + 1]    = 0;
            data[i + 2]    = 0;
            data[i + 3]    = 255;
        }

        m_pixelBuffer->SetData(data, size);
        delete data;
    }
    //void RTLayer::InitTexture()
    //{
    //    glm::vec2 screenSize = { Application::Get().GetInfo().windowWidth, Application::Get().GetInfo().windowHeight };

    //    TextureInfo info{};
    //    info.width = screenSize.x;
    //    info.height = screenSize.y;
    //    info.imageFormat = ImageFormat::RGBA8;
    //    info.imageAspect = ImageAspect::COLOR;
    //    info.imageLayout = ImageLayout::SHADER_READ_ONLY;
    //    info.imageUsage = ImageUsage::SAMPLED | ImageUsage::TRASNFER_DST;
    //    TextureData data{};
    //    data.info = info;
    //    data.size = info.width * info.height * FormatToChannels(info.imageFormat);
    //    data.data = new uint8_t[data.size];

    //    for (int i = 0; i < data.size; i += 4)
    //    {
    //        data.data[i]        = 0;
    //        data.data[i + 1]    = 0;
    //        data.data[i + 2]    = 0;
    //        data.data[i + 3]    = 255;
    //    }

    //    m_previousFrame = Texture2D::Create(data);
    //    m_rayMaterial->SetSampler("uPreviousFrame", m_previousFrame);
    //    delete data.data;
    //}
}