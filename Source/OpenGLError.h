#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <print>

enum class GLMessageType
{
    Error = GL_DEBUG_TYPE_ERROR,
    Deprecated = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
    Undefined = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
};

template<>
struct std::formatter<GLMessageType> : std::formatter<std::string>
{
	auto format(const GLMessageType& type, format_context& ctx) const
	{
		if (type == GLMessageType::Error)
			return std::formatter<std::string>::format(std::format("{}", "GL ERROR"), ctx);

        if (type == GLMessageType::Deprecated)
            return std::formatter<std::string>::format(std::format("{}", "GL Deprecated Behavior"), ctx);

        if (type == GLMessageType::Undefined)
            return std::formatter<std::string>::format(std::format("{}", "GL Undefined Behavior"), ctx);

		else
			return std::formatter<std::string>::format(std::format("{}", "<NO_TYPE>"), ctx);
	}
};

enum class GLMessageSeverity
{

    High = GL_DEBUG_SEVERITY_HIGH,
    Medium = GL_DEBUG_SEVERITY_MEDIUM,
    Low = GL_DEBUG_SEVERITY_LOW,
    Trace = GL_DEBUG_SEVERITY_NOTIFICATION
};

template<>
struct std::formatter<GLMessageSeverity> : std::formatter<std::string>
{
    auto format(const GLMessageSeverity& type, format_context& ctx) const
    {
        if (type == GLMessageSeverity::High)
            return std::formatter<std::string>::format(std::format("{}", "HIGH"), ctx);

        if (type == GLMessageSeverity::Medium)
            return std::formatter<std::string>::format(std::format("{}", "MEDIUM"), ctx);

        if (type == GLMessageSeverity::Low)
            return std::formatter<std::string>::format(std::format("{}", "LOW"), ctx);

        if (type == GLMessageSeverity::Trace)
            return std::formatter<std::string>::format(std::format("{}", "TRACE"), ctx);

        else
            return std::formatter<std::string>::format(std::format("{}", "<NO_SEVERITY>"), ctx);
    }
};

static void GLAPIENTRY
GLMessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    std::print("{}: {} Severity: {}\n", (GLMessageType)type, (GLMessageSeverity)severity, message);
}