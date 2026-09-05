# Agent tools

The optional agent-tool family lets a plugin register JSON-schema-described
tools during startup. A handler runs on the host owner thread and must issue
exactly one Ok, Text, or Error reply before returning.

The C++ handler receives a borrowed `const fwgui::McpReply&`. It is
noncopyable and valid only during that handler call. The wrapper accepts only
the first terminal reply.

Tool registration, lookup, and unregistration are scoped to the plugin
instance. Input JSON is borrowed for the handler call. Reply methods copy their
input. tool_full_name returns a borrowed frame-lifetime string because the
host may add a namespace to the registered name.

Tool names use letters, digits, underscores, and hyphens and are limited to 62
characters. Register with an object-shaped JSON Schema, or an empty object for
a tool with no arguments.
