#!/usr/bin/env bash
set -euo pipefail

if [ $# -lt 2 ]; then
        echo "Usage: $0 OWNER REPO [artifact_name]" >&2
        exit 1
fi

owner=$1
repo=$2
artifact=${3:-dist}

if [ -z "${GITHUB_TOKEN:-}" ]; then
        echo "GITHUB_TOKEN is not set" >&2
        exit 1
fi

api="https://api.github.com/repos/$owner/$repo/actions/artifacts?per_page=100"

id=$(curl -s -H "Authorization: Bearer $GITHUB_TOKEN" "$api" | jq -r ".artifacts[] | select(.name==\"$artifact\" and .expired==false) | .id" | head -n1)

if [ -z "$id" ] || [ "$id" = "null" ]; then
        echo "Artifact $artifact not found" >&2
        exit 1
fi

curl -L -H "Authorization: Bearer $GITHUB_TOKEN" -o artifact.zip "https://api.github.com/repos/$owner/$repo/actions/artifacts/$id/zip"
rm -rf dist
unzip artifact.zip >/dev/null
rm artifact.zip
