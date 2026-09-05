# FreeWili devices

The optional FreeWili family exposes asynchronous device commands. Check
HasFreeWili before use.

list_devices returns a JSON array whose entries contain an id and display name.
list_commands returns a JSON array with a command name, description, and
arguments_schema. submit accepts a device id, command name, and JSON object,
then returns an owner-scoped job number. job_result returns a JSON object with
a state such as queued, running, completed, failed, or cancelled.

Release a finished job when its result is no longer needed. Event
subscriptions have independent cursors and are owner scoped. read_events
returns at most the requested number of JSON event entries. Unsubscribe when
the feed is no longer needed. The host reclaims remaining jobs and
subscriptions when their plugin owner shuts down.
