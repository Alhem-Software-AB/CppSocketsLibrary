# Artifact Demo

This example fetches the prebuilt Sockets library artifact from GitHub and builds a tiny telnet chat server.

## Fetch the artifact

Set `GITHUB_TOKEN` for authentication and run:

```sh
./fetch-dist.sh OWNER REPO
```

The script downloads the latest `dist` artifact and extracts it into `dist/`.

## Build the chat server

```sh
make
```

Run the server and connect with any telnet client:

```sh
./chat_server &
telnet localhost 7000
```

Open multiple telnet sessions to chat between them.
