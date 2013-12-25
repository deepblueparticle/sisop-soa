/*
 * registryregistrationrequest.cpp
 */

#include <registryregistrationrequest.h>
#include <enumhelper.h>

#include <stdexcept>

#include <yaml-cpp/yaml.h>

using std::string;

namespace ssoa
{
    RegistryRegistrationRequest * RegistryRegistrationRequest::fromYaml(const YAML::Node& node)
    {
        RegistryMessage::Type t = EnumHelper::fromString<RegistryMessage::Type>(node["type"].to<string>());
        if (t != TYPE_REGISTRATION_REQUEST)
            throw std::logic_error("Message type mismatch");

        string service = node["service"].to<string>();
        string host = node["host"].to<string>();
        string port = node["port"].to<string>();
        bool unregister = node["unregister"].to<bool>();
        return new RegistryRegistrationRequest(service, host, port, unregister);
    }

    string RegistryRegistrationRequest::toYaml() const
    {
        YAML::Emitter e;
        e << YAML::BeginMap;
        e << YAML::Key << "type" << YAML::Value << EnumHelper::toString(TYPE_REGISTRATION_REQUEST);
        e << YAML::Key << "service" << YAML::Value << service;
        e << YAML::Key << "host" << YAML::Value << host;
        e << YAML::Key << "port" << YAML::Value << port;
        if (unregister) {
            e << YAML::Key << "unregister" << YAML::Value << unregister;
        }
        e << YAML::EndMap;
        return e.c_str();
    }
}
