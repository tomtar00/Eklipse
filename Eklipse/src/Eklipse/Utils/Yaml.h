#pragma once
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

namespace YAML {

    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::mat3>
    {
        static Node encode(const glm::mat3& rhs)
        {
            Node node;
            node.push_back(rhs[0]);
            node.push_back(rhs[1]);
            node.push_back(rhs[2]);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::mat3& rhs)
        {
            rhs[0] = node[0].as<glm::vec3>();
            rhs[1] = node[1].as<glm::vec3>();
            rhs[2] = node[2].as<glm::vec3>();
            return true;
        }
    };
    template<>
    struct convert<glm::mat4>
    {
        static Node encode(const glm::mat4& rhs)
        {
            Node node;
            node.push_back(rhs[0]);
            node.push_back(rhs[1]);
            node.push_back(rhs[2]);
            node.push_back(rhs[3]);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::mat4& rhs)
        {
            rhs[0] = node[0].as<glm::vec4>();
            rhs[1] = node[1].as<glm::vec4>();
            rhs[2] = node[2].as<glm::vec4>();
            rhs[3] = node[3].as<glm::vec4>();
            return true;
        }
    };

    template<>
    struct convert<glm::ivec2>
    {
        static Node encode(const glm::ivec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::ivec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::ivec3>
    {
        static Node encode(const glm::ivec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::ivec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::ivec4>
    {
        static Node encode(const glm::ivec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::ivec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };
}

namespace Eklipse
{
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v);

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat3& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat4& v);

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::ivec2& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::ivec3& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::ivec4& v);

    template<typename T>
    void TryDeserailize(const YAML::Node& node, const String& key, T* value)
	{
		if (node[key])
			*value = node[key].as<T>();
        else EK_CORE_WARN("Could not find key {0} in node {1}", key, node.Tag());
    }
    template<typename T>
    T TryDeserailize(const YAML::Node& node, const String& key, T defaultValue)
    {
        if (node[key])
			return node[key].as<T>();
		EK_CORE_WARN("Could not find key {0} in node {1}", key, node.Tag());
		return defaultValue;
    }
}