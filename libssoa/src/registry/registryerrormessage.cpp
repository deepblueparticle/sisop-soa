/*
 * registryerrormessage.cpp
 */

#include <ssoa/registry/registryerrormessage.h>

#include <stdexcept>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryMessage * RegistryErrorMessage::fromYaml(const YAML::Node& node)
    {
        if (node["type"].to<string>() != messageType())
            throw std::logic_error("Message type mismatch");

        string status = node["status"].to<string>();
        return new RegistryErrorMessage(status);
    }

    string RegistryErrorMessage::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << messageType();
        e << YAML::Key << "status" << YAML::Value << status;
        e << YAML::EndMap;
        return e.c_str();
    }
}
